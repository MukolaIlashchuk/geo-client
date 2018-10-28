#include "EthereumCloseChannelTransaction.h"

EthereumCloseChannelTransaction::EthereumCloseChannelTransaction(
    const NodeUUID &nodeUUID,
    const string &ethereumAddress,
    EthereumCloseChannelCommand::Shared command,
    TrustLinesManager *manager,
    Logger &logger) :

    BaseTransaction(
        BaseTransaction::EthereumCloseChannelTransactionType,
        nodeUUID,
        command->equivalent(),
        logger),
    mCommand(command),
    mEthereumAddress(ethereumAddress),
    mTrustLinesManager(manager)
{}

TransactionResult::SharedConst EthereumCloseChannelTransaction::run()
{
    info() << "step " << mStep;
    switch (mStep) {
        case Stages::TrustLineInitialization: {
            return runInitializationStage();
        }
        case Stages::TrustLineResponseProcessing: {
            return runResponseProcessingStage();
        }
        default:
            throw ValueError(logHeader() + "::run: wrong value of mStep");
    }
}

TransactionResult::SharedConst EthereumCloseChannelTransaction::runInitializationStage()
{
    info() << "CloseChannel to " << mCommand->contractorUUID();

    if (mCommand->contractorUUID() == mNodeUUID) {
        warning() << "Attempt to launch transaction against itself was prevented.";
        return resultProtocolError();
    }

    if (!mTrustLinesManager->trustLineIsPresent(mCommand->contractorUUID())) {
        warning() << "Trust line is absent.";
        return resultProtocolError();
    }

    if (!mTrustLinesManager->isStateChannel(mCommand->contractorUUID())) {
        warning() << "Not state channel";
        return resultProtocolError();
    }


    // todo [hackathon] : sign audit and sent it to contractor

    mStep = TrustLineResponseProcessing;
    return resultOK();
}

TransactionResult::SharedConst EthereumCloseChannelTransaction::runResponseProcessingStage()
{
    if (mContext.empty()) {
        warning() << "Contractor don't send response. Transaction will be closed, and wait for message";
        return resultDone();
    }
    // todo [hackathon] : check signature and save audit
    return resultDone();
}

TransactionResult::SharedConst EthereumCloseChannelTransaction::resultOK()
{
    return transactionResultFromCommandAndWaitForMessageTypes(
        mCommand->responseOK(),
        // todo [hackathon] : set correct message type
        {Message::TrustLines_Confirmation},
        10000);
}

TransactionResult::SharedConst EthereumCloseChannelTransaction::resultProtocolError()
{
    return transactionResultFromCommand(
        mCommand->responseProtocolError());
}


const string EthereumCloseChannelTransaction::logHeader() const
noexcept
{
    stringstream s;
    s << "[EthereumCloseChannelTA: " << currentTransactionUUID() << " " << mEquivalent << "]";
    return s.str();
}