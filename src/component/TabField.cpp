#include "TabField.h"
#include <QFrame>

TabField::TabField(FieldManager* srv, QWidget* parent)
    : QWidget(parent), service(srv) {
    setupUi();
    refreshData();
}

void TabField::refreshData() {
    auto allFields = service->getAll();
    int avail = 0, inUse = 0, maint = 0;
    for (const auto& f : allFields) {
        if (f.status == FieldStatus::AVAILABLE) avail++;
        else if (f.status == FieldStatus::IN_USE) inUse++;
        else maint++;
    }

    // Cập nhật 4 thẻ KPI chỉ số trực quan
    lblTotal->setText(
        QString("<div style='margin-bottom: 2px;'><span style='font-size: 11px; font-weight: bold; color: #64748b; letter-spacing: 0.5px;'>TỔNG SỐ SÂN</span></div>"
                "<div><span style='font-size: 24px; font-weight: 800; color: #1e3a8a;'>%1</span> <span style='font-size: 13px; font-weight: 600; color: #64748b;'>sân</span></div>"
                "<div style='margin-top: 2px;'><span style='font-size: 11px; color: #94a3b8;'>Toàn bộ cụm sân bóng</span></div>")
        .arg(allFields.size())
    );

    lblAvail->setText(
        QString("<div style='margin-bottom: 2px;'><span style='font-size: 11px; font-weight: bold; color: #059669; letter-spacing: 0.5px;'>SÂN ĐANG TRỐNG</span></div>"
                "<div><span style='font-size: 24px; font-weight: 800; color: #10b981;'>%1</span> <span style='font-size: 13px; font-weight: 600; color: #059669;'>sân</span></div>"
                "<div style='margin-top: 2px;'><span style='font-size: 11px; color: #059669;'>Sẵn sàng nhận khách</span></div>")
        .arg(avail)
    );

    lblInUse->setText(
        QString("<div style='margin-bottom: 2px;'><span style='font-size: 11px; font-weight: bold; color: #2563eb; letter-spacing: 0.5px;'>ĐANG CÓ KHÁCH</span></div>"
                "<div><span style='font-size: 24px; font-weight: 800; color: #3b82f6;'>%1</span> <span style='font-size: 13px; font-weight: 600; color: #2563eb;'>sân</span></div>"
                "<div style='margin-top: 2px;'><span style='font-size: 11px; color: #2563eb;'>Đang trong giờ đấu</span></div>")
        .arg(inUse)
    );

    lblMaint->setText(
        QString("<div style='margin-bottom: 2px;'><span style='font-size: 11px; font-weight: bold; color: #dc2626; letter-spacing: 0.5px;'>ĐANG BẢO TRÌ</span></div>"
                "<div><span style='font-size: 24px; font-weight: 800; color: #ef4444;'>%1</span> <span style='font-size: 13px; font-weight: 600; color: #dc2626;'>sân</span></div>"
                "<div style='margin-top: 2px;'><span style='font-size: 11px; color: #dc2626;'>Tạm dừng nhận lịch</span></div>")
        .arg(maint)
    );

    // Lọc dữ liệu theo từ khóa tìm kiếm và combobox
    auto filtered = service->filter(
        cboFilterStatus->currentText(),
        cboFilterType->currentText(),
        txtSearch->text()
    );

    tableFields->setRowCount(static_cast<int>(filtered.size()));
    for (int i = 0; i < static_cast<int>(filtered.size()); ++i) {
        const auto& f = filtered[i];

        // Mã sân
        auto* itemCode = new QTableWidgetItem(f.field_id);
        itemCode->setTextAlignment(Qt::AlignCenter);
        QFont fBold = itemCode->font();
        fBold.setBold(true);
        itemCode->setFont(fBold);

        // Tên sân
        auto* itemName = new QTableWidgetItem("⚽ " + f.name);

        // Loại sân
        auto* itemType = new QTableWidgetItem(f.field_type);
        itemType->setTextAlignment(Qt::AlignCenter);

        // Đơn giá
        auto* itemPrice = new QTableWidgetItem(QString("%L1 đ/h").arg(f.price_per_hour, 0, 'f', 0));
        itemPrice->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        itemPrice->setForeground(QBrush(QColor("#047857")));
        itemPrice->setFont(fBold);

        // Trạng thái với màu sắc chuyên nghiệp
        QString statusText = f.status;
        auto* itemStatus = new QTableWidgetItem(statusText);
        itemStatus->setTextAlignment(Qt::AlignCenter);
        itemStatus->setFont(fBold);

        if (statusText.contains("Trống", Qt::CaseInsensitive) || statusText.contains("Available", Qt::CaseInsensitive)) {
            itemStatus->setText("🟢 " + statusText);
            itemStatus->setForeground(QBrush(QColor("#059669")));
        } else if (statusText.contains("Có khách", Qt::CaseInsensitive) || statusText.contains("In Use", Qt::CaseInsensitive)) {
            itemStatus->setText("🟡 " + statusText);
            itemStatus->setForeground(QBrush(QColor("#d97706")));
        } else {
            itemStatus->setText("🔴 " + statusText);
            itemStatus->setForeground(QBrush(QColor("#dc2626")));
        }

        // Ngày cập nhật
        auto* itemDate = new QTableWidgetItem(f.updated_at);
        itemDate->setTextAlignment(Qt::AlignCenter);
        itemDate->setForeground(QBrush(QColor("#64748b")));

        tableFields->setItem(i, 0, itemCode);
        tableFields->setItem(i, 1, itemName);
        tableFields->setItem(i, 2, itemType);
        tableFields->setItem(i, 3, itemPrice);
        tableFields->setItem(i, 4, itemStatus);
        tableFields->setItem(i, 5, itemDate);
    }

    // Cập nhật danh sách mã sân trong combobox Sửa & Xóa
    QString currentEdit = cboEditCode->currentData().toString();
    cboEditCode->blockSignals(true);
    cboEditCode->clear();
    cboDelCode->clear();
    for (const auto& f : allFields) {
        QString label = QString("%1 — %2 (%3)").arg(f.field_id, f.name, f.status);
        cboEditCode->addItem(label, f.field_id);
        cboDelCode->addItem(label, f.field_id);
    }
    int idx = cboEditCode->findData(currentEdit);
    if (idx >= 0) cboEditCode->setCurrentIndex(idx);
    cboEditCode->blockSignals(false);
    loadSelectedFieldToEditForm();
}

