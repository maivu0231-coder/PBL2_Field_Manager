#ifndef STATISTICS_MANAGER_H
#define STATISTICS_MANAGER_H

#include <QString>
#include <QMap>
#include "../core/PaymentManager.h"
#include "../core/BookingManager.h"

class StatisticsManager {
private:
    PaymentManager* m_paymentManager;
    BookingManager* m_bookingManager;

public:
    StatisticsManager(PaymentManager* paymentManager, BookingManager* bookingManager);

    //get revenue by date, return a QMap with key is date in format "yyyy-MM-dd" and value is total revenue of that date
    QMap<QString, double> getRevenueByDate() const;

    //get peak hours, return a QMap with key is hour in format "HH" and value is number of bookings in that hour
    QMap<QString, int> getPeakHours() const;
};

#endif // STATISTICS_MANAGER_H