#include "persistence/StatisticRepository.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

bool StatisticRepository::open(const QString& databaseFilePath) {
    connectionName_ = "simulation_bank_" + databaseFilePath;

    QSqlDatabase db = QSqlDatabase::contains(connectionName_)
                           ? QSqlDatabase::database(connectionName_)
                           : QSqlDatabase::addDatabase("QSQLITE", connectionName_);
    db.setDatabaseName(databaseFilePath);

    if (!db.isOpen() && !db.open()) {
        return false;
    }

    ensureSchema();
    return true;
}

void StatisticRepository::ensureSchema() {
    QSqlDatabase db = QSqlDatabase::database(connectionName_);
    QSqlQuery query(db);

    query.exec(
        "CREATE TABLE IF NOT EXISTS simulation_runs ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  timestamp TEXT NOT NULL,"
        "  simulation_duration INTEGER NOT NULL,"
        "  cashier_count INTEGER NOT NULL,"
        "  min_service_time INTEGER NOT NULL,"
        "  max_service_time INTEGER NOT NULL,"
        "  client_arrival_interval INTEGER NOT NULL,"
        "  vip_client_rate REAL NOT NULL,"
        "  client_patience_time INTEGER NOT NULL,"
        "  served_count INTEGER NOT NULL,"
        "  non_served_count INTEGER NOT NULL,"
        "  average_waiting_time REAL NOT NULL,"
        "  average_service_time REAL NOT NULL,"
        "  occupation_rate REAL NOT NULL,"
        "  satisfaction_rate REAL NOT NULL"
        ")"
    );

    query.exec(
        "CREATE TABLE IF NOT EXISTS clients ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  run_id INTEGER NOT NULL REFERENCES simulation_runs(id),"
        "  client_type TEXT NOT NULL,"
        "  operation_type TEXT NOT NULL,"
        "  arrival_time INTEGER NOT NULL,"
        "  service_start_time INTEGER NOT NULL,"
        "  departure_time INTEGER NOT NULL,"
        "  served INTEGER NOT NULL"
        ")"
    );
}

int StatisticRepository::saveRun(const SimulationConfig& config, const StatisticManager& stats) {
    QSqlDatabase db = QSqlDatabase::database(connectionName_);

    QSqlQuery insertRun(db);
    insertRun.prepare(
        "INSERT INTO simulation_runs ("
        "  timestamp, simulation_duration, cashier_count, min_service_time, max_service_time,"
        "  client_arrival_interval, vip_client_rate, client_patience_time,"
        "  served_count, non_served_count, average_waiting_time, average_service_time,"
        "  occupation_rate, satisfaction_rate"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );
    insertRun.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    insertRun.addBindValue(config.simulationDuration);
    insertRun.addBindValue(config.cashierCount);
    insertRun.addBindValue(config.minServiceTime);
    insertRun.addBindValue(config.maxServiceTime);
    insertRun.addBindValue(config.clientArrivalInterval);
    insertRun.addBindValue(config.vipClientRate);
    insertRun.addBindValue(config.clientPatienceTime);
    insertRun.addBindValue(stats.servedClientCount());
    insertRun.addBindValue(stats.nonServedClientCount());
    insertRun.addBindValue(stats.averageClientWaitingTime());
    insertRun.addBindValue(stats.averageClientServiceTime());
    insertRun.addBindValue(stats.averageCashierOccupationRate(config.cashierCount));
    insertRun.addBindValue(stats.clientSatisfactionRate());

    if (!insertRun.exec()) {
        return -1;
    }

    int runId = insertRun.lastInsertId().toInt();

    QSqlQuery insertClient(db);
    insertClient.prepare(
        "INSERT INTO clients ("
        "  run_id, client_type, operation_type, arrival_time, service_start_time, departure_time, served"
        ") VALUES (?, ?, ?, ?, ?, ?, ?)"
    );

    auto persistClient = [&](const std::shared_ptr<AbstractClient>& client, bool served) {
        insertClient.addBindValue(runId);
        insertClient.addBindValue(QString::fromStdString(client->label()));
        insertClient.addBindValue(QString::fromStdString(client->getOperation().name()));
        insertClient.addBindValue(client->getArrivalTime());
        insertClient.addBindValue(client->getServiceStartTime());
        insertClient.addBindValue(client->getDepartureTime());
        insertClient.addBindValue(served ? 1 : 0);
        insertClient.exec();
    };

    for (const auto& client : stats.servedClients()) {
        persistClient(client, true);
    }
    for (const auto& client : stats.nonServedClients()) {
        persistClient(client, false);
    }

    return runId;
}

QVector<RunSummary> StatisticRepository::listRuns() const {
    QSqlDatabase db = QSqlDatabase::database(connectionName_);
    QSqlQuery query(db);
    query.exec(
        "SELECT id, timestamp, simulation_duration, cashier_count, min_service_time, max_service_time,"
        "       client_arrival_interval, vip_client_rate, client_patience_time,"
        "       served_count, non_served_count, average_waiting_time, average_service_time,"
        "       occupation_rate, satisfaction_rate "
        "FROM simulation_runs ORDER BY id DESC"
    );

    QVector<RunSummary> runs;
    while (query.next()) {
        RunSummary run;
        run.id = query.value(0).toInt();
        run.timestamp = QDateTime::fromString(query.value(1).toString(), Qt::ISODate);
        run.config.simulationDuration = query.value(2).toInt();
        run.config.cashierCount = query.value(3).toInt();
        run.config.minServiceTime = query.value(4).toInt();
        run.config.maxServiceTime = query.value(5).toInt();
        run.config.clientArrivalInterval = query.value(6).toInt();
        run.config.vipClientRate = query.value(7).toDouble();
        run.config.clientPatienceTime = query.value(8).toInt();
        run.servedCount = query.value(9).toInt();
        run.nonServedCount = query.value(10).toInt();
        run.averageWaitingTime = query.value(11).toDouble();
        run.averageServiceTime = query.value(12).toDouble();
        run.occupationRate = query.value(13).toDouble();
        run.satisfactionRate = query.value(14).toDouble();
        runs.push_back(run);
    }
    return runs;
}

QVector<ClientRecord> StatisticRepository::clientsForRun(int runId) const {
    QSqlDatabase db = QSqlDatabase::database(connectionName_);
    QSqlQuery query(db);
    query.prepare(
        "SELECT client_type, operation_type, arrival_time, service_start_time, departure_time, served "
        "FROM clients WHERE run_id = ?"
    );
    query.addBindValue(runId);
    query.exec();

    QVector<ClientRecord> clients;
    while (query.next()) {
        ClientRecord record;
        record.clientType = query.value(0).toString();
        record.operationType = query.value(1).toString();
        record.arrivalTime = query.value(2).toInt();
        record.serviceStartTime = query.value(3).toInt();
        record.departureTime = query.value(4).toInt();
        record.served = query.value(5).toInt() != 0;
        clients.push_back(record);
    }
    return clients;
}
