#include "TabPayment.h"
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>

TabPayment::TabPayment(PaymentManager* paymentMgr, QWidget* parent)
    : QWidget(parent), pm(paymentMgr) {
    setupUi();
    refreshUnpaid();
}

void TabPayment::setupUi() {
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto* scrollContent = new QWidget(scrollArea);
    auto* mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(14);

    // ================= 1. THANH TÌM KIẾM ĐƠN =================
    auto* grpSearch = new QGroupBox("🔍 Tra Cứu Đơn Đặt Sân Cần Thanh Toán", scrollContent);
    auto* searchLayout = new QHBoxLayout(grpSearch);
    searchLayout->setContentsMargins(12, 12, 12, 12);
    searchLayout->setSpacing(10);

    txtSearchName = new QLineEdit(grpSearch);
    txtSearchName->setPlaceholderText("Nhập tên khách hàng cần tìm (VD: Nguyễn Văn A, mai999)...");
    txtSearchName->setClearButtonEnabled(true);

    btnSearch = new QPushButton("🔍 Tìm Kiếm", grpSearch);
    btnSearch->setCursor(Qt::PointingHandCursor);
    btnSearch->setStyleSheet("background-color: #2563eb; color: white; padding: 7px 20px; font-weight: bold; border-radius: 8px;");

    lblSearchResult = new QLabel("", grpSearch);
    lblSearchResult->setStyleSheet("font-weight: bold; padding-left: 8px;");

    searchLayout->addWidget(txtSearchName, 3);
    searchLayout->addWidget(btnSearch, 1);
    searchLayout->addWidget(lblSearchResult, 3);
    mainLayout->addWidget(grpSearch);

    // ================= 2. KHU VỰC THU TIỀN: 2 CỘT =================
    auto* middleLayout = new QHBoxLayout();
    middleLayout->setSpacing(14);

    // --- Cột Trái: Danh Sách Đơn Chờ Thanh Toán ---
    auto* grpUnpaid = new QGroupBox("📋 Danh Sách Đơn Chờ Thanh Toán", scrollContent);
    auto* unpaidLayout = new QVBoxLayout(grpUnpaid);
    unpaidLayout->setContentsMargins(12, 12, 12, 12);

    listUnpaid = new QListWidget(grpUnpaid);
    listUnpaid->setMinimumHeight(240);
    unpaidLayout->addWidget(listUnpaid);
    middleLayout->addWidget(grpUnpaid, 4);

    // --- Cột Phải: Dịch Vụ Phụ Phí & Chi Tiết Hóa Đơn ---
    auto* grpDetails = new QGroupBox("💳 Chi Tiết Hóa Đơn & Thu Phụ Phí", scrollContent);
    auto* detailsLayout = new QVBoxLayout(grpDetails);
    detailsLayout->setContentsMargins(14, 14, 14, 14);
    detailsLayout->setSpacing(10);

    // Bảng chọn phụ phí dịch vụ sân bóng
    auto* grpServices = new QGroupBox("➕ Phụ Phí Dịch Vụ Thêm (Nước uống, thuê áo/bóng, đèn đêm)", grpDetails);
    grpServices->setStyleSheet("QGroupBox { background: #f8fafc; font-weight: 700; font-size: 12px; }");
    auto* servicesGrid = new QGridLayout(grpServices);
    servicesGrid->setContentsMargins(10, 12, 10, 10);
    servicesGrid->setSpacing(10);

    auto createSpinBox = [this](QWidget* parent) {
        auto* sp = new QSpinBox(parent);
        sp->setRange(0, 50);
        sp->setValue(0);
        sp->setFixedWidth(68);
        connect(sp, QOverload<int>::of(&QSpinBox::valueChanged), this, &TabPayment::updatePreview);
        return sp;
    };

    spnWater = createSpinBox(grpServices);
    spnRevive = createSpinBox(grpServices);
    spnBall = createSpinBox(grpServices);
    spnBib = createSpinBox(grpServices);
    spnSocks = createSpinBox(grpServices);
    spnLight = createSpinBox(grpServices);

    servicesGrid->addWidget(new QLabel("💧 Nước suối (10k):", grpServices), 0, 0);
    servicesGrid->addWidget(spnWater, 0, 1);
    servicesGrid->addWidget(new QLabel("⚡ Revive chanh muối (15k):", grpServices), 0, 2);
    servicesGrid->addWidget(spnRevive, 0, 3);
    servicesGrid->addWidget(new QLabel("⚽ Thuê bóng da (30k):", grpServices), 0, 4);
    servicesGrid->addWidget(spnBall, 0, 5);

    servicesGrid->addWidget(new QLabel("🎽 Áo bib đội (20k):", grpServices), 1, 0);
    servicesGrid->addWidget(spnBib, 1, 1);
    servicesGrid->addWidget(new QLabel("🧦 Vớ thể thao (20k):", grpServices), 1, 2);
    servicesGrid->addWidget(spnSocks, 1, 3);
    servicesGrid->addWidget(new QLabel("💡 Đèn cao áp (50k):", grpServices), 1, 4);
    servicesGrid->addWidget(spnLight, 1, 5);

    auto* subServLayout = new QHBoxLayout();
    lblTotalServiceFee = new QLabel("Tổng phụ phí: <b>0 đ</b>", grpDetails);
    lblTotalServiceFee->setStyleSheet("color: #2563eb; font-size: 13px; font-weight: bold;");

    btnResetServices = new QPushButton("🔄 Đặt lại phụ phí (0đ)", grpDetails);
    btnResetServices->setCursor(Qt::PointingHandCursor);
    btnResetServices->setStyleSheet("background-color: #64748b; color: white; padding: 4px 12px; font-size: 11px; border-radius: 6px;");
    connect(btnResetServices, &QPushButton::clicked, this, &TabPayment::resetServices);

    subServLayout->addWidget(lblTotalServiceFee);
    subServLayout->addStretch();
    subServLayout->addWidget(btnResetServices);

    detailsLayout->addWidget(grpServices);
    detailsLayout->addLayout(subServLayout);

    // Khung hiển thị chi tiết hóa đơn kiểu POS Card
    lblInvoiceDetails = new QLabel("Vui lòng chọn một đơn đặt sân để tiến hành thanh toán.", grpDetails);
    lblInvoiceDetails->setStyleSheet(
        "background-color: #ffffff; border: 1.5px solid #e2e8f0; border-radius: 10px; "
        "padding: 14px; font-size: 13px; line-height: 1.6;"
    );
    lblInvoiceDetails->setTextFormat(Qt::RichText);
    detailsLayout->addWidget(lblInvoiceDetails);

    btnPay = new QPushButton("💳 XÁC NHẬN THANH TOÁN & IN BIÊN LAI", grpDetails);
    btnPay->setCursor(Qt::PointingHandCursor);
    btnPay->setFixedHeight(46);
    btnPay->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #059669, stop:1 #10b981);"
        "  color: white;"
        "  font-size: 15px;"
        "  font-weight: 800;"
        "  letter-spacing: 0.5px;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #047857, stop:1 #059669);"
        "}"
        "QPushButton:disabled {"
        "  background: #cbd5e1; color: #94a3b8;"
        "}"
    );
    detailsLayout->addWidget(btnPay);

    middleLayout->addWidget(grpDetails, 6);
    mainLayout->addLayout(middleLayout);

    // ================= 3. BẢNG LỊCH SỬ HÓA ĐƠN =================
    auto* grpHistory = new QGroupBox("📜 Lịch Sử Hóa Đơn Đã Thu Ngân (Nhấp để xem chi tiết biên lai)", scrollContent);
    auto* histLayout = new QVBoxLayout(grpHistory);
    histLayout->setContentsMargins(12, 12, 12, 12);

    tableHistory = new QTableWidget(0, 6, grpHistory);
    tableHistory->setMinimumHeight(240);
    tableHistory->setHorizontalHeaderLabels({"Mã HĐ", "Mã Đơn", "Khách Hàng", "Sân Bóng", "Thực Thu (VNĐ)", "Thời Điểm Thu"});
    tableHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableHistory->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableHistory->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableHistory->setSelectionMode(QAbstractItemView::SingleSelection);
    tableHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableHistory->setAlternatingRowColors(true);
    histLayout->addWidget(tableHistory);

    mainLayout->addWidget(grpHistory);

    scrollArea->setWidget(scrollContent);
    outerLayout->addWidget(scrollArea);

    // Kết nối các tín hiệu
    connect(txtSearchName, &QLineEdit::returnPressed, this, &TabPayment::onSearchByName);
    connect(btnSearch, &QPushButton::clicked, this, &TabPayment::onSearchByName);
    connect(listUnpaid, &QListWidget::currentRowChanged, this, &TabPayment::updatePreview);
    connect(btnPay, &QPushButton::clicked, this, &TabPayment::onPayClicked);
    connect(tableHistory, &QTableWidget::cellClicked, this, &TabPayment::onHistoryItemClicked);
    connect(tableHistory, &QTableWidget::cellDoubleClicked, this, &TabPayment::onHistoryItemClicked);
}

