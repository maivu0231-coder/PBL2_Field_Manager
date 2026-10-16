#include "repositories.h"
BaseRepository::BaseRepository(const QString &filePath) : filePath(filePath) {}
QJsonArray BaseRepository::getAll()
{
    return JsonHandler::ReadJsonFile(filePath);
}
QJsonObject BaseRepository::getById(const QString &id)
{
    QJsonArray records= getAll();
    for (const QJsonValue &value: records)
    {
        QJsonObject record= value.toObject();
        if(record["FieldId"].toString() == id)
        {
            return record;
        }
    }
    return QJsonObject();
}
void BaseRepository::add(const QJsonObject &record)
{
    QJsonArray records=getAll();
    records.append(record);
    JsonHandler::WriteJsonFile(filePath, records);

}
void BaseRepository::update(const QString &id,const QJsonObject &record)
{
    QJsonArray records=getAll();
    for(const QJsonValue &value:records)
    {
        QJsonObject record=value.toObject();
        if(record["FieldId"].toString()==id)
        {
            records.removeAt(records.indexOf(value));
            records.append(record);
            JsonHandler::WriteJsonFile(filePath, records);
            return;

        }
    }

}
void BaseRepository::remove(const QString &id)
{
    QJsonArray records=getAll();
    for(const QJsonValue &value:records)
    {
        QJsonObject record=value.toObject();
        if(record["FieldId"].toString()==id)
        {
            records.removeAt(records.indexOf(value));
            JsonHandler::WriteJsonFile(filePath, records);
            return;
        }
    }
}
FieldsRepository::FieldsRepository():BaseRepository("Data/Fields.json"){}
BookingsRepository::BookingsRepository():BaseRepository("Data/Booking.json"){}
PaymentsRepository::PaymentsRepository():BaseRepository("Data/Payments.json"){}