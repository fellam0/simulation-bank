#include <QCoreApplication>

#include <iostream>

#include "persistence/StatisticRepository.h"
#include "simulation/Simulation.h"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    SimulationEntry entry;
    entry.setSimulationDuration(60);
    entry.setCashierCount(3);
    entry.setMinServiceTime(2);
    entry.setMaxServiceTime(8);
    entry.setClientArrivalInterval(4);
    entry.setPriorityClientRate(0.15);
    entry.setClientPatienceTime(5);

    Simulation simulation(entry);
    simulation.simulate();

    StatisticRepository repository;
    if (!repository.open("simulation_bank.db")) {
        std::cerr << "Impossible d'ouvrir la base de donnees\n";
        return 1;
    }

    int runId = repository.saveRun(entry, simulation.statisticManager());
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
