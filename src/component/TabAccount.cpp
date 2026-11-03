#include "TabAccount.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFrame>

TabAccount::TabAccount(AuthManager* authManager, QWidget* parent)
    : QWidget(parent), m_authManager(authManager) {
    setupUi();
    refreshData();
}

void TabAccount::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(14);

    // ================= 1. BẢNG DANH SÁCH TÀI KHOẢN =================
    auto* grpList = new QGroupBox("👥 Danh Sách Tài Khoản Hệ Thống (Phân quyền & Kiểm soát truy cập)", this);
    auto* listLayout = new QVBoxLayout(grpList);
    listLayout->setContentsMargins(12, 14, 12, 12);

    tableUsers = new QTableWidget(0, 3);
    tableUsers->setHorizontalHeaderLabels({"👤 Tên Đăng Nhập", "🔒 Mật Khẩu", "🛡️ Vai Trò / Phân Quyền"});
    tableUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableUsers->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableUsers->setSelectionMode(QAbstractItemView::SingleSelection);
    tableUsers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableUsers->setAlternatingRowColors(true);
    listLayout->addWidget(tableUsers);

    mainLayout->addWidget(grpList, 3);

    // ================= 2. CHIA ĐÔI: THÊM & SỬA / XÓA =================
    auto* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(14);

    // --- Form Thêm Tài Khoản ---
    auto* grpAdd = new QGroupBox("➕ Thêm Tài Khoản Mới", this);
    auto* addLayout = new QGridLayout(grpAdd);
    addLayout->setContentsMargins(14, 14, 14, 14);
    addLayout->setSpacing(10);

    txtAddUsername = new QLineEdit();
    txtAddUsername->setPlaceholderText("VD: nhanvien01");

    txtAddPassword = new QLineEdit();
    txtAddPassword->setPlaceholderText("Nhập mật khẩu...");
    txtAddPassword->setEchoMode(QLineEdit::Normal);

    cboAddRole = new QComboBox();
    cboAddRole->addItem("Thu Ngân (Cashier)", "Cashier");
    cboAddRole->addItem("Quản Trị Viên (Admin)", "Admin");

    btnAddUser = new QPushButton("➕ Tạo Tài Khoản Mới");
    btnAddUser->setCursor(Qt::PointingHandCursor);
    btnAddUser->setStyleSheet(
        "QPushButton { background-color: #059669; color: white; font-weight: bold; padding: 10px; border-radius: 8px; }"
        "QPushButton:hover { background-color: #047857; }"
    );

    addLayout->addWidget(new QLabel("Tên đăng nhập*:"), 0, 0);
    addLayout->addWidget(txtAddUsername, 0, 1);
    addLayout->addWidget(new QLabel("Mật khẩu*:"), 1, 0);
    addLayout->addWidget(txtAddPassword, 1, 1);
    addLayout->addWidget(new QLabel("Vai trò:"), 2, 0);
    addLayout->addWidget(cboAddRole, 2, 1);
    addLayout->addWidget(btnAddUser, 3, 0, 1, 2);

    bottomLayout->addWidget(grpAdd, 1);

    // --- Form Sửa / Xóa Tài Khoản ---
    auto* grpEdit = new QGroupBox("⚙️ Đổi Mật Khẩu & Quản Lý Phân Quyền", this);
    auto* editLayout = new QGridLayout(grpEdit);
    editLayout->setContentsMargins(14, 14, 14, 14);
    editLayout->setSpacing(10);

    cboSelectUser = new QComboBox();

    txtUpdatePassword = new QLineEdit();
    txtUpdatePassword->setPlaceholderText("Nhập mật khẩu mới...");
    txtUpdatePassword->setEchoMode(QLineEdit::Normal);

    cboUpdateRole = new QComboBox();
    cboUpdateRole->addItem("Thu Ngân (Cashier)", "Cashier");
    cboUpdateRole->addItem("Quản Trị Viên (Admin)", "Admin");

    auto* actionBtnLayout = new QHBoxLayout();
    actionBtnLayout->setSpacing(10);

    btnUpdateUser = new QPushButton("💾 Lưu Thay Đổi");
    btnUpdateUser->setCursor(Qt::PointingHandCursor);
    btnUpdateUser->setStyleSheet(
        "QPushButton { background-color: #2563eb; color: white; font-weight: bold; padding: 10px; border-radius: 8px; }"
        "QPushButton:hover { background-color: #1d4ed8; }"
    );

    btnDeleteUser = new QPushButton("🗑️ Xóa Tài Khoản");
    btnDeleteUser->setCursor(Qt::PointingHandCursor);
    btnDeleteUser->setStyleSheet(
        "QPushButton { background-color: #dc2626; color: white; font-weight: bold; padding: 10px; border-radius: 8px; }"
        "QPushButton:hover { background-color: #b91c1c; }"
    );

    actionBtnLayout->addWidget(btnUpdateUser);
    actionBtnLayout->addWidget(btnDeleteUser);

    editLayout->addWidget(new QLabel("Chọn tài khoản:"), 0, 0);
    editLayout->addWidget(cboSelectUser, 0, 1);
    editLayout->addWidget(new QLabel("Mật khẩu mới:"), 1, 0);
    editLayout->addWidget(txtUpdatePassword, 1, 1);
    editLayout->addWidget(new QLabel("Vai trò:"), 2, 0);
    editLayout->addWidget(cboUpdateRole, 2, 1);
    editLayout->addLayout(actionBtnLayout, 3, 0, 1, 2);

    bottomLayout->addWidget(grpEdit, 1);

    mainLayout->addLayout(bottomLayout, 2);

    // Kết nối tín hiệu
    connect(btnAddUser, &QPushButton::clicked, this, &TabAccount::onAddUser);
    connect(btnUpdateUser, &QPushButton::clicked, this, &TabAccount::onUpdateUser);
    connect(btnDeleteUser, &QPushButton::clicked, this, &TabAccount::onDeleteUser);
    connect(tableUsers, &QTableWidget::cellClicked, this, &TabAccount::onUserSelectedFromTable);
    connect(cboSelectUser, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int idx) {
        if (idx < 0) return;
        QString uname = cboSelectUser->itemData(idx).toString();
        for (const auto& u : m_authManager->getAllUsers()) {
            if (u.getUsername().compare(uname, Qt::CaseInsensitive) == 0) {
                txtUpdatePassword->setText(u.getPassword());
                int roleIdx = cboUpdateRole->findData(u.getRole());
                if (roleIdx >= 0) cboUpdateRole->setCurrentIndex(roleIdx);
                break;
            }
        }
    });
}

