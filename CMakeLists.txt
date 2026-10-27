cmake_minimum_required(VERSION 3.16)
project(FieldsManagerQt VERSION 1.0 LANGUAGES CXX)

# Thiết lập chuẩn C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tự động hóa các công cụ của Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Tìm thư viện Qt6 (hoặc thay bằng Qt5 nếu bạn dùng Qt5)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Liệt kê tất cả các file nguồn dựa theo cấu trúc thư mục src/ của bạn
set(SOURCES
    src/main.cpp
    # Thêm các file Core
    src/core/BookingManager.h
    src/core/FieldManager.h
    src/core/PaymentManager.h
    # Thêm các file Component (UI)
    src/component/MainWindow.h
    src/component/TabBooking.h
    src/component/TabField.h
    src/component/TabPayment.h
    # Thêm các file Storage & Models (bạn liệt kê các file .h/.cpp thực tế tại đây)
    src/storage/HashTable.h
    src/storage/JsonHandler.h
    src/storage/Repositories.h
)

# Tạo file thực thi ứng dụng
add_executable(FieldsManagerQt ${SOURCES})

# Liên kết thư viện Qt Widgets vào chương trình
target_link_libraries(FieldsManagerQt PRIVATE Qt6::Widgets)
# Tìm kiếm các module cần thiết của Qt6 (thêm Charts vào đây)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

# Liên kết thư viện Charts vào file thực thi của bạn
target_link_libraries(FieldsManagerQt PRIVATE Qt6::Widgets Qt6::Charts)