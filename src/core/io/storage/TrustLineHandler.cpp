#include "TrustLineHandler.h"

TrustLineHandler::TrustLineHandler(
    sqlite3 *dbConnection,
    const string &tableName,
    Logger &logger) :

    mDataBase(dbConnection),
    mTableName(tableName),
    mLog(logger)
{
    sqlite3_stmt *stmt;
    string query = "CREATE TABLE IF NOT EXISTS " + mTableName +
                   "(id INTEGER PRIMARY KEY, "
                   "state INTEGER NOT NULL, "
                   "contractor BLOB NOT NULL, "
                   "equivalent INTEGER NOT NULL, "
                   "is_contractor_gateway INTEGER NOT NULL DEFAULT 0, "

                   "contractor_ethereum_address TEXT DEFAULT '0000000000000000000000000000000000000000', "
                   "ethereum_channel_id TEXT, "
                   "contractor_bitcoin_pubkey TEXT DEFAULT '000000000000000000000000000000000000000000000000000000000000000000', "
                   "is_allice INTEGER NOT NULL DEFAULT 0, "
                   "current_epoch_id INTEGER DEFAULT 0, "
                   "current_payment_receipt_id INTEGER DEFAULT 0);";
    int rc = sqlite3_prepare_v2( mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::creating table: "
                          "Bad query; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
    } else {
        throw IOError("TrustLineHandler::creating table: "
                          "Run query; sqlite error: " + to_string(rc));
    }

    query = "CREATE UNIQUE INDEX IF NOT EXISTS " + mTableName
            + "_id_idx on " + mTableName + "(id);";
    rc = sqlite3_prepare_v2(mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::creating index for ID: "
                          "Bad query; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
    } else {
        throw IOError("TrustLineHandler::creating index for ID: "
                          "Run query; sqlite error: " + to_string(rc));
    }

    query = "CREATE INDEX IF NOT EXISTS " + mTableName
            + "_equivalent_idx on " + mTableName + "(equivalent);";
    rc = sqlite3_prepare_v2(mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::creating index for Equivalent: "
                          "Bad query; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
    } else {
        throw IOError("TrustLineHandler::creating index for Equivalent: "
                          "Run query; sqlite error: " + to_string(rc));
    }

    query = "CREATE UNIQUE INDEX IF NOT EXISTS " + mTableName
            + "_contractor_equivalent_idx on " + mTableName + "(contractor, equivalent);";
    rc = sqlite3_prepare_v2(mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::creating unique index for Contractor and Equivalent: "
                          "Bad query; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
    } else {
        throw IOError("TrustLineHandler::creating unique index for Contractor and Equivalent: "
                          "Run query; sqlite error: " + to_string(rc));
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
}

vector<TrustLine::Shared> TrustLineHandler::allTrustLinesByEquivalent(
    const SerializedEquivalent equivalent)
{
    string queryCount = "SELECT count(*) FROM " + mTableName + " WHERE equivalent = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(mDataBase, queryCount.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::allTrustLinesByEquivalent: "
                          "Bad count query; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 1, equivalent);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::allTrustLinesByEquivalent: "
                          "Bad binding of Equivalent; sqlite error: " + to_string(rc));
    }
    sqlite3_step(stmt);
    auto rowCount = (uint32_t)sqlite3_column_int(stmt, 0);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    vector<TrustLine::Shared> result;
    result.reserve(rowCount);

    string query = "SELECT id, state, contractor, is_contractor_gateway, "
                   "contractor_ethereum_address, ethereum_channel_id, is_allice, current_epoch_id, current_payment_receipt_id, contractor_bitcoin_pubkey FROM "
                   + mTableName + " WHERE equivalent = ?";
    rc = sqlite3_prepare_v2(mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::allTrustLinesByEquivalent: "
                          "Bad query; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 1, equivalent);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::allTrustLinesByEquivalent: "
                          "Bad binding of Equivalent; sqlite error: " + to_string(rc));
    }
    while (sqlite3_step(stmt) == SQLITE_ROW ) {
        auto id = (TrustLineID)sqlite3_column_int(stmt, 0);

        auto state = (TrustLine::TrustLineState)sqlite3_column_int(stmt, 1);

        NodeUUID contractor((uint8_t*)sqlite3_column_blob(stmt, 2));

        int32_t isContractorGateway = sqlite3_column_int(stmt, 3);

        try {

            auto trustLine = make_shared<TrustLine>(
                contractor,
                id,
                isContractorGateway != 0,
                state);

            string ethereumAddress(
                (char*)sqlite3_column_text(stmt, 4),
                kEthereumAddressHexSize - 2);

            if (ethereumAddress != "0000000000000000000000000000000000000000") {
                ethereumAddress = "0x" + ethereumAddress;
                trustLine->setCotntractorEthereumAddress(ethereumAddress);

                string ethereumChannelId(
                    (char*)sqlite3_column_text(stmt, 5),
                    kEthereumChannelIdHexSize - 2);
                ethereumChannelId = "0x" + ethereumChannelId;
                trustLine->setEthereumChannelId(ethereumChannelId);

                int32_t isAllice = sqlite3_column_int(stmt, 6);
                if (isAllice) {
                    trustLine->setAlice();
                }

                int32_t epochId = sqlite3_column_int(stmt, 7);
                //trustLine->setEpochID()

                int32_t paymentReceiptId = sqlite3_column_int(stmt, 8);
                //trustLine->setPaymentReceiptID()
            }

            string contractorBitcoinPubKey((char*)sqlite3_column_text(stmt, 9),
                                           kBitcoinPublicKeyHexSize);
            if (contractorBitcoinPubKey != "000000000000000000000000000000000000000000000000000000000000000000") {
                trustLine->setContractorBtcoinPublicKey(
                    contractorBitcoinPubKey);
            }

            result.push_back(
                trustLine);
        } catch (...) {
            throw Exception("TrustLinesManager::allTrustLinesByEquivalent. "
                                "Unable to create trust line instance from DB.");
        }
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return result;
}

