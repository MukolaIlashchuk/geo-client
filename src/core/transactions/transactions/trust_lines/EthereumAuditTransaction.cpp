#include "EthereumAuditTransaction.h"

EthereumAuditTransaction::EthereumAuditTransaction(
    const NodeUUID &nodeUUID,
    const string &ethereumAddress,
    const string &bitcoinPublicKey,
    EthereumAuditCommand::Shared command,
    TrustLinesManager *manager,
    EthereumSiganturesManager* ethereumSiganturesManager,
    Logger &logger) :

    BaseTransaction(
        BaseTransaction::EthereumAuditTransactionType,
        nodeUUID,
        command->equivalent(),
        logger),
    mCommand(command),
    mEthereumAddress(ethereumAddress),
    mBitcoinPublicKey(bitcoinPublicKey),
    mTrustLinesManager(manager),
    mEthereumSiganturesManager(ethereumSiganturesManager)
{}

TransactionResult::SharedConst EthereumAuditTransaction::run()
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

TransactionResult::SharedConst EthereumAuditTransaction::runInitializationStage()
{
    info() << "Audit to " << mCommand->contractorUUID();

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

    if (mEquivalent == 1 or mEquivalent == 2) {
        auto channel = make_shared<eth::Channel>(
                mTrustLinesManager->ethereumChannelId(mCommand->contractorUUID()),
                mTrustLinesManager->contractorEthereumAddress(mCommand->contractorUUID()));
        info() << "channel built";


        auto trustLine = mTrustLinesManager->trustLineReadOnly(mCommand->contractorUUID());
        auto aliceAmount = absoluteBalanceAmount(trustLine->incomingTrustAmount() + trustLine->balance());
        auto bobAmount = absoluteBalanceAmount(trustLine->outgoingTrustAmount() - trustLine->balance());
        auto audit = make_shared<eth::CooperativeClose>(
                trustLine->ethereumChannelId(),
                aliceAmount,
                bobAmount);

        pair<string, bool> aliceSignature;
        if (mEquivalent == 1) {
            info() << "get alice signature for eth";
            aliceSignature = eth::api::generateCooperativeCloseSignature(channel, audit);
        } else if (mEquivalent == 2) {
            info() << "get alice signature for stable coin";
            aliceSignature = eth::api::generateCooperativeCloseSignatureErc20(channel, audit);
        } else {
            warning() << "invalid token id";
            return resultDone();
        }

        if (!aliceSignature.second) {
            warning() << "Can't sign audit";
            return resultDone();
        }
        info() << "Audit signed A " << aliceAmount << " B " << bobAmount;
        audit->addAliceSignature(aliceSignature.first);
        sendMessage<EthereumAuditMessage>(
                mCommand->contractorUUID(),
                mEquivalent,
                mNodeUUID,
                mTransactionUUID,
                audit,
                "");

    } else if (mEquivalent == 3) {
        info() << "BTC";
        auto trustLine = mTrustLinesManager->trustLineReadOnly(mCommand->contractorUUID());
        auto aliceAmount = absoluteBalanceAmount(trustLine->incomingTrustAmount() + trustLine->balance());
        auto bobAmount = absoluteBalanceAmount(trustLine->outgoingTrustAmount() - trustLine->balance());
        // bitcoin fee
        aliceAmount = aliceAmount - TrustLineAmount(2000);
        //bobAmount = bobAmount - TrustLineAmount(1000);

        info() << "Audit Btc " << trustLine->incomingTrustAmount() << " " << aliceAmount << " " << bobAmount;
        auto signature = eth::api::generateCooperativeCloseSignatureBtc(
                mBitcoinPublicKey,
                mTrustLinesManager->contractorBitcoinPublicKey(
                        mCommand->contractorUUID()),
                trustLine->incomingTrustAmount(),
                aliceAmount,
                bobAmount);
        if (!signature.second) {
            warning() << "Can't sign audit " << signature.first;
            return resultDone();
        }
        info() << "Signature " << signature.first;

        sendMessage<EthereumAuditMessage>(
                mCommand->contractorUUID(),
                mEquivalent,
                mNodeUUID,
                mTransactionUUID,
                nullptr,
                signature.first);
    }

    mStep = TrustLineResponseProcessing;
    return resultOK();
}

