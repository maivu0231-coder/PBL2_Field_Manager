#include "ReceiptDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

ReceiptDialog::ReceiptDialog(const Payments& p, QWidget* parent)
    : QDialog(parent) {
    setWindowTitle(QString("Biên Lai Hóa Đơn - %1").arg(p.invoice_id));
    setFixedSize(440, 620);
    setStyleSheet("background-color: #f1f5f9; font-family: 'Segoe UI', Tahoma, sans-serif;");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(14);

    // Card biên lai
    auto* card = new QFrame(this);
    card->setStyleSheet(
        "QFrame#ReceiptCard {"
        "  background-color: #ffffff;"
        "  border: 1px solid #cbd5e1;"
        "  border-radius: 14px;"
        "}"
    );
    card->setObjectName("ReceiptCard");

    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(22, 24, 22, 22);
    cardLayout->setSpacing(10);

    // Header Logo
    auto* lblStore = new QLabel("⚽ SÂN BÓNG MINI SPORT HUB", card);
    lblStore->setAlignment(Qt::AlignCenter);
    lblStore->setStyleSheet("font-size: 17px; font-weight: 800; color: #0f172a; border: none; letter-spacing: 0.5px;");

    auto* lblSubtitle = new QLabel("PHIẾU THANH TOÁN TIỀN SÂN & DỊCH VỤ", card);
    lblSubtitle->setAlignment(Qt::AlignCenter);
    lblSubtitle->setStyleSheet("font-size: 11px; font-weight: 700; color: #64748b; border: none; letter-spacing: 0.5px;");

    auto* sep1 = new QFrame(card);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet("border: none; border-top: 2px dashed #cbd5e1; max-height: 2px; margin: 4px 0;");

    QString content = QString(
        "<div style='font-family: Consolas, monospace; font-size: 13px; line-height: 1.6;'>"
        "<table width='100%' style='border-collapse: collapse;'>"
        "<tr><td><b>Mã hóa đơn:</b></td><td align='right'><span style='color:#2563eb; font-weight:bold;'>%1</span></td></tr>"
        "<tr><td><b>Mã đơn đặt:</b></td><td align='right'>%2</td></tr>"
        "<tr><td><b>Khách hàng:</b></td><td align='right'><b>%3</b></td></tr>"
        "<tr><td><b>Sân bóng:</b></td><td align='right'>%4</td></tr>"
        "<tr><td><b>Thời lượng:</b></td><td align='right'>%5 giờ</td></tr>"
        "<tr><td><b>Đơn giá sân:</b></td><td align='right'>%L6 đ/h</td></tr>"
        "<tr><td><b>Tiền thuê sân:</b></td><td align='right'>%L7 đ</td></tr>"
        "<tr><td><b>Phụ phí dịch vụ:</b></td><td align='right' style='color:#2563eb;'>+%L8 đ</td></tr>"
        "<tr><td><b>Trừ tiền cọc:</b></td><td align='right' style='color:#dc2626;'>-%L9 đ</td></tr>"
        "</table>"
        "<div style='border-top: 2px dashed #94a3b8; margin: 12px 0;'></div>"
        "<table width='100%'>"
        "<tr><td style='font-size:14px; font-weight:bold; color:#0f172a;'>TỔNG THỰC THU:</td>"
        "<td align='right' style='font-size:20px; font-weight:800; color:#047857;'>%L10 VNĐ</td></tr>"
        "<tr><td colspan='2' align='center' style='padding-top:12px; color:#64748b; font-size:11px;'>"
        "🕒 Thời gian lập phiếu: %11"
        "</td></tr>"
        "</table>"
        "</div>"
    )
    .arg(p.invoice_id)
    .arg(p.booking_id)
    .arg(p.customer_name)
    .arg(p.field_name)
    .arg(p.duration_hours)
    .arg(p.price_per_hour, 0, 'f', 0)
    .arg(p.rental_fee, 0, 'f', 0)
    .arg(p.service_fee, 0, 'f', 0)
    .arg(p.deposit_deducted, 0, 'f', 0)
    .arg(p.total_amount, 0, 'f', 0)
    .arg(p.payment_time);

    auto* lblDetails = new QLabel(card);
    lblDetails->setText(content);
    lblDetails->setStyleSheet("border: none;");

    auto* sep2 = new QFrame(card);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("border: none; border-top: 2px dashed #cbd5e1; max-height: 2px; margin: 4px 0;");

    auto* lblThanks = new QLabel("✨ Xin cảm ơn quý khách và chúc trận đấu vui vẻ! ✨", card);
    lblThanks->setAlignment(Qt::AlignCenter);
    lblThanks->setStyleSheet("color: #64748b; font-size: 11px; font-style: italic; border: none; padding-top: 4px;");

    cardLayout->addWidget(lblStore);
    cardLayout->addWidget(lblSubtitle);
    cardLayout->addWidget(sep1);
    cardLayout->addWidget(lblDetails);
    cardLayout->addWidget(sep2);
    cardLayout->addWidget(lblThanks);

    mainLayout->addWidget(card);

    auto* btnClose = new QPushButton("✅ Hoàn Tất & Đóng", this);
    btnClose->setCursor(Qt::PointingHandCursor);
    btnClose->setFixedHeight(40);
    btnClose->setStyleSheet(
        "QPushButton {"
        "  background-color: #2563eb; color: white; padding: 8px 24px; font-weight: bold; font-size: 13px; border-radius: 8px;"
        "}"
        "QPushButton:hover { background-color: #1d4ed8; }"
    );
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    auto* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(btnClose);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);
}
