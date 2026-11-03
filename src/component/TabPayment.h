#ifndef TABPAYMENT_H
#define TABPAYMENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QListWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <vector>
#include "../core/PaymentManager.h"
#include "ReceiptDialog.h"

class TabPayment : public QWidget {
    Q_OBJECT
private:
    PaymentManager* pm;
    std::vector<Bookings> unpaidList;
    std::vector<Payments> currentPayments;

    QLineEdit* txtSearchName;
    QPushButton* btnSearch;
    QLabel* lblSearchResult;
    QListWidget* listUnpaid;

    // Dịch vụ phụ phí
    QSpinBox* spnWater;     // Nước suối (10k)
    QSpinBox* spnRevive;    // Nước điện giải (15k)
    QSpinBox* spnBall;      // Thuê bóng (30k)
    QSpinBox* spnBib;       // Thuê áo bib (20k)
    QSpinBox* spnSocks;     // Tất/vớ thể thao (20k)
    QSpinBox* spnLight;     // Đèn chiếu sáng (50k)
    QLabel* lblTotalServiceFee;
    QPushButton* btnResetServices;

    QLabel* lblInvoiceDetails;
    QPushButton* btnPay;
    QTableWidget* tableHistory;

    void setupUi();
    double calculateTotalSurcharge() const;

public:
    explicit TabPayment(PaymentManager* paymentMgr, QWidget* parent = nullptr);

signals:
    void paymentCompleted();

public slots:
    void refreshUnpaid();
    void refreshHistoryTable();

private slots:
    void onSearchByName();
    void updatePreview();
    void onPayClicked();
    void onHistoryItemClicked(int row, int column);
    void resetServices();
};

#endif // TABPAYMENT_H
