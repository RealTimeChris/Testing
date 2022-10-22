#include "Jsonifier.hpp"
#include <nlohmann/json.hpp>
#include <scoped_allocator>
#include <source_location>
#include <rapidjson/rapidjson.h>
#include <xmmintrin.h>
struct UpdatePresenceDataTwo {
	std::string status{};///< Current status.
	int64_t since{ 0 };///< When was the activity started?
	bool afk{ false };///< Are we afk.
	std::string theString{};
	operator nlohmann::json();
};

UpdatePresenceDataTwo ::operator nlohmann::json() {
	nlohmann::json theData{};
	theData["status"] = this->status;
	theData["since"] = this->since;
	theData["afk"] = this->afk;
	return theData;
}

struct WebSocketIdentifyDataTwo {
	UpdatePresenceDataTwo presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	std::string theString{};
	int64_t intents{};

	operator nlohmann::json();
};

WebSocketIdentifyDataTwo::operator nlohmann::json(){
	nlohmann::json theSerializer{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);

	UpdatePresenceDataTwo theData{};
	theSerializer["d"]["presence"]["activities"].emplace_back(theData);
	theSerializer["d"]["presence"]["activities"].emplace_back(theData);
	theSerializer["d"]["presence"]["activities"].emplace_back(std::move(theData));
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

struct UpdatePresenceData {
	std::string status{};
	int64_t since{ 0 };
	bool afk{ false };
	std::string theString{};
	operator Jsonifier::Jsonifier();
};

UpdatePresenceData::operator Jsonifier::Jsonifier() {
	Jsonifier::Jsonifier theData{};
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
	std::string theString{};
	int64_t intents{};

	operator Jsonifier::Jsonifier();
};

WebSocketIdentifyData::operator Jsonifier::Jsonifier() {
	Jsonifier::Jsonifier theSerializer{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);

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
	theSerializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
	return theSerializer;
}

int32_t main() noexcept {
	try {

		WebSocketIdentifyDataTwo theDataBewTwoReal{};

		Jsonifier::StopWatch theStopWatch{ std::chrono::milliseconds{} };
		std::vector<std::string> theVector{};
		uint64_t theTotalTime{};
		size_t theSize{};
		WebSocketIdentifyData theDataBewTwo{};

		for (uint32_t x = 0; x < 50; ++x) {
			Jsonifier::Jsonifier theSerializer{ theDataBewTwo.operator Jsonifier::Jsonifier() };
			theStopWatch.resetTimer();
			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				theSerializer["d"]["intents"] = x;
				theSerializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
				theVector.push_back(theSerializer.operator std::string());
				theSize += theVector.back().size();
				if (x == 1024 * 128 - 2) {
					//std::cout << theVector.back() << std::endl;
				}
			}
			theTotalTime += theStopWatch.totalTimePassed();
		}
		std::cout << "The time it took (In milliseconds): " << theTotalTime / 50 << ", with a total number of bytes serialized: " << theSize << std::endl;
		
		theVector.clear();
		theTotalTime = 0;
		theStopWatch.resetTimer();
		theSize = 0;
		for (uint32_t x = 0; x < 50; ++x) {
			auto theReferenceTwo = theDataBewTwoReal.operator nlohmann::json();
			theStopWatch.resetTimer();
			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				theReferenceTwo["d"]["intents"] = x;
				theVector.push_back(theReferenceTwo.dump());
				theSize += theVector.back().size();
				if (x == 1024 * 128 - 2) {
					//std::cout << theVector.back() << std::endl;
				}
			}
			theTotalTime += theStopWatch.totalTimePassed();
		}
		std::cout << "The time it took (In milliseconds): " << theTotalTime / 50 << ", with a total number of bytes serialized: " << theSize << std::endl;
		
			
				
		std::this_thread::sleep_for(std::chrono::milliseconds{ 2000 });

	} catch (...) {
		try {
			auto currentException = std::current_exception();
			if (currentException) {
				std::rethrow_exception(currentException);
			}
		} catch (const std::exception& e) {
			std::stringstream theStream{};
			std::source_location theLocation{};
			theStream << "Error Report: \n"
					  << "Caught At: " << theLocation.file_name() << " (" << std::to_string(theLocation.line()) << ":" << std::to_string(theLocation.column()) << ")"
					  << "\nThe Error: \n"
					  << e.what() << std::endl
					  << std::endl;
			auto theReturnString = theStream.str();
			std::cout << theReturnString;
		}
	};

	return 0;
}

