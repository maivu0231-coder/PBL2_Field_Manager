#include "tab_booking.h"

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
        QString text = QString("%1 (%2) — %L3 đ/h [%4]")
                       .arg(f.name, f.field_id)
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
    bool isMaint = cboField->currentText().contains("Bảo trì", Qt::CaseInsensitive);
    lblFieldNotice->setVisible(isMaint);
    btnCreate->setEnabled(!isMaint && !fieldId.isEmpty());

    auto bookings = manager->listBookings(d, fieldId);
    tableBookings->setRowCount(static_cast<int>(bookings.size()));
    cboCancelBooking->clear();

    for (int i = 0; i < static_cast<int>(bookings.size()); ++i) {
        const auto& b = bookings[i];
        tableBookings->setItem(i, 0, new QTableWidgetItem(b.booking_id));
        tableBookings->setItem(i, 1, new QTableWidgetItem(b.customer_name));
        tableBookings->setItem(i, 2, new QTableWidgetItem(b.phone));
        tableBookings->setItem(i, 3, new QTableWidgetItem(b.start_time + " - " + b.end_time));
        tableBookings->setItem(i, 4, new QTableWidgetItem(QString("%L1 đ").arg(b.deposit, 0, 'f', 0)));
        tableBookings->setItem(i, 5, new QTableWidgetItem(b.status));

        cboCancelBooking->addItem(
            QString("%1 · %2 (%3 - %4)").arg(b.booking_id, b.customer_name, b.start_time, b.end_time),
            b.booking_id
        );
    }
    btnCancel->setEnabled(!bookings.empty());
}

void TabBooking::onCreateBooking() {
    try {
        Bookings created = manager->createBooking(
            cboField->currentData().toString(),
            dateEdit->date(),
            txtCustomer->text(),
            txtPhone->text(),
            timeStart->time(),
            timeEnd->time(),
            spnDeposit->value()
        );
        txtCustomer->clear();
        txtPhone->clear();
        spnDeposit->setValue(0);
        QMessageBox::information(this, "Đặt sân thành công",
            QString("Đã tạo đơn %1 thành công (%2).").arg(created.booking_id, created.status));
        refreshBookings();
        emit bookingChanged();
    } catch (const BookingError& err) {
        QMessageBox::warning(this, "Không thể đặt sân", err.message());
    }
}

void TabBooking::onCancelBooking() {
    QString bookingId = cboCancelBooking->currentData().toString();
    if (bookingId.isEmpty()) return;
    try {
        Bookings cancelled = manager->cancelBooking(bookingId);
        QMessageBox::information(this, "Đã hủy đơn",
            QString("Đã hủy đơn %1 thành công.").arg(cancelled.booking_id));
        refreshBookings();
        emit bookingChanged();
    } catch (const BookingError& err) {
        QMessageBox::warning(this, "Lỗi hủy đơn", err.message());
    }
}

void TabBooking::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);

    // 1. Chọn Ngày & Sân
    auto* grpTop = new QGroupBox("📆 Chọn Ngày & Sân Bóng");
    auto* topLayout = new QHBoxLayout(grpTop);
    dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->setCalendarPopup(true);
    cboField = new QComboBox();
    topLayout->addWidget(new QLabel("Ngày đặt:"));
    topLayout->addWidget(dateEdit, 1);
    topLayout->addWidget(new QLabel("Chọn sân:"));
    topLayout->addWidget(cboField, 3);
    mainLayout->addWidget(grpTop);

    lblFieldNotice = new QLabel("⛔ Sân này đang Bảo trì. Không thể tạo đơn đặt sân mới!");
    lblFieldNotice->setStyleSheet("color: #dc2626; font-weight: bold; padding: 6px;");
    lblFieldNotice->setVisible(false);
    mainLayout->addWidget(lblFieldNotice);

    // 2. Chia 2 cột: Form Đặt Sân (Trái) & Bảng Lịch + Hủy (Phải)
    auto* bodyLayout = new QHBoxLayout();

    // Cột Trái: Tạo đơn
    auto* grpCreate = new QGroupBox("➕ Tạo Đơn Đặt Sân Mới (06:00 - 22:00)");
    auto* formGrid = new QGridLayout(grpCreate);
    txtCustomer = new QLineEdit();
    txtCustomer->setPlaceholderText("VD: Nguyễn Văn A");
    txtPhone = new QLineEdit();
    txtPhone->setPlaceholderText("VD: 0905123456");
    timeStart = new QTimeEdit(QTime(17, 0));
    timeStart->setDisplayFormat("HH:mm");
    timeEnd = new QTimeEdit(QTime(18, 30));
    timeEnd->setDisplayFormat("HH:mm");
    spnDeposit = new QSpinBox();
    spnDeposit->setRange(0, 50000000);
    spnDeposit->setSingleStep(50000);
    btnCreate = new QPushButton("✅ Xác Nhận Đặt Sân");

    formGrid->addWidget(new QLabel("Tên khách hàng*:"), 0, 0);
    formGrid->addWidget(txtCustomer, 0, 1);
    formGrid->addWidget(new QLabel("Số điện thoại*:"), 1, 0);
    formGrid->addWidget(txtPhone, 1, 1);
    formGrid->addWidget(new QLabel("Giờ bắt đầu:"), 2, 0);
    formGrid->addWidget(timeStart, 2, 1);
    formGrid->addWidget(new QLabel("Giờ kết thúc:"), 3, 0);
    formGrid->addWidget(timeEnd, 3, 1);
    formGrid->addWidget(new QLabel("Tiền cọc (VNĐ):"), 4, 0);
    formGrid->addWidget(spnDeposit, 4, 1);
    formGrid->addWidget(btnCreate, 5, 0, 1, 2);
    bodyLayout->addWidget(grpCreate, 2);

    // Cột Phải: Lịch trong ngày & Hủy đơn
    auto* rightLayout = new QVBoxLayout();
    auto* grpSchedule = new QGroupBox("📋 Lịch Đặt Trong Ngày");
    auto* schedLayout = new QVBoxLayout(grpSchedule);
    tableBookings = new QTableWidget(0, 6);
    tableBookings->setHorizontalHeaderLabels({"Mã Đơn", "Tên Khách", "SĐT", "Khung Giờ", "Tiền Cọc", "Trạng Thái"});
    tableBookings->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableBookings->setEditTriggers(QAbstractItemView::NoEditTriggers);
    schedLayout->addWidget(tableBookings);
    rightLayout->addWidget(grpSchedule, 3);

    auto* grpCancel = new QGroupBox("🗑️ Hủy Đơn Đặt Sân");
    auto* cancelLayout = new QHBoxLayout(grpCancel);
    cboCancelBooking = new QComboBox();
    btnCancel = new QPushButton("🗑️ Hủy Đơn Này");
    cancelLayout->addWidget(cboCancelBooking, 3);
    cancelLayout->addWidget(btnCancel, 1);
    rightLayout->addWidget(grpCancel, 1);

    bodyLayout->addLayout(rightLayout, 3);
    mainLayout->addLayout(bodyLayout);

    connect(dateEdit, &QDateEdit::dateChanged, this, &TabBooking::refreshBookings);
    connect(cboField, &QComboBox::currentIndexChanged, this, &TabBooking::refreshBookings);
    connect(btnCreate, &QPushButton::clicked, this, &TabBooking::onCreateBooking);
    connect(btnCancel, &QPushButton::clicked, this, &TabBooking::onCancelBooking);
}