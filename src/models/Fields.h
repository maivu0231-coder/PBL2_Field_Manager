#ifndef Fields_h
#define Fields_h
#include "Base.h"
#include<QString>
#include<QJsonObject>
#include<QStringList>
namespace FieldStatus{
    const QString Available="Available";
    const QString InUse="In Use";
    const QString Maintenance="Maintenance";
}
namespace FieldType{
    const QString Field_5x5="5x5";
    const QString Field_7x7="7x7";
    inline QStringList getAllFieldTypes(){
        return {Field_5x5,Field_7x7};
    }

}
class Fields:public Base{
    public:
        QString name;
        QString field_type;
        QString field_id;
        QString status;
        double price_per_hour;
        Fields():Base(){
            name="";
            field_type="";
            field_id="";
            status="";
            price_per_hour=0;
        }
        Fields(QString name,QString field_type,QString field_id,QString status,double price_per_hour):Base(){
            this->name=name;
            this->field_type=field_type;
            this->field_id=field_id;
            this->status=status;
            this->price_per_hour=price_per_hour;
        }
        QString get_id()const {
            return field_id;
        }
        //convert to json
        QJsonObject toJson() const override{
            QJsonObject obj=Base::toJson();
            obj["name"]=name;
            obj["field_type"]=field_type;
            obj["field_id"]=field_id;
            obj["status"]=status;
            obj["price_per_hour"]=price_per_hour;
            return obj;
        }
        //read from json and return a Fields object
        static Fields fromJsonObject(const QJsonObject &obj)
        {
            Fields field;
            field.fromJson(obj);
            // Read the fields from the JSON object
            field.name=obj["name"].toString();
            field.field_type=obj["field_type"].toString(FieldType::Field_5x5);
            field.field_id=obj["field_id"].toString();
            field.status=obj["status"].toString(FieldStatus::Available);
            field.price_per_hour=obj["price_per_hour"].toDouble(0.0);
            return field;
        }
};
#endif