void TrustLineHandler::deleteTrustLine(
    const NodeUUID &contractorUUID,
    const SerializedEquivalent equivalent)
{
    string query = "DELETE FROM " + mTableName + " WHERE contractor = ? AND equivalent = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2( mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::deleteTrustLine: "
                          "Bad query; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_blob(stmt, 1, contractorUUID.data, NodeUUID::kBytesSize, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::deleteTrustLine: "
                          "Bad binding of Contractor; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 2, equivalent);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::deleteTrustLine: "
                          "Bad binding of Equivalent; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_DONE) {
#ifdef STORAGE_HANDLER_DEBUG_LOG
        info() << "deleting is completed successfully";
#endif
    } else {
        throw IOError("TrustLineHandler::deleteTrustLine: "
                          "Run query; sqlite error: " + to_string(rc));
    }
}

void TrustLineHandler::saveTrustLine(
    TrustLine::Shared trustLine,
    const SerializedEquivalent equivalent)
{
    string query = "INSERT INTO " + mTableName +
                   "(id, state, contractor, equivalent, is_contractor_gateway) "
                   "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2( mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::saveTrustLine: "
                              "Bad query; sqlite error: " + to_string(rc));
    }

    rc = sqlite3_bind_int(stmt, 1, trustLine->trustLineID());
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::saveTrustLine: "
                          "Bad binding of ID; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 2, (int)trustLine->state());
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::saveTrustLine: "
                          "Bad binding of State; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_blob(stmt, 3, trustLine->contractorNodeUUID().data, NodeUUID::kBytesSize, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::saveTrustLine: "
                          "Bad binding of Contractor; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 4, equivalent);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::saveTrustLine: "
                          "Bad binding of Equivalent; sqlite error: " + to_string(rc));
    }
    int32_t isContractorGateway = trustLine->isContractorGateway();
    rc = sqlite3_bind_int(stmt, 5, isContractorGateway);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::saveTrustLine: "
                          "Bad binding of IsContractorGateway; sqlite error: " + to_string(rc));
    }

    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_DONE) {
#ifdef STORAGE_HANDLER_DEBUG_LOG
        info() << "prepare inserting is completed successfully";
#endif
    } else {
        throw IOError("TrustLineHandler::saveTrustLine: "
                          "Run query; sqlite error: " + to_string(rc));
    }
}

void TrustLineHandler::updateTrustLine(
    TrustLine::Shared trustLine,
    const SerializedEquivalent equivalent)
{
    string query = "UPDATE " + mTableName +
                   " SET state = ?, is_contractor_gateway = ? "
                   "WHERE id = ? AND equivalent = ? AND contractor = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2( mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::update: "
                          "Bad query; sqlite error: " + to_string(rc));
    }

    rc = sqlite3_bind_int(stmt, 1, (int)trustLine->state());
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::update: "
                          "Bad binding of State; sqlite error: " + to_string(rc));
    }
    int32_t isContractorGateway = trustLine->isContractorGateway();
    rc = sqlite3_bind_int(stmt, 2, isContractorGateway);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::update: "
                          "Bad binding of IsContractorGateway; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 3, trustLine->trustLineID());
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::update: "
                          "Bad binding of ID; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 4, equivalent);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::update: "
                          "Bad binding of Equivalent; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_blob(stmt, 5, trustLine->contractorNodeUUID().data, NodeUUID::kBytesSize, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::update: "
                          "Bad binding of Contractor; sqlite error: " + to_string(rc));
    }

    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_DONE) {
#ifdef STORAGE_HANDLER_DEBUG_LOG
        info() << "prepare updating is completed successfully";
#endif
    } else {
        throw IOError("TrustLineHandler::update: "
                          "Run query; sqlite error: " + to_string(rc));
    }

    if (sqlite3_changes(mDataBase) == 0) {
        throw ValueError("No data were modified");
    }
}

