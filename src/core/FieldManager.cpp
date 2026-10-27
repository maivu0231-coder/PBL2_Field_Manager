#include "field_manager.h"

std::vector<Fields> FieldManager::getDefaultFields() const {
    std::vector<Fields> defaultFields;
    defaultFields.push_back(Fields("Sân5A", FieldType::Field_5x5, "S01", FieldStatus::Available, 100));
    defaultFields.push_back(Fields("Sân7B", FieldType::Field_7x7, "S02", FieldStatus::Available, 100));
    defaultFields.push_back(Fields("Sân5C", FieldType::Field_5x5, "S03", FieldStatus::Available, 100));
    return defaultFields;
}

FieldManager::FieldManager() {
    // check if fields.json exists, if not create it with default fields
    if(fieldsRepository.getAll().empty()) {
        fieldsRepository.replaceAll(getDefaultFields());
    }
}

QString FieldManager::removeAccent(const QString &str) {
    QString modifiedStr = str;
    modifiedStr.replace("đ", "d").replace("Đ", "D");
    QString decomposed = modifiedStr.normalized(QString::NormalizationForm_D);
    QString result;
    for (QChar c : decomposed) {
        if (c.category() != QChar::Mark_NonSpacing) {
            result.append(c);
        }
    }
    return result.toLower();
}

std::vector<Fields> FieldManager::getAllFields() const {
    return fieldsRepository.getAll();
}

std::optional<Fields> FieldManager::getFieldById(const QString &field_id) const {
    return fieldsRepository.getById(field_id);
}

std::vector<Fields> FieldManager::filter(const QString& status, const QString& fieldType, const QString& searchTerm) {
    std::vector<Fields> records = getAllFields();
    std::vector<Fields> filtered;

    QString cleanKw = searchTerm.simplified(); // remove extra spaces
    QString rawKw = cleanKw.toLower(); // convert to lower case
    QString noAccentKw = removeAccent(cleanKw); // remove accent and convert to lower case

    for (const auto& f : records) {
        if (status != "Tất cả" && f.status != status) continue; // filter by status
        if (fieldType != "Tất cả" && f.field_type != fieldType) continue; // filter by type

        if (!cleanKw.isEmpty()) // filter by search term
        {
            QString nameRaw = f.name.toLower();
            QString codeRaw = f.field_id.toLower();
            // Remove accents from name and code
            QString nameNoAccent = removeAccent(f.name);
            QString codeNoAccent = removeAccent(f.field_id);

            bool matched = nameRaw.contains(rawKw) ||
                           codeRaw.contains(rawKw) ||
                           nameNoAccent.contains(noAccentKw) ||
                           codeNoAccent.contains(noAccentKw); // check if name or code contains search term
            if (!matched) continue;
        }
        filtered.push_back(f);
    }
    return filtered;
}

std::pair<bool, QString> FieldManager::add(const QString& code, const QString& name, const QString& type, double price, const QString& status) {
    QString cleanCode = code.simplified().toUpper();
    QString cleanName = name.simplified();
    
    // Validate inputs
    if (cleanCode.isEmpty() || cleanName.isEmpty()) // check if code or name is empty
    {
        return {false, "Mã sân và Tên sân không được để trống!"};
    }
    if (cleanCode.length() > 20) {
        return {false, "Mã sân không được vượt quá 20 ký tự!"};
    }
    if (cleanName.length() > 100) {
        return {false, "Tên sân không được vượt quá 100 ký tự!"};
    }
    if (price <= 0 || price > 50000000) {
        return {false, "Giá thuê phải lớn hơn 0 và tối đa 50,000,000 VNĐ!"};
    }
    if (fieldsRepository.getById(cleanCode).has_value()) // check if code already exists
    {
        return {false, QString("Mã sân '%1' đã tồn tại trên hệ thống!").arg(cleanCode)};
    }

    Fields newField(cleanName, type, cleanCode, status, price); // create new field object
    if (!fieldsRepository.add(newField)) {
        return {false, "Không thể lưu sân mới vào file dữ liệu."};
    }
    return {true, QString("Thêm thành công sân '%1' vào Bảng băm & File JSON!").arg(cleanCode)};
}

std::pair<bool, QString> FieldManager::update(const QString& code, const QString& name, const QString& type, double price, const QString& status) {
    QString cleanName = name.simplified();
    
    // Validate inputs
    if (cleanName.isEmpty()) {
        return {false, "Tên sân bóng không được để trống!"};
    }
    if (price <= 0 || price > 50000000) {
        return {false, "Giá thuê không hợp lệ!"};
    }
    if (type.isEmpty()) {
        return {false, "Loại sân không được để trống!"};
    }

    auto existingOpt = fieldsRepository.getById(code);
    if (!existingOpt.has_value()) {
        return {false, QString("Không tìm thấy sân mã '%1'.").arg(code)};
    }
    Fields updated = existingOpt.value(); // copy existing field
    QString oldName = updated.name;
    updated.name = cleanName;
    updated.field_type = type;
    updated.price_per_hour = price;
    updated.status = status;

    if (!fieldsRepository.update(updated)) // if update failed, return false and message
    {
        return {false, "Không thể lưu thay đổi vào file dữ liệu."};
    }
    if (oldName != cleanName) {
        // if name changed, update all bookings with this field_id and status not "Đã hủy"
        for (auto b : bookingsRepository.getAll()) // update all bookings with this field_id and status not "Đã hủy"
            if (b.field_id == code && b.status != "Đã hủy") {
                b.field_name = cleanName;
                bookingsRepository.update(b);
            }
    }
    return {true, QString("Cập nhật sân '%1' thành công!").arg(code)};
}

std::pair<bool, QString> FieldManager::remove(const QString& code) {
    auto targetOpt = fieldsRepository.getById(code); // get field by id
    if (!targetOpt.has_value()) { 
        return {false, QString("Không tìm thấy sân mã '%1'.").arg(code)};
    }

    if (targetOpt->status == FieldStatus::InUse) // if field is in use,cannot delete
    {
        return {false, QString("Sân '%1' đang ở trạng thái 'Đang có khách'. Không thể xóa!").arg(code)};
    }

    int activeCount = 0;        
    for (const auto& b : bookingsRepository.getAll()) // count all bookings with this field_id and status not "Đã hủy"
    {
        if (b.field_id == code && b.status != "Đã hủy") {
            activeCount++;
        }
    }
    if (activeCount > 0) // if there exists any bookings so that we cannot delete the field, return false and message
    {
        return {false, QString("Sân '%1' đang có %2 đơn đặt sân chưa hủy. Không thể xóa!").arg(code).arg(activeCount)};
    }

    if (!fieldsRepository.remove(code)) // if remove failed, return false and message
    {
        return {false, "Không thể cập nhật file sau khi xóa sân."};
    }
    return {true, QString("Đã xóa sân '%1' khỏi hệ thống!").arg(code)};
}

std::pair<bool, QString> FieldManager::resetToDefault() {
    if (!fieldsRepository.replaceAll(getDefaultFields())) {
        return {false, "Không thể ghi dữ liệu mặc định vào file."};
    }
    return {true, "Đã khôi phục dữ liệu 4 sân mặc định thành công!"};
}