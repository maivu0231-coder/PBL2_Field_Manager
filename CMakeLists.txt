cmake_minimum_required(VERSION 3.16)

project(FieldsManagerQt VERSION 1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

# Source files
set(SOURCES
    # Main
    main.cpp

    # Core
    src/core/AuthManger.cpp
    src/core/BookingManager.cpp
    src/core/FieldManager.cpp
    src/core/PaymentManager.cpp
    src/core/StatisticsManager.cpp

    # UI components
    src/component/LoginDialog.cpp
    src/component/MainWindow.cpp
    src/component/TabBooking.cpp
    src/component/TabField.cpp
    src/component/TabPayment.cpp
    src/component/TabStatistics.cpp
    src/component/TabAccount.cpp
    src/component/ReceiptDialog.cpp
)

# Header files
set(HEADERS
    # Core
    src/core/AuthManager.h
    src/core/BookingManager.h
    src/core/FieldManager.h
    src/core/PaymentManager.h
    src/core/StatisticsManager.h

    # UI components
    src/component/LoginDialog.h
    src/component/MainWindow.h
    src/component/TabBooking.h
    src/component/TabField.h
    src/component/TabPayment.h
    src/component/TabStatistics.h
    src/component/TabAccount.h
    src/component/ReceiptDialog.h

    # Models
    src/models/Base.h
    src/models/Bookings.h
    src/models/Fields.h
    src/models/Payments.h
    src/models/User.h

    # Storage
    src/storage/HashTable.h
    src/storage/JsonHandler.h
    src/storage/Repositories.h
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Charts
)
