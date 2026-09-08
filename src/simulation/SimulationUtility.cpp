#include "simulation/SimulationUtility.h"

#include <random>

#include "client/Consultation.h"
#include "client/Transfer.h"
#include "client/Withdraw.h"

namespace {

std::mt19937& rng() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

}

int SimulationUtility::getRandomServiceTime(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng());
}

bool SimulationUtility::isPriorityClient(double priorityClientRate) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng()) < priorityClientRate;
}

std::unique_ptr<AbstractOperation> SimulationUtility::getRandomOperation() {
    std::uniform_int_distribution<int> dist(0, 2);
    switch (dist(rng())) {
        case 0: return std::make_unique<Consultation>();
        case 1: return std::make_unique<Transfer>();
        default: return std::make_unique<Withdraw>();
    }
}
