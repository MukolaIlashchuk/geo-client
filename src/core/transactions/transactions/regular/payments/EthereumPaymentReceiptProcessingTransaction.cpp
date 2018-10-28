#include "EthereumPaymentReceiptProcessingTransaction.h"

EthereumPaymentReceiptProcessingTransaction::EthereumPaymentReceiptProcessingTransaction(
    const NodeUUID &nodeUUID,
    EthereumOutgoingReceiptMessage::Shared message,
    TrustLinesManager* trustLines,
    EthereumSiganturesManager* signaturesManager,
    Logger &log) :
    BaseTransaction(
        BaseTransaction::Payments_EthereumPaymentReceiptProcessing,
        nodeUUID,
        message->equivalent(),
        log),
    mMessage(message),
    mTrustLines(trustLines),
    mSignaturesManager(signaturesManager)
{}

TransactionResult::SharedConst EthereumPaymentReceiptProcessingTransaction::run()
    noexcept
{
    info() << "Node " << mMessage->senderUUID << " send receipt " << mMessage->receipt()->mAmount;

    // ??????
    mMessage->receipt()->mReceiverAddressHex = mTrustLines->contractorEthereumAddress(mMessage->senderUUID);

    auto check = eth::api::verifyReceiptSignature(
        mMessage->receipt());

    if (!check.second) {
        warning() << "Can't execute operation";
        return resultDone();
    }

    if (!check.first) {
        warning() << "Signature is incorrect";
        return resultDone();
    }

    info() << "Signature is correct";

    mSignaturesManager->addPaymentReceipt(
        mMessage->senderUUID,
        mMessage->receipt());

    mTrustLines->setTrustLineState(
        mMessage->senderUUID,
        TrustLine::Active);
    return resultDone();
}

const string EthereumPaymentReceiptProcessingTransaction::logHeader() const
{
    stringstream s;
    s << "[EthereumPaymentReceiptProcessingTA: " << currentTransactionUUID() << " " << mEquivalent << "] ";
    return s.str();
}