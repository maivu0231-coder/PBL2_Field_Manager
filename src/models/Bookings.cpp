#include<Bookings.h>
QJsonObject Bookings::toJson(QJsonObject &json) const{
    Base::toJson(json);
    json["FieldId"] = FieldId;
    json["CustomerName"] = CustomerName;
    json["Phone"] = Phone;
    json["Date"] = Date;
    json["StartTime"] = StartTime;
    json["EndTime"] = EndTime;
    json["Status"] = Status;
    json["deposit"] = deposit;
    return json;
}
void Bookings::fromJson(const QJsonObject &json){
    Base::fromJson(json);
    FieldId = json["FieldId"].toString();
    CustomerName = json["CustomerName"].toString();
    Phone = json["Phone"].toString();
    Date = json["Date"].toString();
    StartTime = json["StartTime"].toString();
    EndTime = json["EndTime"].toString();
    Status = json["Status"].toString();
    deposit = json["deposit"].toDouble();
}