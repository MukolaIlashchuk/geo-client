#include "EthereumOutgoingReceiptMessage.h"

EthereumOutgoingReceiptMessage::EthereumOutgoingReceiptMessage(
    const SerializedEquivalent equivalent,
    const NodeUUID &senderUUID,
    Receipt::Shared receipt):
    SenderMessage(
        equivalent,
        senderUUID),
    mReceipt(receipt)
{}

EthereumOutgoingReceiptMessage::EthereumOutgoingReceiptMessage(
    BytesShared buffer) :
    SenderMessage(buffer)
{
    auto bytesBufferOffset = SenderMessage::kOffsetToInheritedBytes();

    auto receipt = make_shared<Receipt>(
        buffer.get() + bytesBufferOffset);

    mReceipt = receipt;
}

const Message::MessageType EthereumOutgoingReceiptMessage::typeID() const
{
    return Message::Payments_EthereumReceipt;
}

Receipt::Shared EthereumOutgoingReceiptMessage::receipt() const
{
    return mReceipt;
}

pair<BytesShared, size_t> EthereumOutgoingReceiptMessage::serializeToBytes() const
    throw (bad_alloc)
{
    const auto parentBytesAndCount = SenderMessage::serializeToBytes();
    auto kBufferSize = parentBytesAndCount.second
            + mReceipt->serializedDataSize();
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
        mReceipt->serializeToBytes().get(),
        mReceipt->serializedDataSize());

    return make_pair(
        buffer,
        kBufferSize);
}
