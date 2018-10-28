#include "EthereumSendPaymentReceiptsTransaction.h"

EthereumSendPaymentReceiptsTransaction::EthereumSendPaymentReceiptsTransaction(
    const NodeUUID &nodeUUID,
    const SerializedEquivalent equivalent,
    map<NodeUUID, TrustLineAmount> outgoingReceipts,
    TrustLinesManager* trustLinesManager,
    Logger &log) :
    BaseTransaction(
        BaseTransaction::Payments_EthereumSendPaymentReceipts,
        nodeUUID,
        equivalent,
        log),
    mOutgoingReceipts(outgoingReceipts),
    mTrustLinesManager(trustLinesManager)
{}

TransactionResult::SharedConst EthereumSendPaymentReceiptsTransaction::run()
    noexcept
{
    info() << "run";
    for (const auto &nodeUUIDAndOutgoingReceipt : mOutgoingReceipts) {
        if (!mTrustLinesManager->isStateChannel(nodeUUIDAndOutgoingReceipt.first)) {
            continue;
        }

        auto trustLine = mTrustLinesManager->trustLineReadOnly(nodeUUIDAndOutgoingReceipt.first);
        auto receiptAmount = nodeUUIDAndOutgoingReceipt.second;
        auto outgoingReceipt = make_shared<eth::Receipt>(
            trustLine->currentPaymentReceiptId(),
            trustLine->currentEpochId(),
            receiptAmount,
            trustLine->contractorEthereumAddress());

        mTrustLinesManager->incrementPaymentReceiptId(nodeUUIDAndOutgoingReceipt.first);
        auto signedReceipt = eth::api::signReceipt(outgoingReceipt);

        if (!signedReceipt.second) {
            warning() << "Can't sign receipt with contractor " << nodeUUIDAndOutgoingReceipt.first;
            return resultDone();
        }

        info() << "Receipt to " << nodeUUIDAndOutgoingReceipt.first << " successfully signed";

        sendMessage<EthereumOutgoingReceiptMessage>(
            nodeUUIDAndOutgoingReceipt.first,
            mEquivalent,
            mNodeUUID,
            signedReceipt.first);
        info() << "send receipt " << nodeUUIDAndOutgoingReceipt.second << " to " << nodeUUIDAndOutgoingReceipt.first;
    }
    return resultDone();
}

const string EthereumSendPaymentReceiptsTransaction::logHeader() const
{
    stringstream s;
    s << "[EthereumSendPaymentReceiptsTA: " << currentTransactionUUID() << " " << mEquivalent << "] ";
    return s.str();
}