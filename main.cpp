#include <QApplication>
#include "Src/Component/MainWindow.h"
#include "Src/Component/LoginDialog.h"
#include "Src/Core/AuthManager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Áp dụng giao diện (StyleSheet)
    app.setStyleSheet(
        "QGroupBox { font-weight: bold; border: 1px solid #cbd5e1; border-radius: 8px; margin-top: 10px; padding-top: 10px; background: white; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; color: #0f172a; }"
        "QPushButton { background-color: #2563eb; color: white; font-weight: bold; padding: 7px 14px; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #1d4ed8; }"
        "QPushButton:disabled { background-color: #94a3b8; }"
        "QLineEdit, QComboBox, QSpinBox, QDateEdit, QTimeEdit { padding: 5px; border: 1px solid #cbd5e1; border-radius: 5px; }"
        "QTabWidget::pane { border: 1px solid #cbd5e1; border-radius: 6px; }"
    );

    // 1. Khởi tạo AuthManager và nạp dữ liệu tài khoản
    AuthManager authManager;
    authManager.loadUsersFromJson("src/storage/users.json");

    // 2. Bật cửa sổ Đăng nhập trước
    LoginDialog loginDialog(&authManager);
    if (loginDialog.exec() == QDialog::Accepted) {
        // 3. Đăng nhập thành công -> Mở màn hình chính và truyền pointer AuthManager vào
        MainWindow window(&authManager);
        window.show();
        return app.exec();
    }

    // Nếu người dùng hủy hoặc tắt bảng đăng nhập thì thoát ứng dụng
    return 0;
}