void TrustLineHandler::updateEthereumTrustLine(
    TrustLine::Shared trustLine,
    const SerializedEquivalent equivalent)
{
    string query = "UPDATE " + mTableName +
                   " SET contractor_ethereum_address = ?, ethereum_channel_id = ?, is_allice = ?, "
                           "current_epoch_id = ?, current_payment_receipt_id = ? "
                           "WHERE id = ? AND equivalent = ? AND contractor = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2( mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Bad query; sqlite error: " + to_string(rc));
    }

    auto ethereumAddress = trustLine->contractorEthereumAddress();
    ethereumAddress.erase(0, 2);
    rc = sqlite3_bind_text(stmt, 1, ethereumAddress.c_str(), kEthereumAddressHexSize - 2, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Bad binding of Ethereum address; sqlite error: " + to_string(rc));
    }
    auto ethereumChannelId = trustLine->ethereumChannelId();
    ethereumChannelId.erase(0, 2);
    rc = sqlite3_bind_text(stmt, 2, ethereumChannelId.c_str(), kEthereumChannelIdHexSize - 2, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Bad binding of Ethereum channel id; sqlite error: " + to_string(rc));
    }
    int32_t isAllice = trustLine->isAlice();
    rc = sqlite3_bind_int(stmt, 3, isAllice);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Bad binding of IsAllice; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 4, (uint32_t)trustLine->currentEpochId());
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Bad binding of epoch ID; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 5, (uint32_t)trustLine->currentPaymentReceiptId());
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Bad binding of payment receipt ID; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 6, trustLine->trustLineID());
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Bad binding of ID; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 7, equivalent);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Bad binding of Equivalent; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_blob(stmt, 8, trustLine->contractorNodeUUID().data, NodeUUID::kBytesSize, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Bad binding of Contractor; sqlite error: " + to_string(rc));
    }

    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_DONE) {
#ifdef STORAGE_HANDLER_DEBUG_LOG
        info() << "prepare updating is completed successfully";
#endif
    } else {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Run query; sqlite error: " + to_string(rc));
    }

    if (sqlite3_changes(mDataBase) == 0) {
        throw ValueError("No data were modified");
    }
}

void TrustLineHandler::updateBitcoinTrustLine(
    TrustLine::Shared trustLine,
    const SerializedEquivalent equivalent)
{
    string query = "UPDATE " + mTableName +
                   " SET contractor_bitcoin_pubkey = ? WHERE id = ? AND equivalent = ? AND contractor = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2( mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateBitcoinTrustLine: "
                              "Bad query; sqlite error: " + to_string(rc));
    }

    rc = sqlite3_bind_text(stmt, 1, trustLine->contractorBitcoinPubKey().c_str(), kBitcoinPublicKeyHexSize - 2, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateBitcoinTrustLine: "
                              "Bad binding of Bitcoin pubKey; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 2, trustLine->trustLineID());
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateBitcoinTrustLine: "
                              "Bad binding of ID; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_int(stmt, 3, equivalent);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateBitcoinTrustLine: "
                              "Bad binding of Equivalent; sqlite error: " + to_string(rc));
    }
    rc = sqlite3_bind_blob(stmt, 4, trustLine->contractorNodeUUID().data, NodeUUID::kBytesSize, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::updateBitcoinTrustLine: "
                              "Bad binding of Contractor; sqlite error: " + to_string(rc));
    }

    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_DONE) {
#ifdef STORAGE_HANDLER_DEBUG_LOG
        info() << "prepare updating is completed successfully";
#endif
    } else {
        throw IOError("TrustLineHandler::updateEthereumTrustLine: "
                              "Run query; sqlite error: " + to_string(rc));
    }

    if (sqlite3_changes(mDataBase) == 0) {
        throw ValueError("No data were modified");
    }
}

vector<SerializedEquivalent> TrustLineHandler::equivalents()
{
    string query = "SELECT DISTINCT equivalent FROM " + mTableName;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2( mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::equivalents: "
                          "Bad query; sqlite error: " + to_string(rc));
    }

    vector<SerializedEquivalent> result;
    while (sqlite3_step(stmt) == SQLITE_ROW ) {
        auto equivalent = (SerializedEquivalent)sqlite3_column_int(stmt, 0);
        result.push_back(equivalent);
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return result;
}

vector<TrustLineID> TrustLineHandler::allIDs()
{
    string queryCount = "SELECT count(*) FROM " + mTableName;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(mDataBase, queryCount.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::allIDs: "
                          "Bad count query; sqlite error: " + to_string(rc));
    }
    sqlite3_step(stmt);
    auto rowCount = (uint32_t)sqlite3_column_int(stmt, 0);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    vector<TrustLineID> result;
    result.reserve(rowCount);

    string query = "SELECT id FROM " + mTableName;
    rc = sqlite3_prepare_v2(mDataBase, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw IOError("TrustLineHandler::allIDs: "
                          "Bad query; sqlite error: " + to_string(rc));
    }
    while (sqlite3_step(stmt) == SQLITE_ROW ) {
        result.push_back(
            (TrustLineID)sqlite3_column_int(stmt, 0));
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return result;
}

LoggerStream TrustLineHandler::info() const
{
    return mLog.info(logHeader());
}

LoggerStream TrustLineHandler::warning() const
{
    return mLog.warning(logHeader());
}

const string TrustLineHandler::logHeader() const
{
    stringstream s;
    s << "[TrustLineHandler]";
    return s.str();
}
