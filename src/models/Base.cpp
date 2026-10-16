#include<"Base.h">
Base::Base(){
    FieldId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    CreatedAt = QDateTime::currentDateTime();
    UpdatedAt = CreatedAt;
}
Base::~Base(){
    // Destructor implementation
}
QJsonObject Base::toJson(QJsonObject &json) const{
    json["FieldId"] = FieldId;
    json["CreatedAt"] = CreatedAt.toString(Qt::ISODate);
    json["UpdatedAt"] = UpdatedAt.toString(Qt::ISODate);
    return json;
}
void Base::fromJson(const QJsonObject &json){
    FieldId=json["FieldId"].toString();
    CreatedAt=QDateTime::fromString(json["CreatedAt"].toString(),Qt::ISODate);
    UpdatedAt=QDateTime::fromString(json["UpdatedAt"].toString(),Qt::ISODate);
}

