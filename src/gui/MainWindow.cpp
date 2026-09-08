#include "gui/MainWindow.h"

#include <algorithm>

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "gui/ResultsChartView.h"

namespace {

QSpinBox* makeSpin(int min, int max, int value) {
    auto* spin = new QSpinBox();
    spin->setRange(min, max);
    spin->setValue(value);
    return spin;
}

}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Simulation Bank");
    resize(1000, 640);

    auto* tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    // ----- Simulation tab -----
    auto* simulationTab = new QWidget();
    auto* simulationLayout = new QHBoxLayout(simulationTab);

    auto* formGroup = new QGroupBox("Parametres");
    auto* form = new QFormLayout(formGroup);

    durationSpin_ = makeSpin(1, 10000, 100);
    cashierCountSpin_ = makeSpin(1, 20, 3);
    minServiceSpin_ = makeSpin(1, 1000, 2);
    maxServiceSpin_ = makeSpin(1, 1000, 8);
    arrivalIntervalSpin_ = makeSpin(1, 1000, 4);
    patienceSpin_ = makeSpin(0, 1000, 5);
    vipRateSpin_ = new QDoubleSpinBox();
    vipRateSpin_->setRange(0.0, 1.0);
    vipRateSpin_->setSingleStep(0.05);
    vipRateSpin_->setValue(0.15);
    speedSpin_ = makeSpin(20, 2000, 300);

    form->addRow("Duree de la simulation", durationSpin_);
    form->addRow("Nombre de caissiers", cashierCountSpin_);
    form->addRow("Temps de service min", minServiceSpin_);
    form->addRow("Temps de service max", maxServiceSpin_);
    form->addRow("Intervalle d'arrivee (N)", arrivalIntervalSpin_);
    form->addRow("Patience client", patienceSpin_);
    form->addRow("Taux de clients VIP", vipRateSpin_);
    form->addRow("Vitesse (ms/tick)", speedSpin_);

    startButton_ = new QPushButton("Demarrer");
    pauseButton_ = new QPushButton("Pause");
    pauseButton_->setEnabled(false);
    form->addRow(startButton_);
    form->addRow(pauseButton_);

    statsLabel_ = new QLabel("En attente du demarrage...");
    statsLabel_->setMinimumHeight(24);

    auto* leftColumn = new QVBoxLayout();
    leftColumn->addWidget(formGroup);
    leftColumn->addStretch();

    auto* rightColumn = new QVBoxLayout();
    bankView_ = new BankView();
    rightColumn->addWidget(bankView_, 1);
    rightColumn->addWidget(statsLabel_, 0);

    simulationLayout->addLayout(leftColumn);
    simulationLayout->addLayout(rightColumn, 1);

    tabs->addTab(simulationTab, "Simulation");

    // ----- Historique tab -----
    auto* historyTab = new QWidget();
    auto* historyLayout = new QHBoxLayout(historyTab);

    historyList_ = new QListWidget();
    historyLayout->addWidget(historyList_, 1);

    auto* chartContainer = new QWidget();
    historyChartLayout_ = new QVBoxLayout(chartContainer);
    historyLayout->addWidget(chartContainer, 2);

    tabs->addTab(historyTab, "Historique");

    connect(startButton_, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(pauseButton_, &QPushButton::clicked, this, &MainWindow::onPauseClicked);
    connect(&tickTimer_, &QTimer::timeout, this, &MainWindow::onTick);
    connect(historyList_, &QListWidget::itemSelectionChanged, this, &MainWindow::onHistorySelectionChanged);

    repository_.open("simulation_bank.db");
    refreshHistoryList();
}

SimulationEntry MainWindow::readConfigFromForm() const {
    SimulationEntry entry;
    entry.setSimulationDuration(durationSpin_->value());
    entry.setCashierCount(cashierCountSpin_->value());
    entry.setMinServiceTime(minServiceSpin_->value());
    entry.setMaxServiceTime(maxServiceSpin_->value());
    entry.setClientArrivalInterval(arrivalIntervalSpin_->value());
    entry.setClientPatienceTime(patienceSpin_->value());
    entry.setPriorityClientRate(vipRateSpin_->value());
    return entry;
}

