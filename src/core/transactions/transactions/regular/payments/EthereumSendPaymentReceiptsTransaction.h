#ifndef GEO_NETWORK_CLIENT_ETHEREUMSENDPAYMENTRECEIPTSTRANSACTION_H
#define GEO_NETWORK_CLIENT_ETHEREUMSENDPAYMENTRECEIPTSTRANSACTION_H

#include "../../base/BaseTransaction.h"
#include "../../../../trust_lines/manager/TrustLinesManager.h"
#include "../../../../network/messages/payments/EthereumOutgoingReceiptMessage.h"
#include <map>

#include "../../../../ethereum/connector.h"

class EthereumSendPaymentReceiptsTransaction : public BaseTransaction {

public:
    typedef shared_ptr<EthereumSendPaymentReceiptsTransaction> Shared;

public:
    EthereumSendPaymentReceiptsTransaction(
        const NodeUUID &nodeUUID,
        const SerializedEquivalent equivalent,
        map<NodeUUID, TrustLineAmount> outgoingReceipts,
        TrustLinesManager* trustLinesManager,
        Logger &log);

    TransactionResult::SharedConst run()
        noexcept;

protected:
    const string logHeader() const;

private:
    map<NodeUUID, TrustLineAmount> mOutgoingReceipts;
    TrustLinesManager* mTrustLinesManager;
};


#endif //GEO_NETWORK_CLIENT_ETHEREUMSENDPAYMENTRECEIPTSTRANSACTION_H
