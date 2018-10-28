#ifndef GEO_NETWORK_CLIENT_CREDITUSAGEMULTYCOMMAND_H
#define GEO_NETWORK_CLIENT_CREDITUSAGEMULTYCOMMAND_H

#include "../BaseUserCommand.h"

#include "../../../../common/multiprecision/MultiprecisionUtils.h"

#include "../../../../common/exceptions/ValueError.h"
#include "../../../../common/exceptions/MemoryError.h"

#include "../../../../logger/Logger.h"

class CreditUsageMultyCommand : public BaseUserCommand {

public:
    typedef shared_ptr<CreditUsageMultyCommand> Shared;

public:
    CreditUsageMultyCommand(
        const CommandUUID &uuid,
        const string &commandBuffer);

    static const string &identifier();

    const TrustLineAmount& amount() const;

    const NodeUUID& contractorUUID() const;

    const NodeUUID& intermediateUUID() const;

    const NodeUUID& nextIntermediateUUID() const;

    const SerializedEquivalent equivalent1() const;

    const SerializedEquivalent equivalent2() const;

    const SerializedEquivalent equivalent3() const;

public:
    // Results handlers
    CommandResult::SharedConst responseNoConsensus() const;
    CommandResult::SharedConst responseOK(
            string &transactionUUID) const;

private:
    NodeUUID mContractorUUID;
    NodeUUID mIntermediateUUID;
    NodeUUID mNextIntermediateUUID;
    TrustLineAmount mAmount;
    SerializedEquivalent mEquivalent1;
    SerializedEquivalent mEquivalent2;
    SerializedEquivalent mEquivalent3;
};


#endif //GEO_NETWORK_CLIENT_CREDITUSAGEMULTYCOMMAND_H
