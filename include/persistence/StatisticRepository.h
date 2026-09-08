#pragma once

#include <QDateTime>
#include <QString>
#include <QVector>

#include "simulation/SimulationConfig.h"
#include "simulation/StatisticManager.h"

struct RunSummary {
    int id = 0;
    QDateTime timestamp;
    SimulationConfig config;
    int servedCount = 0;
    int nonServedCount = 0;
    double averageWaitingTime = 0.0;
    double averageServiceTime = 0.0;
    double occupationRate = 0.0;
    double satisfactionRate = 0.0;
};

struct ClientRecord {
    QString clientType;      // "Client" or "VIP"
    QString operationType;   // "Consultation", "Virement", "Retrait important"
    int arrivalTime = 0;
    int serviceStartTime = 0;
    int departureTime = 0;
    bool served = false;
};

// Wraps a SQLite database (via Qt Sql / QSQLITE driver) storing one row per
// simulation run plus one row per client of that run, so results can be
// persisted, re-read, and charted independently of a live simulation.
class StatisticRepository {
public:
    bool open(const QString& databaseFilePath);

    int saveRun(const SimulationConfig& config, const StatisticManager& stats);

    QVector<RunSummary> listRuns() const;
    QVector<ClientRecord> clientsForRun(int runId) const;

private:
    void ensureSchema();

    QString connectionName_;
};
