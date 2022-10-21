#include "Include/Jsonifier.hpp"
#include <nlohmann/json.hpp>

struct UpdatePresenceData {
	String status{};
	Int64 since{ 0 };
	Bool afk{ false };
	String theString{};
	operator Jsonifier();
};

UpdatePresenceData::operator Jsonifier() {
	Jsonifier theData{};
	theData["status"] = this->status;
	theData["since"] = this->since;
	theData["afk"] = this->afk;
	return theData;
}

struct WebSocketIdentifyData {
	UpdatePresenceData presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	String theString{};
	int64_t intents{};

	operator Jsonifier();
};
enum class TestEnum { Test01 = 0, Test02 = 1 };
WebSocketIdentifyData::operator Jsonifier() {
	Jsonifier theSerializer{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	std::map<String, TestEnum> theMap{};
	theSerializer["d"]["large_threshold"] = theMap;

	UpdatePresenceData theData{};
	theSerializer["d"]["presence"]["activities"].emplaceBack(theData);
	theSerializer["d"]["presence"]["activities"].emplaceBack(theData);
	theSerializer["d"]["presence"]["activities"].emplaceBack(std::move(theData));
	theSerializer["d"]["afk"] = this->presence.afk;
	if (this->presence.since != 0) {
		theSerializer["since"] = this->presence.since;
	}
	theSerializer["d"]["status"] = this->presence.status;
	theSerializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
	theSerializer["d"]["properties"]["device"] = "DiscordCoreAPI";
	theSerializer["d"]["properties"]["os"] = "Windows";
	theSerializer["d"]["shard"].emplaceBack(0);
	theSerializer["d"]["shard"].emplaceBack(1);
	theSerializer["d"]["token"] = this->botToken;
	theSerializer["op"] = 2;
	theSerializer.refreshString(JsonifierSerializeType::Json);
	return theSerializer;
}

struct UpdatePresenceDataTwo {
	String status{};
	Int64 since{ 0 };
	Bool afk{ false };
	String theString{};
	operator nlohmann::json();
};

UpdatePresenceDataTwo::operator nlohmann::json() {
	nlohmann::json theDataTwo{};
	theDataTwo["status"] = this->status;
	theDataTwo["since"] = this->since;
	theDataTwo["afk"] = this->afk;
	return theDataTwo;
}

struct WebSocketIdentifyDataTwo {
	UpdatePresenceDataTwo presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	String theString{};
	int64_t intents{};

	operator nlohmann::json();
};
WebSocketIdentifyDataTwo::operator nlohmann::json() {
	nlohmann::json theSerializer{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	std::map<String, TestEnum> theMap{};
	theSerializer["d"]["large_threshold"] = theMap;

	UpdatePresenceDataTwo theDataTwo{};
	theSerializer["d"]["presence"]["activities"].emplace_back(theDataTwo);
	theSerializer["d"]["presence"]["activities"].emplace_back(theDataTwo);
	theSerializer["d"]["presence"]["activities"].emplace_back(std::move(theDataTwo));
	theSerializer["d"]["afk"] = this->presence.afk;
	if (this->presence.since != 0) {
		theSerializer["since"] = this->presence.since;
	}
	theSerializer["d"]["status"] = this->presence.status;
	theSerializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
	theSerializer["d"]["properties"]["device"] = "DiscordCoreAPI";
	theSerializer["d"]["properties"]["os"] = "Windows";
	theSerializer["d"]["shard"].emplace_back(0);
	theSerializer["d"]["shard"].emplace_back(1);
	theSerializer["d"]["token"] = this->botToken;
	theSerializer["op"] = 2;
	return theSerializer;
}

int32_t main() noexcept {
	try {
		StopWatch theStopWatch{ std::chrono::milliseconds{} };
		
		WebSocketIdentifyDataTwo theData02{};
		Vector<String> theVector{};
		Uint64 theTotalTime{};
		size_t theSize{};
		theStopWatch.resetTimer();
		for (Uint32 x = 0; x < 50; ++x) {
			nlohmann::json theSerializer = theData02.operator nlohmann::json();
			theStopWatch.resetTimer();
			for (Uint32 x = 0; x < 1024 * 128; ++x) {
				theSerializer["d"]["intents"] = x;
				theVector.push_back(theSerializer.dump());
				theSize += theVector.back().size();
				if (x % 100000 == 0) {
					//std::cout << theVector.back() << std::endl;
				}
			}
			theTotalTime += theStopWatch.totalTimePassed();
		}
		std::cout << "The time it took (In milliseconds): " << theTotalTime / 50 << ", with a total number of bytes serialized: " << theSize << std::endl;	

		std::this_thread::sleep_for(std::chrono::milliseconds{ 2000 });

		WebSocketIdentifyData theData01{};
		theVector.clear();
		theTotalTime = 0;
		theSize = 0;
		for (Uint32 x = 0; x < 50; ++x) {
			Jsonifier theSerializer = theData01.operator Jsonifier();
			theStopWatch.resetTimer();
			for (Uint32 x = 0; x < 1024 * 128; ++x) {
				theSerializer["d"]["intents"] = x;
				theSerializer.refreshString(JsonifierSerializeType::Json);
				theVector.push_back(theSerializer.operator String&&());
				theSize += theVector.back().size();
				if (x % 100000 == 0) {
					//std::cout << theVector.back() << std::endl;
				}
			}
			theTotalTime += theStopWatch.totalTimePassed();
		}
		std::cout << "The time it took (In milliseconds): " << theTotalTime / 50 << ", with a total number of bytes serialized: " << theSize << std::endl;	


	} catch (...) {
		
	};

	return 0;
}
