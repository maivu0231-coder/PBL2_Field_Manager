#include "TabStatistics.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QHeaderView>
#include <QGroupBox>
#include <QToolTip>
#include <QCursor>
#include "../storage/Repositories.h"

TabStatistics::TabStatistics(StatisticsManager* statManager, QWidget* parent)
    : QWidget(parent), m_statManager(statManager) {
    
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto* scrollContent = new QWidget(scrollArea);
    auto* mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(14);

    // ================= 1. BỘ LỌC TIÊU CHÍ THỐNG KÊ =================
    auto* filterFrame = new QFrame(scrollContent);
    filterFrame->setStyleSheet(
        "QFrame { background: #ffffff; border: 1px solid #e2e8f0; border-radius: 12px; padding: 6px; }"
    );
    auto* filterLayout = new QHBoxLayout(filterFrame);
    filterLayout->setContentsMargins(12, 8, 12, 8);
    filterLayout->setSpacing(12);

    auto* lblFilterTitle = new QLabel("📊 <b>Tiêu chí thống kê doanh thu:</b>", filterFrame);
    lblFilterTitle->setStyleSheet("font-size: 13px; color: #1e293b; border: none;");
    filterLayout->addWidget(lblFilterTitle);

    cboStatMode = new QComboBox(filterFrame);
    cboStatMode->setStyleSheet(
        "QComboBox { background: #f8fafc; border: 1.5px solid #cbd5e1; border-radius: 8px; padding: 6px 14px; font-weight: bold; color: #1e3a8a; min-width: 320px; }"
    );
    cboStatMode->addItem("📅 Theo ngày đặt sân (Ngày thi đấu thực tế)");
    cboStatMode->addItem("💵 Theo ngày thanh toán (Ngày thu ngân xuất hóa đơn)");
    connect(cboStatMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TabStatistics::refreshData);
    filterLayout->addWidget(cboStatMode);

    filterLayout->addStretch();

    btnRefresh = new QPushButton("🔄 Làm Mới Dữ Liệu", filterFrame);
    btnRefresh->setCursor(Qt::PointingHandCursor);
    btnRefresh->setStyleSheet(
        "QPushButton { background-color: #2563eb; color: white; font-weight: bold; padding: 8px 18px; border-radius: 8px; }"
        "QPushButton:hover { background-color: #1d4ed8; }"
    );
    connect(btnRefresh, &QPushButton::clicked, this, &TabStatistics::refreshData);
    filterLayout->addWidget(btnRefresh);

    mainLayout->addWidget(filterFrame);

    // ================= 2. THANH KPI TỔNG QUAN =================
    auto* kpiLayout = new QHBoxLayout();
    kpiLayout->setSpacing(12);

    auto createKpiCard = [](const QString& iconTitle, QLabel*& valLabel, const QString& colorHex, const QString& subtitle) {
        auto* card = new QFrame();
        card->setStyleSheet(QString(
            "QFrame {"
            "  background: #ffffff;"
            "  border: 1px solid #e2e8f0;"
            "  border-left: 5px solid %1;"
            "  border-radius: 12px;"
            "}"
        ).arg(colorHex));
        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(14, 12, 14, 12);
        cardLayout->setSpacing(4);

        auto* lblTitle = new QLabel(iconTitle, card);
        lblTitle->setStyleSheet("font-size: 11px; font-weight: bold; color: #64748b; border: none; letter-spacing: 0.5px;");

        valLabel = new QLabel("0", card);
        valLabel->setStyleSheet(QString("font-size: 22px; font-weight: 800; color: %1; border: none;").arg(colorHex));

        auto* lblSub = new QLabel(subtitle, card);
        lblSub->setStyleSheet("font-size: 11px; color: #94a3b8; border: none;");

        cardLayout->addWidget(lblTitle);
        cardLayout->addWidget(valLabel);
        cardLayout->addWidget(lblSub);
        return card;
    };

    kpiLayout->addWidget(createKpiCard("💰 TỔNG DOANH THU", valTotalRevenue, "#059669", "Doanh thu tích lũy hệ thống"));
    kpiLayout->addWidget(createKpiCard("📄 HÓA ĐƠN ĐÃ THU", valTotalInvoices, "#2563eb", "Giao dịch thu ngân hoàn tất"));
    kpiLayout->addWidget(createKpiCard("📅 LƯỢT ĐẶT SÂN", valTotalBookings, "#d97706", "Lượt đặt hợp lệ thành công"));
    kpiLayout->addWidget(createKpiCard("⏰ GIỜ CAO ĐIỂM NHẤT", valPeakTime, "#7c3aed", "Khung giờ đông khách nhất"));

    mainLayout->addLayout(kpiLayout);

    // ================= 3. HAI BIỂU ĐỒ TRỰC QUAN =================
    auto* chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(14);

    revenueChartView = new QChartView(scrollContent);
    revenueChartView->setMinimumHeight(400);
    revenueChartView->setRenderHint(QPainter::Antialiasing);
    revenueChartView->setMouseTracking(true);
    revenueChartView->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 12px; padding: 6px;");

    peakChartView = new QChartView(scrollContent);
    peakChartView->setMinimumHeight(400);
    peakChartView->setRenderHint(QPainter::Antialiasing);
    peakChartView->setMouseTracking(true);
    peakChartView->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 12px; padding: 6px;");

    chartsLayout->addWidget(revenueChartView, 6);
    chartsLayout->addWidget(peakChartView, 4);

    mainLayout->addLayout(chartsLayout);

    // ================= 4. BẢNG CHI TIẾT DOANH THU THEO TỪNG NGÀY =================
    auto* grpTable = new QGroupBox("📋 Báo Cáo Doanh Thu Chi Tiết Theo Ngày", scrollContent);
    auto* tableLayout = new QVBoxLayout(grpTable);
    tableLayout->setContentsMargins(12, 14, 12, 12);

    tableRevenueDetails = new QTableWidget(0, 5, grpTable);
    tableRevenueDetails->setMinimumHeight(200);
    tableRevenueDetails->setHorizontalHeaderLabels({"📅 Ngày", "💵 Doanh Thu Thực Thu", "📊 Tỷ Trọng", "📄 Số Hóa Đơn", "ℹ️ Ghi Chú"});
    tableRevenueDetails->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableRevenueDetails->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableRevenueDetails->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableRevenueDetails->setSelectionMode(QAbstractItemView::SingleSelection);
    tableRevenueDetails->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableRevenueDetails->setAlternatingRowColors(true);
    tableLayout->addWidget(tableRevenueDetails);

    mainLayout->addWidget(grpTable);

    scrollArea->setWidget(scrollContent);
    outerLayout->addWidget(scrollArea);

    refreshData();
}

