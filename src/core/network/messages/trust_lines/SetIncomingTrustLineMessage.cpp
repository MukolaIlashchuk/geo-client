#include "SetIncomingTrustLineMessage.h"


SetIncomingTrustLineMessage::SetIncomingTrustLineMessage(
    const SerializedEquivalent equivalent,
    const NodeUUID &sender,
    const TransactionUUID &transactionUUID,
    const NodeUUID &destination,
    const TrustLineAmount &amount,
    const string ethereumAddress,
    const string bitcoinPublicKey)
    noexcept :

    DestinationMessage(
        equivalent,
        sender,
        transactionUUID,
        destination),
    mAmount(amount),
    mEthereumAddress(ethereumAddress),
    mBitcoinPublicKey(bitcoinPublicKey)
{}

SetIncomingTrustLineMessage::SetIncomingTrustLineMessage(
    BytesShared buffer)
    noexcept :
    DestinationMessage(buffer)
{
    // todo: use desrializer

    size_t bytesBufferOffset = DestinationMessage::kOffsetToInheritedBytes();
    //----------------------------------------------------
    vector<byte> amountBytes(
        buffer.get() + bytesBufferOffset,
        buffer.get() + bytesBufferOffset + kTrustLineAmountBytesCount);
    mAmount = bytesToTrustLineAmount(amountBytes);
    bytesBufferOffset += kTrustLineAmountBytesCount;

    string ethereumAddressBuffer(
            (char*)(buffer.get() + bytesBufferOffset),
            kEthereumAddressHexSize);
    bytesBufferOffset += kEthereumAddressHexSize;
    if (ethereumAddressBuffer != "0x0000000000000000000000000000000000000000") {
        mEthereumAddress = ethereumAddressBuffer;
    } else {
        mEthereumAddress = "";
    }

    string bitcoinPublicKeyBuffer(
                (char*)(buffer.get() + bytesBufferOffset),
                kBitcoinPublicKeyHexSize);
    if (bitcoinPublicKeyBuffer != "000000000000000000000000000000000000000000000000000000000000000000") {
        mBitcoinPublicKey = bitcoinPublicKeyBuffer;
    } else {
        mBitcoinPublicKey = "";
    }
}


const Message::MessageType SetIncomingTrustLineMessage::typeID() const
    noexcept
{
    return Message::TrustLines_SetIncoming;
}

const TrustLineAmount &SetIncomingTrustLineMessage::amount() const
    noexcept
{
    return mAmount;
}

const string SetIncomingTrustLineMessage::ethereumAddress() const
{
    return mEthereumAddress;
}

const string SetIncomingTrustLineMessage::bitcoinPublicKey() const
{
    return mBitcoinPublicKey;
}

pair<BytesShared, size_t> SetIncomingTrustLineMessage::serializeToBytes() const
{
    // todo: use serializer

    auto parentBytesAndCount = DestinationMessage::serializeToBytes();

    size_t bytesCount = parentBytesAndCount.second
                        + kTrustLineAmountBytesCount
                        + kEthereumAddressHexSize
                        + kBitcoinPublicKeyHexSize;

    BytesShared dataBytesShared = tryCalloc(bytesCount);
    size_t dataBytesOffset = 0;
    //----------------------------------------------------
    memcpy(
        dataBytesShared.get(),
        parentBytesAndCount.first.get(),
        parentBytesAndCount.second);
    dataBytesOffset += parentBytesAndCount.second;
    //----------------------------------------------------
    vector<byte> buffer = trustLineAmountToBytes(mAmount);
    memcpy(
        dataBytesShared.get() + dataBytesOffset,
        buffer.data(),
        kTrustLineAmountBytesCount);
    dataBytesOffset += kTrustLineAmountBytesCount;
    //----------------------------
    if (!mEthereumAddress.empty()) {
        memcpy(
            dataBytesShared.get() + dataBytesOffset,
            mEthereumAddress.c_str(),
            kEthereumAddressHexSize);
        dataBytesOffset += kEthereumAddressHexSize;
    } else {
        string ethereumAddressBuffer = "0x0000000000000000000000000000000000000000";
        memcpy(
                dataBytesShared.get() + dataBytesOffset,
                ethereumAddressBuffer.c_str(),
                kEthereumAddressHexSize);
        dataBytesOffset += kEthereumAddressHexSize;
    }
    //----------------------------
    if (!mBitcoinPublicKey.empty()) {
        memcpy(
                dataBytesShared.get() + dataBytesOffset,
                mBitcoinPublicKey.c_str(),
                kBitcoinPublicKeyHexSize);
    } else {
        string bitcoinPublicKeyBuffer = "000000000000000000000000000000000000000000000000000000000000000000";
        memcpy(
                dataBytesShared.get() + dataBytesOffset,
                bitcoinPublicKeyBuffer.c_str(),
                kBitcoinPublicKeyHexSize);
    }
    //----------------------------
    return make_pair(
        dataBytesShared,
        bytesCount);
}

const bool SetIncomingTrustLineMessage::isAddToConfirmationRequiredMessagesHandler() const
{
    return true;
}

const bool SetIncomingTrustLineMessage::isCheckCachedResponse() const
{
    return true;
}

const size_t SetIncomingTrustLineMessage::kOffsetToInheritedBytes() const
    noexcept
{
    static const auto kOffset =
            DestinationMessage::kOffsetToInheritedBytes()
            + kTrustLineAmountBytesCount
            + kEthereumAddressHexSize
            + kBitcoinPublicKeyHexSize;

    return kOffset;
}
