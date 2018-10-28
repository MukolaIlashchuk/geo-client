#ifndef GEO_NETWORK_CLIENT_ETHEREUMAUDITTARGETTRANSACTION_H
#define GEO_NETWORK_CLIENT_ETHEREUMAUDITTARGETTRANSACTION_H

#include "../base/BaseTransaction.h"
#include "../../../network/messages/trust_lines/EthereumAuditMessage.h"
#include "../../../network/messages/trust_lines/EthereumAuditResponseMessage.h"
#include "../../../trust_lines/manager/TrustLinesManager.h"
#include "../../../ethereum/EthereumSiganturesManager.h"

#include "../../../ethereum/connector.h"

class EthereumAuditTargetTransaction : public BaseTransaction {

public:
    typedef shared_ptr<EthereumAuditTargetTransaction> Shared;

public:
    EthereumAuditTargetTransaction(
        const NodeUUID &nodeUUID,
        const string &ethereumAddress,
        const string &bitcoinPublicKey,
        EthereumAuditMessage::Shared message,
        TrustLinesManager *manager,
        EthereumSiganturesManager *ethereumSiganturesManager,
        Logger &logger);

    TransactionResult::SharedConst run();

protected: // log
    const string logHeader() const;

private:
    string mEthereumAddress;
    string mBitcoinPublicKey;
    EthereumAuditMessage::Shared mMessage;
    TrustLinesManager *mTrustLinesManager;
    EthereumSiganturesManager *mEthereumSiganturesManager;
};


#endif //GEO_NETWORK_CLIENT_ETHEREUMAUDITTARGETTRANSACTION_H
