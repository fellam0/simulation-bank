#include "simulation/Simulation.h"

#include <sstream>

#include "simulation/RandomUtil.h"

Simulation::Simulation(SimulationConfig config)
    : config_(config), bank_(config.cashierCount) {}

void Simulation::serveClient(Cashier& cashier, std::shared_ptr<AbstractClient> client) {
    client->setServiceStartTime(currentTime_);
    cashier.serve(std::move(client));
}

bool Simulation::step() {
    if (isFinished()) {
        return false;
    }

    statisticManager_.recordSimulationTick();

    for (auto& cashier : bank_.getCashiers()) {
        if (!cashier.isFree()) {
            statisticManager_.recordOccupiedCashier();
        }

        cashier.tick();

        if (cashier.serviceFinished()) {
            auto leavingClient = cashier.release();
            leavingClient->setDepartureTime(currentTime_);
            statisticManager_.registerServedClient(leavingClient);

            auto& queue = bank_.getQueue();
            if (!queue.isEmpty()) {
                auto nextClient = queue.takeNextToServe();
                serveClient(cashier, nextClient);
            }
        }
    }

    auto& queue = bank_.getQueue();
    queue.updatePatience();
    for (auto& impatientClient : queue.removeImpatientClients()) {
        impatientClient->setDepartureTime(currentTime_);
        statisticManager_.registerNonServedClient(impatientClient);
    }

    if (currentTime_ % config_.clientArrivalInterval == 0) {
        int serviceTime = RandomUtil::randomServiceTime(config_.minServiceTime, config_.maxServiceTime);
        auto client = RandomUtil::randomClient(nextClientId_++, currentTime_, config_.clientPatienceTime,
                                                serviceTime, config_.vipClientRate);

        Cashier* freeCashier = bank_.getFreeCashier();
        if (freeCashier != nullptr) {
            serveClient(*freeCashier, client);
        } else {
            bank_.getQueue().pushBack(client);
        }
    }

    currentTime_++;
    return true;
}

int Simulation::currentTime() const {
    return currentTime_;
}

bool Simulation::isFinished() const {
    return currentTime_ > config_.simulationDuration;
}

Bank& Simulation::bank() {
    return bank_;
}

const StatisticManager& Simulation::statisticManager() const {
    return statisticManager_;
}

const SimulationConfig& Simulation::config() const {
    return config_;
}

std::string Simulation::resultsSummary() const {
    std::ostringstream out;
    out << "########## Resultats de la simulation ##########\n";
    out << "Duree de simulation : " << config_.simulationDuration << "\n";
    out << "Clients servis : " << statisticManager_.servedClientCount() << "\n";
    out << "Temps d'attente moyen : " << statisticManager_.averageClientWaitingTime() << "\n";
    out << "Temps de service moyen : " << statisticManager_.averageClientServiceTime() << "\n";
    out << "Taux d'occupation des caissiers : " << statisticManager_.averageCashierOccupationRate(config_.cashierCount) << " %\n";
    out << "Clients non servis (impatients) : " << statisticManager_.nonServedClientCount() << "\n";
    out << "Taux de satisfaction client : " << statisticManager_.clientSatisfactionRate() << " %\n";
    return out.str();
}
