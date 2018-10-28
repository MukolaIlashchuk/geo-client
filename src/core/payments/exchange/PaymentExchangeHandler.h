#ifndef GEO_NETWORK_CLIENT_PAYMENTEXCHANGEHANDLER_H
#define GEO_NETWORK_CLIENT_PAYMENTEXCHANGEHANDLER_H

#include "../../common/Types.h"
#include "../../logger/Logger.h"

class PaymentExchangeHandler {

public:
    PaymentExchangeHandler(
        Logger &logger);

    bool isRatePresent(
        SerializedEquivalent from,
        SerializedEquivalent to) const;

    double getRate(
        SerializedEquivalent from,
        SerializedEquivalent to);

    void setRate(
        SerializedEquivalent from,
        SerializedEquivalent to,
        double rate);

private:
    Logger &mLogger;

    map<pair<SerializedEquivalent, SerializedEquivalent>, double> mChanges;
};


#endif //GEO_NETWORK_CLIENT_PAYMENTEXCHANGEHANDLER_H
