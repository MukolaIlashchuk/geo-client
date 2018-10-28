#include "CyclesFourNodesBalancesResponseMessage.h"

CyclesFourNodesBalancesResponseMessage::CyclesFourNodesBalancesResponseMessage(
    const SerializedEquivalent equivalent,
    const NodeUUID &senderUUID,
    const TransactionUUID &transactionUUID,
    vector<NodeUUID> &suitableNodes):

    TransactionMessage(
        equivalent,
        senderUUID,
        transactionUUID),
    mSuitableNodes(suitableNodes)
{}

CyclesFourNodesBalancesResponseMessage::CyclesFourNodesBalancesResponseMessage(
    BytesShared buffer):

    TransactionMessage(buffer)
{
    size_t bytesBufferOffset = TransactionMessage::kOffsetToInheritedBytes();
    SerializedRecordsCount suitableNodesCount;
    memcpy(
        &suitableNodesCount,
        buffer.get() + bytesBufferOffset,
        sizeof(SerializedRecordsCount));
    bytesBufferOffset += sizeof(SerializedRecordsCount);

    for (SerializedRecordNumber i = 1; i <= suitableNodesCount; ++i) {
        NodeUUID stepNode(buffer.get() + bytesBufferOffset);
        bytesBufferOffset += NodeUUID::kBytesSize;
        mSuitableNodes.push_back(stepNode);
    }
}

pair<BytesShared, size_t> CyclesFourNodesBalancesResponseMessage::serializeToBytes() const
    throw(bad_alloc)
{
    auto parentBytesAndCount = TransactionMessage::serializeToBytes();

    auto debtorsCount = (SerializedRecordsCount)mSuitableNodes.size();
    size_t bytesCount = parentBytesAndCount.second
                        + sizeof(SerializedRecordsCount)
                        + debtorsCount * NodeUUID::kBytesSize;

    BytesShared dataBytesShared = tryCalloc(bytesCount);
    size_t dataBytesOffset = 0;
    //----------------------------------------------------
    memcpy(
        dataBytesShared.get(),
        parentBytesAndCount.first.get(),
        parentBytesAndCount.second);
    dataBytesOffset += parentBytesAndCount.second;

    memcpy(
        dataBytesShared.get() + dataBytesOffset,
        &debtorsCount,
        sizeof(SerializedRecordsCount));
    dataBytesOffset += sizeof(SerializedRecordsCount);

    for(auto const &debtor: mSuitableNodes) {
        memcpy(
            dataBytesShared.get() + dataBytesOffset,
            &debtor,
            NodeUUID::kBytesSize);
        dataBytesOffset += NodeUUID::kBytesSize;
    }

    return make_pair(
        dataBytesShared,
        bytesCount);
}

vector<NodeUUID> CyclesFourNodesBalancesResponseMessage::suitableNodes() const
{
    return mSuitableNodes;
}

const Message::MessageType CyclesFourNodesBalancesResponseMessage::typeID() const
{
    return Message::MessageType::Cycles_FourNodesBalancesResponse;
}