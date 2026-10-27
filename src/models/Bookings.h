#ifndef bookings_h
#define bookings_h
#include "Base.h"
#include<QString>
#include<QJsonObject>
#include<QStringList>

class Bookings:public Base{
    public:
        QString booking_id;
        QString field_id;
        QString customer_name;
        QString customer_phone;
        QString booking_date;
        QString start_time;
        QString end_time;
        double deposit=0.0;
        QString status="Pending";
        QString field_name;
        double price_per_hour=0.0;
        Bookings():Base(){}
        QString get_id()const {
            return booking_id;
        }
        //convert to json
        QJsonObject toJson() const override{
            QJsonObject obj=Base::toJson();
            obj["booking_id"]=booking_id;
            obj["field_id"]=field_id;
            obj["customer_name"]=customer_name;
            obj["customer_phone"]=customer_phone;
            obj["booking_date"]=booking_date;
            obj["start_time"]=start_time;
            obj["end_time"]=end_time;
            obj["deposit"]=deposit;
            obj["status"]=status;
            obj["field_name"]=field_name;
            obj["price_per_hour"]=price_per_hour;
            return obj;
        }
        //read from json and return a Bookings object
        static Bookings fromJsonObject(const QJsonObject &obj)
        {
            Bookings booking;
            booking.fromJson(obj);
            // Read the fields from the JSON object
            booking.booking_id=obj["booking_id"].toString();
            booking.field_id=obj["field_id"].toString();
            booking.customer_name=obj["customer_name"].toString();
            booking.customer_phone=obj["customer_phone"].toString();
            booking.booking_date=obj["booking_date"].toString();
            booking.start_time=obj["start_time"].toString();
            booking.end_time=obj["end_time"].toString();
            booking.deposit=obj["deposit"].toDouble(0.0);
            booking.status=obj["status"].toString("Deposited");
            booking.field_name=obj["field_name"].toString();
            booking.price_per_hour=obj["price_per_hour"].toDouble(0.0);
            return booking;
        }

};
#endif