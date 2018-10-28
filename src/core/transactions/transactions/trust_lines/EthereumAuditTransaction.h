#ifndef GEO_NETWORK_CLIENT_ETHEREUMAUDITTRANSACTION_H
#define GEO_NETWORK_CLIENT_ETHEREUMAUDITTRANSACTION_H

#include "../base/BaseTransaction.h"
#include "../../../interface/commands_interface/commands/trust_lines/EthereumAuditCommand.h"
#include "../../../network/messages/trust_lines/EthereumAuditMessage.h"
#include "../../../network/messages/trust_lines/EthereumAuditResponseMessage.h"

#include "../../../trust_lines/manager/TrustLinesManager.h"
#include "../../../ethereum/EthereumSiganturesManager.h"

#include "../../../ethereum/connector.h"

class EthereumAuditTransaction : public BaseTransaction {

public:
    typedef shared_ptr<EthereumAuditTransaction> Shared;

public:
    EthereumAuditTransaction(
        const NodeUUID &nodeUUID,
        const string &ethereumAddress,
        const string &bitcoinPublicKey,
        EthereumAuditCommand::Shared command,
        TrustLinesManager *manager,
        EthereumSiganturesManager* ethereumSiganturesManager,
        Logger &logger);

    TransactionResult::SharedConst run();

protected:
    enum Stages {
        TrustLineInitialization = 1,
        TrustLineResponseProcessing = 2,
    };

protected:
    TransactionResult::SharedConst resultOK();

    TransactionResult::SharedConst resultProtocolError();

protected: // trust lines history shortcuts
    const string logHeader() const
    noexcept;

private:
    TransactionResult::SharedConst runInitializationStage();

    TransactionResult::SharedConst runResponseProcessingStage();

protected:
    EthereumAuditCommand::Shared mCommand;
    string mEthereumAddress;
    string mBitcoinPublicKey;
    string mBitcoinTransactionID;
    TrustLinesManager* mTrustLinesManager;
    EthereumSiganturesManager* mEthereumSiganturesManager;
};


#endif //GEO_NETWORK_CLIENT_ETHEREUMAUDITTRANSACTION_H
