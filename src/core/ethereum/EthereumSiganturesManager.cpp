#include "EthereumSiganturesManager.h"

EthereumSiganturesManager::EthereumSiganturesManager(
    const SerializedEquivalent equivalent,
    Logger &logger) :

    mEquivalent(equivalent),
    mLogger(logger)
{}

void EthereumSiganturesManager::addPaymentReceipt(
    const NodeUUID &contractorUUID,
    Receipt::Shared paymentReceipt)
{
    if (mIncomingPaymentReceipts.count(contractorUUID) == 0) {
        info() << "initial payment receipt for contractor " << contractorUUID;
        vector<Receipt::Shared> initialPaymentReceipts;
        initialPaymentReceipts.push_back(
            paymentReceipt);
        mIncomingPaymentReceipts.insert(
            make_pair(
                contractorUUID,
                initialPaymentReceipts));
    } else {
        info() << "next payment receipt for contractor " << contractorUUID;
        mIncomingPaymentReceipts[contractorUUID].push_back(paymentReceipt);
    }
}

vector<Receipt::Shared> EthereumSiganturesManager::paymentReceipts(
    const NodeUUID &contractorUUID)
{
    if (mIncomingPaymentReceipts.count(contractorUUID) == 0) {
        info() << "emptyPaymentReceipts for contractor " << contractorUUID;
        vector<Receipt::Shared> emptyPaymentReceipts;
        return emptyPaymentReceipts;
    } else {
        info() << "count payment receipts for " << contractorUUID << " is "
               << mIncomingPaymentReceipts[contractorUUID].size();
        return mIncomingPaymentReceipts[contractorUUID];
    }
}

void EthereumSiganturesManager::setAudit(
    const NodeUUID &contractorUUID,
    CooperativeClose::Shared audit)
{
    mCurrentAudit[contractorUUID] = audit;
}

CooperativeClose::Shared EthereumSiganturesManager::getAudit(
    const NodeUUID &contractorUUID)
{
    return mCurrentAudit[contractorUUID];
}

const string EthereumSiganturesManager::logHeader() const
    noexcept
{
    stringstream s;
    s << "[EthereumSiganturesManager: " << mEquivalent << "] ";
    return s.str();
}

LoggerStream EthereumSiganturesManager::info() const
    noexcept
{
    return mLogger.info(logHeader());
}