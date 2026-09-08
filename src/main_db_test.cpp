#include <QCoreApplication>

#include <iostream>

#include "persistence/StatisticRepository.h"
#include "simulation/Simulation.h"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    SimulationConfig config;
    config.simulationDuration = 60;
    config.cashierCount = 3;
    config.minServiceTime = 2;
    config.maxServiceTime = 8;
    config.clientArrivalInterval = 4;
    config.vipClientRate = 0.15;
    config.clientPatienceTime = 5;

    Simulation simulation(config);
    while (simulation.step()) {
    }

    StatisticRepository repository;
    if (!repository.open("simulation_bank.db")) {
        std::cerr << "Impossible d'ouvrir la base de donnees\n";
        return 1;
    }

    int runId = repository.saveRun(config, simulation.statisticManager());
    std::cout << "Simulation enregistree sous l'id " << runId << "\n";

    for (const RunSummary& run : repository.listRuns()) {
        std::cout << "Run " << run.id << " " << run.timestamp.toString().toStdString()
                   << " servis=" << run.servedCount
                   << " non-servis=" << run.nonServedCount
                   << " satisfaction=" << run.satisfactionRate << "%\n";
    }

    std::cout << "Clients du dernier run: " << repository.clientsForRun(runId).size() << "\n";

    return 0;
}
