#ifndef GEO_NETWORK_CLIENT_ETHEREUMPAYMENTRECEIPTPROCESSINGTRANSACTION_H
#define GEO_NETWORK_CLIENT_ETHEREUMPAYMENTRECEIPTPROCESSINGTRANSACTION_H

#include "../../base/BaseTransaction.h"
#include "../../../../network/messages/payments/EthereumOutgoingReceiptMessage.h"
#include "../../../../trust_lines/manager/TrustLinesManager.h"
#include "../../../../ethereum/EthereumSiganturesManager.h"

#include "../../../../ethereum/connector.h"

class EthereumPaymentReceiptProcessingTransaction : public BaseTransaction {

public:
    typedef shared_ptr<EthereumPaymentReceiptProcessingTransaction> Shared;

public:
    EthereumPaymentReceiptProcessingTransaction(
        const NodeUUID &nodeUUID,
        EthereumOutgoingReceiptMessage::Shared message,
        TrustLinesManager* trustLines,
        EthereumSiganturesManager* signaturesManager,
        Logger &log);

    TransactionResult::SharedConst run()
        noexcept;

protected:
    const string logHeader() const;

private:
    EthereumOutgoingReceiptMessage::Shared mMessage;
    TrustLinesManager* mTrustLines;
    EthereumSiganturesManager *mSignaturesManager;
};


#endif //GEO_NETWORK_CLIENT_ETHEREUMPAYMENTRECEIPTPROCESSINGTRANSACTION_H