void TabField::loadSelectedFieldToEditForm() {
    QString code = cboEditCode->currentData().toString();
    auto fOpt = service->getById(code);
    if (!fOpt.has_value()) return;
    txtEditName->setText(fOpt->name);
    cboEditType->setCurrentText(fOpt->field_type);
    spnEditPrice->setValue(static_cast<int>(fOpt->price_per_hour));
    cboEditStatus->setCurrentText(fOpt->status);
}

void TabField::onUpdateField() {
    QString code = cboEditCode->currentData().toString();
    auto [ok, msg] = service->update(
        code,
        txtEditName->text(),
        cboEditType->currentText(),
        spnEditPrice->value(),
        cboEditStatus->currentText()
    );
    if (ok) {
        QMessageBox::information(this, "Thành công", msg);
        refreshData();
        emit dataChanged();
    } else {
        QMessageBox::warning(this, "Lỗi cập nhật", msg);
    }
}

void TabField::onAddField() {
    auto [ok, msg] = service->add(
        txtAddCode->text().trimmed(),
        txtAddName->text().trimmed(),
        cboAddType->currentText(),
        spnAddPrice->value(),
        cboAddStatus->currentText()
    );
    if (ok) {
        txtAddCode->clear();
        txtAddName->clear();
        QMessageBox::information(this, "Thành công", msg);
        refreshData();
        emit dataChanged();
    } else {
        QMessageBox::warning(this, "Lỗi thêm sân", msg);
    }
}

void TabField::onDeleteField() {
    QString code = cboDelCode->currentData().toString();
    if (code.isEmpty()) return;

    auto reply = QMessageBox::question(
        this, "Xác nhận xóa sân",
        QString("Bạn có chắc chắn muốn xóa sân %1 khỏi hệ thống không?\nHành động này không thể hoàn tác!").arg(code),
        QMessageBox::Yes | QMessageBox::No
    );
    if (reply != QMessageBox::Yes) return;

    auto [ok, msg] = service->remove(code);
    if (ok) {
        QMessageBox::information(this, "Thành công", msg);
        refreshData();
        emit dataChanged();
    } else {
        QMessageBox::warning(this, "Cảnh báo", msg);
    }
}

