#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

LoginDialog::LoginDialog(AuthManager* authManager, QWidget* parent)
    : QDialog(parent), m_authManager(authManager) {
    setWindowTitle("Đăng nhập - Fields Manager");
    resize(300, 180);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    txtUsername = new QLineEdit(this);
    txtUsername->setPlaceholderText("Tên đăng nhập (admin / thu_ngan)");
    
    txtPassword = new QLineEdit(this);
    txtPassword->setPlaceholderText("Mật khẩu (123)");
    txtPassword->setEchoMode(QLineEdit::Password);

    btnLogin = new QPushButton("Đăng nhập", this);
    lblStatus = new QLabel("", this);
    lblStatus->setStyleSheet("color: red;");

    mainLayout->addWidget(new QLabel("Đăng nhập hệ thống", this));
    mainLayout->addWidget(txtUsername);
    mainLayout->addWidget(txtPassword);
    mainLayout->addWidget(btnLogin);
    mainLayout->addWidget(lblStatus);

    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::handleLogin);
}

void LoginDialog::handleLogin() {
    QString username = txtUsername->text().trimmed();
    QString password = txtPassword->text();

    if (m_authManager->login(username, password)) {
        accept(); // Đăng nhập thành công, đóng Dialog trả về QDialog::Accepted
    } else {
        lblStatus->setText("Sai tên đăng nhập hoặc mật khẩu!");
    }
}