void TabStatistics::refreshData() {
    PaymentsRepository payRepo;
    BookingsRepository bookRepo;
    auto payments = payRepo.getAll();
    auto bookings = bookRepo.getAll();

    double totalRev = 0.0;
    for (const auto& p : payments) {
        totalRev += p.total_amount;
    }

    int validBookingsCount = 0;
    for (const auto& b : bookings) {
        if (b.status != "Đã hủy") {
            validBookingsCount++;
        }
    }

    auto peakData = m_statManager->getPeakHours();
    QString bestHour = "Chưa có";
    int maxCount = 0;
    for (auto it = peakData.begin(); it != peakData.end(); ++it) {
        if (it.value() > maxCount) {
            maxCount = it.value();
            bestHour = QString("%1:00").arg(it.key());
        }
    }

    valTotalRevenue->setText(QString("%L1 VNĐ").arg(totalRev, 0, 'f', 0));
    valTotalInvoices->setText(QString("%1 hóa đơn").arg(payments.size()));
    valTotalBookings->setText(QString("%1 lượt").arg(validBookingsCount));
    valPeakTime->setText(QString("%1 (%2 lượt)").arg(bestHour).arg(maxCount));

    bool byBookingDate = (cboStatMode ? (cboStatMode->currentIndex() == 0) : true);
    QMap<QString, double> revData = m_statManager->getRevenueByDate(byBookingDate);

    // Cập nhật biểu đồ doanh thu
    QChart* oldRevChart = revenueChartView->chart();
    QChart* newRevChart = createRevenueBarChart();
    revenueChartView->setChart(newRevChart);
    if (oldRevChart) delete oldRevChart;

    // Cập nhật biểu đồ giờ cao điểm
    QChart* oldPeakChart = peakChartView->chart();
    QChart* newPeakChart = createPeakHourPieChart();
    peakChartView->setChart(newPeakChart);
    if (oldPeakChart) delete oldPeakChart;

    // Cập nhật bảng chi tiết doanh thu từng ngày
    updateRevenueTable(revData, totalRev, byBookingDate);
}

