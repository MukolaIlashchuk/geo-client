#ifndef GEO_NETWORK_CLIENT_ETHEREUMOUTGOINGRECEIPTMESSAGE_H
#define GEO_NETWORK_CLIENT_ETHEREUMOUTGOINGRECEIPTMESSAGE_H

#include "../SenderMessage.h"
#include "../../../ethereum/connector.h"

using namespace eth;

class EthereumOutgoingReceiptMessage : public SenderMessage {

public:
    typedef shared_ptr<EthereumOutgoingReceiptMessage> Shared;

public:
    EthereumOutgoingReceiptMessage(
        const SerializedEquivalent equivalent,
        const NodeUUID &senderUUID,
        Receipt::Shared receipt);

    EthereumOutgoingReceiptMessage(
        BytesShared buffer);

    Receipt::Shared receipt() const;

    const MessageType typeID() const;

    pair<BytesShared, size_t> serializeToBytes() const
        throw (bad_alloc);

private:
    Receipt::Shared mReceipt;
};


#endif //GEO_NETWORK_CLIENT_ETHEREUMOUTGOINGRECEIPTMESSAGE_H