void MainWindow::setFormEnabled(bool enabled) {
    durationSpin_->setEnabled(enabled);
    cashierCountSpin_->setEnabled(enabled);
    minServiceSpin_->setEnabled(enabled);
    maxServiceSpin_->setEnabled(enabled);
    arrivalIntervalSpin_->setEnabled(enabled);
    patienceSpin_->setEnabled(enabled);
    vipRateSpin_->setEnabled(enabled);
    startButton_->setEnabled(enabled);
}

void MainWindow::onStartClicked() {
    SimulationEntry entry = readConfigFromForm();
    if (entry.getMaxServiceTime() < entry.getMinServiceTime()) {
        statsLabel_->setText("Le temps de service max doit etre >= au temps de service min.");
        return;
    }

    simulation_ = std::make_unique<Simulation>(entry);
    bankView_->attachSimulation(simulation_.get());

    setFormEnabled(false);
    pauseButton_->setEnabled(true);
    pauseButton_->setText("Pause");

    tickTimer_.start(speedSpin_->value());
}

void MainWindow::onPauseClicked() {
    if (tickTimer_.isActive()) {
        tickTimer_.stop();
        pauseButton_->setText("Reprendre");
    } else {
        tickTimer_.start(speedSpin_->value());
        pauseButton_->setText("Pause");
    }
}

void MainWindow::onTick() {
    if (!simulation_) {
        return;
    }

    bool running = simulation_->step();
    bankView_->update();

    const auto& stats = simulation_->statisticManager();
    statsLabel_->setText(QString(
        "t = %1 / %2 | file d'attente = %3 | servis = %4 | non servis = %5")
        .arg(simulation_->currentTime())
        .arg(simulation_->entry().getSimulationDuration())
        .arg(simulation_->bank().getQueue().size())
        .arg(stats.servedClientCount())
        .arg(stats.nonServedClientCount()));

    if (!running) {
        tickTimer_.stop();
        int runId = repository_.saveRun(simulation_->entry(), simulation_->statisticManager());
        statusBar()->showMessage(QString("Simulation terminee, resultats enregistres (run #%1).").arg(runId), 5000);

        setFormEnabled(true);
        pauseButton_->setEnabled(false);
        pauseButton_->setText("Pause");

        refreshHistoryList();
    }
}

void MainWindow::refreshHistoryList() {
    historyRuns_ = repository_.listRuns();

    historyList_->clear();
    for (const RunSummary& run : historyRuns_) {
        auto* item = new QListWidgetItem(QString("Run #%1 - %2 - servis %3 - satisfaction %4%")
                                              .arg(run.id)
                                              .arg(run.timestamp.toString("dd/MM hh:mm:ss"))
                                              .arg(run.servedCount)
                                              .arg(run.satisfactionRate, 0, 'f', 1));
        item->setData(Qt::UserRole, run.id);
        historyList_->addItem(item);
    }
}

void MainWindow::onHistorySelectionChanged() {
    QListWidgetItem* item = historyList_->currentItem();
    if (item == nullptr) {
        return;
    }
    refreshHistoryChart(item->data(Qt::UserRole).toInt());
}

void MainWindow::refreshHistoryChart(int runId) {
    QLayoutItem* child;
    while ((child = historyChartLayout_->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    auto runIt = std::find_if(historyRuns_.begin(), historyRuns_.end(),
                               [runId](const RunSummary& run) { return run.id == runId; });
    if (runIt == historyRuns_.end()) {
        return;
    }

    QVector<ClientRecord> clients = repository_.clientsForRun(runId);

    historyChartLayout_->addWidget(ResultsChartView::buildSatisfactionChart(*runIt));
    historyChartLayout_->addWidget(ResultsChartView::buildOperationBreakdownChart(clients));
}
