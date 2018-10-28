#ifndef GEO_NETWORK_CLIENT_ETHEREUMCLOSECHANNELTRANSACTION_H
#define GEO_NETWORK_CLIENT_ETHEREUMCLOSECHANNELTRANSACTION_H

#include "../base/BaseTransaction.h"
#include "../../../interface/commands_interface/commands/trust_lines/EthereumCloseChannelCommand.h"

#include "../../../trust_lines/manager/TrustLinesManager.h"

class EthereumCloseChannelTransaction : public BaseTransaction {

public:
    typedef shared_ptr<EthereumCloseChannelTransaction> Shared;

public:
    EthereumCloseChannelTransaction(
        const NodeUUID &nodeUUID,
        const string &ethereumAddress,
        EthereumCloseChannelCommand::Shared command,
        TrustLinesManager *manager,
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

protected:
    const string logHeader() const
    noexcept;

private:
    TransactionResult::SharedConst runInitializationStage();

    TransactionResult::SharedConst runResponseProcessingStage();

protected:
    EthereumCloseChannelCommand::Shared mCommand;
    string mEthereumAddress;
    TrustLinesManager* mTrustLinesManager;
};


#endif //GEO_NETWORK_CLIENT_ETHEREUMCLOSECHANNELTRANSACTION_H
