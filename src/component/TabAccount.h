#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "../core/AuthManager.h"

class TabAccount : public QWidget {
    Q_OBJECT
private:
    AuthManager* m_authManager;

    QTableWidget* tableUsers;
    QLineEdit* txtAddUsername;
    QLineEdit* txtAddPassword;
    QComboBox* cboAddRole;
    QPushButton* btnAddUser;

    QComboBox* cboSelectUser;
    QLineEdit* txtUpdatePassword;
    QComboBox* cboUpdateRole;
    QPushButton* btnUpdateUser;
    QPushButton* btnDeleteUser;

public:
    explicit TabAccount(AuthManager* authManager, QWidget* parent = nullptr);

public slots:
    void refreshData();

private slots:
    void onAddUser();
    void onUpdateUser();
    void onDeleteUser();
    void onUserSelectedFromTable(int row, int column);

private:
    void setupUi();
};