TransactionResult::SharedConst EthereumAuditTransaction::runResponseProcessingStage()
{
    if (mContext.empty()) {
        warning() << "Contractor don't send response. Transaction will be closed, and wait for message";
        return resultDone();
    }

    auto message = popNextMessage<EthereumAuditResponseMessage>();
    info() << "contractor " << message->senderUUID << " send response on audit";
    if (message->senderUUID != mCommand->contractorUUID()) {
        warning() << "Sender is not contractor of this transaction";
        return resultContinuePreviousState();
    }

    if (message->state() == EthereumAuditResponseMessage::Reject) {
        warning() << "Contractor reject our signature";
        return resultDone();
    }

    if (mEquivalent == 1 or mEquivalent == 2) {
        auto channel = make_shared<eth::Channel>(
                mTrustLinesManager->ethereumChannelId(mCommand->contractorUUID()),
                mTrustLinesManager->contractorEthereumAddress(mCommand->contractorUUID()));
        info() << "channel built";

        pair<bool, bool> check;
        if (mEquivalent == 1) {
            info() << "verify bob signature for eth";
            check = eth::api::verifyCooperativeCloseSignature(
                    channel,
                    message->audit(),
                    message->audit()->mBobSignature);
        } else if (mEquivalent == 2) {
            info() << "verify bob signature for stable coin";
            check = eth::api::verifyCooperativeCloseSignatureErc20(
                    channel,
                    message->audit(),
                    message->audit()->mBobSignature);
        } else {
            warning() << "invalid token id";
            return resultDone();
        }

        if (!check.second) {
            warning() << "Can't execute operation";
            return resultDone();
        }

        if (!check.first) {
            warning() << "Signature is incorrect";
            return resultDone();
        }

        info() << "Signature is correct";

        mEthereumSiganturesManager->setAudit(
                mCommand->contractorUUID(),
                message->audit());

        pair<string, bool> result;
        if (mEquivalent == 1) {
            info() << "close channel for eth";
            result = eth::api::closeChannelThroughCooperativeClose(
                    channel,
                    message->audit());
        } else if (mEquivalent == 2) {
            info() << "close channel for stable coin";
            result = eth::api::closeChannelThroughCooperativeCloseErc20(
                    channel,
                    message->audit());
        } else {
            warning() << "invalid token id";
            return resultDone();
        }

        if (!result.second) {
            warning() << "Can't close channel";
            return resultDone();
        }

        pair<string, bool> transactionState;
        if (mEquivalent == 1) {
            info() << "get transaction state for eth";
            transactionState = eth::api::transactionStatus(result.first);
        } else if (mEquivalent == 2) {
            info() << "get transaction state for stable coin";
            transactionState = eth::api::transactionStatusErc20(result.first);
        } else {
            warning() << "invalid token id";
            return resultDone();
        }

        if (!transactionState.second) {
            warning() << "Can't get transaction state";
            return resultDone();
        }

        while (transactionState.first != "CONFIRMED") {
            info() << "Transaction state: " << transactionState.first;
            sleep(5);
            if (mEquivalent == 1) {
                info() << "get transaction state for eth";
                transactionState = eth::api::transactionStatus(result.first);
            } else if (mEquivalent == 2) {
                info() << "get transaction state for stable coin";
                transactionState = eth::api::transactionStatusErc20(result.first);
            } else {
                warning() << "invalid token id";
                return resultDone();
            }
        }

    } else if (mEquivalent == 3) {
        mBitcoinTransactionID = message->bitcoinTransactionID();
        info() << "TransactionID " << mBitcoinTransactionID;
        auto channelInfo = eth::api::transactionStatusBitcoin(mBitcoinTransactionID);
        if (!channelInfo.second) {
            warning() << "Can't get channel info";
            return resultDone();
        }

        while (channelInfo.first == "0") {
            info() << "Transaction state: " << channelInfo.first;
            sleep(5);
            info() << "get transaction state for btc";
            channelInfo = eth::api::transactionStatusBitcoin(mBitcoinTransactionID);
        }
        info() << "CONFIRMED " << channelInfo.first;
    }
    info() << "Chanel closed";

    return resultDone();
}

TransactionResult::SharedConst EthereumAuditTransaction::resultOK()
{
    return transactionResultFromCommandAndWaitForMessageTypes(
        mCommand->responseOK(),
        {Message::TrustLines_EthereumAuditResponse},
        10000);
}

TransactionResult::SharedConst EthereumAuditTransaction::resultProtocolError()
{
    return transactionResultFromCommand(
        mCommand->responseProtocolError());
}


const string EthereumAuditTransaction::logHeader() const
noexcept
{
    stringstream s;
    s << "[EthereumAuditTA: " << currentTransactionUUID() << " " << mEquivalent << "]";
    return s.str();
}