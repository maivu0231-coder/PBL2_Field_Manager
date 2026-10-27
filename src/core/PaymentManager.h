#ifndef PAYMENTMANAGER_H
#define PAYMENTMANAGER_H

#include <algorithm>
#include <optional>
#include <QSet>
#include <QTime>
#include <QRegularExpression>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <QString>
#include "../storage/Repositories.h"

// create a struct to hold the payment preview information
struct PaymentPreview {
    double duration_hours;
    double price_per_hour;
    double rental_fee; // rental_fee=duration_hours*price_per_hour
    double surcharge;
    double deposit_deducted;
    double total_amount;
};

class PaymentManager {
private:
    BookingsRepository bookingsRepository;
    PaymentsRepository paymentsRepository;

    // get all paid booking ids
    QSet<QString> getPaidBookingIds();

    // get the next available invoice id, format: INV01, INV02, INV03,...,INV99, INV100,...
    QString nextInvoiceId();

public:
    PaymentManager() = default;

    // find payment by customer name
    std::optional<Bookings> findBookingByName(const QString& name);

    // get unpaid bookings, which are bookings with status "Chờ thanh toán" or "Đã cọc" and not in payments repository
    std::vector<Bookings> getUnpaidBookings();

    // get all payments
    std::vector<Payments> getAllPayments();

    // calculate hours between start and end time, rounded to 2 decimal places
    static double calcHours(const QString& startStr, const QString& endStr);

    // get payment preview for a booking
    PaymentPreview preview(const Bookings& booking, double surcharge = 0.0) const;

    // return the payment object
    Payments pay(Bookings booking, double surcharge = 0.0);

    // return a formatted receipt string for a payment
    static QString formatReceipt(const Payments& p);
    // Xuất hóa đơn ra file txt
    // Nếu filePath rỗng, file sẽ tự động lưu vào thư mục "invoices/<invoice_id>.txt"
    static bool exportReceiptToTxt(const Payments& p, const QString& filePath = "");

    // Thanh toán và tự động in hóa đơn ra file txt
    // Nếu outputFolder rỗng, file sẽ tự động lưu vào thư mục "invoices/<invoice_id>.txt"
    Payments payAndExport(Bookings booking, double surcharge = 0.0, const QString& outputFolder = "invoices");
};

#endif // PAYMENTMANAGER_H