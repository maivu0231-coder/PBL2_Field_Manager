#ifndef FIELD_MANAGER_H
#define FIELD_MANAGER_H

#include "../storage/Repositories.h"
#include <vector>
#include <QString>
#include <QRegularExpression>
#include <utility>
#include <optional>

class FieldManager {
private:
    FieldsRepository fieldsRepository;
    BookingsRepository bookingsRepository;
    
    std::vector<Fields> getDefaultFields() const;

public:
    // constructor
    FieldManager();

    // remove accent from string
    static QString removeAccent(const QString &str);

    // get all fields:we use std::vector instead of QVector because QVector is not compatible with std::sort
    std::vector<Fields> getAllFields() const;

    // get field by id
    std::optional<Fields> getFieldById(const QString &field_id) const;

    // filter fields by status, type and search term,search term: search by name or field_id, case insensitive, accent insensitive
    std::vector<Fields> filter(const QString& status, const QString& fieldType, const QString& searchTerm);

    // add new field, return pair of success and message
    std::pair<bool, QString> add(const QString& code, const QString& name, const QString& type, double price, const QString& status);

    // update field,return pair of success and message,we use pair<bool,QString> means that the first element is success or not, the second element is message
    std::pair<bool, QString> update(const QString& code, const QString& name, const QString& type, double price, const QString& status);

    // remove field by id, return pair of success and message
    std::pair<bool, QString> remove(const QString& code);

    // reset to default fields, return pair of success and message
    std::pair<bool, QString> resetToDefault();
};

#endif // FIELD_MANAGER_H