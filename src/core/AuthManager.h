#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QString>
#include <QVector>
#include "../models/User.h"

class AuthManager {
private:
    QVector<User> Users;
    User currentUser;
    bool m_isLoggedIn;
    QString m_filePath;

public:
    // Constructor
    AuthManager();

    // Load users from JSON file
    bool loadUsersFromJson(const QString& filePath);

    // Save users to JSON file
    bool saveUsersToJson(const QString& filePath = "");

    // Login
    bool login(const QString& username, const QString& password);

    // Logout
    void logout();

    // Check whether the user is logged in
    bool isLoggedIn() const;

    // Get current user name
    QString getCurrentUsername() const;

    // Get current user role
    QString getCurrentUserRole() const;

    // Lấy toàn bộ danh sách tài khoản
    const QVector<User>& getAllUsers() const;

    // Đổi mật khẩu cho tài khoản hiện tại
    bool changePassword(const QString& username, const QString& oldPassword, const QString& newPassword);

    // Thêm tài khoản mới (dành cho Admin)
    bool addUser(const QString& username, const QString& password, const QString& role);

    // Xóa tài khoản (dành cho Admin)
    bool deleteUser(const QString& username);

    // Admin đặt lại mật khẩu hoặc đổi vai trò
    bool adminUpdateUser(const QString& username, const QString& newPassword, const QString& newRole);
};

#endif // AUTHMANAGER_H
