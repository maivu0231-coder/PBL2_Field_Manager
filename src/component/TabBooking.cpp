#include "TabBooking.h"
#include <QFrame>

TabBooking::TabBooking(BookingManager* mgr, QWidget* parent)
    : QWidget(parent), manager(mgr) {
    setupUi();
    reloadFields();
}

void TabBooking::reloadFields() {
    QString currentId = cboField->currentData().toString();
    cboField->blockSignals(true);
    cboField->clear();
    auto fields = manager->listFields();
    for (const auto& f : fields) {
        QString statusIcon = "🟢";
        if (f.status.contains("Bảo trì", Qt::CaseInsensitive) || f.status.contains("Maintenance", Qt::CaseInsensitive)) {
            statusIcon = "🔴";
        } else if (f.status.contains("Có khách", Qt::CaseInsensitive) || f.status.contains("In Use", Qt::CaseInsensitive)) {
            statusIcon = "🟡";
        }

        QString text = QString("%1 %2 (%3) — %L4 đ/h [%5]")
                           .arg(statusIcon)
                           .arg(f.name)
                           .arg(f.field_id)
                           .arg(f.price_per_hour, 0, 'f', 0)
                           .arg(f.status);
        cboField->addItem(text, f.field_id);
    }
    int idx = cboField->findData(currentId);
    if (idx >= 0) cboField->setCurrentIndex(idx);
    cboField->blockSignals(false);
    refreshBookings();
}

void TabBooking::refreshBookings() {
    QString fieldId = cboField->currentData().toString();
    QDate d = dateEdit->date();

    // Kiểm tra bảo trì
    bool isMaint = cboField->currentText().contains("Bảo trì", Qt::CaseInsensitive)
                || cboField->currentText().contains("Maintenance", Qt::CaseInsensitive);
    lblFieldNotice->setVisible(isMaint);
    btnCreate->setEnabled(!isMaint && !fieldId.isEmpty());

    auto bookings = manager->listBookings(d, fieldId);
    tableBookings->setRowCount(static_cast<int>(bookings.size()));
    cboCancelBooking->clear();

    for (int i = 0; i < static_cast<int>(bookings.size()); ++i) {
        const auto& b = bookings[i];

        auto* itemCode = new QTableWidgetItem(b.booking_id);
        itemCode->setTextAlignment(Qt::AlignCenter);
        QFont fBold = itemCode->font();
        fBold.setBold(true);
        itemCode->setFont(fBold);

        auto* itemDate = new QTableWidgetItem(b.booking_date);
        itemDate->setTextAlignment(Qt::AlignCenter);

        auto* itemCust = new QTableWidgetItem("👤 " + b.customer_name);
        itemCust->setFont(fBold);

        auto* itemPhone = new QTableWidgetItem("📞 " + b.customer_phone);
        itemPhone->setTextAlignment(Qt::AlignCenter);

        auto* itemTime = new QTableWidgetItem("⏰ " + b.start_time + " - " + b.end_time);
        itemTime->setTextAlignment(Qt::AlignCenter);

        auto* itemDeposit = new QTableWidgetItem(QString("%L1 đ").arg(b.deposit, 0, 'f', 0));
        itemDeposit->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        itemDeposit->setForeground(QBrush(QColor("#059669")));
        itemDeposit->setFont(fBold);

        auto* itemStatus = new QTableWidgetItem();
        itemStatus->setTextAlignment(Qt::AlignCenter);
        itemStatus->setFont(fBold);

        if (b.status.contains("Đã thanh toán", Qt::CaseInsensitive)) {
            itemStatus->setText("🟢 Đã thanh toán");
            itemStatus->setForeground(QBrush(QColor("#059669")));
        } else if (b.status.contains("Đã hủy", Qt::CaseInsensitive)) {
            itemStatus->setText("⚪ Đã hủy");
            itemStatus->setForeground(QBrush(QColor("#94a3b8")));
        } else {
            itemStatus->setText("🔵 Chờ thanh toán");
            itemStatus->setForeground(QBrush(QColor("#2563eb")));
        }

        tableBookings->setItem(i, 0, itemCode);
        tableBookings->setItem(i, 1, itemDate);
        tableBookings->setItem(i, 2, itemCust);
        tableBookings->setItem(i, 3, itemPhone);
        tableBookings->setItem(i, 4, itemTime);
        tableBookings->setItem(i, 5, itemDeposit);
        tableBookings->setItem(i, 6, itemStatus);

        // Chỉ cho phép hủy các đơn chưa hủy
        if (!b.status.contains("Đã hủy", Qt::CaseInsensitive)) {
            cboCancelBooking->addItem(
                QString("%1 · %2 (%3-%4)")
                    .arg(b.booking_id)
                    .arg(b.customer_name)
                    .arg(b.start_time)
                    .arg(b.end_time),
                b.booking_id
            );
        }
    }
    btnCancel->setEnabled(cboCancelBooking->count() > 0);
}

