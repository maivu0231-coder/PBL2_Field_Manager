#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QFont>

LoginDialog::LoginDialog(AuthManager* authManager, QWidget* parent)
    : QDialog(parent), m_authManager(authManager) {
    setWindowTitle("Đăng Nhập - Fields Manager Pro");
    setFixedSize(410, 460);
    setStyleSheet(
        "QDialog { background-color: #f1f5f9; font-family: 'Segoe UI', Tahoma, sans-serif; }"
        "QLineEdit { background: #ffffff; border: 1.5px solid #cbd5e1; border-radius: 8px; padding: 10px 14px; font-size: 13px; color: #0f172a; }"
        "QLineEdit:focus { border: 2px solid #2563eb; background: #ffffff; }"
    );

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(18, 18, 18, 18);

    // Main Card
    auto* card = new QFrame(this);
    card->setStyleSheet(
        "QFrame#LoginCard { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 16px; }"
    );
    card->setObjectName("LoginCard");

    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 24, 24, 24);
    cardLayout->setSpacing(14);

    // Header Banner
    auto* headerBanner = new QFrame(card);
    headerBanner->setStyleSheet(
        "QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0f172a, stop:0.5 #1e3a8a, stop:1 #2563eb);"
        "border-radius: 12px; }"
    );
    auto* bannerLayout = new QVBoxLayout(headerBanner);
    bannerLayout->setContentsMargins(16, 20, 16, 20);
    bannerLayout->setSpacing(6);
    bannerLayout->setAlignment(Qt::AlignCenter);

    auto* lblTitle = new QLabel("⚽ FIELDS MANAGER PRO", headerBanner);
    lblTitle->setStyleSheet("color: #ffffff; font-size: 20px; font-weight: 800; letter-spacing: 0.5px; background: transparent; border: none;");
    lblTitle->setAlignment(Qt::AlignCenter);

    auto* lblSubTitle = new QLabel("Hệ thống quản lý sân bóng", headerBanner);
    lblSubTitle->setStyleSheet("color: #93c5fd; font-size: 13px; font-weight: 600; background: transparent; border: none;");
    lblSubTitle->setAlignment(Qt::AlignCenter);

    bannerLayout->addWidget(lblTitle);
    bannerLayout->addWidget(lblSubTitle);
    cardLayout->addWidget(headerBanner);

    // Username Field
    auto* lblUser = new QLabel("👤 Tên đăng nhập:", card);
    lblUser->setStyleSheet("font-size: 12px; font-weight: 600; color: #475569; margin-top: 4px;");
    txtUsername = new QLineEdit(card);
    txtUsername->setPlaceholderText("Nhập tên tài khoản...");
    cardLayout->addWidget(lblUser);
    cardLayout->addWidget(txtUsername);

    // Password Field with Toggle Show/Hide
    auto* lblPass = new QLabel("🔒 Mật khẩu:", card);
    lblPass->setStyleSheet("font-size: 12px; font-weight: 600; color: #475569;");
    
    auto* passContainer = new QWidget(card);
    auto* passLayout = new QHBoxLayout(passContainer);
    passLayout->setContentsMargins(0, 0, 0, 0);
    passLayout->setSpacing(6);

    txtPassword = new QLineEdit(passContainer);
    txtPassword->setPlaceholderText("Nhập mật khẩu...");
    txtPassword->setEchoMode(QLineEdit::Password);

    btnTogglePass = new QPushButton("👁️", passContainer);
    btnTogglePass->setFixedSize(42, 40);
    btnTogglePass->setToolTip("Hiển thị / Ẩn mật khẩu");
    btnTogglePass->setCursor(Qt::PointingHandCursor);
    btnTogglePass->setStyleSheet(
        "QPushButton { background: #f1f5f9; border: 1.5px solid #cbd5e1; border-radius: 8px; font-size: 16px; }"
        "QPushButton:hover { background: #e2e8f0; border-color: #94a3b8; }"
    );

    passLayout->addWidget(txtPassword, 1);
    passLayout->addWidget(btnTogglePass);

    cardLayout->addWidget(lblPass);
    cardLayout->addWidget(passContainer);

    // Status / Error message
    lblStatus = new QLabel("", card);
    lblStatus->setAlignment(Qt::AlignCenter);
    lblStatus->setWordWrap(true);
    lblStatus->setStyleSheet("color: #dc2626; font-size: 12px; font-weight: 600; min-height: 18px;");
    cardLayout->addWidget(lblStatus);

    // Login Button
    btnLogin = new QPushButton("⚡ ĐĂNG NHẬP HỆ THỐNG", card);
    btnLogin->setCursor(Qt::PointingHandCursor);
    btnLogin->setFixedHeight(44);
    btnLogin->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2563eb, stop:1 #1d4ed8);"
        "  color: white;"
        "  font-size: 14px;"
        "  font-weight: 700;"
        "  letter-spacing: 0.5px;"
        "  border: none;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1d4ed8, stop:1 #1e40af);"
        "}"
        "QPushButton:pressed {"
        "  background: #1e3a8a;"
        "}"
    );
    cardLayout->addWidget(btnLogin);

    rootLayout->addWidget(card);

    // Connect signals
    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::handleLogin);
    connect(txtUsername, &QLineEdit::returnPressed, this, &LoginDialog::handleLogin);
    connect(txtPassword, &QLineEdit::returnPressed, this, &LoginDialog::handleLogin);
    connect(btnTogglePass, &QPushButton::clicked, this, &LoginDialog::togglePasswordVisibility);

    txtUsername->setFocus();
}

void LoginDialog::togglePasswordVisibility() {
    if (txtPassword->echoMode() == QLineEdit::Password) {
        txtPassword->setEchoMode(QLineEdit::Normal);
        btnTogglePass->setText("🙈");
    } else {
        txtPassword->setEchoMode(QLineEdit::Password);
        btnTogglePass->setText("👁️");
    }
}

void LoginDialog::handleLogin() {
    QString username = txtUsername->text().trimmed();
    QString password = txtPassword->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        lblStatus->setText("⚠️ Vui lòng nhập đầy đủ tên tài khoản và mật khẩu!");
        lblStatus->setStyleSheet("color: #dc2626; font-size: 12px; font-weight: 600; min-height: 18px;");
        return;
    }

    if (m_authManager->login(username, password)) {
        accept();
    } else {
        lblStatus->setText("❌ Sai tên đăng nhập hoặc mật khẩu! Vui lòng thử lại.");
        lblStatus->setStyleSheet("color: #dc2626; font-size: 12px; font-weight: 600; min-height: 18px;");
        txtPassword->selectAll();
        txtPassword->setFocus();
    }
}
