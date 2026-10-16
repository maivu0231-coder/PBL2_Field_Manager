#ifndef bookings_h
#define bookings_h
#include<QString>
#include<QDateTime>
#include<QJsonObject>
#include"Base.h"
class Bookings:public Base{
    public:
        QString FieldId;
        QString CustomerName;
        QString Phone;
        QString Date;
        QString StartTime;
        QString EndTime;
        QString Status;
        double deposit;
        //override json methods
        QJsonObject toJson(QJsonObject &json) const override;
        //override json methods
        void fromJson(const QJsonObject &json) override;
        
}