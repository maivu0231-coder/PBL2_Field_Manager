#ifndef TAB_STATISTICS_H
#define TAB_STATISTICS_H

#include <QWidget>
#include <QtCharts>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTableWidget>
#include "../core/StatisticsManager.h"

class TabStatistics : public QWidget {
    Q_OBJECT
private:
    StatisticsManager* m_statManager;

    QLabel* valTotalRevenue;
    QLabel* valTotalInvoices;
    QLabel* valTotalBookings;
    QLabel* valPeakTime;
    QPushButton* btnRefresh;
    QComboBox* cboStatMode;

    QChartView* revenueChartView;
    QChartView* peakChartView;
    QTableWidget* tableRevenueDetails;

    QChart* createRevenueBarChart();
    QChart* createPeakHourPieChart();
    void updateRevenueTable(const QMap<QString, double>& revenueData, double totalRevenue, bool byBookingDate);

public:
    TabStatistics(StatisticsManager* statManager, QWidget* parent = nullptr);

public slots:
    void refreshData();
};

#endif // TAB_STATISTICS_H
