#ifndef GEO_NETWORK_CLIENT_CONNECTOR_H
#define GEO_NETWORK_CLIENT_CONNECTOR_H


#include <boost/algorithm/string.hpp>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include "../common/memory/MemoryUtils.h"
#include "../common/multiprecision/MultiprecisionUtils.h"


#include "../common/Types.h"
#include "../../libs/json/json.h"

#include <fstream>
#include <string>

namespace eth {

    using namespace std;
    using namespace nlohmann;


    class Channel {
    public:
        typedef shared_ptr<Channel> Shared;

    public:
        Channel(
                const string &id,
                const string &contractorAddressHex):

                mID(id),
                mContractorAddressHex(contractorAddressHex)
        {};

    public:
        string mID;
        string mContractorAddressHex;
    };

    class Receipt {
    public:
        typedef shared_ptr<Receipt> Shared;

    public:
        Receipt(
                const uint64_t id,
                const uint64_t epochID,
                TrustLineAmount &amount,
                const string &receiverAddressHex):

                mID(id),
                mEpochID(epochID),
                mAmount(amount),
                mReceiverAddressHex(receiverAddressHex)
        {};

        Receipt(
                const uint64_t id,
                const uint64_t epochID,
                TrustLineAmount &amount,
                string &receiverAddressHex,
                string &senderSignatureHex):

                mID(id),
                mEpochID(epochID),
                mAmount(amount),
                mReceiverAddressHex(receiverAddressHex),
                mSenderSignatureHex(senderSignatureHex)
        {};

        Receipt(
            byte* buffer) {
            auto bytesBufferOffset = 0;
            memcpy(
                    &mID,
                    buffer + bytesBufferOffset,
                    sizeof(uint64_t));
            bytesBufferOffset += sizeof(uint64_t);

            memcpy(
                    &mEpochID,
                    buffer + bytesBufferOffset,
                    sizeof(uint64_t));
            bytesBufferOffset += sizeof(uint64_t);

            vector<byte> incomingAmountBytes(
                    buffer + bytesBufferOffset,
                    buffer + bytesBufferOffset + kTrustLineAmountBytesCount);
            mAmount = bytesToTrustLineAmount(incomingAmountBytes);
            bytesBufferOffset += kTrustLineAmountBytesCount;

            string receiverAddressHexBuffer(
                    (char*)(buffer + bytesBufferOffset),
                    kEthereumAddressHexSize);
            mReceiverAddressHex = receiverAddressHexBuffer;
            bytesBufferOffset += kEthereumAddressHexSize;

            string senderSignatureHexBuffer(
                    (char*)(buffer + bytesBufferOffset),
                    kEthereumSignatureHexSize);
            mSenderSignatureHex = senderSignatureHexBuffer;
        };

        BytesShared serializeToBytes() {
            BytesShared result = tryMalloc(serializedDataSize());

            size_t dataBytesOffset = 0;

            memcpy(
                    result.get() + dataBytesOffset,
                    &mID,
                    sizeof(uint64_t));
            dataBytesOffset += sizeof(uint64_t);

            memcpy(
                    result.get() + dataBytesOffset,
                    &mEpochID,
                    sizeof(uint64_t));
            dataBytesOffset += sizeof(uint64_t);

            vector<byte> amountBufferBytes = trustLineAmountToBytes(
                    mAmount);
            memcpy(
                    result.get() + dataBytesOffset,
                    amountBufferBytes.data(),
                    kTrustLineAmountBytesCount);
            dataBytesOffset += kTrustLineAmountBytesCount;

            memcpy(
                    result.get() + dataBytesOffset,
                    mReceiverAddressHex.c_str(),
                    kEthereumAddressHexSize);
            dataBytesOffset += kEthereumAddressHexSize;

            memcpy(
                    result.get() + dataBytesOffset,
                    mSenderSignatureHex.c_str(),
                    kEthereumSignatureHexSize);

            return result;
        };

        size_t serializedDataSize() {
            return sizeof(uint64_t)
                    + sizeof(uint64_t)
                    + kTrustLineAmountBytesCount
                    + kEthereumAddressHexSize
                    + kEthereumSignatureHexSize;
        };

    public:
        uint64_t mID;
        uint64_t mEpochID;
        TrustLineAmount mAmount;
        string mReceiverAddressHex;
        string mSenderSignatureHex;
    };

    class Audit {
    public:
        typedef shared_ptr<Audit> Shared;

    public:
        Audit(
                uint64_t id,
                const string &channelID,
                TrustLineAmount &aliceAmount,
                TrustLineAmount &bobAmount,
                const string &aliceSignature,
                const string &bobSignature) :

                mID(id),
                mChannelID(channelID),
                mAliceAmount(aliceAmount),
                mBobAmount(bobAmount),
                mAliceSignature(aliceSignature),
                mBobSignature(bobSignature)
        {};

