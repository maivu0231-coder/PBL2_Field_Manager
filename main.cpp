#include <QApplication>
#include <QFont>
#include <QIcon>
#include "src/component/MainWindow.h"
#include "src/component/LoginDialog.h"
#include "src/core/AuthManager.h"
#include "src/storage/Repositories.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Áp dụng font chữ Segoe UI sắc nét tiêu chuẩn Windows 11
    QFont appFont("Segoe UI", 10);
    app.setFont(appFont);

    // Áp dụng hệ thống Style Sheet đồng bộ, hiện đại chuẩn SaaS Dashboard
    app.setStyleSheet(
        "QWidget { font-family: 'Segoe UI', Tahoma, sans-serif; color: #0f172a; }"
        "QMainWindow { background-color: #f1f5f9; }"
        
        // GroupBox Cards
        "QGroupBox {"
        "  font-weight: 700; font-size: 13px;"
        "  border: 1px solid #e2e8f0;"
        "  border-radius: 12px;"
        "  margin-top: 14px;"
        "  padding: 16px 12px 12px 12px;"
        "  background: #ffffff;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 14px;"
        "  padding: 2px 8px;"
        "  color: #1e293b;"
        "  font-size: 13px;"
        "  font-weight: 700;"
        "  background: #ffffff;"
        "  border-radius: 4px;"
        "}"

        // Buttons
        "QPushButton {"
        "  background-color: #2563eb;"
        "  color: #ffffff;"
        "  font-weight: 600;"
        "  font-size: 13px;"
        "  padding: 8px 18px;"
        "  border-radius: 8px;"
        "  border: none;"
        "  min-height: 22px;"
        "}"
        "QPushButton:hover { background-color: #1d4ed8; }"
        "QPushButton:pressed { background-color: #1e40af; }"
        "QPushButton:disabled { background-color: #cbd5e1; color: #94a3b8; }"

        // Inputs
        "QLineEdit, QComboBox, QSpinBox, QDateEdit, QTimeEdit {"
        "  background-color: #ffffff;"
        "  color: #0f172a;"
        "  padding: 7px 12px;"
        "  border: 1.5px solid #cbd5e1;"
        "  border-radius: 8px;"
        "  font-size: 13px;"
        "  selection-background-color: #bfdbfe;"
        "  selection-color: #1e3a8a;"
        "}"
        "QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDateEdit:focus, QTimeEdit:focus {"
        "  border: 2px solid #2563eb;"
        "  background-color: #ffffff;"
        "}"
        "QComboBox::drop-down {"
        "  subcontrol-origin: padding;"
        "  subcontrol-position: top right;"
        "  width: 26px;"
        "  border-left: 1px solid #e2e8f0;"
        "  border-top-right-radius: 8px;"
        "  border-bottom-right-radius: 8px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background: #ffffff;"
        "  border: 1px solid #cbd5e1;"
        "  border-radius: 8px;"
        "  selection-background-color: #eff6ff;"
        "  selection-color: #1d4ed8;"
        "  padding: 4px;"
        "}"

        // Tables
        "QTableWidget {"
        "  background-color: #ffffff;"
        "  border: 1px solid #e2e8f0;"
        "  border-radius: 10px;"
        "  gridline-color: #f1f5f9;"
        "  alternate-background-color: #f8fafc;"
        "  font-size: 13px;"
        "}"
        "QTableWidget::item {"
        "  padding: 8px 10px;"
        "  border-bottom: 1px solid #f1f5f9;"
        "}"
        "QTableWidget::item:selected {"
        "  background-color: #eff6ff;"
        "  color: #1e40af;"
        "  font-weight: 600;"
        "}"
        "QHeaderView::section {"
        "  background-color: #f8fafc;"
        "  color: #475569;"
        "  font-weight: 700;"
        "  font-size: 12px;"
        "  padding: 10px 8px;"
        "  border: none;"
        "  border-bottom: 2px solid #cbd5e1;"
        "}"

        // Lists
        "QListWidget {"
        "  background-color: #ffffff;"
        "  border: 1px solid #e2e8f0;"
        "  border-radius: 10px;"
        "  padding: 6px;"
        "}"
        "QListWidget::item {"
        "  padding: 10px 12px;"
        "  border-radius: 8px;"
        "  margin-bottom: 5px;"
        "  border: 1px solid #f1f5f9;"
        "}"
        "QListWidget::item:hover {"
        "  background-color: #f8fafc;"
        "  border-color: #cbd5e1;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #eff6ff;"
        "  color: #1d4ed8;"
        "  font-weight: 700;"
        "  border: 1.5px solid #93c5fd;"
        "}"

        // Scrollbars
        "QScrollBar:vertical {"
        "  border: none;"
        "  background: transparent;"
        "  width: 8px;"
        "  border-radius: 4px;"
        "  margin: 2px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: #cbd5e1;"
        "  border-radius: 4px;"
        "  min-height: 24px;"
        "}"
        "QScrollBar::handle:vertical:hover { background: #94a3b8; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        "QScrollBar:horizontal {"
        "  border: none;"
        "  background: transparent;"
        "  height: 8px;"
        "  border-radius: 4px;"
        "  margin: 2px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "  background: #cbd5e1;"
        "  border-radius: 4px;"
        "  min-width: 24px;"
        "}"
        "QScrollBar::handle:horizontal:hover { background: #94a3b8; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }"

        // Tooltips
        "QToolTip {"
        "  background-color: #0f172a;"
        "  color: #ffffff;"
        "  border: 1px solid #334155;"
        "  border-radius: 6px;"
        "  padding: 6px 10px;"
        "  font-size: 12px;"
        "}"
    );

    // Khởi tạo AuthManager và nạp dữ liệu tài khoản từ thư mục Data
    AuthManager authManager;
    QString usersPath = resolveDataDir() + "/users.json";
    authManager.loadUsersFromJson(usersPath);

    int retCode = 0;
    do {
        // Mở cửa sổ Đăng nhập với giao diện card hiện đại
        LoginDialog loginDialog(&authManager);
        if (loginDialog.exec() != QDialog::Accepted) {
            break;
        }

        // Đăng nhập thành công -> Mở màn hình chính
        MainWindow window(&authManager);
        window.show();
        retCode = app.exec();

    } while (retCode == 1000); // 1000 là mã tín hiệu Đăng xuất để hiển thị lại LoginDialog

    return 0;
}
