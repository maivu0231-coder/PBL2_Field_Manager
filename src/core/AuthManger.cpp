#include "AuthManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
//constructor
AuthManager::AuthManager():isLoggedIn(false) {}
//load users from json file
bool AuthManager::loadUsersFromJson(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Không thể mở file users.json để đọc!";
        return false;
    }

    QByteArray data = file.readAll();// vector of objects
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data); //QJsonDocument doc return all QJsonObject in the file
    if (!doc.isArray()) return false;

    Users.clear();
    QJsonArray arr = doc.array();
    for (const QJsonValue& val : arr) {
        QJsonObject obj = val.toObject();
        QString uname = obj["username"].toString();
        QString pass = obj["password"].toString();
        QString role = obj["role"].toString();
        Users.append(User(uname, pass, role));
    }
    return true;
}
//login
bool AuthManager::login(const QString& username, const QString& password) {
    for (const User& user : Users) {
        if (user.getUsername() == username && user.getPassword() == password) {
            currentUser = user;
            isLoggedIn = true;
            return true;
        }
    }
    return false;
}
//logout
void AuthManager::logout() {
    Users.clear();
    currentUser = User();
    isLoggedIn = false;
}
//whether the user is logged in
bool AuthManager::isLoggedIn() const {
    return isLoggedIn;
}
//get current user role
QString AuthManager::getCurrentUserRole() const {
    return currentUser.getRole();
}
//get current user name
QString AuthManager::getCurrentUsername() const {
    return currentUser.getUsername();
}