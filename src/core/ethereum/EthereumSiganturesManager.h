#ifndef GEO_NETWORK_CLIENT_ETHEREUMSIGANTURESMANAGER_H
#define GEO_NETWORK_CLIENT_ETHEREUMSIGANTURESMANAGER_H

#include "../common/NodeUUID.h"
#include "../common/Types.h"
#include "../logger/Logger.h"

#include "connector.h"

#include <map>

using namespace eth;

class EthereumSiganturesManager {

public:
    EthereumSiganturesManager(
        const SerializedEquivalent equivalent,
        Logger &logger);

    void addPaymentReceipt(
        const NodeUUID &contractorUUID,
        Receipt::Shared paymentReceipt);

    vector<Receipt::Shared> paymentReceipts(
        const NodeUUID &contractorUUID);

    void setAudit(
        const NodeUUID &contractorUUID,
        CooperativeClose::Shared audit);

    CooperativeClose::Shared getAudit(const NodeUUID &contractorUUID);

protected: // log shortcuts
    const string logHeader() const
    noexcept;

    LoggerStream info() const
    noexcept;

private:
    SerializedEquivalent mEquivalent;
    Logger &mLogger;

    map<NodeUUID, vector<Receipt::Shared>> mIncomingPaymentReceipts;
    map<NodeUUID, CooperativeClose::Shared> mCurrentAudit;
};


#endif //GEO_NETWORK_CLIENT_ETHEREUMSIGANTURESMANAGER_H