double TabPayment::calculateTotalSurcharge() const {
    double total = 0.0;
    total += spnWater->value() * 10000.0;
    total += spnRevive->value() * 15000.0;
    total += spnBall->value() * 30000.0;
    total += spnBib->value() * 20000.0;
    total += spnSocks->value() * 20000.0;
    total += spnLight->value() * 50000.0;
    return total;
}

void TabPayment::resetServices() {
    spnWater->setValue(0);
    spnRevive->setValue(0);
    spnBall->setValue(0);
    spnBib->setValue(0);
    spnSocks->setValue(0);
    spnLight->setValue(0);
    updatePreview();
}

void TabPayment::refreshUnpaid() {
    unpaidList = pm->getUnpaidBookings();
    listUnpaid->clear();
    for (const auto& b : unpaidList) {
        listUnpaid->addItem(
            QString("🏷️ <b>%1</b> &nbsp;|&nbsp; ⚽ %2 &nbsp;|&nbsp; 👤 <b>%3</b> &nbsp;|&nbsp; 📅 %4 (%5 - %6)")
            .arg(b.booking_id, b.field_name, b.customer_name, b.booking_date, b.start_time, b.end_time)
        );
    }
    if (!unpaidList.empty()) {
        listUnpaid->setCurrentRow(0);
    } else {
        lblInvoiceDetails->setText(
            "<div style='text-align: center; padding: 20px; color: #059669; font-size: 14px;'>"
            "🎉 <b>Không có đơn nào chờ thanh toán!</b><br>"
            "<span style='color: #64748b; font-size: 12px;'>Mọi đơn đặt sân hiện tại đều đã được thu ngân xử lý.</span>"
            "</div>"
        );
        btnPay->setEnabled(false);
    }
    refreshHistoryTable();
}

