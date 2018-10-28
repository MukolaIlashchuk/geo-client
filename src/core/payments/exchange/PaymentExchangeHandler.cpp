#include "PaymentExchangeHandler.h"

PaymentExchangeHandler::PaymentExchangeHandler(
    Logger &logger) :
    mLogger(logger)
{}

bool PaymentExchangeHandler::isRatePresent(
    SerializedEquivalent from,
    SerializedEquivalent to) const
{
    auto key = make_pair(from, to);
    return mChanges.count(key) != 0;
}

double PaymentExchangeHandler::getRate(
        SerializedEquivalent from,
        SerializedEquivalent to)
{
    auto key = make_pair(from, to);
    auto result = mChanges[key];
    return result;
}

void PaymentExchangeHandler::setRate(
        SerializedEquivalent from,
        SerializedEquivalent to,
        double rate)
{
    auto key = make_pair(from, to);
    mChanges[key] = rate;
}