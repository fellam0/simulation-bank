#pragma once

#include "client/AbstractClient.h"

class Client : public AbstractClient {
public:
    using AbstractClient::AbstractClient;

    bool isPriority() const override { return false; }
    std::string label() const override { return "Client"; }
};
