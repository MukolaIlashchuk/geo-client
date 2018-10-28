#include "EthereumAuditCommand.h"

EthereumAuditCommand::EthereumAuditCommand(
    const CommandUUID &commandUUID,
    const string &command) :

    BaseUserCommand(
        commandUUID,
        identifier())
{
    static const auto minCommandLength = NodeUUID::kHexSize + 1;

    if (command.size() < minCommandLength) {
        throw ValueError(
            "EthereumAuditCommand: can't parse command. "
                "Received command is to short.");
    }

    try {
        string hexUUID = command.substr(0, NodeUUID::kHexSize);
        mContractorUUID = boost::lexical_cast<uuids::uuid>(hexUUID);

    } catch (...) {
        throw ValueError(
            "EthereumAuditCommand: can't parse command. "
                "Error occurred while parsing 'Contractor UUID' token.");
    }

    size_t equivalentOffset = NodeUUID::kHexSize + 1;
    string equivalentStr = command.substr(
            equivalentOffset,
            command.size() - equivalentOffset - 1);
    try {
        mEquivalent = (uint32_t)std::stoul(equivalentStr);
    } catch (...) {
        throw ValueError(
            "EthereumAuditCommand: can't parse command. "
                "Error occurred while parsing  'equivalent' token.");
    }
}

const string &EthereumAuditCommand::identifier()
    noexcept
{
    static const string identifier = "ETHEREUM:contractors/audit";
    return identifier;
}

const NodeUUID &EthereumAuditCommand::contractorUUID() const
    noexcept
{
    return mContractorUUID;
}

const SerializedEquivalent EthereumAuditCommand::equivalent() const
    noexcept
{
    return mEquivalent;
}