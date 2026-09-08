#pragma once

#include "client/AbstractClient.h"

class VIPClient : public AbstractClient {
public:
    using AbstractClient::AbstractClient;

    bool isPriority() const override { return true; }
    std::string label() const override { return "VIP"; }
};