        Audit(
                uint64_t id,
                const string &channelID,
                TrustLineAmount &aliceAmount,
                TrustLineAmount &bobAmount) :

                mID(id),
                mChannelID(channelID),
                mAliceAmount(aliceAmount),
                mBobAmount(bobAmount),
                mAliceSignature(""),
                mBobSignature("")
        {};

        Audit(byte* buffer) {
            auto bytesBufferOffset = 0;
            memcpy(
                    &mID,
                    buffer + bytesBufferOffset,
                    sizeof(uint64_t));
            bytesBufferOffset += sizeof(uint64_t);

            string channelIdBuffer(
                    (char*)(buffer + bytesBufferOffset),
                    kEthereumChannelIdHexSize);
            mChannelID = channelIdBuffer;
            bytesBufferOffset += kEthereumChannelIdHexSize;

            vector<byte> aliceAmountBytes(
                    buffer + bytesBufferOffset,
                    buffer + bytesBufferOffset + kTrustLineAmountBytesCount);
            mAliceAmount = bytesToTrustLineAmount(aliceAmountBytes);
            bytesBufferOffset += kTrustLineAmountBytesCount;

            vector<byte> bobAmountBytes(
                    buffer + bytesBufferOffset,
                    buffer + bytesBufferOffset + kTrustLineAmountBytesCount);
            mBobAmount = bytesToTrustLineAmount(bobAmountBytes);
            bytesBufferOffset += kTrustLineAmountBytesCount;

            string aliceSignatureHexBuffer(
                    (char*)(buffer + bytesBufferOffset),
                    kEthereumSignatureHexSize);
            mAliceSignature = aliceSignatureHexBuffer;
            bytesBufferOffset += kEthereumSignatureHexSize;

            bool bobDataPresence = false;
            memcpy(
                    &bobDataPresence,
                    buffer + bytesBufferOffset,
                    sizeof(byte));
            bytesBufferOffset += sizeof(byte);

            if (bobDataPresence) {
                string bobSignatureHexBuffer(
                        (char*)(buffer + bytesBufferOffset),
                        kEthereumSignatureHexSize);
                mBobSignature = bobSignatureHexBuffer;
            }
        };

        BytesShared serializeToBytes() {
            BytesShared result = tryMalloc(serializedDataSize());

            size_t dataBytesOffset = 0;

            memcpy(
                    result.get() + dataBytesOffset,
                    &mID,
                    sizeof(uint64_t));
            dataBytesOffset += sizeof(uint64_t);

            memcpy(
                    result.get() + dataBytesOffset,
                    mChannelID.c_str(),
                    kEthereumChannelIdHexSize);
            dataBytesOffset += kEthereumChannelIdHexSize;

            vector<byte> aliceAmountBufferBytes = trustLineAmountToBytes(
                    mAliceAmount);
            memcpy(
                    result.get() + dataBytesOffset,
                    aliceAmountBufferBytes.data(),
                    kTrustLineAmountBytesCount);
            dataBytesOffset += kTrustLineAmountBytesCount;

            vector<byte> bobAmountBufferBytes = trustLineAmountToBytes(
                    mBobAmount);
            memcpy(
                    result.get() + dataBytesOffset,
                    bobAmountBufferBytes.data(),
                    kTrustLineAmountBytesCount);
            dataBytesOffset += kTrustLineAmountBytesCount;

            memcpy(
                    result.get() + dataBytesOffset,
                    mAliceSignature.c_str(),
                    kEthereumSignatureHexSize);
            dataBytesOffset += kEthereumSignatureHexSize;

            bool bobDataPresence = (mBobSignature != "");
            memcpy(
                    result.get() + dataBytesOffset,
                    &bobDataPresence,
                    sizeof(byte));
            dataBytesOffset += sizeof(byte);

            if (bobDataPresence) {
                memcpy(
                        result.get() + dataBytesOffset,
                        mBobSignature.c_str(),
                        kEthereumSignatureHexSize);
            }

            return result;
        };

        size_t serializedDataSize() {
            size_t result =  sizeof(uint64_t)
                   + kEthereumChannelIdHexSize
                   + kTrustLineAmountBytesCount
                   + kTrustLineAmountBytesCount
                   + kEthereumSignatureHexSize
                   + sizeof(byte);

            if (mBobSignature != "") {
                result += kEthereumSignatureHexSize;
            }
            return result;
        };

        void addAliceSignature(
                const string &aliceSignature) {

            mAliceSignature = aliceSignature;
        }

        void addBobSignature(
            const string &bobSignature) {

            mBobSignature = bobSignature;
        }

    public:
        uint64_t mID;
        string mChannelID;
        TrustLineAmount mAliceAmount;
        TrustLineAmount mBobAmount;
        string mAliceSignature;
        string mBobSignature;
    };

