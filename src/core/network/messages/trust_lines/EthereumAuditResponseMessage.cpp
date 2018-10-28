#include "EthereumAuditResponseMessage.h"

EthereumAuditResponseMessage::EthereumAuditResponseMessage(
    const SerializedEquivalent equivalent,
    const NodeUUID &senderUUID,
    const TransactionUUID &transactionUUID,
    const OperationState state,
    CooperativeClose::Shared audit,
    const string &bitcointTransactionID):
    TransactionMessage(
        equivalent,
        senderUUID,
        transactionUUID),
    mState(state),
    mAudit(audit),
    mBitcointTransactonID(bitcointTransactionID)
{}

EthereumAuditResponseMessage::EthereumAuditResponseMessage(
    BytesShared buffer) :
    TransactionMessage(buffer)
{
    auto bytesBufferOffset = TransactionMessage::kOffsetToInheritedBytes();

    auto *state = new (buffer.get() + bytesBufferOffset) SerializedOperationState;
    mState = (OperationState) (*state);
    bytesBufferOffset += sizeof(SerializedOperationState);

    if (mState == Reject) {
        mAudit = nullptr;
    } else {

        if (equivalent() == 1 or equivalent() == 2) {
            auto audit = make_shared<CooperativeClose>(
                    buffer.get() + bytesBufferOffset);
            mAudit = audit;
            mBitcointTransactonID = "";
        } else if (equivalent() == 3) {
            string bitcointransactionID(
                    (char*)(buffer.get() + bytesBufferOffset),
                    kBitcoinTransactionIDHexSize);
            mBitcointTransactonID = bitcointransactionID;
            mAudit = nullptr;
        }
    }
}

const Message::MessageType EthereumAuditResponseMessage::typeID() const
{
    return Message::TrustLines_EthereumAuditResponse;
}

CooperativeClose::Shared EthereumAuditResponseMessage::audit() const
{
    return mAudit;
}

const string EthereumAuditResponseMessage::bitcoinTransactionID() const
{
    return mBitcointTransactonID;
}

const EthereumAuditResponseMessage::OperationState EthereumAuditResponseMessage::state() const
{
    return mState;
}

pair<BytesShared, size_t> EthereumAuditResponseMessage::serializeToBytes() const
    throw (bad_alloc)
{
    const auto parentBytesAndCount = TransactionMessage::serializeToBytes();
    auto kBufferSize = parentBytesAndCount.second
            + sizeof(SerializedOperationState);
    if (equivalent() == 1 or equivalent() == 2) {
        kBufferSize += mAudit->serializedDataSize();
    } else if (equivalent() == 3) {
        kBufferSize += kBitcoinTransactionIDHexSize;
    }
    BytesShared buffer = tryMalloc(kBufferSize);

    size_t dataBytesOffset = 0;
    // Parent message content
    memcpy(
        buffer.get(),
        parentBytesAndCount.first.get(),
        parentBytesAndCount.second);
    dataBytesOffset += parentBytesAndCount.second;

    memcpy(
        buffer.get() + dataBytesOffset,
        &mState,
        sizeof(SerializedOperationState));
    dataBytesOffset += sizeof(SerializedOperationState);

    if (mState == OK) {
        if (equivalent() == 1 or equivalent() == 2) {
            memcpy(
                    buffer.get() + dataBytesOffset,
                    mAudit->serializeToBytes().get(),
                    mAudit->serializedDataSize());
        } else if (equivalent() == 3) {
            memcpy(
                    buffer.get() + dataBytesOffset,
                    mBitcointTransactonID.c_str(),
                    kBitcoinTransactionIDHexSize);
        }
    }

    return make_pair(
        buffer,
        kBufferSize);
}