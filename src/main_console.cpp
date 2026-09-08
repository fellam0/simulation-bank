#include <iostream>

#include "simulation/Simulation.h"

namespace {

void printTick(const Simulation& simulation) {
    Bank& bank = const_cast<Simulation&>(simulation).bank();
    std::cout << "t=" << simulation.currentTime()
              << " | file d'attente=" << bank.getQueue().size();
    for (auto& cashier : bank.getCashiers()) {
        std::cout << " | caissier" << cashier.getId() << "="
                  << (cashier.isFree() ? "libre" : cashier.getServingClient()->toString());
    }
    std::cout << "\n";
}

}

int main() {
    SimulationConfig config;
    config.simulationDuration = 60;
    config.cashierCount = 3;
    config.minServiceTime = 2;
    config.maxServiceTime = 8;
    config.clientArrivalInterval = 4;
    config.vipClientRate = 0.15;
    config.clientPatienceTime = 5;

    Simulation simulation(config);

    do {
        printTick(simulation);
    } while (simulation.step());

    std::cout << "\n" << simulation.resultsSummary();

    return 0;
}
