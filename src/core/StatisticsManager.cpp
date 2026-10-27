#include"StatisticsManager.h"
StatisticsManager::StatisticsManager()
{}
//statistics revenue by date
QMap<QString,double>StatisticsManager::getRevenueByDate() const
{
    QMap<QString,double>revenueByDate;
    PaymentsRepository paymentsRepository;
    for(const auto&payment:paymentsRepository.getAll())
    {
        revenueByDate[payment.payment_time.left(10)]+=payment.total_amount;// add the total amount of each payment to the corresponding date in the map
    }
    return revenueByDate;
}
//statistics peak hours
QMap<QString,int>StatisticsManager::getPeakHours() const
{
    QMap<QString,int>peakHours;
    BookingsRepository bookingsRepository;
    for(const auto&booking:bookingsRepository.getAll())
    {
        int startHour=QTime::fromString(booking.start_time,"HH:mm").hour();
        int endHour=QTime::fromString(booking.end_time,"HH:mm").hour();
        for(int hour=startHour;hour<endHour;hour++)
        {
            peakHours[QString("%1").arg(hour,2,10,QChar('0'))]++;// increment the count for each hour in the booking range
        }
    }
    return peakHours;
}