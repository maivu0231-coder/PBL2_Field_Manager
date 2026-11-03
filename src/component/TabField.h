#ifndef TABFIELD_H
#define TABFIELD_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>

#include "../core/FieldManager.h"

class TabField : public QWidget {
    Q_OBJECT

private:
    FieldManager* service;

    QLabel* lblTotal;
    QLabel* lblAvail;
    QLabel* lblInUse;
    QLabel* lblMaint;

    QLineEdit* txtSearch;
    QComboBox* cboFilterStatus;
    QComboBox* cboFilterType;
    QTableWidget* tableFields;

    // Update form
    QComboBox* cboEditCode;
    QLineEdit* txtEditName;
    QComboBox* cboEditType;
    QSpinBox* spnEditPrice;
    QComboBox* cboEditStatus;

    // Add and delete form
    QLineEdit* txtAddCode;
    QLineEdit* txtAddName;
    QComboBox* cboAddType;
    QSpinBox* spnAddPrice;
    QComboBox* cboAddStatus;
    QComboBox* cboDelCode;

    void setupUi();

public:
    explicit TabField(FieldManager* srv, QWidget* parent = nullptr);

signals:
    void dataChanged();

public slots:
    void refreshData();

private slots:
    void loadSelectedFieldToEditForm();
    void onUpdateField();
    void onAddField();
    void onDeleteField();
};

#endif // TABFIELD_H
