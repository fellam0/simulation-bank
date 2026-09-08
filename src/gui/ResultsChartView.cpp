#include "gui/ResultsChartView.h"

#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QLegend>
#include <QMap>
#include <QPainter>
#include <QPieSeries>
#include <QValueAxis>

namespace ResultsChartView {

QChartView* buildSatisfactionChart(const RunSummary& run) {
    auto* series = new QPieSeries();
    series->append(QString("Servis (%1)").arg(run.servedCount), run.servedCount);
    series->append(QString("Non servis (%1)").arg(run.nonServedCount), run.nonServedCount);
    if (series->slices().size() > 0) {
        series->slices().at(0)->setBrush(QColor(70, 130, 180));
    }
    if (series->slices().size() > 1) {
        series->slices().at(1)->setBrush(QColor(220, 80, 80));
    }
    for (auto* slice : series->slices()) {
        slice->setLabelVisible(true);
    }

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("Satisfaction client : %1 %").arg(run.satisfactionRate, 0, 'f', 1));
    chart->legend()->setAlignment(Qt::AlignBottom);

    auto* view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumHeight(300);
    return view;
}

QChartView* buildOperationBreakdownChart(const QVector<ClientRecord>& clients) {
    QMap<QString, int> servedByOperation;
    QMap<QString, int> nonServedByOperation;

    for (const ClientRecord& client : clients) {
        if (client.served) {
            servedByOperation[client.operationType]++;
        } else {
            nonServedByOperation[client.operationType]++;
        }
    }

    QStringList operations = servedByOperation.keys() + nonServedByOperation.keys();
    operations.removeDuplicates();

    auto* servedSet = new QBarSet("Servis");
    auto* nonServedSet = new QBarSet("Non servis");
    for (const QString& operation : operations) {
        servedSet->append(servedByOperation.value(operation, 0));
        nonServedSet->append(nonServedByOperation.value(operation, 0));
    }

    auto* series = new QBarSeries();
    series->append(servedSet);
    series->append(nonServedSet);

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Repartition par type d'operation");
    chart->legend()->setAlignment(Qt::AlignBottom);

    auto* axisX = new QBarCategoryAxis();
    axisX->append(operations);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    auto* view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumHeight(300);
    return view;
}

}
