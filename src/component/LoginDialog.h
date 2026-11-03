#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../core/AuthManager.h"

class LoginDialog : public QDialog {
    Q_OBJECT
private:
    QLineEdit* txtUsername;
    QLineEdit* txtPassword;
    QPushButton* btnTogglePass;
    QPushButton* btnLogin;
    QLabel* lblStatus;
    AuthManager* m_authManager;

private slots:
    void handleLogin();
    void togglePasswordVisibility();

public:
    explicit LoginDialog(AuthManager* authManager, QWidget* parent = nullptr);
};

#endif // LOGINDIALOG_H
