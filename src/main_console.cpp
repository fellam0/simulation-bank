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
    SimulationEntry entry;
    entry.setSimulationDuration(60);
    entry.setCashierCount(3);
    entry.setMinServiceTime(2);
    entry.setMaxServiceTime(8);
    entry.setClientArrivalInterval(4);
    entry.setPriorityClientRate(0.15);
    entry.setClientPatienceTime(5);

    Simulation simulation(entry);

    do {
        printTick(simulation);
    } while (simulation.step());

    std::cout << "\n" << simulation.resultsSummary();

    return 0;
}
