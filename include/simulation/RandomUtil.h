#pragma once

#include <memory>

#include "client/AbstractClient.h"

namespace RandomUtil {

int randomServiceTime(int minServiceTime, int maxServiceTime);

std::shared_ptr<AbstractClient> randomClient(int id, int arrivalTime, int patienceTime,
                                              int serviceTime, double vipClientRate);

}
