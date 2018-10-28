#include "CreditUsageMultyCommand.h"

CreditUsageMultyCommand::CreditUsageMultyCommand(
        const CommandUUID &uuid,
        const string &commandBuffer) :

        BaseUserCommand(
                uuid,
                identifier())
{
    static const auto minCommandLength = CommandUUID::kHexSize + 2;
    if (commandBuffer.size() < minCommandLength) {
        throw ValueError(
                "CreditUsageCommand::parse: "
                        "can't parse command. "
                        "Received command is too short.");
    }

    unique_ptr<Logger> mLog;
    mLog = make_unique<Logger>(NodeUUID::empty());
    auto logger = mLog->debug("CreditUsageMultyCommand");
    try {
        string hexUUID = commandBuffer.substr(
                0,
                CommandUUID::kHexSize);
        mContractorUUID = boost::lexical_cast<uuids::uuid>(hexUUID);

    } catch (...) {
        throw ValueError(
                "CreditUsageCommand: can't parse command. "
                        "Error occurred while parsing 'Contractor UUID' token.");
    }
    logger << "contractorUUID " << mContractorUUID << "\n";

    size_t intermediateNodeStartPos = commandBuffer.find(
            kTokensSeparator,
            0) + 1;
    try {
        string hexIntermediateUUID = commandBuffer.substr(
                intermediateNodeStartPos,
                CommandUUID::kHexSize);
        mIntermediateUUID = boost::lexical_cast<uuids::uuid>(hexIntermediateUUID);

    } catch (...) {
        throw ValueError(
                "CreditUsageCommand: can't parse command. "
                        "Error occurred while parsing 'Contractor UUID' token.");
    }
    logger << "intermediateUUID " << mIntermediateUUID << "\n";

    size_t nextIntermediateNodeStartPos = NodeUUID::kHexSize+1+NodeUUID::kHexSize+1;
    try {
        string hexNextIntermediateUUID = commandBuffer.substr(
                nextIntermediateNodeStartPos,
                CommandUUID::kHexSize);
        mNextIntermediateUUID = boost::lexical_cast<uuids::uuid>(hexNextIntermediateUUID);

    } catch (...) {
        throw ValueError(
                "CreditUsageCommand: can't parse command. "
                        "Error occurred while parsing 'Contractor UUID' token.");
    }
    logger << "nextIntermediateUUID " << mIntermediateUUID << "\n";

    size_t amountStartPos = NodeUUID::kHexSize+1+NodeUUID::kHexSize+1+NodeUUID::kHexSize+1;
    size_t tokenSeparatorPos = commandBuffer.find(
            kTokensSeparator,
            amountStartPos);
    try {
        mAmount = TrustLineAmount(
                commandBuffer.substr(
                        amountStartPos,
                        tokenSeparatorPos - amountStartPos));

    } catch (...) {
        throw ValueError(
                "CreditUsageCommand: can't parse command. "
                        "Error occurred while parsing 'Amount' token.");
    }
    if (mAmount == TrustLineAmount(0)) {
        throw ValueError(
                "CreditUsageCommand: can't parse command. "
                        "Received 'Amount' can't be 0.");
    }
    logger << "amount " << mAmount << "\n";

    size_t equivalent1StartPoint = tokenSeparatorPos + 1;
    tokenSeparatorPos = commandBuffer.find(
            kTokensSeparator,
            equivalent1StartPoint);
    string equivalent1Str = commandBuffer.substr(
            equivalent1StartPoint,
            tokenSeparatorPos - equivalent1StartPoint);
    try {
        mEquivalent1 = (uint32_t)std::stoul(equivalent1Str);
    } catch (...) {
        throw ValueError(
                "CreditUsageCommand: can't parse command. "
                        "Error occurred while parsing  'equivalent1' token.");
    }
    logger << "equivalent1 " << mEquivalent1 << "\n";

    size_t equivalent2StartPoint = tokenSeparatorPos + 1;
    tokenSeparatorPos = commandBuffer.find(
            kTokensSeparator,
            equivalent2StartPoint);
    string equivalent2Str = commandBuffer.substr(
            equivalent2StartPoint,
            tokenSeparatorPos - equivalent2StartPoint);
    try {
        mEquivalent2 = (uint32_t)std::stoul(equivalent2Str);
    } catch (...) {
        throw ValueError(
                "CreditUsageCommand: can't parse command. "
                        "Error occurred while parsing  'equivalent2' token.");
    }
    logger << "equivalent2 " << mEquivalent2 << "\n";

    size_t equivalent3StartPoint = tokenSeparatorPos + 1;
    string equivalent3Str = commandBuffer.substr(
            equivalent3StartPoint,
            commandBuffer.size() - equivalent3StartPoint - 1);
    try {
        mEquivalent3 = (uint32_t)std::stoul(equivalent3Str);
    } catch (...) {
        throw ValueError(
                "CreditUsageCommand: can't parse command. "
                        "Error occurred while parsing  'equivalent3' token.");
    }
    logger << "equivalent3 " << mEquivalent3 << "\n";
}

const string& CreditUsageMultyCommand::identifier()
{
    static const string identifier = "CREATE:contractors/transactions-multy";
    return identifier;
}

const NodeUUID& CreditUsageMultyCommand::contractorUUID() const
{
    return mContractorUUID;
}

const NodeUUID& CreditUsageMultyCommand::intermediateUUID() const
{
    return mIntermediateUUID;
}

const NodeUUID& CreditUsageMultyCommand::nextIntermediateUUID() const
{
    return mNextIntermediateUUID;
}

const TrustLineAmount& CreditUsageMultyCommand::amount() const
{
    return mAmount;
}

const SerializedEquivalent CreditUsageMultyCommand::equivalent1() const
{
    return mEquivalent1;
}

const SerializedEquivalent CreditUsageMultyCommand::equivalent2() const
{
    return mEquivalent2;
}

const SerializedEquivalent CreditUsageMultyCommand::equivalent3() const
{
    return mEquivalent3;
}

CommandResult::SharedConst CreditUsageMultyCommand::responseNoConsensus () const
{
    return makeResult(409);
}

CommandResult::SharedConst CreditUsageMultyCommand::responseOK(
        string &transactionUUID) const
{
    return make_shared<const CommandResult>(
            identifier(),
            UUID(),
            201,
            transactionUUID);
}