#ifndef GEO_NETWORK_CLIENT_ETHEREUMAUDITMESSAGE_H
#define GEO_NETWORK_CLIENT_ETHEREUMAUDITMESSAGE_H

#include "../base/transaction/TransactionMessage.h"

#include "../../../ethereum/connector.h"

using namespace eth;

class EthereumAuditMessage : public TransactionMessage {

public:
    typedef shared_ptr<EthereumAuditMessage> Shared;

public:
    EthereumAuditMessage(
        const SerializedEquivalent equivalent,
        const NodeUUID &senderUUID,
        const TransactionUUID &transactionUUID,
        CooperativeClose::Shared audit,
        const string &bitcoinSgnature);

    EthereumAuditMessage(
        BytesShared buffer);

    CooperativeClose::Shared audit() const;

    const string bitcoinSignature() const;

    const MessageType typeID() const;

    pair<BytesShared, size_t> serializeToBytes() const
    throw (bad_alloc);

private:
    CooperativeClose::Shared mAudit;
    string mBitcoinSignature;
};


#endif //GEO_NETWORK_CLIENT_ETHEREUMAUDITMESSAGE_H
