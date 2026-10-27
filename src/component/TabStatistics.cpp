#include "TabStatistics.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

TabStatistics::TabStatistics(StatisticsManager* statManager, QWidget* parent)
    : QWidget(parent), m_statManager(statManager) {
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* chartsLayout = new QHBoxLayout();

    // 1. Biểu đồ cột Doanh thu
    QChartView* revenueChartView = new QChartView(createRevenueBarChart());
    revenueChartView->setRenderHint(QPainter::Antialiasing);

    // 2. Biểu đồ tròn Khung giờ
    QChartView* peakChartView = new QChartView(createPeakHourPieChart());
    peakChartView->setRenderHint(QPainter::Antialiasing);

    chartsLayout->addWidget(revenueChartView);
    chartsLayout->addWidget(peakChartView);

    mainLayout->addLayout(chartsLayout);
}

QChart* TabStatistics::createRevenueBarChart() {
    QBarSet* set = new QBarSet("Doanh thu (VNND)");
    
    QMap<QString, double> data = m_statManager->getRevenueByDate();
    QStringList categories;

    for (auto it = data.begin(); it != data.end(); ++it) {
        *set << it.value();
        categories << it.key();
    }

    QBarSeries* series = new QBarSeries();
    series->append(set);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Thống kê doanh thu theo ngày");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    ValueAxis* axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    return chart;
}

QChart* TabStatistics::createPeakHourPieChart() {
    QPieSeries* series = new QPieSeries();
    
    QMap<QString, int> data = m_statManager->getPeakHours();
    for (auto it = data.begin(); it != data.end(); ++it) {
        series->append(it.key(), it.value());
    }

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Tỷ lệ khung giờ khách đặt sân");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    return chart;
}