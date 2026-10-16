#ifndef base_h
#define base_h
#include<QString>
#include<QDateTime>
#include<QJsonObject>
#include<QUuid>
class Base{
        QString FieldId;
        QDateTime CreatedAt;
        QDateTime UpdatedAt;
        Base();
        virtual ~Base();
        //convert to json
        virtual QJsonObject toJson(QJsonObject &json) const;
        //read from json
        virtual void fromJson(const QJsonObject &json);
};
#endif