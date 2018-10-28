#include "EthereumAuditMessage.h"

EthereumAuditMessage::EthereumAuditMessage(
    const SerializedEquivalent equivalent,
    const NodeUUID &senderUUID,
    const TransactionUUID &transactionUUID,
    CooperativeClose::Shared audit,
    const string &bitcoinSignature):
    TransactionMessage(
        equivalent,
        senderUUID,
        transactionUUID),
    mAudit(audit),
    mBitcoinSignature(bitcoinSignature)
{}

EthereumAuditMessage::EthereumAuditMessage(
    BytesShared buffer) :
    TransactionMessage(buffer)
{
    auto bytesBufferOffset = TransactionMessage::kOffsetToInheritedBytes();

    if (equivalent() == 1 or equivalent() == 2) {
        auto audit = make_shared<CooperativeClose>(buffer.get() + bytesBufferOffset);
        mAudit = audit;
        mBitcoinSignature = "";
    } else if (equivalent() == 3) {
        mAudit = nullptr;
        string bitcoinSignature(
                (char*)(buffer.get() + bytesBufferOffset),
                kBitcoinSignatureHexSize);
        mBitcoinSignature = bitcoinSignature;
    }
}

const Message::MessageType EthereumAuditMessage::typeID() const
{
    return Message::TrustLines_EthereumAudit;
}

CooperativeClose::Shared EthereumAuditMessage::audit() const
{
    return mAudit;
}

const string EthereumAuditMessage::bitcoinSignature() const
{
    return mBitcoinSignature;
}

pair<BytesShared, size_t> EthereumAuditMessage::serializeToBytes() const
    throw (bad_alloc)
{
    const auto parentBytesAndCount = TransactionMessage::serializeToBytes();
    auto kBufferSize = parentBytesAndCount.second;
    if (equivalent() == 1 or equivalent() == 2) {
        kBufferSize += mAudit->serializedDataSize();
    } else if (equivalent() == 3) {
        kBufferSize += kBitcoinSignatureHexSize;
    }
    BytesShared buffer = tryMalloc(kBufferSize);

    size_t dataBytesOffset = 0;
    // Parent message content
    memcpy(
        buffer.get(),
        parentBytesAndCount.first.get(),
        parentBytesAndCount.second);
    dataBytesOffset += parentBytesAndCount.second;

    if (equivalent() == 1 or equivalent() == 2) {
        memcpy(
                buffer.get() + dataBytesOffset,
                mAudit->serializeToBytes().get(),
                mAudit->serializedDataSize());
    } else if (equivalent() == 3) {
        memcpy(
                buffer.get() + dataBytesOffset,
                mBitcoinSignature.c_str(),
                kBitcoinSignatureHexSize);
    }

    return make_pair(
        buffer,
        kBufferSize);
}