#ifndef TAB_STATISTICS_H
#define TAB_STATISTICS_H

#include <QWidget>
#include <QtCharts>
#include "../core/StatisticsManager.h"

class TabStatistics : public QWidget {
    _OBJECT
private:
    StatisticsManager* m_statManager;

    QChart* createRevenueBarChart();
    QChart* createPeakHourPieChart();

public:
    TabStatistics(StatisticsManager* statManager, QWidget* parent = nullptr);
};

#endif // TAB_STATISTICS_H