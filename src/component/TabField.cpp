#include "tabfield.h"

TabField::TabField(FieldService* srv, QWidget* parent)
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

    lblTotal->setText(QString("🏟️ Tổng số sân: %1").arg(allFields.size()));
    lblAvail->setText(QString("🟢 Đang trống: %1").arg(avail));
    lblInUse->setText(QString("🔵 Có khách: %1").arg(inUse));
    lblMaint->setText(QString("🔴 Bảo trì: %1").arg(maint));

    // filter table based on current filter settings
    auto filtered = service->filter(
        cboFilterStatus->currentText(),
        cboFilterType->currentText(),
        txtSearch->text()
    );

    tableFields->setRowCount(static_cast<int>(filtered.size()));
    for (int i = 0; i < static_cast<int>(filtered.size()); ++i) {
        const auto& f = filtered[i];
        tableFields->setItem(i, 0, new QTableWidgetItem(f.field_id));
        tableFields->setItem(i, 1, new QTableWidgetItem(f.name));
        tableFields->setItem(i, 2, new QTableWidgetItem(f.field_type));
        tableFields->setItem(i, 3, new QTableWidgetItem(QString("%L1 đ").arg(f.price_per_hour, 0, 'f', 0)));
        tableFields->setItem(i, 4, new QTableWidgetItem(f.status));
        tableFields->setItem(i, 5, new QTableWidgetItem(f.updated_at));
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
        txtAddCode->text(),
        txtAddName->text(),
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
    auto [ok, msg] = service->remove(code);
    if (ok) {
        QMessageBox::information(this, "Thành công", msg);
        refreshData();
        emit dataChanged();
    } else {
        QMessageBox::warning(this, "Cảnh báo", msg);
    }
}

void TabField::onResetDefault() {
    auto [ok, msg] = service->resetToDefault();
    if (ok) {
        QMessageBox::information(this, "Khôi phục", msg);
        refreshData();
        emit dataChanged();
    }
}

