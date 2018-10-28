#ifndef GEO_NETWORK_CLIENT_ETHEREUMCLOSECHANNELCOMMAND_H
#define GEO_NETWORK_CLIENT_ETHEREUMCLOSECHANNELCOMMAND_H

#include "../BaseUserCommand.h"
#include "../../../../common/exceptions/ValueError.h"

class EthereumCloseChannelCommand : public BaseUserCommand {

public:
    typedef shared_ptr<EthereumCloseChannelCommand> Shared;

public:
    EthereumCloseChannelCommand(
        const CommandUUID &commandUUID,
        const string &commandBuffer);

    static const string &identifier()
        noexcept;

    const NodeUUID &contractorUUID() const
        noexcept;

    const SerializedEquivalent equivalent() const
        noexcept;

private:
    NodeUUID mContractorUUID;
    SerializedEquivalent mEquivalent;
};


#endif //GEO_NETWORK_CLIENT_ETHEREUMCLOSECHANNELCOMMAND_H