    class CooperativeClose {
    public:
        typedef shared_ptr<CooperativeClose> Shared;

    public:
        CooperativeClose(
                const string &channelID,
                TrustLineAmount &aliceAmount,
                TrustLineAmount &bobAmount,
                const string &aliceSignature,
                const string &bobSignature) :

                mChannelID(channelID),
                mAliceAmount(aliceAmount),
                mBobAmount(bobAmount),
                mAliceSignature(aliceSignature),
                mBobSignature(bobSignature)
        {};

        CooperativeClose(
            const string &channelID,
            TrustLineAmount &aliceAmount,
            TrustLineAmount &bobAmount) :

            mChannelID(channelID),
            mAliceAmount(aliceAmount),
            mBobAmount(bobAmount),
            mAliceSignature(""),
            mBobSignature("")
        {};

        CooperativeClose(byte* buffer) {
            auto bytesBufferOffset = 0;

            string channelIdBuffer(
                    (char*)(buffer + bytesBufferOffset),
                    kEthereumChannelIdHexSize);
            mChannelID = channelIdBuffer;
            bytesBufferOffset += kEthereumChannelIdHexSize;

            vector<byte> aliceAmountBytes(
                    buffer + bytesBufferOffset,
                    buffer + bytesBufferOffset + kTrustLineAmountBytesCount);
            mAliceAmount = bytesToTrustLineAmount(aliceAmountBytes);
            bytesBufferOffset += kTrustLineAmountBytesCount;

            vector<byte> bobAmountBytes(
                    buffer + bytesBufferOffset,
                    buffer + bytesBufferOffset + kTrustLineAmountBytesCount);
            mBobAmount = bytesToTrustLineAmount(bobAmountBytes);
            bytesBufferOffset += kTrustLineAmountBytesCount;

            string aliceSignatureHexBuffer(
                    (char*)(buffer + bytesBufferOffset),
                    kEthereumSignatureHexSize);
            mAliceSignature = aliceSignatureHexBuffer;
            bytesBufferOffset += kEthereumSignatureHexSize;

            bool bobDataPresence = false;
            memcpy(
                    &bobDataPresence,
                    buffer + bytesBufferOffset,
                    sizeof(byte));
            bytesBufferOffset += sizeof(byte);

            if (bobDataPresence) {
                string bobSignatureHexBuffer(
                        (char*)(buffer + bytesBufferOffset),
                        kEthereumSignatureHexSize);
                mBobSignature = bobSignatureHexBuffer;
            }
        };

        BytesShared serializeToBytes() {
            BytesShared result = tryMalloc(serializedDataSize());

            size_t dataBytesOffset = 0;

            memcpy(
                    result.get() + dataBytesOffset,
                    mChannelID.c_str(),
                    kEthereumChannelIdHexSize);
            dataBytesOffset += kEthereumChannelIdHexSize;

            vector<byte> aliceAmountBufferBytes = trustLineAmountToBytes(
                    mAliceAmount);
            memcpy(
                    result.get() + dataBytesOffset,
                    aliceAmountBufferBytes.data(),
                    kTrustLineAmountBytesCount);
            dataBytesOffset += kTrustLineAmountBytesCount;

            vector<byte> bobAmountBufferBytes = trustLineAmountToBytes(
                    mBobAmount);
            memcpy(
                    result.get() + dataBytesOffset,
                    bobAmountBufferBytes.data(),
                    kTrustLineAmountBytesCount);
            dataBytesOffset += kTrustLineAmountBytesCount;

            memcpy(
                    result.get() + dataBytesOffset,
                    mAliceSignature.c_str(),
                    kEthereumSignatureHexSize);
            dataBytesOffset += kEthereumSignatureHexSize;

            bool bobDataPresence = (mBobSignature != "");
            memcpy(
                    result.get() + dataBytesOffset,
                    &bobDataPresence,
                    sizeof(byte));
            dataBytesOffset += sizeof(byte);

            if (bobDataPresence) {
                memcpy(
                        result.get() + dataBytesOffset,
                        mBobSignature.c_str(),
                        kEthereumSignatureHexSize);
            }

            return result;
        };

        size_t serializedDataSize() {
            size_t result =  kEthereumChannelIdHexSize
                             + kTrustLineAmountBytesCount
                             + kTrustLineAmountBytesCount
                             + kEthereumSignatureHexSize
                             + sizeof(byte);

            if (mBobSignature != "") {
                result += kEthereumSignatureHexSize;
            }
            return result;
        };

        void addAliceSignature(
                const string &aliceSignature) {

            mAliceSignature = aliceSignature;
        };

        void addBobSignature(
                const string &bobSignature) {

            mBobSignature = bobSignature;
        };

    public:
        string mChannelID;
        TrustLineAmount mAliceAmount;
        TrustLineAmount mBobAmount;
        string mAliceSignature;
        string mBobSignature;
    };

