#ifndef TAB_BOOKING_H
#define TAB_BOOKING_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QDateEdit>
#include <QTimeEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include "Src/Core/BookingManager.h"

class TabBooking : public QWidget {
    Q_OBJECT
private:
    BookingManager* manager;

    QDateEdit* dateEdit;
    QComboBox* cboField;
    QLabel* lblFieldNotice;

    QLineEdit* txtCustomer;
    QLineEdit* txtPhone;
    QTimeEdit* timeStart;
    QTimeEdit* timeEnd;
    QSpinBox* spnDeposit;
    QPushButton* btnCreate;

    QTableWidget* tableBookings;
    QComboBox* cboCancelBooking;
    QPushButton* btnCancel;

    void setupUi();

public:
    explicit TabBooking(BookingManager* mgr, QWidget* parent = nullptr);

signals:
    void bookingChanged();

public slots:
    void reloadFields();
    void refreshBookings();

private slots:
    void onCreateBooking();
    void onCancelBooking();
};

#endif // TAB_BOOKING_H