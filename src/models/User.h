#ifndef user_h  
#define user_h
#include<QString>
class User{
    private:
        QString username;
        QString password;
        QString role;
    public:
        //constructor
        User() : m_username(""), m_password(""), m_role("Cashier") {}
        User(const QString& username, const QString& password, const QString& role)
            : m_username(username), m_password(password), m_role(role) {}
        //get user name
        QString getUsername() const { return m_username; }
        //get user name
        void setUsername(const QString& username) 
        { m_username = username; }
        //get password
        QString getPassword() const { return m_password; }
        //set password
        void setPassword(const QString& password) 
        { m_password = password; }
        //get role
        QString getRole() const { return m_role; }
        //set role
        void setRole(const QString& role) 
        { m_role = role; }

};
#endif