void TabField::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);

    // 1. Thanh KPI
    auto* kpiLayout = new QHBoxLayout();
    lblTotal = new QLabel();
    lblAvail = new QLabel();
    lblInUse = new QLabel();
    lblMaint = new QLabel();
    for (auto* lbl : {lblTotal, lblAvail, lblInUse, lblMaint}) {
        lbl->setStyleSheet("background: #f8fafc; border: 1px solid #cbd5e1; border-radius: 8px; padding: 10px; font-weight: bold; font-size: 14px;");
        kpiLayout->addWidget(lbl);
    }
    mainLayout->addLayout(kpiLayout);

    // 2. Bộ lọc & Bảng danh sách
    auto* grpList = new QGroupBox("📋 Danh sách & Bộ lọc sân bóng");
    auto* listLayout = new QVBoxLayout(grpList);

    auto* filterLayout = new QHBoxLayout();
    txtSearch = new QLineEdit();
    txtSearch->setPlaceholderText("🔍 Tìm theo tên hoặc mã sân (hỗ trợ tiếng Việt không dấu)...");
    cboFilterStatus = new QComboBox();
    cboFilterStatus->addItem("Tất cả");
    cboFilterStatus->addItems(FieldStatus::all());
    cboFilterType = new QComboBox();
    cboFilterType->addItem("Tất cả");
    cboFilterType->addItems(FieldType::all());

    filterLayout->addWidget(txtSearch, 3);
    filterLayout->addWidget(new QLabel("Trạng thái:"));
    filterLayout->addWidget(cboFilterStatus, 1);
    filterLayout->addWidget(new QLabel("Loại sân:"));
    filterLayout->addWidget(cboFilterType, 1);
    listLayout->addLayout(filterLayout);

    tableFields = new QTableWidget(0, 6);
    tableFields->setHorizontalHeaderLabels({"Mã Sân", "Tên Sân", "Loại Sân", "Giá Thuê/Giờ", "Trạng Thái", "Cập Nhật"});
    tableFields->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableFields->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableFields->setSelectionBehavior(QAbstractItemView::SelectRows);
    listLayout->addWidget(tableFields);
    mainLayout->addWidget(grpList, 3);

    // 3. Khu vực thao tác Quản lý (Chia 3 cột: Sửa - Thêm - Xóa/Khôi phục)
    auto* bottomLayout = new QHBoxLayout();

    // Cột Sửa
    auto* grpEdit = new QGroupBox("✏️ Cập Nhật Thông Tin Sân");
    auto* editGrid = new QGridLayout(grpEdit);
    cboEditCode = new QComboBox();
    txtEditName = new QLineEdit();
    cboEditType = new QComboBox();
    cboEditType->addItems(FieldType::all());
    spnEditPrice = new QSpinBox();
    spnEditPrice->setRange(0, 50000000);
    spnEditPrice->setSingleStep(50000);
    cboEditStatus = new QComboBox();
    cboEditStatus->addItems(FieldStatus::all());
    auto* btnUpdate = new QPushButton("💾 Lưu Cập Nhật");

    editGrid->addWidget(new QLabel("Chọn sân:"), 0, 0);
    editGrid->addWidget(cboEditCode, 0, 1);
    editGrid->addWidget(new QLabel("Tên sân:"), 1, 0);
    editGrid->addWidget(txtEditName, 1, 1);
    editGrid->addWidget(new QLabel("Loại sân:"), 2, 0);
    editGrid->addWidget(cboEditType, 2, 1);
    editGrid->addWidget(new QLabel("Giá/giờ (VNĐ):"), 3, 0);
    editGrid->addWidget(spnEditPrice, 3, 1);
    editGrid->addWidget(new QLabel("Trạng thái:"), 4, 0);
    editGrid->addWidget(cboEditStatus, 4, 1);
    editGrid->addWidget(btnUpdate, 5, 0, 1, 2);
    bottomLayout->addWidget(grpEdit, 1);

    // Cột Thêm
    auto* grpAdd = new QGroupBox("➕ Thêm Sân Bóng Mới");
    auto* addGrid = new QGridLayout(grpAdd);
    txtAddCode = new QLineEdit();
    txtAddCode->setPlaceholderText("VD: S05");
    txtAddName = new QLineEdit();
    txtAddName->setPlaceholderText("VD: Sân 5C");
    cboAddType = new QComboBox();
    cboAddType->addItems(FieldType::all());
    spnAddPrice = new QSpinBox();
    spnAddPrice->setRange(0, 50000000);
    spnAddPrice->setSingleStep(50000);
    spnAddPrice->setValue(200000);
    cboAddStatus = new QComboBox();
    cboAddStatus->addItems(FieldStatus::all());
    auto* btnAdd = new QPushButton("➕ Thêm Sân Mới");

    addGrid->addWidget(new QLabel("Mã sân*:"), 0, 0);
    addGrid->addWidget(txtAddCode, 0, 1);
    addGrid->addWidget(new QLabel("Tên sân*:"), 1, 0);
    addGrid->addWidget(txtAddName, 1, 1);
    addGrid->addWidget(new QLabel("Loại sân:"), 2, 0);
    addGrid->addWidget(cboAddType, 2, 1);
    addGrid->addWidget(new QLabel("Giá/giờ (VNĐ):"), 3, 0);
    addGrid->addWidget(spnAddPrice, 3, 1);
    addGrid->addWidget(new QLabel("Trạng thái:"), 4, 0);
    addGrid->addWidget(cboAddStatus, 4, 1);
    addGrid->addWidget(btnAdd, 5, 0, 1, 2);
    bottomLayout->addWidget(grpAdd, 1);

    // Cột Xóa & Reset
    auto* grpDel = new QGroupBox("🗑️ Xóa Sân & Khôi Phục Mẫu");
    auto* delLayout = new QVBoxLayout(grpDel);
    cboDelCode = new QComboBox();
    auto* btnDel = new QPushButton("🗑️ Xác Nhận Xóa Sân");
    auto* btnReset = new QPushButton("⚙️ Khôi Phục Dữ Liệu Gốc");
    delLayout->addWidget(new QLabel("Chọn sân cần xóa:"));
    delLayout->addWidget(cboDelCode);
    delLayout->addWidget(btnDel);
    delLayout->addStretch();
    delLayout->addWidget(new QLabel("Khôi phục 4 sân mẫu ban đầu:"));
    delLayout->addWidget(btnReset);
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
    connect(btnReset, &QPushButton::clicked, this, &TabField::onResetDefault);
}