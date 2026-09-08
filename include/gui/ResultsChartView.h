#pragma once

#include <QtCharts/QChartView>
#include <QVector>

#include "persistence/StatisticRepository.h"

namespace ResultsChartView {

// Pie chart: served vs non-served (impatient) clients for one run.
QChartView* buildSatisfactionChart(const RunSummary& run);

// Bar chart: how many clients of each operation type were served, for one run.
QChartView* buildOperationBreakdownChart(const QVector<ClientRecord>& clients);

}
