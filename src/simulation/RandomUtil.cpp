#include "simulation/RandomUtil.h"

#include <random>

#include "client/Client.h"
#include "client/Consultation.h"
#include "client/Transfer.h"
#include "client/VIPClient.h"
#include "client/Withdraw.h"

namespace {

std::mt19937& rng() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

}

namespace RandomUtil {

int randomServiceTime(int minServiceTime, int maxServiceTime) {
    std::uniform_int_distribution<int> dist(minServiceTime, maxServiceTime);
    return dist(rng());
}

std::shared_ptr<AbstractClient> randomClient(int id, int arrivalTime, int patienceTime,
                                              int serviceTime, double vipClientRate) {
    std::uniform_int_distribution<int> operationDist(0, 2);
    std::unique_ptr<AbstractOperation> operation;
    switch (operationDist(rng())) {
        case 0: operation = std::make_unique<Consultation>(serviceTime); break;
        case 1: operation = std::make_unique<Transfer>(serviceTime); break;
        default: operation = std::make_unique<Withdraw>(serviceTime); break;
    }

    std::uniform_real_distribution<double> vipDist(0.0, 1.0);
    bool isVip = vipDist(rng()) < vipClientRate;

    if (isVip) {
        return std::make_shared<VIPClient>(id, arrivalTime, std::move(operation), patienceTime);
    }
    return std::make_shared<Client>(id, arrivalTime, std::move(operation), patienceTime);
}

}
