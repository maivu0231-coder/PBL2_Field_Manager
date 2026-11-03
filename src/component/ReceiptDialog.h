#pragma once

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include "../models/Payments.h"

class ReceiptDialog : public QDialog {
    Q_OBJECT
public:
    explicit ReceiptDialog(const Payments& payment, QWidget* parent = nullptr);
};
