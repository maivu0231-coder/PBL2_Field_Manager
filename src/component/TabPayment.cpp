#include "tabpayment.h"

TabPayment::TabPayment(PaymentManager* paymentMgr, QWidget* parent)
    : QWidget(parent), pm(paymentMgr) {
    setupUi();
    refreshUnpaid();
}

void TabPayment::refreshUnpaid() {
    unpaidList = pm->getUnpaidBookings();
    listUnpaid->clear();
    for (const auto& b : unpaidList) {
        listUnpaid->addItem(
            QString("🏷️ %1 | %2 | 👤 %3 (%4: %5-%6)")
            .arg(b.booking_id, b.field_name, b.customer_name, b.booking_date, b.start_time, b.end_time)
        );
    }
    if (!unpaidList.empty()) {
        listUnpaid->setCurrentRow(0);
    } else {
        lblInvoiceDetails->setText("🎉 Không có đơn nào chờ thanh toán.");
    }
    btnPay->setEnabled(!unpaidList.empty());
    refreshHistoryTable();
}

void TabPayment::refreshHistoryTable() {
    auto payments = pm->getAllPayments();
    tableHistory->setRowCount(static_cast<int>(payments.size()));
    for (int i = 0; i < static_cast<int>(payments.size()); ++i) {
        const auto& p = payments[i];
        tableHistory->setItem(i, 0, new QTableWidgetItem(p.invoice_id));
        tableHistory->setItem(i, 1, new QTableWidgetItem(p.booking_id));
        tableHistory->setItem(i, 2, new QTableWidgetItem(p.customer_name));
        tableHistory->setItem(i, 3, new QTableWidgetItem(p.field_name));
        tableHistory->setItem(i, 4, new QTableWidgetItem(QString("%L1 đ").arg(p.total_amount, 0, 'f', 0)));
        tableHistory->setItem(i, 5, new QTableWidgetItem(p.payment_time));
    }
}

void TabPayment::onSearchByName() {
    auto bOpt = pm->findBookingByName(txtSearchName->text());
    if (bOpt.has_value()) {
        lblSearchResult->setText(
            QString("✅ Tìm thấy: %1 — %2 (%3) | Ngày: %4 (%5-%6) | Trạng thái: %7")
            .arg(bOpt->booking_id, bOpt->customer_name, bOpt->field_name,
                 bOpt->booking_date, bOpt->start_time, bOpt->end_time, bOpt->status)
        );
    } else {
        lblSearchResult->setText("⚠️ Không tìm thấy đơn đặt sân phù hợp.");
    }
}

void TabPayment::updatePreview() {
    int row = listUnpaid->currentRow();
    if (row < 0 || row >= static_cast<int>(unpaidList.size())) return;

    const Bookings& b = unpaidList[row];
    PaymentPreview info = pm->preview(b, spnSurcharge->value());

    lblInvoiceDetails->setText(QString(
        "<b>Mã đơn:</b> %1 &nbsp;&nbsp;|&nbsp;&nbsp; <b>Trạng thái:</b> %2<br>"
        "<b>Khách hàng:</b> %3 (%4)<br>"
        "<b>Sân bóng:</b> %5 — <b>Ngày:</b> %6<br>"
        "<b>Khung giờ:</b> %7 → %8 (%9 giờ)<br>"
        "<hr>"
        "<b>Đơn giá:</b> %L10 đ/giờ<br>"
        "<b>Tiền thuê sân:</b> %L11 đ<br>"
        "<b>Phụ phí:</b> +%L12 đ<br>"
        "<span style='color:#dc2626;'><b>Trừ tiền cọc:</b> -%L13 đ</span><br>"
        "<h3 style='color:#1d4ed8;'>TỔNG CẦN THU: %L14 VNĐ</h3>"
    )
    .arg(b.booking_id, b.status, b.customer_name, b.customer_phone, b.field_name, b.booking_date, b.start_time, b.end_time)
    .arg(info.duration_hours)
    .arg(info.price_per_hour, 0, 'f', 0)
    .arg(info.rental_fee, 0, 'f', 0)
    .arg(info.surcharge, 0, 'f', 0)
    .arg(info.deposit_deducted, 0, 'f', 0)
    .arg(info.total_amount, 0, 'f', 0));
}



