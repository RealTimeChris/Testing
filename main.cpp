#include <include/jsonifier/Jsonifier.hpp>
#include <nlohmann/json.hpp>
#include <scoped_allocator>
#include <source_location>
#include <rapidjson/rapidjson.h>
#include <immintrin.h>

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
	nlohmann::json serializer{};
	serializer["d"]["intents"] = this->intents;

	UpdatePresenceDataTwo theData{};
	serializer["d"]["presence"]["activities"].emplace_back(theData);
	serializer["d"]["presence"]["activities"].emplace_back(theData);
	serializer["d"]["presence"]["activities"].emplace_back(theData);
	serializer["d"]["presence"]["activities"].emplace_back(theData);
	serializer["d"]["afk"] = this->presence.afk;
	if (this->presence.since != 0) {
		serializer["since"] = this->presence.since;
	}
	serializer["d"]["status"] = this->presence.status;
	serializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
	serializer["d"]["properties"]["device"] = "DiscordCoreAPI";
	serializer["d"]["properties"]["os"] = "Windows";
	serializer["d"]["shard"].emplace_back(0);
	serializer["d"]["shard"].emplace_back(1);
	serializer["d"]["token"] = this->botToken;
	serializer["op"] = 2;
	return serializer;
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
	Jsonifier::Jsonifier serializer{};
	serializer["d"]["intents"] = this->intents;

	UpdatePresenceData theData{};
	serializer["d"]["presence"]["activities"].emplaceBack(theData);
	serializer["d"]["presence"]["activities"].emplaceBack(theData);
	serializer["d"]["presence"]["activities"].emplaceBack(theData);
	serializer["d"]["presence"]["activities"].emplaceBack(theData);
	serializer["d"]["afk"] = this->presence.afk;
	if (this->presence.since != 0) {
		serializer["since"] = this->presence.since;
	}
	serializer["d"]["status"] = this->presence.status;
	serializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
	serializer["d"]["properties"]["device"] = "DiscordCoreAPI";
	serializer["d"]["properties"]["os"] = "Windows";
	serializer["d"]["shard"].emplaceBack(0);
	serializer["d"]["shard"].emplaceBack(1);
	serializer["d"]["token"] = this->botToken;
	serializer["op"] = 2;
	return serializer;
}

int32_t main() noexcept {
	try {

		Jsonifier::StopWatch<std::chrono::milliseconds> stopWatch{ std::chrono::milliseconds{ 1 } };
		std::vector<std::string> vector{};
		uint64_t totalTime{};
		size_t size{};
		WebSocketIdentifyData data{};
		auto serializer = data.operator Jsonifier::Jsonifier();
		stopWatch.resetTimer();
		for (uint32_t x = 0; x < 50; ++x) {
			stopWatch.resetTimer();
			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				serializer["d"]["intents"] = x;
				serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
				vector.push_back(serializer.operator std::string());
				size += vector.back().size();
			}
			totalTime += stopWatch.totalTimePassed();
		}
		std::cout << "The time it took (In milliseconds, on average): " << totalTime / 50 << ", with a total number of bytes serialized: " << size << std::endl;
		
		vector.clear();
		totalTime = 0;
		size = 0;
		WebSocketIdentifyDataTwo dataTwo{};
		auto serializerTwo = dataTwo.operator nlohmann::json();
		stopWatch.resetTimer();
		for (uint32_t x = 0; x < 50; ++x) {			
			stopWatch.resetTimer();
			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				serializerTwo["d"]["intents"] = x;
				vector.push_back(serializerTwo.dump());
				size += vector.back().size();
			}
			totalTime += stopWatch.totalTimePassed();
		}
		std::cout << "The time it took (In milliseconds, on average): " << totalTime / 50 << ", with a total number of bytes serialized: " << size << std::endl;
				
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

