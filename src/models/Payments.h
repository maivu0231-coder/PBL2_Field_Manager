#ifndef payments_h
#define payments_h
#include "Base.h"
#include<QString>
#include<QJsonObject>
class Payments:public Base{
    public:
        QString invoice_id;
        QString booking_id;
        QString customer_name;
        QString field_name;
        double duration_hours=0.0;
        double price_per_hour=0.0;
        double rental_fee=0.0;
        double service_fee=0.0;
        double deposit_deducted=0.0;
        double total_amount=0.0;
        QString payment_time;
        Payments():Base(){
            payment_time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        }
        QString get_id()const {
            return invoice_id;
        }
        //convert to json
        QJsonObject toJson() const override{
            QJsonObject obj=Base::toJson();
            obj["invoice_id"]=invoice_id;
            obj["booking_id"]=booking_id;
            obj["customer_name"]=customer_name;
            obj["field_name"]=field_name;
            obj["duration_hours"]=duration_hours;
            obj["price_per_hour"]=price_per_hour;
            obj["rental_fee"]=rental_fee;
            obj["service_fee"]=service_fee;
            obj["deposit_deducted"]=deposit_deducted;
            obj["total_amount"]=total_amount;
            obj["payment_time"]=payment_time;
            return obj;
        }
        //read from json and return a Payments object
        static Payments fromJsonObject(const QJsonObject &obj)
        {
            Payments payment;
            payment.fromJson(obj);
            // Read the fields from the JSON object
            payment.invoice_id=obj["invoice_id"].toString();
            payment.booking_id=obj["booking_id"].toString();
            payment.customer_name=obj["customer_name"].toString();
            payment.field_name=obj["field_name"].toString();
            payment.duration_hours=obj["duration_hours"].toDouble(0.0);
            payment.price_per_hour=obj["price_per_hour"].toDouble(0.0);
            payment.rental_fee=obj["rental_fee"].toDouble(0.0);
            payment.service_fee=obj["service_fee"].toDouble(0.0);
            payment.deposit_deducted=obj["deposit_deducted"].toDouble(0.0);
            payment.total_amount=obj["total_amount"].toDouble(0.0);
            payment.payment_time=obj["payment_time"].toString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            return payment;
        }

};
#endif