void TabBooking::onCreateBooking() {
    QString fieldId = cboField->currentData().toString();
    if (fieldId.isEmpty()) {
        QMessageBox::warning(this, "Chưa chọn sân", "Vui lòng chọn sân bóng trước khi tạo đơn!");
        return;
    }

    if (txtCustomer->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập tên khách hàng!");
        txtCustomer->setFocus();
        return;
    }

    if (txtPhone->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập số điện thoại khách hàng!");
        txtPhone->setFocus();
        return;
    }

    try {
        Bookings created = manager->createBooking(
            fieldId,
            dateEdit->date(),
            txtCustomer->text().trimmed(),
            txtPhone->text().trimmed(),
            timeStart->time(),
            timeEnd->time(),
            spnDeposit->value()
        );
        txtCustomer->clear();
        txtPhone->clear();
        spnDeposit->setValue(0);
        
        QMessageBox::information(this, "Đặt sân thành công 🎉",
            QString("Đã tạo đơn <b>%1</b> thành công!<br>Trạng thái: <b>%2</b>")
                .arg(created.booking_id)
                .arg(created.status));
                
        refreshBookings();
        emit bookingChanged();
    } catch (const BookingError& err) {
        QMessageBox::warning(this, "Không thể đặt sân", err.message());
    }
}

void TabBooking::onCancelBooking() {
    QString bookingId = cboCancelBooking->currentData().toString();
    if (bookingId.isEmpty()) return;

    auto reply = QMessageBox::question(
        this, "Xác nhận hủy đơn",
        QString("Bạn có chắc chắn muốn hủy đơn đặt sân <b>%1</b> không?").arg(bookingId),
        QMessageBox::Yes | QMessageBox::No
    );
    if (reply != QMessageBox::Yes) return;

    try {
        Bookings cancelled = manager->cancelBooking(bookingId);
        QMessageBox::information(this, "Đã hủy đơn thành công",
            QString("Đã hủy đơn <b>%1</b> thành công.").arg(cancelled.booking_id));
        refreshBookings();
        emit bookingChanged();
    } catch (const BookingError& err) {
        QMessageBox::warning(this, "Lỗi hủy đơn", err.message());
    }
}

