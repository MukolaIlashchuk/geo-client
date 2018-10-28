#ifndef GEO_NETWORK_CLIENT_TRUSTLINECONFIRMATIONMESSAGE_H
#define GEO_NETWORK_CLIENT_TRUSTLINECONFIRMATIONMESSAGE_H

#include "../base/transaction/ConfirmationMessage.h"

class TrustLineConfirmationMessage : public ConfirmationMessage {

public:
    typedef shared_ptr<TrustLineConfirmationMessage> Shared;

public:
    TrustLineConfirmationMessage(
        const SerializedEquivalent equivalent,
        const NodeUUID &senderUUID,
        const TransactionUUID &transactionUUID,
        bool isContractorGateway,
        const OperationState state,
        const string &ethereumAddress = "",
        const string &ethereumChannelId = "",
        const string &bitcoinPublicKey = "",
        const string &bitcoinTransactonID = "");

    TrustLineConfirmationMessage(
        BytesShared buffer);

    const MessageType typeID() const;

    const bool isContractorGateway() const;

    const string& ethereumAddress() const;

    const string& ethereumChannelId() const;

    const string& bitcoinPublicKey() const;

    const string& bitcoinTransactonID() const;

    pair<BytesShared, size_t> serializeToBytes() const
        throw (bad_alloc);

private:
    bool mIsContractorGateway;
    string mEthereumAddress;
    string mEthereumChannelId;
    string mBitcoinPublicKey;
    string mBitcoinTransactionID;
};


#endif //GEO_NETWORK_CLIENT_TRUSTLINECONFIRMATIONMESSAGE_H
