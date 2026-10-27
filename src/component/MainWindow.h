#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "TabField.h"
#include "TabBooking.h"
#include "TabPayment.h"
#include "TabStatistics.h"
#include "Src/Core/FieldManager.h"
#include "Src/Core/BookingManager.h"
#include "Src/Core/PaymentManager.h"
#include "Src/Core/StatisticsManager.h"
#include "Src/Core/AuthManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    FieldService fieldService;
    BookingManager bookingManager;
    PaymentManager paymentManager;
    
    // Lớp quản lý phân quyền & thống kê
    AuthManager* m_authManager;
    StatisticsManager statisticsManager;

    QTabWidget* tabs;
    TabField* tabField;
    TabBooking* tabBooking;
    TabPayment* tabPayment;
    TabStatistics* tabStatistics;

public:
    explicit MainWindow(AuthManager* authManager, QWidget* parent = nullptr);
};