void TabBooking::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(12);

    // Tính toán giờ mặc định hợp lệ theo thời gian thực
    QTime now = QTime::currentTime();
    QDate today = QDate::currentDate();
    QTime defStart, defEnd;

    if (now >= QTime(21, 0)) {
        today = today.addDays(1);
        defStart = QTime(8, 0);
        defEnd = QTime(9, 30);
    } else if (now < QTime(5, 30)) {
        defStart = QTime(6, 0);
        defEnd = QTime(7, 30);
    } else {
        int nextHour = now.hour() + 1;
        if (nextHour > 20) {
            today = today.addDays(1);
            defStart = QTime(8, 0);
            defEnd = QTime(9, 30);
        } else {
            defStart = QTime(nextHour, 0);
            defEnd = defStart.addSecs(5400); // +1.5 giờ (90 phút)
            if (defEnd > QTime(22, 0)) defEnd = QTime(22, 0);
        }
    }

    // ================= 1. CHỌN NGÀY & SÂN BÓNG =================
    auto* grpTop = new QGroupBox("📆 Chọn Thời Điểm & Sân Bóng Cần Quản Lý", this);
    auto* topLayout = new QHBoxLayout(grpTop);
    topLayout->setContentsMargins(12, 12, 12, 12);
    topLayout->setSpacing(12);

    auto* lblDate = new QLabel("📅 Ngày:");
    lblDate->setStyleSheet("font-weight: bold;");
    dateEdit = new QDateEdit(today);
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->setCalendarPopup(true);
    dateEdit->setMinimumWidth(130);

    // Nút chọn nhanh ngày
    auto* btnToday = new QPushButton("Hôm nay");
    btnToday->setCursor(Qt::PointingHandCursor);
    btnToday->setStyleSheet("background: #eff6ff; color: #1d4ed8; border: 1px solid #bfdbfe; font-size: 11px; padding: 5px 10px;");

    auto* btnTomorrow = new QPushButton("Ngày mai");
    btnTomorrow->setCursor(Qt::PointingHandCursor);
    btnTomorrow->setStyleSheet("background: #f1f5f9; color: #334155; border: 1px solid #cbd5e1; font-size: 11px; padding: 5px 10px;");

    auto* lblField = new QLabel("⚽ Chọn sân:");
    lblField->setStyleSheet("font-weight: bold;");
    cboField = new QComboBox();
    cboField->setMinimumWidth(320);

    topLayout->addWidget(lblDate);
    topLayout->addWidget(dateEdit);
    topLayout->addWidget(btnToday);
    topLayout->addWidget(btnTomorrow);
    topLayout->addSpacing(15);
    topLayout->addWidget(lblField);
    topLayout->addWidget(cboField, 1);
    mainLayout->addWidget(grpTop);

    // Cảnh báo bảo trì
    lblFieldNotice = new QLabel("⛔ SÂN NÀY ĐANG BẢO TRÌ — Hệ thống tạm dừng nhận đặt lịch mới trên sân này!", this);
    lblFieldNotice->setStyleSheet(
        "background-color: #fef2f2; color: #b91c1c; font-weight: bold; font-size: 13px; "
        "border: 1px solid #fecaca; border-radius: 8px; padding: 10px 14px;"
    );
    lblFieldNotice->setVisible(false);
    mainLayout->addWidget(lblFieldNotice);

    // ================= 2. CHIA 2 CỘT: FORM TẠO ĐƠN & LỊCH TRONG NGÀY =================
    auto* bodyLayout = new QHBoxLayout();
    bodyLayout->setSpacing(14);

    // --- Cột Trái: Tạo Đơn Mới ---
    auto* grpCreate = new QGroupBox("➕ Tạo Đơn Đặt Sân Mới (06:00 - 22:00)", this);
    auto* formLayout = new QVBoxLayout(grpCreate);
    formLayout->setContentsMargins(14, 14, 14, 14);
    formLayout->setSpacing(10);

    auto* formGrid = new QGridLayout();
    formGrid->setSpacing(8);

    txtCustomer = new QLineEdit();
    txtCustomer->setPlaceholderText("VD: Nguyễn Văn Anh");

    txtPhone = new QLineEdit();
    txtPhone->setPlaceholderText("VD: 0912345678");

    timeStart = new QTimeEdit(defStart);
    timeStart->setDisplayFormat("HH:mm");

    timeEnd = new QTimeEdit(defEnd);
    timeEnd->setDisplayFormat("HH:mm");

    // Duration helper buttons
    auto* durContainer = new QWidget();
    auto* durLayout = new QHBoxLayout(durContainer);
    durLayout->setContentsMargins(0, 0, 0, 0);
    durLayout->setSpacing(6);

    auto makeDurBtn = [this](const QString& text, int minutes) {
        auto* b = new QPushButton(text);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet("background: #f1f5f9; color: #334155; font-size: 11px; padding: 4px 8px; border: 1px solid #cbd5e1; border-radius: 6px;");
        connect(b, &QPushButton::clicked, this, [this, minutes]() {
            QTime end = timeStart->time().addSecs(minutes * 60);
            if (end > QTime(22, 0)) end = QTime(22, 0);
            timeEnd->setTime(end);
        });
        return b;
    };

    durLayout->addWidget(makeDurBtn("⏱️ 1 Giờ", 60));
    durLayout->addWidget(makeDurBtn("⏱️ 1.5 Giờ (90p)", 90));
    durLayout->addWidget(makeDurBtn("⏱️ 2 Giờ", 120));

    // Deposit spinbox
    spnDeposit = new QSpinBox();
    spnDeposit->setRange(0, 50000000);
    spnDeposit->setSingleStep(50000);

    // Deposit helper chips
    auto* depContainer = new QWidget();
    auto* depLayout = new QHBoxLayout(depContainer);
    depLayout->setContentsMargins(0, 0, 0, 0);
    depLayout->setSpacing(6);

    auto makeDepBtn = [this](const QString& text, int val) {
        auto* b = new QPushButton(text);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet("background: #f1f5f9; color: #334155; font-size: 11px; padding: 4px 8px; border: 1px solid #cbd5e1; border-radius: 6px;");
        connect(b, &QPushButton::clicked, this, [this, val]() {
            spnDeposit->setValue(val);
        });
        return b;
    };

    depLayout->addWidget(makeDepBtn("0 đ", 0));
    depLayout->addWidget(makeDepBtn("50.000 đ", 50000));
    depLayout->addWidget(makeDepBtn("100.000 đ", 100000));
    depLayout->addWidget(makeDepBtn("200.000 đ", 200000));

    formGrid->addWidget(new QLabel("Khách hàng*:"), 0, 0);
    formGrid->addWidget(txtCustomer, 0, 1);
    formGrid->addWidget(new QLabel("Số điện thoại*:"), 1, 0);
    formGrid->addWidget(txtPhone, 1, 1);
    formGrid->addWidget(new QLabel("Giờ bắt đầu:"), 2, 0);
    formGrid->addWidget(timeStart, 2, 1);
    formGrid->addWidget(new QLabel("Giờ kết thúc:"), 3, 0);
    formGrid->addWidget(timeEnd, 3, 1);
    formGrid->addWidget(new QLabel("Thời lượng nhanh:"), 4, 0);
    formGrid->addWidget(durContainer, 4, 1);
    formGrid->addWidget(new QLabel("Tiền cọc (VNĐ):"), 5, 0);
    formGrid->addWidget(spnDeposit, 5, 1);
    formGrid->addWidget(new QLabel("Cọc nhanh:"), 6, 0);
    formGrid->addWidget(depContainer, 6, 1);

    formLayout->addLayout(formGrid);

    btnCreate = new QPushButton("⚡ XÁC NHẬN ĐẶT SÂN", grpCreate);
    btnCreate->setCursor(Qt::PointingHandCursor);
    btnCreate->setFixedHeight(42);
    btnCreate->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #059669, stop:1 #10b981);"
        "  color: white;"
        "  font-size: 14px;"
        "  font-weight: 800;"
        "  letter-spacing: 0.5px;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #047857, stop:1 #059669);"
        "}"
    );
    formLayout->addWidget(btnCreate);
    formLayout->addStretch();

    bodyLayout->addWidget(grpCreate, 2);

    // --- Cột Phải: Lịch Trong Ngày & Hủy Đơn ---
    auto* rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(12);

    auto* grpSchedule = new QGroupBox("📋 Lịch Đặt Sân Trong Ngày (Cập nhật trực tiếp)", this);
    auto* schedLayout = new QVBoxLayout(grpSchedule);
    schedLayout->setContentsMargins(12, 12, 12, 12);

    tableBookings = new QTableWidget(0, 7);
    tableBookings->setHorizontalHeaderLabels({"Mã Đơn", "Ngày", "Khách Hàng", "SĐT", "Khung Giờ", "Tiền Cọc", "Trạng Thái"});
    tableBookings->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableBookings->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableBookings->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableBookings->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableBookings->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableBookings->setSelectionMode(QAbstractItemView::SingleSelection);
    tableBookings->setAlternatingRowColors(true);
    schedLayout->addWidget(tableBookings);
    rightLayout->addWidget(grpSchedule, 4);

    auto* grpCancel = new QGroupBox("🗑️ Hủy Đơn Đặt Sân", this);
    auto* cancelLayout = new QHBoxLayout(grpCancel);
    cancelLayout->setContentsMargins(12, 12, 12, 12);
    cancelLayout->setSpacing(10);

    cboCancelBooking = new QComboBox();
    btnCancel = new QPushButton("🗑️ Xác Nhận Hủy Đơn");
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setStyleSheet(
        "QPushButton { background-color: #ef4444; color: white; font-weight: bold; padding: 9px 16px; border-radius: 8px; }"
        "QPushButton:hover { background-color: #dc2626; }"
        "QPushButton:disabled { background-color: #cbd5e1; color: #94a3b8; }"
    );

    cancelLayout->addWidget(new QLabel("Chọn đơn:"));
    cancelLayout->addWidget(cboCancelBooking, 3);
    cancelLayout->addWidget(btnCancel, 1);
    rightLayout->addWidget(grpCancel, 1);

    bodyLayout->addLayout(rightLayout, 3);
    mainLayout->addLayout(bodyLayout);

    // Kết nối sự kiện
    connect(btnToday, &QPushButton::clicked, this, [this]() {
        dateEdit->setDate(QDate::currentDate());
    });
    connect(btnTomorrow, &QPushButton::clicked, this, [this]() {
        dateEdit->setDate(QDate::currentDate().addDays(1));
    });

    connect(dateEdit, &QDateEdit::dateChanged, this, &TabBooking::refreshBookings);
    connect(cboField, &QComboBox::currentIndexChanged, this, &TabBooking::refreshBookings);
    connect(btnCreate, &QPushButton::clicked, this, &TabBooking::onCreateBooking);
    connect(btnCancel, &QPushButton::clicked, this, &TabBooking::onCancelBooking);
}
