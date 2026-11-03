#include "MainWindow.h"
#include <QFrame>
#include <QMessageBox>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QDateTime>
#include <QStatusBar>

MainWindow::MainWindow(AuthManager* authManager, QWidget* parent) 
    : QMainWindow(parent), 
      m_authManager(authManager),
      statisticsManager(&paymentManager, &bookingManager),
      tabAccount(nullptr),
      clockTimer(nullptr) {

    QString role = m_authManager ? m_authManager->getCurrentUserRole() : "Admin";
    QString username = m_authManager ? m_authManager->getCurrentUsername() : "User";
    bool isAdmin = (role.compare("Admin", Qt::CaseInsensitive) == 0);
    QString roleVietnamese = isAdmin ? "Quản Trị Viên" : "Thu Ngân";

    setWindowTitle(QString("Fields Manager Pro - Phần Mềm Quản Lý Sân Bóng Mini Thông Minh | [%1 - %2]")
                   .arg(username, roleVietnamese));
    setMinimumSize(1200, 780);
    resize(1360, 880);

    auto* central = new QWidget(this);
    central->setStyleSheet("background-color: #f1f5f9;");
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(12);

    // ================= HEADER BAR SANG TRỌNG & HIỆN ĐẠI =================
    auto* headerFrame = new QFrame(this);
    headerFrame->setStyleSheet(
        "QFrame#HeaderFrame {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0f172a, stop:0.45 #1e293b, stop:1 #1e3a8a);"
        "  border-radius: 12px;"
        "  border: 1px solid rgba(255, 255, 255, 0.1);"
        "}"
    );
    headerFrame->setObjectName("HeaderFrame");
    auto* headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(18, 10, 18, 10);
    headerLayout->setSpacing(14);

    // 1. Tiêu đề & Logo bên trái
    auto* titleContainer = new QWidget(headerFrame);
    titleContainer->setStyleSheet("background: transparent;");
    auto* titleLayout = new QHBoxLayout(titleContainer);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);

    auto* lblLogo = new QLabel("⚽", titleContainer);
    lblLogo->setStyleSheet("font-size: 26px; background: transparent; border: none;");

    auto* lblTitle = new QLabel(
        "<span style='color: #ffffff; font-size: 19px; font-weight: 800; letter-spacing: 0.5px;'>FIELDS MANAGER PRO</span>"
        "<br><span style='color: #94a3b8; font-size: 11px; font-weight: 500;'>HỆ THỐNG QUẢN LÝ SÂN BÓNG MINI THÔNG MINH</span>",
        titleContainer
    );
    lblTitle->setStyleSheet("background: transparent; border: none;");

    titleLayout->addWidget(lblLogo);
    titleLayout->addWidget(lblTitle);
    headerLayout->addWidget(titleContainer);

    headerLayout->addStretch(1);

    // 2. Đồng hồ kỹ thuật số trung tâm (Real-time Clock)
    lblClock = new QLabel(headerFrame);
    lblClock->setStyleSheet(
        "QLabel {"
        "  background: rgba(15, 23, 42, 0.6);"
        "  color: #38bdf8;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "  padding: 6px 16px;"
        "  border-radius: 20px;"
        "  border: 1px solid rgba(56, 189, 248, 0.3);"
        "}"
    );
    lblClock->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(lblClock);

    headerLayout->addStretch(1);

    // 3. Status Badge: Hệ thống hoạt động
    auto* lblOnline = new QLabel("🟢 Trực Tuyến", headerFrame);
    lblOnline->setStyleSheet(
        "QLabel {"
        "  background: rgba(16, 185, 129, 0.15);"
        "  color: #34d399;"
        "  font-size: 11px;"
        "  font-weight: bold;"
        "  padding: 5px 12px;"
        "  border-radius: 12px;"
        "  border: 1px solid rgba(52, 211, 153, 0.4);"
        "}"
    );
    headerLayout->addWidget(lblOnline);

    // 4. User profile pill (Avatar + Name + Role Badge)
    auto* userPill = new QFrame(headerFrame);
    userPill->setStyleSheet(
        "QFrame {"
        "  background: rgba(255, 255, 255, 0.08);"
        "  border: 1px solid rgba(255, 255, 255, 0.15);"
        "  border-radius: 22px;"
        "  padding: 2px;"
        "}"
    );
    auto* userPillLayout = new QHBoxLayout(userPill);
    userPillLayout->setContentsMargins(6, 4, 10, 4);
    userPillLayout->setSpacing(8);

    QString initialChar = username.isEmpty() ? "U" : username.left(1).toUpper();
    btnUserAvatar = new QPushButton(initialChar, userPill);
    btnUserAvatar->setFixedSize(34, 34);
    btnUserAvatar->setCursor(Qt::PointingHandCursor);
    btnUserAvatar->setToolTip(QString("Tài khoản: %1 (%2)\nNhấp để xem tùy chọn").arg(username, roleVietnamese));
    btnUserAvatar->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #818cf8, stop:1 #4f46e5);"
        "  color: white;"
        "  font-size: 15px;"
        "  font-weight: 800;"
        "  border: 2px solid #ffffff;"
        "  border-radius: 17px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #a5b4fc, stop:1 #6366f1);"
        "}"
    );

    QString roleBadgeColor = isAdmin ? "#f59e0b" : "#10b981";
    auto* lblUserInfo = new QLabel(
        QString("<span style='color: white; font-weight: bold; font-size: 13px;'>%1</span><br>"
                "<span style='color: %2; font-size: 10px; font-weight: 700;'>%3</span>")
        .arg(username, roleBadgeColor, roleVietnamese.toUpper()),
        userPill
    );
    lblUserInfo->setStyleSheet("background: transparent; border: none;");

    userPillLayout->addWidget(btnUserAvatar);
    userPillLayout->addWidget(lblUserInfo);
    headerLayout->addWidget(userPill);

    // 5. Nút đăng xuất trực tiếp trên Header
    btnHeaderLogout = new QPushButton("🚪 Đăng Xuất", headerFrame);
    btnHeaderLogout->setCursor(Qt::PointingHandCursor);
    btnHeaderLogout->setStyleSheet(
        "QPushButton {"
        "  background: rgba(239, 68, 68, 0.15);"
        "  color: #fca5a5;"
        "  font-size: 12px;"
        "  font-weight: bold;"
        "  padding: 7px 14px;"
        "  border-radius: 8px;"
        "  border: 1px solid rgba(248, 113, 113, 0.4);"
        "}"
        "QPushButton:hover {"
        "  background: #dc2626;"
        "  color: white;"
        "  border: 1px solid #dc2626;"
        "}"
    );
    headerLayout->addWidget(btnHeaderLogout);

    layout->addWidget(headerFrame);

    // ================= TABS CHÍNH CỦA ỨNG DỤNG =================
    tabs = new QTabWidget(this);
    tabs->setStyleSheet(
        "QTabWidget::pane {"
        "  border: 1px solid #e2e8f0;"
        "  border-radius: 12px;"
        "  background: #ffffff;"
        "  top: -1px;"
        "}"
        "QTabBar::tab {"
        "  background: #e2e8f0;"
        "  color: #475569;"
        "  font-size: 13px;"
        "  font-weight: 700;"
        "  padding: 10px 22px;"
        "  border: 1px solid #cbd5e1;"
        "  border-bottom: none;"
        "  border-top-left-radius: 10px;"
        "  border-top-right-radius: 10px;"
        "  margin-right: 4px;"
        "  min-height: 24px;"
        "}"
        "QTabBar::tab:selected {"
        "  background: #ffffff;"
        "  color: #1d4ed8;"
        "  border-top: 3.5px solid #2563eb;"
        "  border-bottom: 1px solid #ffffff;"
        "}"
        "QTabBar::tab:hover:!selected {"
        "  background: #f1f5f9;"
        "  color: #0f172a;"
        "}"
    );

    tabBooking = new TabBooking(&bookingManager, this);
    tabPayment = new TabPayment(&paymentManager, this);
    tabField = new TabField(&fieldService, this);
    tabStatistics = new TabStatistics(&statisticsManager, this);

    tabs->addTab(tabBooking, "📅 Đặt Sân & Lịch Thi Đấu");
    tabs->addTab(tabPayment, "💳 Thanh Toán & Hóa Đơn");

    // Phân quyền RBAC
    if (isAdmin) {
        tabs->addTab(tabField, "⚽ Quản Lý Sân Bóng");
        tabs->addTab(tabStatistics, "📊 Báo Cáo & Thống Kê");

        tabAccount = new TabAccount(m_authManager, this);
        tabs->addTab(tabAccount, "👥 Quản Lý Tài Khoản");
    }

    layout->addWidget(tabs);
    setCentralWidget(central);

    // ================= BOTTOM STATUS BAR =================
    auto* statusBar = this->statusBar();
    statusBar->setStyleSheet(
        "QStatusBar { background: #ffffff; color: #64748b; font-size: 12px; border-top: 1px solid #e2e8f0; padding: 4px 10px; }"
    );
    statusBar->showMessage(QString("🟢 Cơ sở dữ liệu: Hoạt động bình thường | Đăng nhập với quyền: %1 | Fields Manager Pro v2.5")
                          .arg(roleVietnamese));

    // Khởi tạo đồng hồ thời gian thực
    updateClock();
    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    clockTimer->start(1000);

    // Kết nối các tín hiệu đồng bộ giữa các Tab
    connect(tabField, &TabField::dataChanged, tabBooking, &TabBooking::reloadFields);
    connect(tabBooking, &TabBooking::bookingChanged, tabPayment, &TabPayment::refreshUnpaid);
    connect(tabPayment, &TabPayment::paymentCompleted, tabBooking, &TabBooking::refreshBookings);
    connect(tabPayment, &TabPayment::paymentCompleted, tabStatistics, &TabStatistics::refreshData);

    // Sự kiện chuyển Tab để làm mới dữ liệu tự động
    connect(tabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    // Sự kiện click nút Avatar tròn & nút Logout
    connect(btnUserAvatar, &QPushButton::clicked, this, &MainWindow::onAvatarClicked);
    connect(btnHeaderLogout, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
}

void MainWindow::updateClock() {
    QDateTime now = QDateTime::currentDateTime();
    static const QString days[] = {"Chủ Nhật", "Thứ Hai", "Thứ Ba", "Thứ Tư", "Thứ Năm", "Thứ Sáu", "Thứ Bảy"};
    int dayOfWeek = now.date().dayOfWeek() % 7;
    QString dayName = days[dayOfWeek];

    lblClock->setText(QString("📅 %1, %2   •   🕒 %3")
                      .arg(dayName)
                      .arg(now.toString("dd/MM/yyyy"))
                      .arg(now.toString("HH:mm:ss")));
}

void MainWindow::onAvatarClicked() {
    QString role = m_authManager ? m_authManager->getCurrentUserRole() : "Admin";
    QString username = m_authManager ? m_authManager->getCurrentUsername() : "User";
    QString roleVietnamese = (role.compare("Admin", Qt::CaseInsensitive) == 0) ? "Quản Trị Viên (Admin)" : "Thu Ngân (Cashier)";

    auto* menu = new QMenu(this);
    menu->setStyleSheet(
        "QMenu { background: white; border: 1px solid #cbd5e1; border-radius: 10px; padding: 6px; font-size: 13px; }"
        "QMenu::item { padding: 9px 20px; border-radius: 6px; color: #1e293b; }"
        "QMenu::item:selected { background-color: #eff6ff; color: #1d4ed8; font-weight: bold; }"
        "QMenu::separator { height: 1px; background: #e2e8f0; margin: 5px 0; }"
    );

    auto* actUser = menu->addAction(QString("👤 Người dùng: %1").arg(username));
    actUser->setEnabled(false);

    auto* actRole = menu->addAction(QString("🛡️ Phân quyền: %1").arg(roleVietnamese));
    actRole->setEnabled(false);

    menu->addSeparator();

    auto* actLogout = menu->addAction("🚪 Đăng Xuất");
    connect(actLogout, &QAction::triggered, this, &MainWindow::onLogoutClicked);

    QPoint pos = btnUserAvatar->mapToGlobal(QPoint(0, btnUserAvatar->height() + 6));
    menu->exec(pos);
}

void MainWindow::onTabChanged(int index) {
    QWidget* current = tabs->widget(index);
    if (current == tabStatistics) {
        tabStatistics->refreshData();
    } else if (current == tabPayment) {
        tabPayment->refreshUnpaid();
    } else if (current == tabBooking) {
        tabBooking->reloadFields();
    } else if (tabAccount && current == tabAccount) {
        tabAccount->refreshData();
    }
}

void MainWindow::onLogoutClicked() {
    auto reply = QMessageBox::question(this, "Xác nhận đăng xuất",
                                       "Bạn có chắc chắn muốn đăng xuất tài khoản hiện tại không?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (clockTimer) clockTimer->stop();
        m_authManager->logout();
        emit requestLogout();
        qApp->exit(1000);
    }
}
