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
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QFont>
#include "Src/Core/PaymentManager.h"

class TabPayment : public QWidget {
    Q_OBJECT
private:
    PaymentManager* pm;
    std::vector<Bookings> unpaidList;

    QLineEdit* txtSearchName;
    QLabel* lblSearchResult;
    QListWidget* listUnpaid;
    QSpinBox* spnSurcharge;
    QLabel* lblInvoiceDetails;
    QPushButton* btnPay;
    QPlainTextEdit* txtReceipt;
    QTableWidget* tableHistory;

    void setupUi();

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
};

#endif // TABPAYMENT_H