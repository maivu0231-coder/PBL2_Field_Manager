#ifndef repositories_h
#define repositories_h
#include<QString>
#include<QJsonArray>
#include<QJsonObject>
#include "jsonHandler.h"
class BaseRepository{
    protected:
        QString filePath;
    public:
        BaseRepository(const QString &filePath);
        //get all records from the json file
        QJsonArray getAll();
        //get a record by id from the json file
        QJsonObject getById(const QString &id);
        //add a record to the json file
        void add(const QJsonObject &record);
        //update a record in the json file
        void update(const QString &id, const QJsonObject &record);
        //delete a record from the json file
        void remove(const QString &id);
};
class FieldsRepository:public BaseRepository{
    public:
        FieldsRepository();
};
class BookingsRepository:public BaseRepository{
    public:
        BookingsRepository();
};
class PaymentsRepository:public BaseRepository{
    public:
        PaymentsRepository();
};
