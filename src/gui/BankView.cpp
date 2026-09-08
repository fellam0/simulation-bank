#include "gui/BankView.h"

#include <QPainter>

namespace {

constexpr int kCashierBoxSize = 90;
constexpr int kCashierGap = 20;
constexpr int kQueueTokenSize = 36;
constexpr int kQueueGap = 8;
constexpr int kMargin = 24;

QString operationInitial(const AbstractClient& client) {
    const std::string name = client.getOperation().name();
    return name.empty() ? QString("?") : QString(QChar(name.front()));
}

void paintClientToken(QPainter& painter, const QRectF& rect, const AbstractClient& client) {
    const QColor fill = client.isPriority() ? QColor(240, 173, 78) : QColor(70, 130, 180);
    painter.setBrush(fill);
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRoundedRect(rect, 6, 6);

    painter.setPen(Qt::white);
    painter.drawText(rect, Qt::AlignCenter, operationInitial(client));
}

}

BankView::BankView(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(260);
}

void BankView::attachSimulation(Simulation* simulation) {
    simulation_ = simulation;
    update();
}

QSize BankView::sizeHint() const {
    return QSize(640, 320);
}

void BankView::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), palette().base());

    if (simulation_ == nullptr) {
        painter.drawText(rect(), Qt::AlignCenter, "Aucune simulation en cours");
        return;
    }

    painter.setPen(palette().text().color());
    painter.drawText(kMargin, 18, QString("t = %1").arg(simulation_->currentTime()));

    int x = kMargin;
    const int y = 36;
    for (Cashier& cashier : simulation_->bank().getCashiers()) {
        const QRectF box(x, y, kCashierBoxSize, kCashierBoxSize);

        if (cashier.isFree()) {
            painter.setBrush(QColor(220, 220, 220));
            painter.setPen(QPen(Qt::darkGray, 1));
            painter.drawRoundedRect(box, 8, 8);
            painter.setPen(Qt::darkGray);
            painter.drawText(box, Qt::AlignCenter, "libre");
        } else {
            auto client = cashier.getServingClient();
            painter.setBrush(client->isPriority() ? QColor(240, 173, 78) : QColor(70, 130, 180));
            painter.setPen(QPen(Qt::black, 1));
            painter.drawRoundedRect(box, 8, 8);

            const int totalServiceTime = client->getOperation().getServiceTime();
            const int remaining = cashier.remainingServiceTime();
            const double ratio = totalServiceTime > 0
                                      ? static_cast<double>(remaining) / totalServiceTime
                                      : 0.0;

            const QRectF textBox(box.x(), box.y(), box.width(), box.height() - 16);
            painter.setPen(Qt::white);
            painter.drawText(textBox, Qt::AlignCenter,
                              QString("%1\n#%2\n%3\nreste : %4")
                                  .arg(QString::fromStdString(client->label()))
                                  .arg(client->getId())
                                  .arg(QString::fromStdString(client->getOperation().name()))
                                  .arg(remaining));

            const QRectF barTrack(box.x() + 8, box.bottom() - 12, box.width() - 16, 6);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(0, 0, 0, 90));
            painter.drawRoundedRect(barTrack, 3, 3);

            QRectF barFill = barTrack;
            barFill.setWidth(barTrack.width() * ratio);
            painter.setBrush(Qt::white);
            painter.drawRoundedRect(barFill, 3, 3);
        }

        painter.setPen(palette().text().color());
        painter.drawText(QRectF(x, y + kCashierBoxSize + 4, kCashierBoxSize, 16),
                          Qt::AlignCenter, QString("caissier %1").arg(cashier.getId()));

        x += kCashierBoxSize + kCashierGap;
    }

    const int queueY = y + kCashierBoxSize + 40;
    painter.drawText(kMargin, queueY - 8,
                      QString("File d'attente (%1)").arg(simulation_->bank().getQueue().size()));

    int qx = kMargin;
    int qy = queueY;
    const int rowWidth = width() - 2 * kMargin;
    for (const auto& client : simulation_->bank().getQueue().clients()) {
        if (qx + kQueueTokenSize > kMargin + rowWidth) {
            qx = kMargin;
            qy += kQueueTokenSize + kQueueGap;
        }
        paintClientToken(painter, QRectF(qx, qy, kQueueTokenSize, kQueueTokenSize), *client);
        qx += kQueueTokenSize + kQueueGap;
    }
}
