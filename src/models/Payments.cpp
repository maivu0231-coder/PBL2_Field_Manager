#include"Payments.h"
QJsonObject Payments::toJson(QJsonObject &json) const{
    Base::toJson(json);
    json["BookingId"] = BookingId;
    json["hours"] = hours;
    json["PricePerHour"] = PricePerHour;
    json["Surcharge"] = Surcharge;
    json["TotalPrice"] = TotalPrice;
    return json;

}
void Payments::fromJson(const QJsonObject &json){
    Base::fromJson(json);
    BookingId = json["BookingId"].toString();
    hours = json["hours"].toDouble();
    PricePerHour = json["PricePerHour"].toDouble();
    Surcharge = json["Surcharge"].toDouble();
    TotalPrice = json["TotalPrice"].toDouble();
}