void TabPayment::refreshHistoryTable() {
    currentPayments = pm->getAllPayments();
    tableHistory->setRowCount(static_cast<int>(currentPayments.size()));
    for (int i = 0; i < static_cast<int>(currentPayments.size()); ++i) {
        const auto& p = currentPayments[i];

        auto* itemInv = new QTableWidgetItem(p.invoice_id);
        itemInv->setTextAlignment(Qt::AlignCenter);
        QFont fBold = itemInv->font();
        fBold.setBold(true);
        itemInv->setFont(fBold);

        auto* itemBook = new QTableWidgetItem(p.booking_id);
        itemBook->setTextAlignment(Qt::AlignCenter);

        auto* itemCust = new QTableWidgetItem("👤 " + p.customer_name);
        itemCust->setFont(fBold);

        auto* itemField = new QTableWidgetItem("⚽ " + p.field_name);

        auto* itemTotal = new QTableWidgetItem(QString("%L1 đ").arg(p.total_amount, 0, 'f', 0));
        itemTotal->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        itemTotal->setForeground(QBrush(QColor("#059669")));
        itemTotal->setFont(fBold);

        auto* itemTime = new QTableWidgetItem(p.payment_time);
        itemTime->setTextAlignment(Qt::AlignCenter);
        itemTime->setForeground(QBrush(QColor("#64748b")));

        tableHistory->setItem(i, 0, itemInv);
        tableHistory->setItem(i, 1, itemBook);
        tableHistory->setItem(i, 2, itemCust);
        tableHistory->setItem(i, 3, itemField);
        tableHistory->setItem(i, 4, itemTotal);
        tableHistory->setItem(i, 5, itemTime);
    }
}

