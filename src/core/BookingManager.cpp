#include "booking_manager.h"

QString BookingManager::nextBookingId() {
    int maxNum = 0;
    QRegularExpression re("^BK(\\d+)$"); // regex to match booking id format BKxx where xx is a number
    for (const auto& b : bookingRepository.getAll()) // iterate through all bookings and find the maximum number in the booking id
    {
        auto match = re.match(b.booking_id); // match the booking id with the regex
        if (match.hasMatch()) {
            maxNum = std::max(maxNum, match.captured(1).toInt());
        }
    }
    int next = maxNum + 1;
    QString candidate = QString("BK%1").arg(next, 2, 10, QChar('0')); // format the next booking id as BKxx where xx is the next number with leading zeros
    while (bookingRepository.getById(candidate).has_value()) // if the candidate booking id already exists, increment the number and try again
    {
        next++;
        candidate = QString("BK%1").arg(next, 2, 10, QChar('0'));
    }
    return candidate; // return the next available booking id
}

std::vector<Fields> BookingManager::listFields() {
    return fieldRepository.getAll();
}

std::vector<Bookings> BookingManager::listBookings(const QDate& date, const QString& fieldId) {
    return bookingRepository.getBookingsByDateAndField(date.toString("yyyy-MM-dd"), fieldId);
}

Bookings BookingManager::createBooking(
    const QString& fieldId,
    const QDate& bookingDate,
    const QString& customerName,
    const QString& phone,
    const QTime& startTime,
    const QTime& endTime,
    double deposit
) {
    QString cleanName = customerName.simplified();
    QString cleanPhone = phone.trimmed();

    if (cleanName.isEmpty()) {
        throw BookingError("Vui lòng nhập tên khách hàng.");
    }
    QRegularExpression phoneRe("^0\\d{9}$");
    if (!phoneRe.match(cleanPhone).hasMatch()) {
        throw BookingError("Số điện thoại cần có 10 chữ số và bắt đầu bằng số 0.");
    }
    if (startTime >= endTime) {
        throw BookingError("Giờ kết thúc phải lớn hơn giờ bắt đầu.");
    }
    if (startTime < QTime(6, 0) || endTime > QTime(22, 0)) {
        throw BookingError("Sân chỉ hoạt động từ 06:00 đến 22:00.");
    }
    if (QDateTime(bookingDate, startTime) < QDateTime::currentDateTime()) {
        throw BookingError("Không thể đặt sân trong quá khứ.");
    }
    if (deposit < 0) {
        throw BookingError("Tiền cọc không được là số âm.");
    }

    auto fieldOpt = fieldRepository.getById(fieldId); // get field by id
    if (!fieldOpt.has_value()) {
        throw BookingError("Không tìm thấy sân đã chọn.");
    }
    Fields field = fieldOpt.value(); // copy the field object
    if (field.status.trimmed().compare("Bảo trì", Qt::CaseInsensitive) == 0) // chekc if the field is under maintenance return false and message
    {
        throw BookingError("Sân đang bảo trì, không thể nhận đặt sân.");
    }

    double durationHours = startTime.secsTo(endTime) / 3600.0; // duration hours=(endTime-startTime)/3600
    double estimatedFee = durationHours * field.price_per_hour;
    if (deposit > estimatedFee) // check if deposit large than estimated fee
    {
        throw BookingError(QString("Tiền cọc (%L1 đ) không được vượt quá tiền thuê dự kiến (%L2 đ).")
                            .arg(deposit, 0, 'f', 0)
                            .arg(estimatedFee, 0, 'f', 0));
    }
    
    // check for booking conflicts with existing bookings for the same field and date
    for (const auto& existing : listBookings(bookingDate, fieldId)) {
        QTime exStart = QTime::fromString(existing.start_time, "HH:mm");
        QTime exEnd = QTime::fromString(existing.end_time, "HH:mm");
        if (startTime < exEnd && endTime > exStart) {
            throw BookingError(QString("Trùng lịch với đơn %1 (%2 - %3).")
                                .arg(existing.booking_id, existing.start_time, existing.end_time));
        }
    }
    
    // inilize booking object 
    Bookings b;
    b.booking_id = nextBookingId();
    b.field_id = field.field_id;
    b.customer_name = cleanName;
    b.customer_phone = cleanPhone;
    b.booking_date = bookingDate.toString("yyyy-MM-dd");
    b.start_time = startTime.toString("HH:mm");
    b.end_time = endTime.toString("HH:mm");
    b.deposit = deposit;
    b.status = (deposit > 0) ? "Đã cọc" : "Chờ thanh toán";
    b.field_name = field.name;
    b.price_per_hour = field.price_per_hour;

    if (!bookingRepository.add(b)) {
        throw BookingError("Không thể lưu đơn đặt sân. Vui lòng thử lại.");
    }
    return b;
}

Bookings BookingManager::cancelBooking(const QString& bookingId) {
    auto bookingOpt = bookingRepository.getById(bookingId);
    if (!bookingOpt.has_value()) {
        throw BookingError("Không tìm thấy đơn đặt sân.");
    }
    Bookings b = bookingOpt.value();
    if (b.status == "Đã hủy") {
        throw BookingError(QString("Đơn %1 đã được hủy trước đó.").arg(bookingId));
    }
    b.status = "Đã hủy";
    if (!bookingRepository.update(b)) {
        throw BookingError("Không thể cập nhật trạng thái hủy đơn.");
    }
    return b;
}