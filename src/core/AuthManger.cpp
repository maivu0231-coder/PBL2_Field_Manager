#include "AuthManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

// Constructor
AuthManager::AuthManager() : m_isLoggedIn(false), m_filePath("") {
    // Luôn có tài khoản mặc định đề phòng file json lỗi hoặc chưa kịp tải
    Users.append(User("admin", "123", "Admin"));
    Users.append(User("thu_ngan", "123", "Cashier"));
}

// Load users from JSON file
bool AuthManager::loadUsersFromJson(const QString& filePath) {
    m_filePath = filePath;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Không thể mở file users.json để đọc! Đang sử dụng tài khoản mặc định.";
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isArray())
        return false;

    QVector<User> loadedUsers;
    QJsonArray arr = doc.array();

    for (const QJsonValue& val : arr) {
        QJsonObject obj = val.toObject();

        QString uname = obj["username"].toString().trimmed();
        QString pass = obj["password"].toString().trimmed();
        QString role = obj["role"].toString().trimmed();

        if (!uname.isEmpty() && !pass.isEmpty()) {
            loadedUsers.append(User(uname, pass, role.isEmpty() ? "Cashier" : role));
        }
    }

    if (!loadedUsers.isEmpty()) {
        Users = loadedUsers;
    }

    // Đảm bảo luôn có ít nhất admin và thu_ngan
    bool hasAdmin = false;
    bool hasCashier = false;
    for (const auto& u : Users) {
        if (u.getUsername().compare("admin", Qt::CaseInsensitive) == 0) hasAdmin = true;
        if (u.getUsername().compare("thu_ngan", Qt::CaseInsensitive) == 0) hasCashier = true;
    }
    if (!hasAdmin) Users.append(User("admin", "123", "Admin"));
    if (!hasCashier) Users.append(User("thu_ngan", "123", "Cashier"));

    return true;
}

// Save users to JSON file
bool AuthManager::saveUsersToJson(const QString& filePath) {
    QString path = filePath.isEmpty() ? m_filePath : filePath;
    if (path.isEmpty()) return false;

    QJsonArray arr;
    for (const auto& u : Users) {
        QJsonObject obj;
        obj["username"] = u.getUsername();
        obj["password"] = u.getPassword();
        obj["role"] = u.getRole();
        arr.append(obj);
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug() << "Không thể ghi vào file users.json:" << path;
        return false;
    }

    QJsonDocument doc(arr);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

// Login
bool AuthManager::login(const QString& username, const QString& password) {
    QString cleanUname = username.trimmed();
    QString cleanPass = password.trimmed();

    for (const User& user : Users) {
        if (user.getUsername().compare(cleanUname, Qt::CaseInsensitive) == 0 &&
            user.getPassword() == cleanPass) {

            currentUser = user;
            m_isLoggedIn = true;

            return true;
        }
    }

    return false;
}

// Logout
void AuthManager::logout() {
    currentUser = User();
    m_isLoggedIn = false;
}

// Check whether the user is logged in
bool AuthManager::isLoggedIn() const {
    return m_isLoggedIn;
}

// Get current user role
QString AuthManager::getCurrentUserRole() const {
    return currentUser.getRole();
}

// Get current user name
QString AuthManager::getCurrentUsername() const {
    return currentUser.getUsername();
}

// Lấy toàn bộ danh sách tài khoản
const QVector<User>& AuthManager::getAllUsers() const {
    return Users;
}

// Đổi mật khẩu cho tài khoản hiện tại
bool AuthManager::changePassword(const QString& username, const QString& oldPassword, const QString& newPassword) {
    QString cleanUname = username.trimmed();
    QString cleanOld = oldPassword.trimmed();
    QString cleanNew = newPassword.trimmed();

    if (cleanNew.isEmpty()) return false;

    for (int i = 0; i < Users.size(); ++i) {
        if (Users[i].getUsername().compare(cleanUname, Qt::CaseInsensitive) == 0) {
            if (Users[i].getPassword() != cleanOld) {
                return false; // Mật khẩu cũ không đúng
            }
            Users[i].setPassword(cleanNew);
            if (currentUser.getUsername().compare(cleanUname, Qt::CaseInsensitive) == 0) {
                currentUser.setPassword(cleanNew);
            }
            saveUsersToJson();
            return true;
        }
    }
    return false;
}

// Thêm tài khoản mới (dành cho Admin)
bool AuthManager::addUser(const QString& username, const QString& password, const QString& role) {
    QString cleanUname = username.trimmed();
    QString cleanPass = password.trimmed();
    QString cleanRole = role.trimmed();

    if (cleanUname.isEmpty() || cleanPass.isEmpty()) return false;

    // Kiểm tra tên trùng lặp
    for (const auto& u : Users) {
        if (u.getUsername().compare(cleanUname, Qt::CaseInsensitive) == 0) {
            return false;
        }
    }

    Users.append(User(cleanUname, cleanPass, cleanRole.isEmpty() ? "Cashier" : cleanRole));
    saveUsersToJson();
    return true;
}

// Xóa tài khoản (dành cho Admin)
bool AuthManager::deleteUser(const QString& username) {
    QString cleanUname = username.trimmed();

    // Không cho phép xóa chính tài khoản đang đăng nhập hoặc admin gốc
    if (currentUser.getUsername().compare(cleanUname, Qt::CaseInsensitive) == 0) {
        return false;
    }
    if (cleanUname.compare("admin", Qt::CaseInsensitive) == 0) {
        return false;
    }

    for (int i = 0; i < Users.size(); ++i) {
        if (Users[i].getUsername().compare(cleanUname, Qt::CaseInsensitive) == 0) {
            Users.removeAt(i);
            saveUsersToJson();
            return true;
        }
    }
    return false;
}

// Admin đặt lại mật khẩu hoặc đổi vai trò
bool AuthManager::adminUpdateUser(const QString& username, const QString& newPassword, const QString& newRole) {
    QString cleanUname = username.trimmed();
    for (int i = 0; i < Users.size(); ++i) {
        if (Users[i].getUsername().compare(cleanUname, Qt::CaseInsensitive) == 0) {
            if (!newPassword.trimmed().isEmpty()) {
                Users[i].setPassword(newPassword.trimmed());
            }
            if (!newRole.trimmed().isEmpty()) {
                Users[i].setRole(newRole.trimmed());
            }
            if (currentUser.getUsername().compare(cleanUname, Qt::CaseInsensitive) == 0) {
                if (!newPassword.trimmed().isEmpty()) currentUser.setPassword(newPassword.trimmed());
                if (!newRole.trimmed().isEmpty()) currentUser.setRole(newRole.trimmed());
            }
            saveUsersToJson();
            return true;
        }
    }
    return false;
}
