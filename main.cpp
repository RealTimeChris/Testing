#include <scoped_allocator>
#include "out/build/Windows-Release/_deps/jsonifier-src/Include/Jsonifier.hpp"

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



int32_t main() noexcept {
	try {
		StopWatch theStopWatch{ std::chrono::milliseconds{} };
		WebSocketIdentifyData theDataBewTwo{};
		Vector<String> theVector{};
		Uint64 theTotalTime{};
		size_t theSize{};
		for (Uint32 x = 0; x < 50; ++x) {			
			Jsonifier theSerializer{ theDataBewTwo.operator Jsonifier() };	
			theStopWatch.resetTimer();
			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				theSerializer["d"]["intents"] = x;
				theSerializer.refreshString(JsonifierSerializeType::Json);
				theVector.push_back(theSerializer.operator String());
				theSize += theVector.back().size();
				if (x == 1024 * 128 - 2) {
					std::cout << theVector.back() << std::endl;
				}
			}
			theTotalTime += theStopWatch.totalTimePassed();
			
		}
		std::cout << "The time it took (In milliseconds): " << theTotalTime / 50 << ", with a total number of bytes serialized: " << theSize << std::endl;	
		
		std::this_thread::sleep_for(std::chrono::milliseconds{ 2000 });


	} catch (...) {
		
	};

	return 0;
}
