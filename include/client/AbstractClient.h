#pragma once

#include <memory>
#include <string>

#include "client/AbstractOperation.h"

class AbstractClient {
public:
    AbstractClient(int id, int arrivalTime, std::unique_ptr<AbstractOperation> operation, int patienceTime);
    virtual ~AbstractClient() = default;

    virtual bool isPriority() const = 0;
    virtual std::string label() const = 0;

    int getId() const;

    int getArrivalTime() const;
    void setArrivalTime(int t);

    int getServiceStartTime() const;
    void setServiceStartTime(int t);

    int getDepartureTime() const;
    void setDepartureTime(int t);

    AbstractOperation& getOperation() const;

    void reducePatience();
    bool isPatient() const;

    int getPatienceTime() const;
    void setPatienceTime(int t);

    std::string toString() const;

private:
    int id_;
    int arrivalTime_;
    int serviceStartTime_ = 0;
    int departureTime_ = 0;
    std::unique_ptr<AbstractOperation> operation_;
    int patienceTime_;
};
