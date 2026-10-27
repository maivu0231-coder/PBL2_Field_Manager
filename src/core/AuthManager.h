#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QString>
#include <QVector>
#include "../models/User.h"
class AuthManager {
    private:
        QVector<User> Users;
        User currentUser;
        bool isLoggedIn;
    public:
        //constructor
        AuthManager();
        //load users from json file
        bool loadUsersFromJson(const QString& filePath);
        //login 
        bool login(const QString& username, const QString& password);
        //logout    
        void logout();
        //whether the user is logged in
        bool isUserLoggedIn() const;
        //get current user name
        QString getCurrentUsername() const;
        //get current user role
        QString getCurrentUserRole() const;
};
#endif
