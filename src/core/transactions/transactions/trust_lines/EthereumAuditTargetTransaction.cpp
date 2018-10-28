#include "EthereumAuditTargetTransaction.h"

EthereumAuditTargetTransaction::EthereumAuditTargetTransaction(
    const NodeUUID &nodeUUID,
    const string &ethereumAddress,
    const string &bitcoinPublicKey,
    EthereumAuditMessage::Shared message,
    TrustLinesManager *manager,
    EthereumSiganturesManager *ethereumSiganturesManager,
    Logger &logger):
    BaseTransaction(
        BaseTransaction::EthereumAuditTargetTransactionType,
        message->transactionUUID(),
        nodeUUID,
        message->equivalent(),
        logger),
    mEthereumAddress(ethereumAddress),
    mBitcoinPublicKey(bitcoinPublicKey),
    mMessage(message),
    mTrustLinesManager(manager),
    mEthereumSiganturesManager(ethereumSiganturesManager)
{}

TransactionResult::SharedConst EthereumAuditTargetTransaction::run()
{
    info() << "runInitializationStage";
    if (!mTrustLinesManager->trustLineIsPresent(mMessage->senderUUID)) {
        warning() << "Trust Line is absent";
        sendMessage<EthereumAuditResponseMessage>(
            mMessage->senderUUID,
            mEquivalent,
            mNodeUUID,
            mTransactionUUID,
            EthereumAuditResponseMessage::Reject,
            nullptr,
            "");
        return resultDone();
    }

    if (mEquivalent == 1 or mEquivalent == 2) {
        auto channel = make_shared<eth::Channel>(
                mTrustLinesManager->ethereumChannelId(mMessage->senderUUID),
                mTrustLinesManager->contractorEthereumAddress(mMessage->senderUUID));
        info() << "channel built";

        pair<bool, bool> check;
        if (mEquivalent == 1) {
            info() << "verify alice signature for eth";
            check = eth::api::verifyCooperativeCloseSignature(
                    channel,
                    mMessage->audit(),
                    mMessage->audit()->mAliceSignature);
        } else if (mEquivalent == 2) {
            info() << "verify alice signature for stable coin";
            check = eth::api::verifyCooperativeCloseSignatureErc20(
                    channel,
                    mMessage->audit(),
                    mMessage->audit()->mAliceSignature);
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

        pair<string, bool> bobSignature;
        if (mEquivalent == 1) {
            info() << "generate bob signature for eth";
            bobSignature = eth::api::generateCooperativeCloseSignature(channel, mMessage->audit());
        } else if (mEquivalent == 2) {
            info() << "generate bob signature for stable coin";
            bobSignature = eth::api::generateCooperativeCloseSignatureErc20(channel, mMessage->audit());
        } else {
            warning() << "invalid token id";
            return resultDone();
        }

        if (!bobSignature.second) {
            warning() << "Can't sign audit";
            return resultDone();
        }
        info() << "Audit signed ";
        mMessage->audit()->addBobSignature(bobSignature.first);

        mEthereumSiganturesManager->setAudit(
                mMessage->senderUUID,
                mMessage->audit());

        sendMessage<EthereumAuditResponseMessage>(
                mMessage->senderUUID,
                mEquivalent,
                mNodeUUID,
                mTransactionUUID,
                EthereumAuditResponseMessage::OK,
                mMessage->audit(),
                "");

    } else if (mEquivalent == 3) {
        info() << "Bitcoin Audit: " << mTrustLinesManager->outgoingTrustAmount(mMessage->senderUUID);
        info() << "Alice signature " << mMessage->bitcoinSignature();
        auto transactionID = eth::api::closeChannelThroughCooperativeCloseBtc(
                mBitcoinPublicKey,
                mTrustLinesManager->contractorBitcoinPublicKey(
                        mMessage->senderUUID),
                mTrustLinesManager->outgoingTrustAmount(
                        mMessage->senderUUID),
                mMessage->bitcoinSignature());

        if (!transactionID.second) {
            warning() << "Can't close channel: " << transactionID.first;
            return resultDone();
        }

        if (transactionID.first.size() != kBitcoinTransactionIDHexSize) {
            warning() << "Can't close channel: invalid txid " << transactionID.first;
            return resultDone();
        }

        info() << "TransactionID: " << transactionID.first;
        sendMessage<EthereumAuditResponseMessage>(
                mMessage->senderUUID,
                mEquivalent,
                mNodeUUID,
                mTransactionUUID,
                EthereumAuditResponseMessage::OK,
                nullptr,
                transactionID.first);
    }

    return resultDone();
}

const string EthereumAuditTargetTransaction::logHeader() const
{
    stringstream s;
    s << "[EthereumAuditTargetTA: " << currentTransactionUUID() << " " << mEquivalent << "]";
    return s.str();
}