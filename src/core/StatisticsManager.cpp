#include "StatisticsManager.h"
#include <QTime>

// Constructor expects the related managers (not used directly in current methods but kept for future extensions)
StatisticsManager::StatisticsManager(PaymentManager* paymentManager, BookingManager* bookingManager)
    : m_paymentManager(paymentManager), m_bookingManager(bookingManager)
{}

// Statistics: revenue by date
QMap<QString, double> StatisticsManager::getRevenueByDate(bool byBookingDate) const {
    QMap<QString, double> revenueByDate;
    PaymentsRepository paymentsRepository;
    BookingsRepository bookingsRepository;

    // Cache mapping booking_id -> booking_date
    QMap<QString, QString> bookingDateMap;
    for (const auto& b : bookingsRepository.getAll()) {
        bookingDateMap[b.booking_id] = b.booking_date;
    }

    for (const auto& payment : paymentsRepository.getAll()) {
        QString dateKey;
        if (byBookingDate) {
            if (!payment.booking_date.isEmpty()) {
                dateKey = payment.booking_date;
            } else if (bookingDateMap.contains(payment.booking_id) && !bookingDateMap[payment.booking_id].isEmpty()) {
                dateKey = bookingDateMap[payment.booking_id];
            } else {
                dateKey = payment.payment_time.left(10);
            }
        } else {
            dateKey = payment.payment_time.left(10);
        }

        if (!dateKey.isEmpty()) {
            revenueByDate[dateKey] += payment.total_amount;
        }
    }
    return revenueByDate;
}

// Statistics: peak booking hours
QMap<QString, int> StatisticsManager::getPeakHours() const {
    QMap<QString, int> peakHours;
    BookingsRepository bookingsRepository;
    for (const auto& booking : bookingsRepository.getAll()) {
        if (booking.status == "Đã hủy") continue;
        int startHour = QTime::fromString(booking.start_time, "HH:mm").hour();
        int endHour   = QTime::fromString(booking.end_time, "HH:mm").hour();
        for (int hour = startHour; hour < endHour; ++hour) {
            peakHours[QString("%1").arg(hour, 2, 10, QChar('0'))]++;
        }
    }
    return peakHours;
}
