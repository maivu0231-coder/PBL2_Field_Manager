#ifndef Fields_h
#define Fields_h
#include "Base.h"
#include <QString>
#include <QJsonObject>
#include <QStringList>

namespace FieldStatus {
    const QString Available = "Available";
    const QString InUse = "In Use";
    const QString Maintenance = "Maintenance";
    const QString AVAILABLE = Available;
    const QString IN_USE = InUse;
    const QString MAINTENANCE = Maintenance;
    inline QStringList all() {
        return {Available, InUse, Maintenance};
    }
}

namespace FieldType {
    const QString Field_5x5 = "5x5";
    const QString Field_7x7 = "7x7";
    inline QStringList getAllFieldTypes() {
        return {Field_5x5, Field_7x7};
    }
    inline QStringList all() {
        return {Field_5x5, Field_7x7};
    }
}

class Fields : public Base {
public:
    QString name;
    QString field_type;
    QString field_id;
    QString status;
    double price_per_hour;

    Fields() : Base() {
        name = "";
        field_type = "";
        field_id = "";
        status = "";
        price_per_hour = 0;
    }

    Fields(QString name, QString field_type, QString field_id, QString status, double price_per_hour) : Base() {
        this->name = name;
        this->field_type = field_type;
        this->field_id = field_id;
        this->status = status;
        this->price_per_hour = price_per_hour;
    }

    QString get_id() const {
        return field_id;
    }

    // Convert to json
    QJsonObject toJson() const override {
        QJsonObject obj = Base::toJson();
        obj["name"] = name;
        obj["field_type"] = field_type;
        obj["field_id"] = field_id;
        obj["status"] = status;
        obj["price_per_hour"] = price_per_hour;
        return obj;
    }

    // Read from json and return a Fields object
    static Fields fromJsonObject(const QJsonObject &obj) {
        Fields field;
        field.fromJson(obj);

        // Name
        field.name = obj["name"].toString();

        // Field ID: hỗ trợ cả "field_id" và "id"
        if (obj.contains("field_id")) {
            field.field_id = obj["field_id"].toString();
        } else if (obj.contains("id")) {
            field.field_id = obj["id"].toString();
        }

        // Field Type: hỗ trợ cả "field_type" và "type", map "Sân 5" -> "5x5", "Sân 7" -> "7x7"
        QString typeVal = obj.contains("field_type") ? obj["field_type"].toString() : obj["type"].toString();
        if (typeVal.contains("7")) {
            field.field_type = FieldType::Field_7x7;
        } else {
            field.field_type = FieldType::Field_5x5;
        }

        // Status: hỗ trợ cả tiếng Anh lẫn tiếng Việt
        QString statusVal = obj["status"].toString(FieldStatus::Available);
        if (statusVal.compare("Trống", Qt::CaseInsensitive) == 0 ||
            statusVal.compare("Available", Qt::CaseInsensitive) == 0) {
            field.status = FieldStatus::Available;
        } else if (statusVal.compare("Đang có khách", Qt::CaseInsensitive) == 0 ||
                   statusVal.compare("In Use", Qt::CaseInsensitive) == 0) {
            field.status = FieldStatus::InUse;
        } else if (statusVal.compare("Bảo trì", Qt::CaseInsensitive) == 0 ||
                   statusVal.compare("Maintenance", Qt::CaseInsensitive) == 0) {
            field.status = FieldStatus::Maintenance;
        } else {
            field.status = statusVal;
        }

        // Price per hour: hỗ trợ cả "price_per_hour" và "pricePerHour"
        if (obj.contains("price_per_hour")) {
            field.price_per_hour = obj["price_per_hour"].toDouble(0.0);
        } else if (obj.contains("pricePerHour")) {
            field.price_per_hour = obj["pricePerHour"].toDouble(0.0);
        }

        return field;
    }
};

#endif
