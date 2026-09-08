#pragma once

#include <string>

class AbstractOperation {
public:
    explicit AbstractOperation(int serviceTime = 0);
    virtual ~AbstractOperation() = default;

    int getServiceTime() const;
    void setServiceTime(int serviceTime);

    virtual bool isUrgent() const = 0;
    virtual std::string name() const = 0;

private:
    int serviceTime_;
};