void TabPayment::onSearchByName() {
    QString keyword = txtSearchName->text().trimmed();
    if (keyword.isEmpty()) {
        lblSearchResult->setText("⚠️ Vui lòng nhập tên khách hàng cần tìm.");
        lblSearchResult->setStyleSheet("color: #d97706; font-weight: bold;");
        return;
    }

    auto bOpt = pm->findBookingByName(keyword);
    if (!bOpt.has_value()) {
        lblSearchResult->setText(QString("⚠️ Không tìm thấy đơn nào cho '%1'.").arg(keyword));
        lblSearchResult->setStyleSheet("color: #dc2626; font-weight: bold;");
        return;
    }

    const Bookings& foundBooking = bOpt.value();
    lblSearchResult->setText(
        QString("✅ Tìm thấy: %1 — %2 (%3) | Trạng thái: %4")
        .arg(foundBooking.booking_id, foundBooking.customer_name, foundBooking.field_name, foundBooking.status)
    );
    lblSearchResult->setStyleSheet("color: #059669; font-weight: bold;");

    // Tìm trong danh sách chờ thanh toán
    int foundRow = -1;
    for (int i = 0; i < static_cast<int>(unpaidList.size()); ++i) {
        if (unpaidList[i].booking_id == foundBooking.booking_id) {
            foundRow = i;
            break;
        }
    }

    if (foundRow >= 0) {
        listUnpaid->setCurrentRow(foundRow);
        updatePreview();
    } else {
        double surcharge = calculateTotalSurcharge();
        PaymentPreview info = pm->preview(foundBooking, surcharge);

        QString statusBadge = (foundBooking.status.contains("hủy", Qt::CaseInsensitive))
            ? "<span style='color: white; background-color: #dc2626; padding: 2px 8px; border-radius: 4px; font-weight: bold;'>ĐÃ HỦY</span>"
            : "<span style='color: white; background-color: #059669; padding: 2px 8px; border-radius: 4px; font-weight: bold;'>ĐÃ THANH TOÁN</span>";

        lblInvoiceDetails->setText(QString(
            "<table width='100%' style='line-height: 1.5; font-size: 13px;'>"
            "<tr><td><b>Mã đơn:</b> <span style='color:#2563eb; font-weight:bold;'>%1</span></td><td align='right'>%2</td></tr>"
            "<tr><td><b>Khách hàng:</b> <b>%3</b> (%4)</td><td align='right'><b>Sân:</b> %5</td></tr>"
            "<tr><td colspan='2'><b>Thời gian:</b> %6 &nbsp;(%7 → %8 - %9 giờ)</td></tr>"
            "</table>"
            "<hr style='border: 0.5px solid #e2e8f0; margin: 8px 0;'>"
            "<table width='100%' style='line-height: 1.5; font-size: 13px;'>"
            "<tr><td>Đơn giá sân:</td><td align='right'>%L10 đ/h</td></tr>"
            "<tr><td>Tiền thuê sân:</td><td align='right'><b>%L11 đ</b></td></tr>"
            "<tr><td>Phụ phí dịch vụ:</td><td align='right'>+%L12 đ</td></tr>"
            "<tr><td>Trừ tiền đã cọc:</td><td align='right' style='color:#dc2626;'>-%L13 đ</td></tr>"
            "</table>"
            "<div style='margin-top: 10px; padding: 10px; background: #f8fafc; border-radius: 8px; text-align: right;'>"
            "<span style='font-size: 12px; font-weight: bold; color: #64748b;'>TỔNG TIỀN: </span>"
            "<span style='font-size: 18px; font-weight: 800; color: #1e3a8a;'>%L14 VNĐ</span>"
            "</div>"
            "<p style='color:#64748b; font-size: 11px; margin-top: 4px;'>ℹ️ Đơn này hiện không nằm trong danh sách chờ thanh toán.</p>"
        )
        .arg(foundBooking.booking_id, statusBadge, foundBooking.customer_name, foundBooking.customer_phone,
             foundBooking.field_name, foundBooking.booking_date, foundBooking.start_time, foundBooking.end_time)
        .arg(info.duration_hours)
        .arg(info.price_per_hour, 0, 'f', 0)
        .arg(info.rental_fee, 0, 'f', 0)
        .arg(info.surcharge, 0, 'f', 0)
        .arg(info.deposit_deducted, 0, 'f', 0)
        .arg(info.total_amount, 0, 'f', 0));

        btnPay->setEnabled(false);
    }
}

