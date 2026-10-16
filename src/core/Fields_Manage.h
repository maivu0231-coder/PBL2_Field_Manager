#ifndef fields_manage_h
#define fields_manage_h
#include<QString>
#include"../models/Fields.h"
#include"../storage/repositories.h"
class FieldsManager{
    private:
        FieldsRepository fieldsRepo;
    public:
        //get all fields
        QJsonArray getAllFields();
        //get a field by id
        QJsonObject getFieldById(const QString &id);
        //add a field
        void addField(const Fields &field);
        //update a field
        bool updateField(const QString &id,const Fields &field);
        //delete a field
        void deleteField(const QString &id);
        //update the status of a field
        void updateFieldStatus(const QString &id,const QString &status);
}