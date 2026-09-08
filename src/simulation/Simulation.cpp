#include "simulation/Simulation.h"

#include <sstream>

#include "client/Client.h"
#include "client/VIPClient.h"
#include "simulation/SimulationUtility.h"

Simulation::Simulation(SimulationEntry entry)
    : entry_(entry), bank_(entry.getCashierCount()) {}

void Simulation::serveClient(Cashier& cashier, std::shared_ptr<AbstractClient> client) {
    client->setServiceStartTime(currentTime_);
    cashier.serve(std::move(client));
}

void Simulation::updateBank() {
    Queue& queue = bank_.getQueue();

    for (Cashier& cashier : bank_.getCashiers()) {
        if (!cashier.isFree()) {
            statisticManager_.recordOccupiedCashier();
        }

        cashier.work();

        if (cashier.serviceFinished()) {
            auto leavingClient = cashier.getServingClient();
            leavingClient->setDepartureTime(currentTime_);
            statisticManager_.registerServedClient(leavingClient);
            cashier.setServingClient(nullptr);

            if (!queue.isEmpty()) {
                std::shared_ptr<AbstractClient> nextClient = queue.findPriorityClient();
                if (nextClient != nullptr) {
                    queue.removePriorityClient(nextClient);
                } else {
                    nextClient = queue.getQueueFirst();
                }
                serveClient(cashier, nextClient);
            }
        }
    }

    queue.updateClientPatience();
    for (auto& impatientClient : queue.removeImpatientClients()) {
        impatientClient->setDepartureTime(currentTime_);
        statisticManager_.registerNonServedClient(impatientClient);
    }
}

void Simulation::handleNewClientArrival() {
    if (currentTime_ % entry_.getClientArrivalInterval() != 0) {
        return;
    }

    auto operation = SimulationUtility::getRandomOperation();
    operation->setServiceTime(SimulationUtility::getRandomServiceTime(
        entry_.getMinServiceTime(), entry_.getMaxServiceTime()));

    std::shared_ptr<AbstractClient> client;
    if (SimulationUtility::isPriorityClient(entry_.getPriorityClientRate())) {
        client = std::make_shared<VIPClient>(nextClientId_++, currentTime_, std::move(operation),
                                              entry_.getClientPatienceTime());
    } else {
        client = std::make_shared<Client>(nextClientId_++, currentTime_, std::move(operation),
                                           entry_.getClientPatienceTime());
    }

    Cashier* freeCashier = bank_.getFreeCashier();
    if (freeCashier != nullptr) {
        serveClient(*freeCashier, client);
    } else {
        bank_.getQueue().addQueueLast(client);
    }
}

bool Simulation::step() {
    if (isFinished()) {
        return false;
    }

    statisticManager_.recordSimulationTick();
    updateBank();
    handleNewClientArrival();

    currentTime_++;
    return true;
}

void Simulation::simulate() {
    while (step()) {
    }
}

int Simulation::currentTime() const {
    return currentTime_;
}

bool Simulation::isFinished() const {
    return currentTime_ > entry_.getSimulationDuration();
}

Bank& Simulation::bank() {
    return bank_;
}

const StatisticManager& Simulation::statisticManager() const {
    return statisticManager_;
}

const SimulationEntry& Simulation::entry() const {
    return entry_;
}

std::string Simulation::resultsSummary() const {
    std::ostringstream out;
    out << "########## Resultats de la simulation ##########\n";
    out << "Duree de simulation : " << entry_.getSimulationDuration() << "\n";
    out << "Clients servis : " << statisticManager_.servedClientCount() << "\n";
    out << "Temps d'attente moyen : " << statisticManager_.averageClientWaitingTime() << "\n";
    out << "Temps de service moyen : " << statisticManager_.averageClientServiceTime() << "\n";
    out << "Taux d'occupation des caissiers : " << statisticManager_.averageCashierOccupationRate(entry_.getCashierCount()) << " %\n";
    out << "Clients non servis (impatients) : " << statisticManager_.nonServedClientCount() << "\n";
    out << "Taux de satisfaction client : " << statisticManager_.clientSatisfactionRate() << " %\n";
    return out.str();
}
