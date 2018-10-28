#include "TrustLineConfirmationMessage.h"

TrustLineConfirmationMessage::TrustLineConfirmationMessage(
    const SerializedEquivalent equivalent,
    const NodeUUID &senderUUID,
    const TransactionUUID &transactionUUID,
    bool isContractorGateway,
    const OperationState state,
    const string &ethereumAddress,
    const string &ethereumChannelId,
    const string &bitcoinPublicKey,
    const string &bitcoinTransactonID) :

    ConfirmationMessage(
        equivalent,
        senderUUID,
        transactionUUID,
        state),
    mIsContractorGateway(isContractorGateway),
    mEthereumAddress(ethereumAddress),
    mEthereumChannelId(ethereumChannelId),
    mBitcoinPublicKey(bitcoinPublicKey),
    mBitcoinTransactionID(bitcoinTransactonID)
{}

TrustLineConfirmationMessage::TrustLineConfirmationMessage(
    BytesShared buffer):

    ConfirmationMessage(buffer)
{
    size_t bytesBufferOffset = ConfirmationMessage::kOffsetToInheritedBytes();
    //----------------------------------------------------
    memcpy(
        &mIsContractorGateway,
        buffer.get() + bytesBufferOffset,
        sizeof(byte));
    bytesBufferOffset += sizeof(byte);

    string ethereumAddressBuffer(
            (char*)(buffer.get() + bytesBufferOffset),
            kEthereumAddressHexSize);
    mEthereumAddress = ethereumAddressBuffer;
    bytesBufferOffset += kEthereumAddressHexSize;

    string ethereumChannelIdBuffer(
            (char*)(buffer.get() + bytesBufferOffset),
            kEthereumChannelIdHexSize);
    mEthereumChannelId = ethereumChannelIdBuffer;
    bytesBufferOffset += kEthereumChannelIdHexSize;

    if (mEthereumAddress == "0x0000000000000000000000000000000000000000") {
        mEthereumAddress = "";
        mEthereumChannelId = "";
    }

    string bitcoinPublicKeyBuffer(
                (char*)(buffer.get() + bytesBufferOffset),
                kBitcoinPublicKeyHexSize);
    mBitcoinPublicKey = bitcoinPublicKeyBuffer;
    bytesBufferOffset += kBitcoinPublicKeyHexSize;

    string bitcoinTransactionIDBuffer(
                (char*)(buffer.get() + bytesBufferOffset),
                kBitcoinTransactionIDHexSize);
    mBitcoinTransactionID = bitcoinTransactionIDBuffer;

    if (mBitcoinPublicKey == "000000000000000000000000000000000000000000000000000000000000000000") {
        mBitcoinPublicKey = "";
        mBitcoinTransactionID = "";
    }
}

const Message::MessageType TrustLineConfirmationMessage::typeID() const
{
    return Message::TrustLines_Confirmation;
}

const bool TrustLineConfirmationMessage::isContractorGateway() const
{
    return mIsContractorGateway;
}

const string& TrustLineConfirmationMessage::ethereumAddress() const
{
    return mEthereumAddress;
}

const string& TrustLineConfirmationMessage::ethereumChannelId() const
{
    return mEthereumChannelId;
}

const string& TrustLineConfirmationMessage::bitcoinPublicKey() const
{
    return mBitcoinPublicKey;
}

const string& TrustLineConfirmationMessage::bitcoinTransactonID() const
{
    return mBitcoinTransactionID;
}

pair<BytesShared, size_t> TrustLineConfirmationMessage::serializeToBytes() const
    throw (bad_alloc)
{
    auto parentBytesAndCount = ConfirmationMessage::serializeToBytes();

    size_t bytesCount =
            parentBytesAndCount.second
            + sizeof(byte)
            + kEthereumAddressHexSize
            + kEthereumChannelIdHexSize
            + kBitcoinPublicKeyHexSize
            + kBitcoinTransactionIDHexSize;

    BytesShared dataBytesShared = tryMalloc(bytesCount);
    size_t dataBytesOffset = 0;
    //----------------------------------------------------
    memcpy(
        dataBytesShared.get(),
        parentBytesAndCount.first.get(),
        parentBytesAndCount.second);
    dataBytesOffset += parentBytesAndCount.second;
    //----------------------------------------------------
    memcpy(
        dataBytesShared.get() + dataBytesOffset,
        &mIsContractorGateway,
        sizeof(byte));
    dataBytesOffset += sizeof(byte);
    //----------------------------
    if (!mEthereumAddress.empty()) {
        memcpy(
            dataBytesShared.get() + dataBytesOffset,
            mEthereumAddress.c_str(),
            kEthereumAddressHexSize);
        dataBytesOffset += kEthereumAddressHexSize;
        memcpy(
            dataBytesShared.get() + dataBytesOffset,
            mEthereumChannelId.c_str(),
            kEthereumChannelIdHexSize);
        dataBytesOffset += kEthereumChannelIdHexSize;
    } else {
        string ethereumAddressBuffer = "0x0000000000000000000000000000000000000000";
        memcpy(
                dataBytesShared.get() + dataBytesOffset,
                ethereumAddressBuffer.c_str(),
                kEthereumAddressHexSize);
        dataBytesOffset += kEthereumAddressHexSize;
        string ethereumChannelIDBuffer = "0x0000000000000000000000000000000000000000000000000000000000000000";
        memcpy(
                dataBytesShared.get() + dataBytesOffset,
                ethereumChannelIDBuffer.c_str(),
                kEthereumChannelIdHexSize);
        dataBytesOffset += kEthereumChannelIdHexSize;
    }
    //----------------------------------------------------
    if (!mBitcoinPublicKey.empty()) {
        memcpy(
                dataBytesShared.get() + dataBytesOffset,
                mBitcoinPublicKey.c_str(),
                kBitcoinPublicKeyHexSize);
        dataBytesOffset += kBitcoinPublicKeyHexSize;
        memcpy(
                dataBytesShared.get() + dataBytesOffset,
                mBitcoinTransactionID.c_str(),
                kBitcoinTransactionIDHexSize);
    } else {
        string bitcoinPublicKeyBuffer = "000000000000000000000000000000000000000000000000000000000000000000";
        memcpy(
                dataBytesShared.get() + dataBytesOffset,
                bitcoinPublicKeyBuffer.c_str(),
                kBitcoinPublicKeyHexSize);
        dataBytesOffset += kBitcoinPublicKeyHexSize;
        string bitcoinTransactonIDBuffer = "0000000000000000000000000000000000000000000000000000000000000000";
        memcpy(
                dataBytesShared.get() + dataBytesOffset,
                bitcoinTransactonIDBuffer.c_str(),
                kBitcoinTransactionIDHexSize);
    }
    //----------------------------------------------------
    return make_pair(
        dataBytesShared,
        bytesCount);
}