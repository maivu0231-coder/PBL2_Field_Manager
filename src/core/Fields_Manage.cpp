#include"Fields_Manage.h"
QJsonArray FieldsManager::getAllFields()
{
    return fieldsRepo.getAll();
}
QJsonObject FieldsManager::getFieldById(const QString &id)
{
    return fieldsRepo.getById(id);
}
void FieldsManager::addField(const Fields &field)
{
    Fields newField = field;
    newField.CreatedAt = QDateTime::currentDateTime();
    newField.UpdatedAt = newField.CreatedAt;
    QJsonObject json;         
    newField.toJson(json);     
    fieldsRepo.add(json);
}

bool FieldsManager::updateField(const QString &id, const Fields &field)
{
    if (fieldsRepo.getById(id).isEmpty())
    {
        return false;
    }
    Fields updatedField = field;
    updatedField.UpdatedAt = QDateTime::currentDateTime();

    QJsonObject json;          
    updatedField.toJson(json);
    fieldsRepo.update(id, json);
    return true;                
}
void FieldsManager::deleteField(const QString &id)
{
    fieldsRepo.remove(id);
}
void FieldsManager::updateFieldStatus(const QString &id,const QString &status)
{
    
}