    struct ChannelInfo {
    public:
        typedef shared_ptr<ChannelInfo> Shared;

    public:
        string aliceAddress;
        string bobAddress;
        ChannelAmount aliceBalance;
        ChannelAmount bobBalance;
        uint64_t state;

        bool closingRequested;
        uint64_t epoch;
        uint64_t aliceNonce;
        uint64_t bobNonce;
    };

    namespace api {
        inline string __amountToStr(TrustLineAmount amount){
            std::stringstream amountStream;
            amountStream << amount;
            return amountStream.str();
        }

        inline string __uint64ToStr(uint64_t i){
            std::stringstream ss;
            ss << i;
            return ss.str();
        }

        inline pair<bool, string> __execOSCommand(const char *cmd) {
            cout << "CMD: " << cmd << endl;

            array<char, 128> buffer;
            string result;

            shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
            if (!pipe) {
                return make_pair(true, "");
            }

            while (!feof(pipe.get())) {
                if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
                    result += buffer.data();
            }

            return make_pair(true, result);
        }

        inline bool __stringReplace(std::string& str, const std::string& from, const std::string& to) {
            size_t start_pos = str.find(from);
            if(start_pos == std::string::npos)
                return false;

            str.replace(start_pos, from.length(), to);
            return true;
        }

        inline pair<string, bool> __processPostResponse(const pair<bool, string> &state) {
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", false);
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                return make_pair(j.at("transaction_id").get<string>(), true);
            }

            return make_pair("Unknown error", false);
        }