void TabPayment::updatePreview() {
    double surcharge = calculateTotalSurcharge();
    lblTotalServiceFee->setText(QString("Tổng phụ phí: <b style='color:#059669;'>%L1 đ</b>").arg(surcharge, 0, 'f', 0));

    int row = listUnpaid->currentRow();
    if (row < 0 || row >= static_cast<int>(unpaidList.size())) {
        btnPay->setEnabled(false);
        return;
    }

    const Bookings& b = unpaidList[row];
    PaymentPreview info = pm->preview(b, surcharge);

    lblInvoiceDetails->setText(QString(
        "<table width='100%' style='line-height: 1.5; font-size: 13px;'>"
        "<tr><td><b>Mã đơn:</b> <span style='color:#2563eb; font-weight:bold;'>%1</span></td>"
        "<td align='right'><span style='color:#1d4ed8; background:#eff6ff; padding:2px 8px; border-radius:4px; font-weight:bold;'>%2</span></td></tr>"
        "<tr><td><b>Khách hàng:</b> <b>%3</b> (%4)</td><td align='right'><b>Sân:</b> %5</td></tr>"
        "<tr><td colspan='2'><b>Thời gian:</b> %6 &nbsp;(%7 → %8 • <b>%9 giờ</b>)</td></tr>"
        "</table>"
        "<hr style='border: 0.5px solid #e2e8f0; margin: 8px 0;'>"
        "<table width='100%' style='line-height: 1.5; font-size: 13px;'>"
        "<tr><td>Đơn giá sân:</td><td align='right'>%L10 đ/giờ</td></tr>"
        "<tr><td>Tiền thuê sân:</td><td align='right'><b>%L11 đ</b></td></tr>"
        "<tr><td>Phụ phí dịch vụ:</td><td align='right' style='color:#2563eb;'>+ %L12 đ</td></tr>"
        "<tr><td>Trừ tiền cọc trước:</td><td align='right' style='color:#dc2626;'>- %L13 đ</td></tr>"
        "</table>"
        "<div style='margin-top: 10px; padding: 12px; background: #ecfdf5; border: 1px solid #a7f3d0; border-radius: 8px; display: flex; justify-content: space-between;'>"
        "<table width='100%'><tr>"
        "<td><span style='font-size: 13px; font-weight: bold; color: #065f46;'>THỰC THU CẦN THANH TOÁN:</span></td>"
        "<td align='right'><span style='font-size: 20px; font-weight: 800; color: #047857;'>%L14 VNĐ</span></td>"
        "</tr></table>"
        "</div>"
    )
    .arg(b.booking_id, b.status, b.customer_name, b.customer_phone, b.field_name, b.booking_date, b.start_time, b.end_time)
    .arg(info.duration_hours)
    .arg(info.price_per_hour, 0, 'f', 0)
    .arg(info.rental_fee, 0, 'f', 0)
    .arg(info.surcharge, 0, 'f', 0)
    .arg(info.deposit_deducted, 0, 'f', 0)
    .arg(info.total_amount, 0, 'f', 0));

    btnPay->setEnabled(true);
}

void TabPayment::onPayClicked() {
    int row = listUnpaid->currentRow();
    if (row < 0 || row >= static_cast<int>(unpaidList.size())) return;

    try {
        double surcharge = calculateTotalSurcharge();
        Payments p = pm->payAndExport(unpaidList[row], surcharge, "Output");
        
        resetServices();
        
        QMessageBox::information(this, "Thanh Toán Thành Công 🎉",
            QString("Đã thu tiền & xuất hóa đơn <b>%1</b> thành công!<br>Tổng số tiền: <b>%L2 VNĐ</b>")
                .arg(p.invoice_id)
                .arg(p.total_amount, 0, 'f', 0));
            
        refreshUnpaid();
        emit paymentCompleted();

        // Mở biên lai hóa đơn ngay sau khi thanh toán xong
        ReceiptDialog dlg(p, this);
        dlg.exec();

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Lỗi thanh toán", e.what());
    }
}

void TabPayment::onHistoryItemClicked(int row, int /*column*/) {
    if (row < 0 || row >= static_cast<int>(currentPayments.size())) return;
    const auto& p = currentPayments[row];

    ReceiptDialog dlg(p, this);
    dlg.exec();
}
