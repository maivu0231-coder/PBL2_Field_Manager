#ifndef bookings_manage_h
#define bookings_manage_h
#include<QString>
#include"../models/Bookings.h"
#include"../storage/repositories.h"
class BookingsManager{
    private:
        BookingsRepository bookingsRepo;
    public:
        //get all bookings
        QJsonArray getAllBookings();
        //get a booking by id
        QJsonObject getBookingById(const QString &id);
        //get all bookings by date and field id
        QJsonArray getBookingsByDateAndFieldId(const QString &date,const QString &fieldId);
        //check overlap booking by date and field id
        bool isBookingOverlap(const QString &date,const QString &fieldId,const QString &startTime,const QString &endTime);
        //add a booking
        void addBooking(const Bookings &booking);
        //update a booking
        bool updateBooking(const QString &id,const Bookings &booking);
        //delete a booking
        void deleteBooking(const QString &id);
        //update the status of a booking
        void updateBookingStatus(const QString &id,const QString &status);
};
#endif