        // POST
        // todo: 'channel' -> 'channels'
        inline pair<string, string> createChannel(string contractorAddressHex, TrustLineAmount &amount) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/create/" --data '{"receiver_address": "{{ receiver_address }}", "amount": "{{ amount }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ receiver_address }}", contractorAddressHex);
            __stringReplace(request, "{{ amount }}", __amountToStr(amount));

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", "");
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", "");
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                auto txID = j.at("transaction_id").get<string>();
                auto channelID = j.at("channel_id").get<string>();
                return make_pair(txID, channelID);
            }

            return make_pair("Unknown error", "");
        }

        // POST
        // todo: 'channel' -> 'channels'
        inline pair<string, bool> tokenApproveErc20(TrustLineAmount &value) {
            std::ifstream ifs("middlewareErc.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/approve/"  --data '{"value": "{{ value }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ value }}", __amountToStr(value));

            return __processPostResponse(__execOSCommand(request.c_str()));
        }

        // POST
        // todo: 'channel' -> 'channels'
        inline pair<string, string> createChannelErc20(string contractorAddressHex, TrustLineAmount &amount) {
            std::ifstream ifs("middlewareErc.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/create/" --data '{"receiver_address": "{{ receiver_address }}", "amount": "{{ amount }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ receiver_address }}", contractorAddressHex);
            __stringReplace(request, "{{ amount }}", __amountToStr(amount));
//            std::stringstream stream;
//            stream << std::hex << amount;
//            std::string result( stream.str() );
//            __stringReplace(request, "{{ amount }}", result);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", "");
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", "");
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                auto txID = j.at("transaction_id").get<string>();
                auto channelID = j.at("channel_id").get<string>();
                return make_pair(txID, channelID);
            }

            return make_pair("Unknown error", "");
        }

        // POST
        // todo: 'channel' -> 'channels'
        inline pair<string, string> createChannelBitcoin(string contractorPublicKey, TrustLineAmount &amount) {
            std::ifstream ifs("bitcoin.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X GET "127.0.0.1:{{ port }}/channel/open?pubKeyBob={{ contractorPublicKey }}&value={{ amount }}")";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ contractorPublicKey }}", contractorPublicKey);
            __stringReplace(request, "{{ amount }}", __amountToStr(amount));

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", "");
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", "");
            }

            auto j = json::parse(response);
            auto txID = j.at("txid").get<string>();
            auto channelBalance = j.at("channel_balance").get<uint32_t>();
            stringstream ss;
            ss << channelBalance;
            return make_pair(txID, ss.str());
        }

        // POST
        // todo: 'channel' -> 'channels'
        inline pair<string, string> acceptChannel(string contractorAddressHex, TrustLineAmount &amount) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/create/reply" --data '{"receiver_address": "{{ receiver_address }}", "amount": "{{ amount }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ receiver_address }}", contractorAddressHex);
            __stringReplace(request, "{{ amount }}", __amountToStr(amount));

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", "");
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", "");
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                auto txID = j.at("transaction_id").get<string>();
                auto channelID = j.at("channel_id").get<string>();
                return make_pair(txID, channelID);
            }

            return make_pair("Unknown error", "");
        }

        // POST
        // todo: 'channel' -> 'channels'
        inline pair<string, string> acceptChannelErc20(string contractorAddressHex, TrustLineAmount &amount) {
            std::ifstream ifs("middlewareErc.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/create/reply" --data '{"receiver_address": "{{ receiver_address }}", "amount": "{{ amount }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ receiver_address }}", contractorAddressHex);
            __stringReplace(request, "{{ amount }}", __amountToStr(amount));

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", "");
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", "");
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                auto txID = j.at("transaction_id").get<string>();
                auto channelID = j.at("channel_id").get<string>();
                return make_pair(txID, channelID);
            }

            return make_pair("Unknown error", "");
        }

        // POST
        // todo: 'channel' -> 'channels'
        inline pair<string, bool> closeChannel(Channel::Shared channel) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/close/" --data '{"channel_id": "{{ channel_id }}" }')";
            __stringReplace(request, "{{ channel_id }}", channel->mID);
            __stringReplace(request, "{{ port }}", content);
            return __processPostResponse(__execOSCommand(request.c_str()));
        }

        // POST
        inline pair<string, bool> generateAuditSignature(Channel::Shared channel, Audit::Shared audit) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/close/audit/sign/" --data '{"epoch_id": "{{ epoch_id }}", "channel_id": "{{ channel_id }}", "alice_amount": "{{ alice_amount }}", "bob_amount": "{{ bob_amount }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ epoch_id }}", __uint64ToStr(audit->mID));
            __stringReplace(request, "{{ channel_id }}",  channel->mID);
            __stringReplace(request, "{{ alice_amount }}",  __amountToStr(audit->mAliceAmount));
            __stringReplace(request, "{{ bob_amount }}",  __amountToStr(audit->mBobAmount));


            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", false);
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                auto signature = j.at("sender_signature").get<string>();
                return make_pair(signature, true);
            }

            return make_pair("Unknown error", false);
        }

        // POST
        // Generates audit signature (without audit signatures fields themselfs) and compares it with "signature".
        inline pair<bool, bool> verifyAuditSignature(Channel::Shared channel, Audit::Shared audit, const string &signature) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/close/audit/verify/" --data '{"epoch_id": "{{ epoch_id }}", "channel_id": "{{ channel_id }}", "alice_amount": "{{ alice_amount }}", "bob_amount": "{{ bob_amount }}", "address_to_verify": "{{ address_to_verify }}", "signature_to_verify": "{{ signature_to_verify }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ epoch_id }}", __uint64ToStr(audit->mID));
            __stringReplace(request, "{{ channel_id }}",  channel->mID);
            __stringReplace(request, "{{ alice_amount }}",  __amountToStr(audit->mAliceAmount));
            __stringReplace(request, "{{ bob_amount }}",  __amountToStr(audit->mBobAmount));
            __stringReplace(request, "{{ address_to_verify }}", channel->mContractorAddressHex);
            __stringReplace(request, "{{ signature_to_verify }}", signature);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair(false, false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair(false, false);
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                auto ok = j.at("result").get<bool>();
                return make_pair(ok, true);
            }

            return make_pair("Unknown error", false);
        }

        // POST
        inline pair<string, bool> closeChannelThroughAudit(Channel::Shared channel, Audit::Shared audit) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/close/audit/" --data '{"epoch_id": "{{ epoch_id }}", "channel_id": "{{ channel_id }}", "alice_amount": "{{ alice_amount }}", "bob_amount": "{{ bob_amount }}", "alice_signature": "{{ alice_signature }}", "bob_signature": "{{ bob_signature }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ epoch_id }}", __uint64ToStr(audit->mID));
            __stringReplace(request, "{{ channel_id }}",  channel->mID);
            __stringReplace(request, "{{ alice_amount }}",  __amountToStr(audit->mAliceAmount));
            __stringReplace(request, "{{ bob_amount }}",  __amountToStr(audit->mBobAmount));
            __stringReplace(request, "{{ alice_signature }}",  audit->mAliceSignature);
            __stringReplace(request, "{{ bob_signature }}",  audit->mBobSignature);
            return __processPostResponse(__execOSCommand(request.c_str()));
        }

        // POST
        inline pair<Receipt::Shared, bool> signReceipt(Receipt::Shared receipt) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/receipts/" --data '{"receipt_id": "{{ receipt_id }}", "epoch_id": "{{ epoch_id }}", "amount": "{{ amount }}", "receiver_address": "{{ receiver_address }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ receipt_id }}", __uint64ToStr(receipt->mID));
            __stringReplace(request, "{{ epoch_id }}", __uint64ToStr(receipt->mEpochID));
            __stringReplace(request, "{{ amount }}", __amountToStr(receipt->mAmount));
            __stringReplace(request, "{{ receiver_address }}", receipt->mReceiverAddressHex);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair(nullptr, false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair(nullptr, false);
            }

            auto j = json::parse(response);
            auto message = j.at("status").get<string>();
            if (message != "OK") {
                return make_pair(nullptr, false);
            }

            auto signatureHex = j.at("sender_signature").get<string>();
            if (signatureHex.empty()) {
                return make_pair(nullptr, false);
            }

            receipt->mSenderSignatureHex = signatureHex;
            return make_pair(receipt, true);
        }

        // POST
        inline pair<bool, bool> verifyReceiptSignature(Receipt::Shared receipt) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/receipts/verify_signature" --data '{"receipt_id": "{{ receipt_id }}", "epoch_id": "{{ epoch_id }}", "amount": "{{ amount }}", "receiver_address": "{{ receiver_address }}", "signature_to_verify": "{{ signature_to_verify }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ receipt_id }}", __uint64ToStr(receipt->mID));
            __stringReplace(request, "{{ epoch_id }}", __uint64ToStr(receipt->mEpochID));
            __stringReplace(request, "{{ amount }}", __amountToStr(receipt->mAmount));
            __stringReplace(request, "{{ receiver_address }}", receipt->mReceiverAddressHex);
            __stringReplace(request, "{{ signature_to_verify }}", receipt->mSenderSignatureHex);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair(false, false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair(false, false);
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message != "OK") {
                return make_pair(false, false);
            }

            bool result = j.at("result");
            return make_pair(result, true);
        }

        // POST
        inline pair<string, bool> generateCooperativeCloseSignature(Channel::Shared channel, CooperativeClose::Shared contract) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/close/cooperative/sign/" --data '{"channel_id": "{{ channel_id }}", "alice_amount": "{{ alice_amount }}", "bob_amount": "{{ bob_amount }}" }')"; //, "alice_signature": "{{ alice_signature }}", "bob_signature": "{{ bob_signature }}"
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ channel_id }}",  channel->mID);
            __stringReplace(request, "{{ alice_amount }}",  __amountToStr(contract->mAliceAmount));
            __stringReplace(request, "{{ bob_amount }}",  __amountToStr(contract->mBobAmount));
