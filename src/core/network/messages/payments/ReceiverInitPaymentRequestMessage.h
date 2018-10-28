#ifndef GEO_NETWORK_CLIENT_RECEIVERINITPAYMENTREQUESTMESSAGE_H
#define GEO_NETWORK_CLIENT_RECEIVERINITPAYMENTREQUESTMESSAGE_H

#include "base/RequestMessage.h"


class ReceiverInitPaymentRequestMessage:
    public RequestMessage {

public:
    typedef shared_ptr<ReceiverInitPaymentRequestMessage> Shared;
    typedef shared_ptr<const ReceiverInitPaymentRequestMessage> ConstShared;

public:
    ReceiverInitPaymentRequestMessage(
            const SerializedEquivalent equivalent,
            const NodeUUID& senderUUID,
            const TransactionUUID& transactionUUID,
            const PathID &pathID,
            const TrustLineAmount &amount,
            const NodeUUID intermediateUUID = NodeUUID::empty(),
            SerializedEquivalent intermediateEquivalent = 0,
            const NodeUUID nextIntermediateUUID = NodeUUID::empty(),
            SerializedEquivalent nextIntermediateEquivalent = 0);

    ReceiverInitPaymentRequestMessage(
        BytesShared buffer);

    const NodeUUID &intermediateUUID() const;

    const SerializedEquivalent intermediateEquivalent() const;

    const NodeUUID &nextIntermediateUUID() const;

    const SerializedEquivalent nextIntermediateEquivalent() const;

private:
    const MessageType typeID() const;

    pair<BytesShared, size_t> serializeToBytes() const
    throw(bad_alloc);

private:
    NodeUUID mIntermediateUUID;
    SerializedEquivalent mIntermediateEquivalent;
    NodeUUID mNextIntermediateUUID;
    SerializedEquivalent mNextIntermediateEquivalent;
};
#endif //GEO_NETWORK_CLIENT_RECEIVERINITPAYMENTMESSAGE_H