void TabAccount::refreshData() {
    auto users = m_authManager->getAllUsers();

    tableUsers->setRowCount(users.size());
    cboSelectUser->blockSignals(true);
    cboSelectUser->clear();

    for (int i = 0; i < users.size(); ++i) {
        const auto& u = users[i];

        // Username
        auto* userItem = new QTableWidgetItem(u.getUsername());
        QFont fBold = userItem->font();
        fBold.setBold(true);
        userItem->setFont(fBold);

        // Password
        auto* passItem = new QTableWidgetItem(u.getPassword());
        passItem->setFont(QFont("Consolas", 10, QFont::Bold));
        passItem->setForeground(QBrush(QColor("#1e3a8a")));
        
        // Role with badge styling
        bool isAdmin = (u.getRole().compare("Admin", Qt::CaseInsensitive) == 0);
        auto* roleItem = new QTableWidgetItem(isAdmin ? "👑 Quản Trị Viên (Admin)" : "💳 Thu Ngân (Cashier)");
        roleItem->setFont(fBold);
        roleItem->setForeground(QBrush(QColor(isAdmin ? "#b45309" : "#047857")));

        tableUsers->setItem(i, 0, userItem);
        tableUsers->setItem(i, 1, passItem);
        tableUsers->setItem(i, 2, roleItem);

        cboSelectUser->addItem(QString("%1 (%2)").arg(u.getUsername(), u.getRole()), u.getUsername());
    }
    cboSelectUser->blockSignals(false);

    if (cboSelectUser->count() > 0) {
        QString firstUname = cboSelectUser->itemData(0).toString();
        for (const auto& u : users) {
            if (u.getUsername().compare(firstUname, Qt::CaseInsensitive) == 0) {
                txtUpdatePassword->setText(u.getPassword());
                break;
            }
        }
    }
}

void TabAccount::onUserSelectedFromTable(int row, int /*column*/) {
    if (row < 0 || row >= tableUsers->rowCount()) return;
    QString username = tableUsers->item(row, 0)->text();
    int idx = cboSelectUser->findData(username);
    if (idx >= 0) {
        cboSelectUser->setCurrentIndex(idx);
    }
}

void TabAccount::onAddUser() {
    QString uname = txtAddUsername->text().trimmed();
    QString pass = txtAddPassword->text().trimmed();
    QString role = cboAddRole->currentData().toString();

    if (uname.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập tên đăng nhập và mật khẩu!");
        return;
    }

    if (m_authManager->addUser(uname, pass, role)) {
        QMessageBox::information(this, "Thành công 🎉", QString("Đã tạo tài khoản '%1' thành công!").arg(uname));
        txtAddUsername->clear();
        txtAddPassword->clear();
        refreshData();
    } else {
        QMessageBox::warning(this, "Lỗi tạo tài khoản", "Tên đăng nhập đã tồn tại trên hệ thống!");
    }
}

void TabAccount::onUpdateUser() {
    QString uname = cboSelectUser->currentData().toString();
    if (uname.isEmpty()) return;

    QString newPass = txtUpdatePassword->text().trimmed();
    QString newRole = cboUpdateRole->currentData().toString();

    if (m_authManager->adminUpdateUser(uname, newPass, newRole)) {
        QMessageBox::information(this, "Thành công 🎉", QString("Đã cập nhật thông tin tài khoản '%1' thành công!").arg(uname));
        refreshData();
    } else {
        QMessageBox::warning(this, "Lỗi cập nhật", "Không thể cập nhật thông tin tài khoản!");
    }
}

void TabAccount::onDeleteUser() {
    QString uname = cboSelectUser->currentData().toString();
    if (uname.isEmpty()) return;

    if (uname.compare(m_authManager->getCurrentUsername(), Qt::CaseInsensitive) == 0) {
        QMessageBox::warning(this, "Không thể xóa", "Không thể tự xóa tài khoản đang đăng nhập!");
        return;
    }

    if (uname.compare("admin", Qt::CaseInsensitive) == 0) {
        QMessageBox::warning(this, "Không thể xóa", "Không thể xóa tài khoản Quản trị viên gốc (admin)!");
        return;
    }

    auto reply = QMessageBox::question(this, "Xác nhận xóa tài khoản",
                                       QString("Bạn có chắc chắn muốn xóa tài khoản '%1' không?\nHành động này không thể hoàn tác!").arg(uname),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_authManager->deleteUser(uname)) {
            QMessageBox::information(this, "Đã xóa thành công", QString("Đã xóa tài khoản '%1'!").arg(uname));
            refreshData();
        } else {
            QMessageBox::warning(this, "Lỗi", "Không thể xóa tài khoản này!");
        }
    }
}
