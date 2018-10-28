#ifndef GEO_NETWORK_CLIENT_ETHEREUMAUDITCOMMAND_H
#define GEO_NETWORK_CLIENT_ETHEREUMAUDITCOMMAND_H

#include "../BaseUserCommand.h"
#include "../../../../common/exceptions/ValueError.h"

class EthereumAuditCommand : public BaseUserCommand {

public:
    typedef shared_ptr<EthereumAuditCommand> Shared;

public:
    EthereumAuditCommand(
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


#endif //GEO_NETWORK_CLIENT_ETHEREUMAUDITCOMMAND_H
