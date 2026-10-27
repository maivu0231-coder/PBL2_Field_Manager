#ifndef BOOKINGMANAGER_H
#define BOOKINGMANAGER_H

#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QRegularExpression>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <QString>
#include "../storage/Repositories.h"

// custom exception class for booking errors
class BookingError : public std::runtime_error {
public:
    // constructor that takes a QString message and returns a std::runtime_error with the message converted to std::string
    explicit BookingError(const QString& message)
        : std::runtime_error(message.toStdString()), qMessage(message) {} // convert QString to std::string and pass it to std::runtime_error constructor
    
    QString message() const { return qMessage; } // return the message as QString

private:
    QString qMessage;
};

class BookingManager {
private:
    BookingsRepository bookingRepository;
    FieldsRepository fieldRepository;
    
    QString nextBookingId();

public:
    // default constructor
    BookingManager() = default;

    // show all fileds 
    std::vector<Fields> listFields();

    // show all bookigs by data and field_id, sorted by start_time
    std::vector<Bookings> listBookings(const QDate& date, const QString& fieldId);

    // create a new booking,validate the input and check for conflicts,return the created booking if successful, otherwise throw a BookingError with the error message
    Bookings createBooking(
        const QString& fieldId,
        const QDate& bookingDate,
        const QString& customerName,
        const QString& phone,
        const QTime& startTime,
        const QTime& endTime,
        double deposit = 0.0
    );

    // cancel a booking by booking id
    Bookings cancelBooking(const QString& bookingId);
};

#endif // BOOKINGMANAGER_H