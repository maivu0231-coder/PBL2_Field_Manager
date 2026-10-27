#include "payment_manager.h"

// get all paid booking ids
QSet<QString> PaymentManager::getPaidBookingIds() {
    QSet<QString> ids;
    for (const auto& p : paymentsRepository.getAll()) {
        ids.insert(p.booking_id);
    }
    return ids;
}

// get the next available invoice id, format: INV01, INV02, INV03,...,INV99, INV100,...
QString PaymentManager::nextInvoiceId() {
    int maxNum = 0;
    QRegularExpression re("^INV(\\d+)$");
    for (const auto& p : paymentsRepository.getAll()) {
        auto m = re.match(p.invoice_id);
        if (m.hasMatch()) {
            maxNum = std::max(maxNum, m.captured(1).toInt());
        }
    }
    return QString("INV%1").arg(maxNum + 1, 2, 10, QChar('0'));
}

// find payment by customer name
std::optional<Bookings> PaymentManager::findBookingByName(const QString& name) {
    return bookingsRepository.getBookingByName(name);
}

// get unpaid bookings, which are bookings with status "Chờ thanh toán" or "Đã cọc" and not in payments repository
std::vector<Bookings> PaymentManager::getUnpaidBookings() {
    QSet<QString> paidIds = getPaidBookingIds();
    std::vector<Bookings> unpaid;
    for (const auto& b : bookingsRepository.getAll()) {
        if ((b.status == "Chờ thanh toán" || b.status == "Đã cọc") && !paidIds.contains(b.booking_id)) {
            unpaid.push_back(b);
        }
    }
    return unpaid;
}

// get all payments
std::vector<Payments> PaymentManager::getAllPayments() {
    return paymentsRepository.getAll();
}

// calculate hours between start and end time, rounded to 2 decimal places
double PaymentManager::calcHours(const QString& startStr, const QString& endStr) {
    QTime s = QTime::fromString(startStr, "HH:mm");
    QTime e = QTime::fromString(endStr, "HH:mm");
    double hours = s.secsTo(e) / 3600.0;
    return std::round(hours * 100.0) / 100.0;
}

// get payment preview for a booking
PaymentPreview PaymentManager::preview(const Bookings& booking, double surcharge) const {
    double duration = calcHours(booking.start_time, booking.end_time);
    double rentalFee = duration * booking.price_per_hour;
    double total = std::max(0.0, rentalFee + surcharge - booking.deposit);
    return {
        duration,
        booking.price_per_hour,
        rentalFee,
        surcharge,
        booking.deposit,
        total
    };
}

// return the payment object
Payments PaymentManager::pay(Bookings booking, double surcharge) {
    QSet<QString> paidIds = getPaidBookingIds();
    
    // if the booking has already been paid, throw an error
    if (paidIds.contains(booking.booking_id) || booking.status == "Đã thanh toán") {
        throw std::runtime_error(QString("Đơn %1 đã thanh toán rồi!").arg(booking.booking_id).toStdString());
    }

    // show preview of the payment information, including duration, rental fee, surcharge, deposit deducted and total amount
    PaymentPreview info = preview(booking, surcharge);
    Payments p;
    
    // inilize payment object
    p.invoice_id = nextInvoiceId();
    p.booking_id = booking.booking_id;
    p.customer_name = booking.customer_name;
    p.field_name = booking.field_name;
    p.duration_hours = info.duration_hours;
    p.price_per_hour = info.price_per_hour;
    p.rental_fee = info.rental_fee;
    p.service_fee = info.surcharge;
    p.deposit_deducted = info.deposit_deducted;
    p.total_amount = info.total_amount;

    if (!paymentsRepository.add(p)) {
        throw std::runtime_error("Không thể lưu hóa đơn thanh toán.");
    }

    booking.status = "Đã thanh toán";
    if (!bookingsRepository.update(booking)) {
        throw std::runtime_error("Không thể cập nhật trạng thái đơn đặt sân.");
    }
    return p;
}

// return a formatted receipt string for a payment
QString PaymentManager::formatReceipt(const Payments& p) {
    return QString(
        "    HÓA ĐƠN %1    \n"
        "Mã đơn    : %2\n"
        "Khách     : %3\n"
        "Sân       : %4\n"
        "Thời lượng: %5 h\n"
        "Đơn giá   : %L6 đ/h\n"
        "Tiền thuê : %L7 đ\n"
        "Phụ phí   : %L8 đ\n"
        "Tiền cọc  : -%L9 đ\n"
        "------------------------------\n"
        "TỔNG TIỀN : %L10 đ\n"
        "Thời điểm : %11\n"
    )
    .arg(p.invoice_id, p.booking_id, p.customer_name, p.field_name)
    .arg(p.duration_hours)
    .arg(p.price_per_hour, 0, 'f', 0)
    .arg(p.rental_fee, 0, 'f', 0)
    .arg(p.service_fee, 0, 'f', 0)
    .arg(p.deposit_deducted, 0, 'f', 0)
    .arg(p.total_amount, 0, 'f', 0)
    .arg(p.payment_time);
}
// export the receipt to a txt file, if filePath is empty, it will automatically save to "invoices/<invoice_id>.txt"
bool PaymentManager::exportReceiptToTxt(const Payments& p, const QString& filePath) {
    QString savePath = filePath;
    if (savePath.isEmpty()) {
        QDir dir("Output");
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        savePath = QString("Output/HoaDon_%1.txt").arg(p.invoice_id);
    }

    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << formatReceipt(p);
    file.close();

    return true;
}

// pay and automatically export the receipt to a txt file, if outputFolder is empty, it will automatically save to "invoices/<invoice_id>.txt"
Payments PaymentManager::payAndExport(Bookings booking, double surcharge, const QString& outputFolder) {
    Payments p = pay(booking, surcharge);
    QString folder = outputFolder.isEmpty() ? "Output" : outputFolder;
    QDir dir(folder);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString filePath = QString("%1/HoaDon_%2.txt").arg(folder, p.invoice_id);
    exportReceiptToTxt(p, filePath);

    return p;
}