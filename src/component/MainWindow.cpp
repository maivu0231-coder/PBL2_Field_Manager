#include "mainwindow.h"

MainWindow::MainWindow(AuthManager* authManager, QWidget* parent) 
    : QMainWindow(parent), 
      m_authManager(authManager),
      statisticsManager(&paymentManager, &bookingManager) {

    QString role = m_authManager ? m_authManager->getCurrentUserRole() : "Admin";
    QString username = m_authManager ? m_authManager->getCurrentUsername() : "User";

    setWindowTitle(QString("Fields Manager - Hệ Thống Quản Lý Sân Bóng Mini | Tài khoản: %1 (%2)")
                   .arg(username, role));
    resize(1280, 820);

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    auto* banner = new QLabel(QString("🏟️ HỆ THỐNG QUẢN LÝ SÂN BÓNG MINI — XIN CHÀO: %1 [%2]")
                              .arg(username.toUpper(), role.toUpper()));
    banner->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0f172a, stop:1 #1e3a8a);"
        "color: white; font-size: 18px; font-weight: bold; padding: 14px; border-radius: 10px;"
    );
    layout->addWidget(banner);

    tabs = new QTabWidget();
    tabField = new TabField(&fieldService, this);
    tabBooking = new TabBooking(&bookingManager, this);
    tabPayment = new TabPayment(&paymentManager, this);
    tabStatistics = new TabStatistics(&statisticsManager, this);

    // Thêm các Tab vào giao diện
    tabs->addTab(tabBooking, "📅 Đặt Sân / Hủy Sân");
    tabs->addTab(tabPayment, "💰 Thanh Toán & Hóa Đơn");
    tabs->addTab(tabField, "⚽ Quản Lý Sân Bóng");
    tabs->addTab(tabStatistics, "📊 Thống Kê & Báo Cáo");

    // PHÂN QUYỀN (RBAC):
    // Nếu là Thu ngân (Cashier), vô hiệu hóa/ẩn các Tab dành riêng cho Admin
    if (role == "Cashier") {
        // Vô hiệu hóa Tab Quản Lý Sân & Tab Thống Kê
        int indexField = tabs->indexOf(tabField);
        int indexStats = tabs->indexOf(tabStatistics);
        
        if (indexField != -1) tabs->setTabEnabled(indexField, false);
        if (indexStats != -1) tabs->setTabEnabled(indexStats, false);

        // Hoặc có thể xóa hẳn Tab nếu muốn ẩn hoàn toàn:
        // tabs->removeTab(indexStats);
        // tabs->removeTab(indexField);
    }

    layout->addWidget(tabs);
    setCentralWidget(central);

    // Đồng bộ dữ liệu tự động giữa các Tab khi có thay đổi
    connect(tabField, &TabField::dataChanged, tabBooking, &TabBooking::reloadFields);
    connect(tabBooking, &TabBooking::bookingChanged, tabPayment, &TabPayment::refreshUnpaid);
    connect(tabPayment, &TabPayment::paymentCompleted, tabBooking, &TabBooking::refreshBookings);
}