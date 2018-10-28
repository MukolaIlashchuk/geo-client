#include "ReceiverInitPaymentRequestMessage.h"

ReceiverInitPaymentRequestMessage::ReceiverInitPaymentRequestMessage(
        const SerializedEquivalent equivalent,
        const NodeUUID &senderUUID,
        const TransactionUUID &transactionUUID,
        const PathID &pathID,
        const TrustLineAmount &amount,
        const NodeUUID intermediateUUID,
        SerializedEquivalent intermediateEquivalent,
        const NodeUUID nextIntermediateUUID,
        SerializedEquivalent nextIntermediateEquivalent):
        RequestMessage(
                equivalent,
                senderUUID,
                transactionUUID,
                pathID,
                amount),
        mIntermediateUUID(intermediateUUID),
        mIntermediateEquivalent(intermediateEquivalent),
        mNextIntermediateUUID(nextIntermediateUUID),
        mNextIntermediateEquivalent(nextIntermediateEquivalent)
{}

ReceiverInitPaymentRequestMessage::ReceiverInitPaymentRequestMessage(
        BytesShared buffer):

        RequestMessage(buffer)
{
    auto parentMessageOffset = RequestMessage::kOffsetToInheritedBytes();
    auto bytesBufferOffset = buffer.get() + parentMessageOffset;

    NodeUUID tmpNodeUUID(bytesBufferOffset);
    mIntermediateUUID = tmpNodeUUID;
    bytesBufferOffset += NodeUUID::kBytesSize;

    memcpy(
            &mIntermediateEquivalent,
            bytesBufferOffset,
            sizeof(SerializedEquivalent));
    bytesBufferOffset += sizeof(SerializedEquivalent);

    NodeUUID tmpNextNodeUUID(bytesBufferOffset);
    mNextIntermediateUUID = tmpNextNodeUUID;
    bytesBufferOffset += NodeUUID::kBytesSize;

    memcpy(
            &mNextIntermediateEquivalent,
            bytesBufferOffset,
            sizeof(SerializedEquivalent));
}

const NodeUUID& ReceiverInitPaymentRequestMessage::intermediateUUID() const
{
    return mIntermediateUUID;
}

const SerializedEquivalent ReceiverInitPaymentRequestMessage::intermediateEquivalent() const
{
    return mIntermediateEquivalent;
}

const NodeUUID& ReceiverInitPaymentRequestMessage::nextIntermediateUUID() const
{
    return mNextIntermediateUUID;
}

const SerializedEquivalent ReceiverInitPaymentRequestMessage::nextIntermediateEquivalent() const
{
    return mNextIntermediateEquivalent;
}

const Message::MessageType ReceiverInitPaymentRequestMessage::typeID() const
{
    return Message::Payments_ReceiverInitPaymentRequest;
}

pair<BytesShared, size_t> ReceiverInitPaymentRequestMessage::serializeToBytes() const
    throw(bad_alloc)
{
    auto parentBytesAndCount = RequestMessage::serializeToBytes();
    size_t bytesCount =
            + parentBytesAndCount.second
            + NodeUUID::kBytesSize
            + sizeof(SerializedEquivalent)
            + NodeUUID::kBytesSize
            + sizeof(SerializedEquivalent);

    BytesShared buffer = tryMalloc(bytesCount);
    auto initialOffset = buffer.get();
    memcpy(
            initialOffset,
            parentBytesAndCount.first.get(),
            parentBytesAndCount.second);

    auto bytesBufferOffset = initialOffset + parentBytesAndCount.second;

    memcpy(
            bytesBufferOffset,
            mIntermediateUUID.data,
            NodeUUID::kBytesSize);
    bytesBufferOffset += NodeUUID::kBytesSize;

    memcpy(
            bytesBufferOffset,
            &mIntermediateEquivalent,
            sizeof(SerializedEquivalent));
    bytesBufferOffset += sizeof(SerializedEquivalent);

    memcpy(
            bytesBufferOffset,
            mNextIntermediateUUID.data,
            NodeUUID::kBytesSize);
    bytesBufferOffset += NodeUUID::kBytesSize;

    memcpy(
            bytesBufferOffset,
            &mNextIntermediateEquivalent,
            sizeof(SerializedEquivalent));

    return make_pair(
            buffer,
            bytesCount);
}
