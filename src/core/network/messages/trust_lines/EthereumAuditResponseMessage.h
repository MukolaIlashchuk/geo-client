#ifndef GEO_NETWORK_CLIENT_ETHEREUMAUDITRESPONSEMESSAGE_H
#define GEO_NETWORK_CLIENT_ETHEREUMAUDITRESPONSEMESSAGE_H

#include "../base/transaction/TransactionMessage.h"

#include "../../../ethereum/connector.h"

using namespace eth;

class EthereumAuditResponseMessage : public TransactionMessage {

public:
    typedef shared_ptr<EthereumAuditResponseMessage> Shared;

public:
    enum OperationState {
        OK = 1,
        Reject = 2,
    };

public:
    EthereumAuditResponseMessage(
        const SerializedEquivalent equivalent,
        const NodeUUID &senderUUID,
        const TransactionUUID &transactionUUID,
        const OperationState state,
        CooperativeClose::Shared audit,
        const string &bitcointTransactionID);

    EthereumAuditResponseMessage(
        BytesShared buffer);

    CooperativeClose::Shared audit() const;

    const string bitcoinTransactionID() const;

    const MessageType typeID() const;

    const OperationState state() const;

    pair<BytesShared, size_t> serializeToBytes() const
    throw (bad_alloc);

private:
    typedef byte SerializedOperationState;

private:
    CooperativeClose::Shared mAudit;
    OperationState mState;
    string mBitcointTransactonID;
};


#endif //GEO_NETWORK_CLIENT_ETHEREUMAUDITRESPONSEMESSAGE_H