void TabField::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(14);

    // ================= 1. THANH KPI CHỈ SỐ =================
    auto* kpiLayout = new QHBoxLayout();
    kpiLayout->setSpacing(12);

    auto createKpiCard = [](QLabel*& lbl, const QString& borderHex, const QString& bgHex) {
        auto* card = new QFrame();
        card->setStyleSheet(QString(
            "QFrame {"
            "  background: %1;"
            "  border: 1px solid #e2e8f0;"
            "  border-left: 5px solid %2;"
            "  border-radius: 10px;"
            "}"
        ).arg(bgHex, borderHex));
        auto* l = new QVBoxLayout(card);
        l->setContentsMargins(14, 10, 14, 10);
        lbl = new QLabel(card);
        lbl->setTextFormat(Qt::RichText);
        lbl->setStyleSheet("border: none; background: transparent;");
        l->addWidget(lbl);
        return card;
    };

    kpiLayout->addWidget(createKpiCard(lblTotal, "#2563eb", "#ffffff"), 1);
    kpiLayout->addWidget(createKpiCard(lblAvail, "#10b981", "#ffffff"), 1);
    kpiLayout->addWidget(createKpiCard(lblInUse, "#f59e0b", "#ffffff"), 1);
    kpiLayout->addWidget(createKpiCard(lblMaint, "#ef4444", "#ffffff"), 1);

    mainLayout->addLayout(kpiLayout);

    // ================= 2. BỘ LỌC & BẢNG SÂN BÓNG =================
    auto* grpList = new QGroupBox("📋 Danh Sách & Tra Cứu Sân Bóng", this);
    auto* listLayout = new QVBoxLayout(grpList);
    listLayout->setContentsMargins(12, 14, 12, 12);
    listLayout->setSpacing(10);

    auto* filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(10);

    txtSearch = new QLineEdit();
    txtSearch->setPlaceholderText("🔍 Tìm theo tên hoặc mã sân (hỗ trợ tiếng Việt có/không dấu)...");
    txtSearch->setClearButtonEnabled(true);

    cboFilterStatus = new QComboBox();
    cboFilterStatus->addItem("Tất cả trạng thái");
    cboFilterStatus->addItems(FieldStatus::all());
    cboFilterStatus->setMinimumWidth(150);

    cboFilterType = new QComboBox();
    cboFilterType->addItem("Tất cả loại sân");
    cboFilterType->addItems(FieldType::all());
    cboFilterType->setMinimumWidth(150);

    filterLayout->addWidget(txtSearch, 5);
    filterLayout->addWidget(new QLabel("Trạng thái:"));
    filterLayout->addWidget(cboFilterStatus, 2);
    filterLayout->addWidget(new QLabel("Loại sân:"));
    filterLayout->addWidget(cboFilterType, 2);
    listLayout->addLayout(filterLayout);

    tableFields = new QTableWidget(0, 6);
    tableFields->setHorizontalHeaderLabels({"Mã Sân", "Tên Sân Bóng", "Phân Loại", "Giá Thuê / Giờ", "Trạng Thái", "Cập Nhật"});
    tableFields->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableFields->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableFields->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tableFields->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableFields->setAlternatingRowColors(true);
    tableFields->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableFields->setSelectionMode(QAbstractItemView::SingleSelection);
    listLayout->addWidget(tableFields);
    mainLayout->addWidget(grpList, 3);

    // ================= 3. THAO TÁC QUẢN LÝ: SỬA, THÊM, XÓA =================
    auto* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(12);

    // --- Cột 1: Cập Nhật Sân ---
    auto* grpEdit = new QGroupBox("✏️ Cập Nhật Thông Tin Sân");
    auto* editGrid = new QGridLayout(grpEdit);
    editGrid->setSpacing(8);

    cboEditCode = new QComboBox();
    txtEditName = new QLineEdit();
    txtEditName->setPlaceholderText("Tên sân...");
    cboEditType = new QComboBox();
    cboEditType->addItems(FieldType::all());
    spnEditPrice = new QSpinBox();
    spnEditPrice->setRange(0, 50000000);
    spnEditPrice->setSingleStep(50000);
    cboEditStatus = new QComboBox();
    cboEditStatus->addItems(FieldStatus::all());

    auto* btnUpdate = new QPushButton("💾 Lưu Cập Nhật");
    btnUpdate->setCursor(Qt::PointingHandCursor);
    btnUpdate->setStyleSheet(
        "QPushButton { background-color: #2563eb; color: white; font-weight: bold; padding: 9px; border-radius: 8px; }"
        "QPushButton:hover { background-color: #1d4ed8; }"
    );

    editGrid->addWidget(new QLabel("Chọn sân:"), 0, 0);
    editGrid->addWidget(cboEditCode, 0, 1);
    editGrid->addWidget(new QLabel("Tên sân:"), 1, 0);
    editGrid->addWidget(txtEditName, 1, 1);
    editGrid->addWidget(new QLabel("Loại sân:"), 2, 0);
    editGrid->addWidget(cboEditType, 2, 1);
    editGrid->addWidget(new QLabel("Giá thuê:"), 3, 0);
    editGrid->addWidget(spnEditPrice, 3, 1);
    editGrid->addWidget(new QLabel("Trạng thái:"), 4, 0);
    editGrid->addWidget(cboEditStatus, 4, 1);
    editGrid->addWidget(btnUpdate, 5, 0, 1, 2);
    bottomLayout->addWidget(grpEdit, 1);

    // --- Cột 2: Thêm Sân Mới ---
    auto* grpAdd = new QGroupBox("➕ Thêm Sân Bóng Mới");
    auto* addGrid = new QGridLayout(grpAdd);
    addGrid->setSpacing(8);

    txtAddCode = new QLineEdit();
    txtAddCode->setPlaceholderText("VD: S05");
    txtAddName = new QLineEdit();
    txtAddName->setPlaceholderText("VD: Sân 5C Cỏ Nhân Tạo");
    cboAddType = new QComboBox();
    cboAddType->addItems(FieldType::all());
    spnAddPrice = new QSpinBox();
    spnAddPrice->setRange(0, 50000000);
    spnAddPrice->setSingleStep(50000);
    spnAddPrice->setValue(200000);
    cboAddStatus = new QComboBox();
    cboAddStatus->addItems(FieldStatus::all());

    auto* btnAdd = new QPushButton("➕ Thêm Sân Mới");
    btnAdd->setCursor(Qt::PointingHandCursor);
    btnAdd->setStyleSheet(
        "QPushButton { background-color: #059669; color: white; font-weight: bold; padding: 9px; border-radius: 8px; }"
        "QPushButton:hover { background-color: #047857; }"
    );

    addGrid->addWidget(new QLabel("Mã sân*:"), 0, 0);
    addGrid->addWidget(txtAddCode, 0, 1);
    addGrid->addWidget(new QLabel("Tên sân*:"), 1, 0);
    addGrid->addWidget(txtAddName, 1, 1);
    addGrid->addWidget(new QLabel("Loại sân:"), 2, 0);
    addGrid->addWidget(cboAddType, 2, 1);
    addGrid->addWidget(new QLabel("Giá thuê:"), 3, 0);
    addGrid->addWidget(spnAddPrice, 3, 1);
    addGrid->addWidget(new QLabel("Trạng thái:"), 4, 0);
    addGrid->addWidget(cboAddStatus, 4, 1);
    addGrid->addWidget(btnAdd, 5, 0, 1, 2);
    bottomLayout->addWidget(grpAdd, 1);

    // --- Cột 3: Xóa Sân Khỏi Hệ Thống ---
    auto* grpDel = new QGroupBox("🗑️ Xóa Sân Khỏi Hệ Thống");
    auto* delLayout = new QVBoxLayout(grpDel);
    delLayout->setSpacing(10);

    cboDelCode = new QComboBox();

    auto* lblDelNotice = new QLabel(
        "⚠️ <i>Lưu ý: Chỉ xóa sân khi sân không còn liên kết với đơn đặt đang hoạt động.</i>", grpDel
    );
    lblDelNotice->setWordWrap(true);
    lblDelNotice->setStyleSheet("color: #64748b; font-size: 11px;");

    auto* btnDel = new QPushButton("🗑️ Xác Nhận Xóa Sân Này");
    btnDel->setCursor(Qt::PointingHandCursor);
    btnDel->setStyleSheet(
        "QPushButton { background-color: #dc2626; color: white; font-weight: bold; padding: 10px; border-radius: 8px; }"
        "QPushButton:hover { background-color: #b91c1c; }"
    );

    delLayout->addWidget(new QLabel("Chọn sân cần xóa:"));
    delLayout->addWidget(cboDelCode);
    delLayout->addWidget(lblDelNotice);
    delLayout->addSpacing(8);
    delLayout->addWidget(btnDel);
    delLayout->addStretch();
    bottomLayout->addWidget(grpDel, 1);

    mainLayout->addLayout(bottomLayout, 2);

    // Kết nối sự kiện
    connect(txtSearch, &QLineEdit::textChanged, this, &TabField::refreshData);
    connect(cboFilterStatus, &QComboBox::currentTextChanged, this, &TabField::refreshData);
    connect(cboFilterType, &QComboBox::currentTextChanged, this, &TabField::refreshData);
    connect(cboEditCode, &QComboBox::currentIndexChanged, this, &TabField::loadSelectedFieldToEditForm);
    connect(btnUpdate, &QPushButton::clicked, this, &TabField::onUpdateField);
    connect(btnAdd, &QPushButton::clicked, this, &TabField::onAddField);
    connect(btnDel, &QPushButton::clicked, this, &TabField::onDeleteField);

    // Khi người dùng click dòng trong bảng thì chọn luôn sân đó trong cboEditCode
    connect(tableFields, &QTableWidget::cellClicked, this, [this](int row, int /*col*/) {
        auto* item = tableFields->item(row, 0);
        if (item) {
            int idx = cboEditCode->findData(item->text());
            if (idx >= 0) cboEditCode->setCurrentIndex(idx);
        }
    });
}
