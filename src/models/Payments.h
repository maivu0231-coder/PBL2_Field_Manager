#ifndef payments_h
#define payments_h
#include<QString>
#include"Base.h"
class Payments:public Base{
    public:
        
        QString BookingId;
        double hours;
        double PricePerHour;
        double Surcharge;
        double TotalPrice;
        //override json methods
        QJsonObject toJson(QJsonObject &json) const override;
        //override json methods
        void fromJson(const QJsonObject &json) override;

};
#endif
