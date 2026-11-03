#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QTimer>
#include "TabField.h"
#include "TabBooking.h"
#include "TabPayment.h"
#include "TabStatistics.h"
#include "TabAccount.h"
#include "../core/FieldManager.h"
#include "../core/BookingManager.h"
#include "../core/PaymentManager.h"
#include "../core/StatisticsManager.h"
#include "../core/AuthManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    FieldManager fieldService;
    BookingManager bookingManager;
    PaymentManager paymentManager;
    
    // Lớp quản lý phân quyền & thống kê
    AuthManager* m_authManager;
    StatisticsManager statisticsManager;

    QTabWidget* tabs;
    TabBooking* tabBooking;
    TabPayment* tabPayment;
    TabField* tabField;
    TabStatistics* tabStatistics;
    TabAccount* tabAccount;

    // Header elements
    QLabel* lblClock;
    QTimer* clockTimer;
    QPushButton* btnUserAvatar;
    QPushButton* btnHeaderLogout;

public:
    explicit MainWindow(AuthManager* authManager, QWidget* parent = nullptr);

signals:
    void requestLogout();

private slots:
    void onAvatarClicked();
    void onLogoutClicked();
    void onTabChanged(int index);
    void updateClock();
};