QChart* TabStatistics::createRevenueBarChart() {
    auto* chart = new QChart();
    bool byBookingDate = (cboStatMode ? (cboStatMode->currentIndex() == 0) : true);

    chart->setTitle(byBookingDate ? "📊 Biểu Đồ Doanh Thu Theo Ngày Đặt Sân (Rê chuột xem số tiền)"
                                  : "📊 Biểu Đồ Doanh Thu Theo Ngày Thu Tiền (Rê chuột xem số tiền)");
    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundRoundness(10);

    QMap<QString, double> data = m_statManager->getRevenueByDate(byBookingDate);

    auto* set = new QBarSet("Doanh thu thực thu");
    set->setColor(QColor("#059669"));

    QStringList categories;
    double maxRevenue = 0.0;

    if (data.isEmpty()) {
        categories << QDate::currentDate().toString("yyyy-MM-dd");
        *set << 0.0;
    } else {
        for (auto it = data.begin(); it != data.end(); ++it) {
            categories << it.key();
            *set << it.value();
            if (it.value() > maxRevenue) maxRevenue = it.value();
        }
    }

    auto* series = new QBarSeries();
    series->append(set);
    series->setLabelsVisible(false); // Ẩn nhãn tĩnh để tạo hiệu ứng động hiện tiền khi di chuột
    chart->addSeries(series);

    // Bắt sự kiện hover trên biểu đồ cột: khi rê chuột vào cột nào thì hiện popup số tiền của cột đó
    connect(series, &QBarSeries::hovered, this, [this, categories, set](bool status, int index, QBarSet* barset) {
        if (status && index >= 0 && index < categories.size()) {
            QString date = categories[index];
            double amount = (barset && index < barset->count()) ? barset->at(index) : 0.0;
            QString tooltipText = QString("📅 <b>Ngày:</b> %1<br>💵 <b>Doanh thu:</b> <span style='color:#059669; font-size:14px; font-weight:bold;'>%L2 VNĐ</span>")
                                      .arg(date)
                                      .arg(amount, 0, 'f', 0);
            QToolTip::showText(QCursor::pos(), tooltipText, revenueChartView);
        } else {
            QToolTip::hideText();
        }
    });

    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    double yMax = (maxRevenue > 0) ? (maxRevenue * 1.25) : 500000.0;
    axisY->setRange(0, yMax);
    axisY->setLabelFormat("%d");
    axisY->setTitleText("Doanh thu (VNĐ)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    return chart;
}

QChart* TabStatistics::createPeakHourPieChart() {
    auto* chart = new QChart();
    chart->setTitle("🕒 Biểu Đồ Tỷ Lệ Giờ Cao Điểm (Rê chuột xem %)");
    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundRoundness(10);

    auto* series = new QPieSeries();

    QMap<QString, int> data = m_statManager->getPeakHours();

    if (data.isEmpty()) {
        auto* slice = series->append("Chưa có dữ liệu đặt sân", 1);
        slice->setColor(QColor("#cbd5e1"));
        slice->setLabelVisible(true);
    } else {
        const QList<QColor> colors = {
            QColor("#2563eb"), QColor("#059669"), QColor("#d97706"),
            QColor("#dc2626"), QColor("#7c3aed"), QColor("#0891b2"),
            QColor("#db2777"), QColor("#10b981")
        };
        int cIdx = 0;
        int totalHours = 0;
        for (int count : data) totalHours += count;

        for (auto it = data.begin(); it != data.end(); ++it) {
            double percent = (totalHours > 0) ? (static_cast<double>(it.value()) * 100.0 / totalHours) : 0.0;
            QString label = QString("%1:00").arg(it.key());
            auto* slice = series->append(label, it.value());
            slice->setColor(colors[cIdx % colors.size()]);
            slice->setLabelVisible(true);
            slice->setLabelColor(QColor("#0f172a"));
            slice->setLabelFont(QFont("Segoe UI", 9, QFont::Bold));
            cIdx++;
        }

        // Bắt sự kiện hover trên biểu đồ tròn: khi rê chuột vào lát cắt thì bung lát cắt và hiện %
        connect(series, &QPieSeries::hovered, this, [this, totalHours](QPieSlice* slice, bool state) {
            if (state && slice) {
                slice->setExploded(true);
                slice->setPen(QPen(QColor("#0f172a"), 2));
                double percent = (totalHours > 0) ? (slice->value() * 100.0 / totalHours) : 0.0;
                QString tooltipText = QString("⏰ <b>Khung giờ:</b> %1<br>"
                                              "📊 <b>Tỷ lệ chiếm:</b> <span style='color:#2563eb; font-size:15px; font-weight:bold;'>%2%</span><br>"
                                              "👥 <b>Số lượt đặt:</b> <b>%3</b> lượt")
                                          .arg(slice->label())
                                          .arg(percent, 0, 'f', 1)
                                          .arg(static_cast<int>(slice->value()));
                QToolTip::showText(QCursor::pos(), tooltipText, peakChartView);
            } else if (slice) {
                slice->setExploded(false);
                slice->setPen(QPen(Qt::white, 1));
                QToolTip::hideText();
            }
        });
    }

    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    return chart;
}

void TabStatistics::updateRevenueTable(const QMap<QString, double>& revenueData, double totalRevenue, bool byBookingDate) {
    PaymentsRepository payRepo;
    BookingsRepository bookRepo;
    auto payments = payRepo.getAll();
    auto bookings = bookRepo.getAll();

    QMap<QString, QString> bookingDateMap;
    for (const auto& b : bookings) {
        bookingDateMap[b.booking_id] = b.booking_date;
    }

    QMap<QString, int> invoiceCountMap;
    for (const auto& p : payments) {
        QString dateKey;
        if (byBookingDate) {
            if (!p.booking_date.isEmpty()) {
                dateKey = p.booking_date;
            } else if (bookingDateMap.contains(p.booking_id) && !bookingDateMap[p.booking_id].isEmpty()) {
                dateKey = bookingDateMap[p.booking_id];
            } else {
                dateKey = p.payment_time.left(10);
            }
        } else {
            dateKey = p.payment_time.left(10);
        }

        if (!dateKey.isEmpty()) {
            invoiceCountMap[dateKey]++;
        }
    }

    tableRevenueDetails->setRowCount(static_cast<int>(revenueData.size()));
    int row = 0;
    for (auto it = revenueData.begin(); it != revenueData.end(); ++it, ++row) {
        QString dateStr = it.key();
        double amount = it.value();
        double ratio = (totalRevenue > 0) ? (amount * 100.0 / totalRevenue) : 0.0;
        int invCount = invoiceCountMap.value(dateStr, 0);

        auto* itemDate = new QTableWidgetItem(dateStr);
        itemDate->setTextAlignment(Qt::AlignCenter);

        auto* itemAmount = new QTableWidgetItem(QString("%L1 VNĐ").arg(amount, 0, 'f', 0));
        itemAmount->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        itemAmount->setForeground(QBrush(QColor("#059669")));
        QFont font = itemAmount->font();
        font.setBold(true);
        itemAmount->setFont(font);

        auto* itemRatio = new QTableWidgetItem(QString("%1 %").arg(ratio, 0, 'f', 1));
        itemRatio->setTextAlignment(Qt::AlignCenter);
        itemRatio->setFont(font);

        auto* itemCount = new QTableWidgetItem(QString("%1 hóa đơn").arg(invCount));
        itemCount->setTextAlignment(Qt::AlignCenter);

        auto* itemNote = new QTableWidgetItem(byBookingDate ? "🟢 Đơn đặt sân hoàn tất" : "💵 Đã vào sổ thu ngân");
        itemNote->setTextAlignment(Qt::AlignCenter);
        itemNote->setForeground(QBrush(QColor("#2563eb")));

        tableRevenueDetails->setItem(row, 0, itemDate);
        tableRevenueDetails->setItem(row, 1, itemAmount);
        tableRevenueDetails->setItem(row, 2, itemRatio);
        tableRevenueDetails->setItem(row, 3, itemCount);
        tableRevenueDetails->setItem(row, 4, itemNote);
    }
}
