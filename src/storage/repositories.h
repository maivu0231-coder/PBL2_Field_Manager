#ifndef repositories_h
#define repositories_h
#include <QCoreApplication>
#include <algorithm>
#include <optional>
#include <vector>
#include <QDir>
#include <QFile>
#include "JsonHandler.h"
#include "HashTable.h"
#include "../models/Fields.h"
#include "../models/Bookings.h"
#include "../models/Payments.h"

// Resolve data directory: looks up from executable or current directory to locate "Data" folder
inline QString resolveDataDir() {
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 4; ++i) {
        if (dir.exists("Data")) {
            return dir.absoluteFilePath("Data");
        }
        dir.cdUp();
    }
    QDir fallback(QDir::currentPath());
    for (int i = 0; i < 4; ++i) {
        if (fallback.exists("Data")) {
            return fallback.absoluteFilePath("Data");
        }
        fallback.cdUp();
    }
    return QDir::currentPath() + "/Data";
}

template<typename V>
class BaseRepository {
protected:
    JsonHandler jsonHandler;
    mutable HashTable<V> hashTable;
    void loadIntoHashTable() const {
        hashTable.clear();
        QJsonArray arr = jsonHandler.readAll();
        for (const auto& obj : arr) {
            V item = V::fromJsonObject(obj.toObject());
            hashTable.put(item.get_id(), item);
        }
    }
    bool syncToFile() {
        QJsonArray arr;
        for (const auto& item : hashTable.getAllValues()) {
            arr.append(item.toJson());
        }
        return jsonHandler.writeAll(arr);
    }
public:
    explicit BaseRepository(const QString &filepath)
        : jsonHandler(filepath), hashTable(16, 0.75) {
        loadIntoHashTable();
    }
    virtual ~BaseRepository() {}

    std::vector<V> getAll() const {
        loadIntoHashTable();
        std::vector<V> items = hashTable.getAllValues();
        std::sort(items.begin(), items.end(), [](const V &a, const V &b) {
            return a.created_at < b.created_at;
        });
        return items;
    }

    std::optional<V> getById(const QString &id) const {
        loadIntoHashTable();
        return hashTable.get(id);
    }

    bool add(const V &item) {
        loadIntoHashTable();
        hashTable.put(item.get_id(), item);
        return syncToFile();
    }

    bool update(const V &item) {
        loadIntoHashTable();
        if (!hashTable.get(item.get_id()).has_value()) {
            return false;
        }
        hashTable.put(item.get_id(), item);
        return syncToFile();
    }

    bool remove(const QString &id) {
        loadIntoHashTable();
        if (!hashTable.contains(id)) {
            return false;
        }
        hashTable.remove(id);
        return syncToFile();
    }

    bool replaceAll(const std::vector<V>& items) {
        hashTable.clear();
        for (const auto& item : items) {
            hashTable.put(item.get_id(), item);
        }
        return syncToFile();
    }
};

class FieldsRepository : public BaseRepository<Fields> {
public:
    FieldsRepository() : BaseRepository<Fields>(resolveDataDir() + "/Fields.json") {}
};

class BookingsRepository : public BaseRepository<Bookings> {
public:
    BookingsRepository() : BaseRepository<Bookings>(
        QFile::exists(resolveDataDir() + "/Booking.json") 
            ? resolveDataDir() + "/Booking.json" 
            : resolveDataDir() + "/Bookings.json") {}

    std::vector<Bookings> getBookingsByDateAndField(const QString &data, const QString &field_id) const {
        loadIntoHashTable();
        std::vector<Bookings> result;
        for (const auto& booking : hashTable.getAllValues()) {
            if (booking.booking_date == data && booking.field_id == field_id) {
                result.push_back(booking);
            }
        }
        std::sort(result.begin(), result.end(), [](const Bookings &a, const Bookings &b) {
            return a.start_time < b.start_time;
        });
        return result;
    }

    std::optional<Bookings> getBookingByName(const QString &customer_name) const {
        loadIntoHashTable();
        for (const auto& booking : hashTable.getAllValues()) {
            if (booking.customer_name == customer_name) {
                return booking;
            }
        }
        return {};
    }
};

class PaymentsRepository : public BaseRepository<Payments> {
public:
    PaymentsRepository() : BaseRepository<Payments>(resolveDataDir() + "/Payments.json") {}
};

#endif
