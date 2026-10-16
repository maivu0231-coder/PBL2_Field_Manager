#ifndef FIELDS_H
#define FIELDS_H
#include<QString>
#include<QDateTime>
#include<QJsonObject>
#include"Base.h"
class Fields:public Base{
public:
    QString FieldName;
    QString FieldType;
    QString Status;
    double PricePerHour;
    //override json methods
    QJsonObject toJson(QJsonObject &json) const override;
    //override json methods
    void fromJson(const QJsonObject &json) override;
};
#endif

