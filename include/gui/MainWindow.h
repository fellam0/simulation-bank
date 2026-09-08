#pragma once

#include <QMainWindow>
#include <QTimer>
#include <memory>

#include "gui/BankView.h"
#include "persistence/StatisticRepository.h"
#include "simulation/Simulation.h"

class QDoubleSpinBox;
class QLabel;
class QListWidget;
class QPushButton;
class QSpinBox;
class QVBoxLayout;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void onStartClicked();
    void onPauseClicked();
    void onTick();
    void onHistorySelectionChanged();

private:
    SimulationEntry readConfigFromForm() const;
    void setFormEnabled(bool enabled);
    void refreshHistoryList();
    void refreshHistoryChart(int runId);

    QSpinBox* durationSpin_;
    QSpinBox* cashierCountSpin_;
    QSpinBox* minServiceSpin_;
    QSpinBox* maxServiceSpin_;
    QSpinBox* arrivalIntervalSpin_;
    QSpinBox* patienceSpin_;
    QDoubleSpinBox* vipRateSpin_;
    QSpinBox* speedSpin_;

    QPushButton* startButton_;
    QPushButton* pauseButton_;
    QLabel* statsLabel_;
    BankView* bankView_;

    QListWidget* historyList_;
    QVBoxLayout* historyChartLayout_;

    QTimer tickTimer_;
    std::unique_ptr<Simulation> simulation_;
    StatisticRepository repository_;
    QVector<RunSummary> historyRuns_;
};