//            __stringReplace(request, "{{ alice_signature }}",  contract->mAliceSignature);
//            __stringReplace(request, "{{ bob_signature }}", contract->mBobSignature);


            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", false);
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                auto signature = j.at("sender_signature").get<string>();
                return make_pair(signature, true);
            }

            return make_pair("Unknown error", false);
        }

        // POST
        inline pair<string, bool> generateCooperativeCloseSignatureErc20(Channel::Shared channel, CooperativeClose::Shared contract) {
            std::ifstream ifs("middlewareErc.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/close/cooperative/sign/" --data '{"channel_id": "{{ channel_id }}", "alice_amount": "{{ alice_amount }}", "bob_amount": "{{ bob_amount }}" }')"; //, "alice_signature": "{{ alice_signature }}", "bob_signature": "{{ bob_signature }}"
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ channel_id }}",  channel->mID);
            __stringReplace(request, "{{ alice_amount }}",  __amountToStr(contract->mAliceAmount));
            __stringReplace(request, "{{ bob_amount }}",  __amountToStr(contract->mBobAmount));
//            __stringReplace(request, "{{ alice_signature }}",  contract->mAliceSignature);
//            __stringReplace(request, "{{ bob_signature }}", contract->mBobSignature);


            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", false);
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                auto signature = j.at("sender_signature").get<string>();
                return make_pair(signature, true);
            }

            return make_pair("Unknown error", false);
        }

        // GET
        inline pair<string, bool> generateCooperativeCloseSignatureBtc(string aliceKey, string bobKey, TrustLineAmount channelBalance, TrustLineAmount aliceAmount, TrustLineAmount bobAmount) {
            std::ifstream ifs("bitcoin.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X GET "127.0.0.1:{{ port }}/channel/coop_close/create_tx?pubKey1={{ alice_key }}&pubKey2={{ bob_key }}&channel_balance={{ channel_balance }}&value1={{ alice_amount }}&value2={{ bob_amount }}")";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ alice_key }}",  aliceKey);
            __stringReplace(request, "{{ bob_key }}", bobKey);
            __stringReplace(request, "{{ channel_balance }}",  __amountToStr(channelBalance));
            __stringReplace(request, "{{ alice_amount }}",  __amountToStr(aliceAmount));
            __stringReplace(request, "{{ bob_amount }}",  __amountToStr(bobAmount));


            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", false);
            }

            return make_pair(response, true);
        }

        // POST
        // Generates audit signature (without audit signatures fields themselfs) and compares it with "signature".
        inline pair<bool, bool> verifyCooperativeCloseSignature(Channel::Shared channel, CooperativeClose::Shared contract, const string &signature) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type:application/json" "127.0.0.1:{{ port }}/api/channel/close/cooperative/verify/" --data '{"channel_id":"{{ channel_id }}","alice_amount":"{{ alice_amount }}","bob_amount":"{{ bob_amount }}","address_to_verify":"{{ address_to_verify }}","signature_to_verify":"{{ signature_to_verify }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ channel_id }}",  channel->mID);
            __stringReplace(request, "{{ alice_amount }}",  __amountToStr(contract->mAliceAmount));
            __stringReplace(request, "{{ bob_amount }}",  __amountToStr(contract->mBobAmount));
            __stringReplace(request, "{{ address_to_verify }}", channel->mContractorAddressHex);
            __stringReplace(request, "{{ signature_to_verify }}", signature);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair(false, false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair(false, false);
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                auto ok = j.at("result").get<bool>();
                return make_pair(ok, true);
            }

            return make_pair("Unknown error", false);
        }

        // POST
        // Generates audit signature (without audit signatures fields themselfs) and compares it with "signature".
        inline pair<bool, bool> verifyCooperativeCloseSignatureErc20(Channel::Shared channel, CooperativeClose::Shared contract, const string &signature) {
            std::ifstream ifs("middlewareErc.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type:application/json" "127.0.0.1:{{ port }}/api/channel/close/cooperative/verify/" --data '{"channel_id":"{{ channel_id }}","alice_amount":"{{ alice_amount }}","bob_amount":"{{ bob_amount }}","address_to_verify":"{{ address_to_verify }}","signature_to_verify":"{{ signature_to_verify }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ channel_id }}",  channel->mID);
            __stringReplace(request, "{{ alice_amount }}",  __amountToStr(contract->mAliceAmount));
            __stringReplace(request, "{{ bob_amount }}",  __amountToStr(contract->mBobAmount));
            __stringReplace(request, "{{ address_to_verify }}", channel->mContractorAddressHex);
            __stringReplace(request, "{{ signature_to_verify }}", signature);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair(false, false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair(false, false);
            }

            auto j = json::parse(response);
            auto message = j.at("status");
            if (message == "OK") { // todo: lowercase(message) == "ok"
                auto ok = j.at("result").get<bool>();
                return make_pair(ok, true);
            }

            return make_pair("Unknown error", false);
        }

        // POST
        inline pair<string, bool> closeChannelThroughCooperativeClose(Channel::Shared channel, CooperativeClose::Shared contract) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/close/cooperative" --data '{"channel_id": "{{ channel_id }}", "alice_amount": "{{ alice_amount }}", "bob_amount": "{{ bob_amount }}", "alice_signature": "{{ alice_signature }}", "bob_signature": "{{ bob_signature }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ channel_id }}",  channel->mID);
            __stringReplace(request, "{{ alice_amount }}",  __amountToStr(contract->mAliceAmount));
            __stringReplace(request, "{{ bob_amount }}",  __amountToStr(contract->mBobAmount));
            __stringReplace(request, "{{ alice_signature }}",  contract->mAliceSignature);
            __stringReplace(request, "{{ bob_signature }}",  contract->mBobSignature);
            return __processPostResponse(__execOSCommand(request.c_str()));
        }

        // POST
        inline pair<string, bool> closeChannelThroughCooperativeCloseErc20(Channel::Shared channel, CooperativeClose::Shared contract) {
            std::ifstream ifs("middlewareErc.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X POST -s -H "Content-Type: application/json" "127.0.0.1:{{ port }}/api/channel/close/cooperative" --data '{"channel_id": "{{ channel_id }}", "alice_amount": "{{ alice_amount }}", "bob_amount": "{{ bob_amount }}", "alice_signature": "{{ alice_signature }}", "bob_signature": "{{ bob_signature }}" }')";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ channel_id }}",  channel->mID);
            __stringReplace(request, "{{ alice_amount }}",  __amountToStr(contract->mAliceAmount));
            __stringReplace(request, "{{ bob_amount }}",  __amountToStr(contract->mBobAmount));
            __stringReplace(request, "{{ alice_signature }}",  contract->mAliceSignature);
            __stringReplace(request, "{{ bob_signature }}",  contract->mBobSignature);
            return __processPostResponse(__execOSCommand(request.c_str()));
        }

        // GET
        inline pair<string, bool> closeChannelThroughCooperativeCloseBtc(string bobPubKey, string alicePubKey, TrustLineAmount channelBalance, string aliceSignature) {
            std::ifstream ifs("bitcoin.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -X GET "127.0.0.1:{{ port }}/channel/coop_close/sign_tx?pubKey1={{ alice_key }}&pubKey2={{ bob_key }}&channel_balance={{ channel_balance }}&hexTx={{ alice_signature }}")";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ alice_key }}",  alicePubKey);
            __stringReplace(request, "{{ bob_key }}",  bobPubKey);
            __stringReplace(request, "{{ channel_balance }}",  __amountToStr(channelBalance));
            __stringReplace(request, "{{ alice_signature }}",  aliceSignature);
            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("Error requesting API.", false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("Error unexpected response.", false);
            }

            return make_pair(response, true);
        }

        // GET
        inline pair<ChannelInfo::Shared, bool> channelInfo(Channel::Shared channel) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -s "http://localhost:{{ port }}/api/channel/info/{{ channel_id }}/" )";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ channel_id }}",  channel->mID);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair(nullptr, false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair(nullptr, false);
            }

            auto j = json::parse(response);
            auto channelInfo = make_shared<ChannelInfo>();

            if (not j.at("channel_details").at("alice").is_null()) {
                channelInfo->aliceAddress = j.at("channel_details").at("alice").get<string>();
            }

            if (not j.at("channel_details").at("bob").is_null()) {
                channelInfo->bobAddress = j.at("channel_details").at("bob").get<string>();
            }

            channelInfo->aliceBalance = ChannelAmount(j.at("channel_details").at("balanceAlice").get<string>());
            channelInfo->bobBalance = ChannelAmount(j.at("channel_details").at("balanceBob").get<string>());
            channelInfo->state = j.at("channel_details").at("state").get<uint64_t>();
            channelInfo->closingRequested = j.at("channel_request_details").at("closingRequested").get<int>() > 0;
            channelInfo->epoch = j.at("channel_request_details").at("channelEpoch").get<uint64_t>();
            channelInfo->aliceNonce = j.at("channel_request_details").at("aliceNonce").get<uint64_t>();
            channelInfo->bobNonce = j.at("channel_request_details").at("bobNonce").get<uint64_t>();
            return make_pair(channelInfo, true);
        }

        // GET
        inline pair<ChannelInfo::Shared, bool> channelInfoErc20(Channel::Shared channel) {
            std::ifstream ifs("middlewareErc.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -s "http://localhost:{{ port }}/api/channel/info/{{ channel_id }}/" )";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ channel_id }}",  channel->mID);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair(nullptr, false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair(nullptr, false);
            }

            auto j = json::parse(response);
            auto channelInfo = make_shared<ChannelInfo>();

            if (not j.at("channel_details").at("alice").is_null()) {
                channelInfo->aliceAddress = j.at("channel_details").at("alice").get<string>();
            }

            if (not j.at("channel_details").at("bob").is_null()) {
                channelInfo->bobAddress = j.at("channel_details").at("bob").get<string>();
            }

            channelInfo->aliceBalance = ChannelAmount(j.at("channel_details").at("balanceAlice").get<string>());
            channelInfo->bobBalance = ChannelAmount(j.at("channel_details").at("balanceBob").get<string>());
            channelInfo->state = j.at("channel_details").at("state").get<uint64_t>();
            channelInfo->closingRequested = j.at("channel_request_details").at("closingRequested").get<int>() > 0;
            channelInfo->epoch = j.at("channel_request_details").at("channelEpoch").get<uint64_t>();
            channelInfo->aliceNonce = j.at("channel_request_details").at("aliceNonce").get<uint64_t>();
            channelInfo->bobNonce = j.at("channel_request_details").at("bobNonce").get<uint64_t>();
            return make_pair(channelInfo, true);
        }

        // GET
        inline pair<string, bool> transactionStatus(const string &transactionID) {
            std::ifstream ifs("middleware.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -s "http://localhost:{{ port }}/api/transaction/{{ transaction_id }}/" )";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ transaction_id }}",  transactionID);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("", false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("", false);
            }

            auto j = json::parse(response);
            return make_pair(j.at("status").get<string>(), true);
        }

        // GET
        inline pair<string, bool> transactionStatusErc20(const string &transactionID) {
            std::ifstream ifs("middlewareErc.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl -s "http://localhost:{{ port }}/api/transaction/{{ transaction_id }}/" )";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ transaction_id }}",  transactionID);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("", false);
            }

            auto response = get<1>(state);
            if (response.empty()) {
                return make_pair("", false);
            }

            auto j = json::parse(response);
            return make_pair(j.at("status").get<string>(), true);
        }

        // GET
        inline pair<string, bool> transactionStatusBitcoin(const string &transactionID) {
            std::ifstream ifs("bitcoin.port");
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            content.pop_back();
            string request = R"(curl "http://localhost:{{ port }}/confirmations/?txid={{ transaction_id }}" )";
            __stringReplace(request, "{{ port }}", content);
            __stringReplace(request, "{{ transaction_id }}",  transactionID);

            auto state = __execOSCommand(request.c_str());
            auto status = get<0>(state);
            if (! status) {
                return make_pair("", false);
            }

            auto response = get<1>(state);
            return make_pair(response, true);
        }
    }
}


#endif //GEO_NETWORK_CLIENT_CONNECTOR_H
