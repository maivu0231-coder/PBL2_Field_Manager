#include"Fields.h"
QJsonObject Fields::toJson(QJsonObject &json) const{
    Base::toJson(json);
    json["FieldName"] = FieldName;
    json["FieldType"] = FieldType;
    json["Status"] = Status;
    json["PricePerHour"] = PricePerHour;
    return json;
}
void Fields::fromJson(const QJsonObject &json){
    Base::fromJson(json);
    FieldName = json["FieldName"].toString();
    FieldType = json["FieldType"].toString();
    Status = json["Status"].toString();
    PricePerHour = json["PricePerHour"].toDouble();
}
