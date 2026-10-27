#ifndef repositories_h
#define repositories_h
#include <QCoreApplication>
#include <algorithm>
#include <optional>
#include <vector>
#include "JsonHandler.h"
#include "HashTable.h"
#include "../models/Fields.h"
#include "../models/Bookings.h"
#include "../models/Payments.h"
//resolve data directory means if the application is run from build directory, it will go up 4 levels to find the Data directory, if not found, it will use the current working directory
inline QString resolveDataDir() {
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 4; ++i) {
        if (dir.exists("Data")) {
            return dir.absoluteFilePath("Data");
        }
        dir.cdUp();
    }
    QDir fallback(QDir::currentPath());
    return fallback.absoluteFilePath("Data");
}
template<typename V>
class BaseRepository{
    protected:
        JsonHandler jsonHandler;
        mutable HashTable<V> hashTable;
        void loadIntoHashTable() const
        {
            hashTable.clear();
            QJsonArray arr=jsonHandler.readAll();
            for(const auto&obj:arr)
            {
                V item=V::fromJsonObject(obj.toObject());//convert QJsonObject to V object
                hashTable.put(item.get_id(),item);//put into hash table
            }
        }
        bool syncToFile()//sync hash table to file
        {
            QJsonArray arr;
            for(const auto&item:hashTable.getAllValues())
            {
                arr.append(item.toJson());//convert V object to QJsonObject
            }
            return jsonHandler.writeAll(arr);//sync to file
        }
    public:
        explicit BaseRepository(const QString &filepath):jsonHandler(filepath),hashTable(16,0.75){
            loadIntoHashTable();
        }
        virtual ~BaseRepository(){}
        vector<V>getAll() const//get all objects from hash table
        {
            loadIntoHashTable();
            //apply merge sort to sort the vector by created_at
            vector<V>items=hashTable.getAllValues();
            sort(items.begin(),items.end(),[](const V &a,const V &b){
                return a.created_at<b.created_at;
            });
            return items;
        }
        optional<V>getById(const QString &id) const// get object by id from hash table
        {
            loadIntoHashTable();
            return hashTable.get(id);
        }
        bool add(const V &item)//add object to hash table and sync to file
        {
            loadIntoHashTable();//read from file and load into hash table
            hashTable.put(item.get_id(),item);
            return syncToFile();
        }
        bool update(const V &item)//update object in hash table and sync to file
        {
            loadIntoHashTable();//read from file and load into hash table
            if(!hashTable.get(item.get_id()).has_value())
            {
                return false;
            }
            hashTable.put(item.get_id(),item);
            return syncToFile();
        }
        bool remove(const QString &id)//remove object from hash table and sync to file
        {
            loadIntoHashTable();
            if(!hashTable.contains(id))
            {
                return false;
            }
            hashTable.remove(id);
            return syncToFile();

        }
        bool replaceAll(const vector<V>&items)//replace all object in hash table and sync to file
        {
            hashTable.clear();
            for(const auto&item:items)
            {
                hashTable.put(item.get_id(),item);
            }
            return syncToFile();
        }



};
class FieldsRepository:public BaseRepository<Fields>{
    public:
        FieldsRepository():BaseRepository<Fields>("Fields.json"){}

};
class BookingsRepository:public BaseRepository<Bookings>
{
    public:
        BookingsRepository():BaseRepository<Bookings>("Bookings.json"){}
        vector<Bookings>getBookingsByDateAndField(const QString &data,const QString &field_id) const//get all bookings by date and field_id

        {
            loadIntoHashTable();
            vector<Bookings>result;
            for(const auto&booking:hashTable.getAllValues())
            {
                if(booking.booking_date==data && booking.field_id==field_id)
                {
                    result.push_back(booking);
                }
            }
            //sort by start_time
            sort(result.begin(),result.end(),[](const Bookings &a,const Bookings &b){
                return a.start_time<b.start_time;
            });
            return result;
        }
        optional<Bookings>getBookingByName(const QString &customer_name)const //get booking by customer name
        {
            loadIntoHashTable();
            for(const auto&booking:hashTable.getAllValues())
            {
                if(booking.customer_name==customer_name)
                {
                    return booking;
                }
            }
            return {};
        }

};
class PaymentsRepository:public BaseRepository<Payments>
{
    public:
        PaymentsRepository():BaseRepository<Payments>("Payments.json"){}
};