void TabPayment::onPayClicked() {
    int row = listUnpaid->currentRow();
    if (row < 0 || row >= static_cast<int>(unpaidList.size())) return;

    try {
        // Gọi payAndExport để vừa thanh toán vừa tự động xuất file txt vào thư mục Output
        Payments p = pm->payAndExport(unpaidList[row], spnSurcharge->value(), "Output");
        
        txtReceipt->setPlainText(PaymentManager::formatReceipt(p));
        spnSurcharge->setValue(0);
        
        QMessageBox::information(this, "Thanh toán thành công",
            QString("Đã xuất hóa đơn %1 thành công và lưu vào thư mục Output!").arg(p.invoice_id));
            
        refreshUnpaid();
        emit paymentCompleted();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Lỗi thanh toán", e.what());
    }
}

void TabPayment::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);

    // 1. Tìm kiếm nhanh theo tên
    auto* grpSearch = new QGroupBox("🔍 Tìm Kiếm Đơn Theo Tên Khách Hàng");
    auto* searchLayout = new QHBoxLayout(grpSearch);
    txtSearchName = new QLineEdit();
    txtSearchName->setPlaceholderText("Nhập tên khách hàng cần tìm...");
    auto* btnSearch = new QPushButton("🔍 Tìm kiếm");
    lblSearchResult = new QLabel();
    searchLayout->addWidget(txtSearchName, 2);
    searchLayout->addWidget(btnSearch, 1);
    searchLayout->addWidget(lblSearchResult, 3);
    mainLayout->addWidget(grpSearch);

    // 2. Khu vực chính: Danh sách chờ (Trái) - Chi tiết hóa đơn (Giữa) - Biên lai in (Phải)
    auto* midLayout = new QHBoxLayout();

    auto* grpLeft = new QGroupBox("📋 Danh Sách Đơn Chờ Thanh Toán");
    auto* leftLayout = new QVBoxLayout(grpLeft);
    listUnpaid = new QListWidget();
    leftLayout->addWidget(listUnpaid);
    midLayout->addWidget(grpLeft, 2);

    auto* grpCenter = new QGroupBox("🧾 Chi Tiết Hóa Đơn & Thu Tiền");
    auto* centerLayout = new QVBoxLayout(grpCenter);
    spnSurcharge = new QSpinBox();
    spnSurcharge->setRange(0, 50000000);
    spnSurcharge->setSingleStep(10000);
    lblInvoiceDetails = new QLabel();
    lblInvoiceDetails->setStyleSheet("background: #f8fafc; border: 1px solid #cbd5e1; border-radius: 8px; padding: 12px;");
    btnPay = new QPushButton("✅ Thanh Toán & Xuất Hóa Đơn");

    centerLayout->addWidget(new QLabel("➕ Phụ phí (nước uống, bóng, đèn... - VNĐ):"));
    centerLayout->addWidget(spnSurcharge);
    centerLayout->addWidget(lblInvoiceDetails, 1);
    centerLayout->addWidget(btnPay);
    midLayout->addWidget(grpCenter, 2);

    auto* grpRight = new QGroupBox("🖨️ Biên Lai Hóa Đơn Vừa Xuất");
    auto* rightLayout = new QVBoxLayout(grpRight);
    txtReceipt = new QPlainTextEdit();
    txtReceipt->setReadOnly(true);
    txtReceipt->setFont(QFont("Consolas", 10));
    rightLayout->addWidget(txtReceipt);
    midLayout->addWidget(grpRight, 2);

    mainLayout->addLayout(midLayout, 3);

    // 3. Lịch sử hóa đơn
    auto* grpHistory = new QGroupBox("📜 Lịch Sử Hóa Đơn Đã Thanh Toán");
    auto* histLayout = new QVBoxLayout(grpHistory);
    tableHistory = new QTableWidget(0, 6);
    tableHistory->setHorizontalHeaderLabels({"Mã HĐ", "Mã Đơn", "Khách Hàng", "Sân", "Thực Thu", "Thời Điểm"});
    tableHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);
    histLayout->addWidget(tableHistory);
    mainLayout->addWidget(grpHistory, 2);

    connect(btnSearch, &QPushButton::clicked, this, &TabPayment::onSearchByName);
    connect(listUnpaid, &QListWidget::currentRowChanged, this, &TabPayment::updatePreview);
    connect(spnSurcharge, &QSpinBox::valueChanged, this, &TabPayment::updatePreview);
    connect(btnPay, &QPushButton::clicked, this, &TabPayment::onPayClicked);
}