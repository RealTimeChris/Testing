#include "jsonifier/Jsonifier.hpp"
#include <nlohmann/json.hpp>
#include <scoped_allocator>
#include <source_location>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <simdjson.h>
#include <iostream>

struct UpdatePresenceDataTwo {
	std::string status{};
	int64_t since{ 0 };
	bool afk{ false };
};

struct WebSocketIdentifyDataThree {
	UpdatePresenceDataTwo presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	std::string theString{};
	int64_t intents{};
	operator rapidjson::StringBuffer();
};

WebSocketIdentifyDataThree::operator rapidjson::StringBuffer() {
	rapidjson::StringBuffer stringBuffer{};
	rapidjson::Writer<rapidjson::StringBuffer> serializer{ stringBuffer };
	serializer.StartObject();
	serializer.Key("op");
	serializer.Int64(2);
	serializer.Key("d");
	serializer.StartObject();
	serializer.Key("large_threshold");
	serializer.Int64(this->largeThreshold);
	serializer.Key("intents");
	serializer.Int64(this->intents);
	serializer.Key("properties");
	serializer.StartObject();
	serializer.String("browser");
	serializer.String("DiscordCoreAPI");
	serializer.String("device");
	serializer.String("DiscordCoreAPI");
	serializer.String("os");
	serializer.String("Windows");
	serializer.EndObject();
	serializer.String("shard");
	serializer.StartArray();
	serializer.Int(0);
	serializer.Int(1);
	serializer.EndArray();
	serializer.Key("token");
	serializer.String(this->botToken.c_str());
	serializer.Key("presence");
	serializer.StartObject();
	serializer.Key("afk");
	serializer.Bool(this->presence.afk);
	serializer.Key("status");
	serializer.String(this->presence.status.c_str());
	serializer.Key("since");
	serializer.Int64(this->presence.since);
	serializer.EndObject();
	serializer.EndObject();
	serializer.EndObject();
	return stringBuffer;
}

struct WebSocketIdentifyDataTwo {
	UpdatePresenceDataTwo presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	std::string theString{};
	int64_t intents{};
};
	
void from_json(const nlohmann::json& j, WebSocketIdentifyDataTwo data) {
	int8_t integer{ 2 };
	j["op"].get_to(integer);
	j["d"]["token"].get_to(data.botToken);
	j["d"]["intents"].get_to(data.intents);
	j["d"]["shard"][0].get_to(data.currentShard);
	j["d"]["shard"][1].get_to(data.numberOfShards);
	j["d"]["large_threshold"].get_to(data.largeThreshold);
	j["d"]["presence"]["afk"].get_to(data.presence.afk);
	j["d"]["presence"]["since"].get_to(data.presence.since);
	j["d"]["presence"]["status"].get_to(data.presence.status);
}

void to_json(nlohmann::json& j, const WebSocketIdentifyDataTwo& p) {
	int8_t integer{ 2 };
	j["op"] = integer;
	j["d"]["token"] = p.botToken;
	j["d"]["intents"] = p.intents;
	j["d"]["shard"][0]=p.currentShard;
	j["d"]["shard"][1] = 1;
	j["d"]["large_threshold"] = p.largeThreshold;
	j["d"]["presence"]["afk"] = p.presence.afk;
	j["d"]["presence"]["since"] = p.presence.since;
	j["d"]["presence"]["status"] = p.presence.status;
	j["d"]["properties"]["browser"] = "DiscordCoreAPI";
	j["d"]["properties"]["device"] = "DiscordCoreAPI";
	j["d"]["properties"]["os"] = "Windows";
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
	serializer["d"]["large_threshold"] = this->largeThreshold;
	serializer["d"]["presence"]["afk"] = false;
	serializer["d"]["presence"]["since"] = this->presence.since;
	serializer["d"]["presence"]["status"] = this->presence.status;
	serializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
	serializer["d"]["properties"]["device"] = "DiscordCoreAPI";
	serializer["d"]["properties"]["os"] = "Windows";
	serializer["d"]["shard"].emplaceBack(0);
	serializer["d"]["shard"].emplaceBack(1);
	serializer["d"]["token"] = this->botToken;
	serializer["op"] = 2;
	return serializer;
}

struct PackedValues {
	PackedValues(char* string, size_t length) {
		for (size_t x = 0; x < length; ++x) {
			this->values.push_back(string + this->currentSize);
			this->currentSize++;
		}
	}

	char getNextValue() {
		if (this->currentIndex >= this->currentSize) {
			return '0';
		}
		auto returnValue = *this->values[this->currentIndex];
		this->currentIndex++;
		return returnValue;
	}

	void resetIndex() {
		this->currentIndex = 0;
	}

	uint8_t getNextIndex() {
		auto currentIndexNew = this->currentIndex + 2;
		this->currentIndex++;
		return currentIndexNew;
	}

	void setNextValue(uint8_t value) {
		if (this->currentIndex >= this->currentSize) {
			return;
		}
		*this->values[this->currentIndex] = value;
		this->currentIndex++;
	}

  protected:
	std::vector<char*> values{};
	size_t currentIndex{};
	size_t currentSize{};
};

void storeBits(PackedValues values, bool reverse) {
	__m256i value{ _mm256_set_epi8(values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(),
		values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(),
		values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(),
		values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(),
		values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue(), values.getNextValue()) };
	values.resetIndex();
	__m256i indexes{ _mm256_set_epi8(values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(),
		values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(),
		values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(),
		values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(),
		values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex(), values.getNextIndex()) };
	values.resetIndex();
	__m256i result{ _mm256_shuffle_epi8(value, indexes) };
	for (size_t x = 0; x < 32; ++x) {
		values.setNextValue(result.m256i_i8[x]);
	}
}
/*
int32_t main() noexcept {
	std::string testString{ "TEST STRING" };
	PackedValues values{ testString.data(), testString.size() };
	storeBits(values, false);
	std::cout << testString << std::endl;

}
*/

template<typename TimeType> class StopWatch {
  public:
	StopWatch() = delete;

	using SysClock = std::chrono::system_clock;

	StopWatch<TimeType>& operator=(const StopWatch<TimeType>& data) {
		std::unique_lock lock{ this->accessMutex };
		this->maxNumberOfMs = data.maxNumberOfMs;
		this->startTime = data.startTime;
		return *this;
	}

	StopWatch(const StopWatch<TimeType>& data) {
		*this = data;
	}

	StopWatch<TimeType>& operator=(uint64_t maxNumberOfMsNew) {
		std::unique_lock lock{ this->accessMutex };
		this->maxNumberOfMs = TimeType{ maxNumberOfMsNew };
		this->startTime = std::chrono::duration_cast<TimeType>(SysClock::now().time_since_epoch());
		return *this;
	}

	StopWatch(uint64_t maxNumberOfMsNew) {
		*this = maxNumberOfMsNew;
	}

	StopWatch<TimeType>& operator=(TimeType maxNumberOfMsNew) {
		std::unique_lock lock{ this->accessMutex };
		this->maxNumberOfMs = std::chrono::duration_cast<TimeType>(maxNumberOfMsNew);
		this->startTime = std::chrono::duration_cast<TimeType>(SysClock::now().time_since_epoch());
		return *this;
	}
	StopWatch(TimeType maxNumberOfMsNew) {

		*this = maxNumberOfMsNew;
	}

	auto totalTimePassed() {
		std::unique_lock lock{ this->accessMutex };
		auto currentTime = std::chrono::duration_cast<TimeType>(SysClock::now().time_since_epoch());
		auto elapsedTime = currentTime - this->startTime;
		return elapsedTime;
	}

	auto getTotalWaitTime() {
		std::unique_lock lock{ this->accessMutex };
		return this->maxNumberOfMs;
	}

	bool hasTimePassed() {
		std::unique_lock lock{ this->accessMutex };
		auto currentTime = std::chrono::duration_cast<TimeType>(SysClock::now().time_since_epoch());
		auto elapsedTime = currentTime - this->startTime;
		if (elapsedTime.count() >= this->maxNumberOfMs.count()) {
			return true;
		} else {
			return false;
		}
	}

	void resetTimer() {
		std::unique_lock lock{ this->accessMutex };
		this->startTime = std::chrono::duration_cast<TimeType>(SysClock::now().time_since_epoch());
	}

  protected:
	TimeType maxNumberOfMs{};
	std::mutex accessMutex{};
	TimeType startTime{};
};

class DCAException : public std::exception, std::string {
  public:
	DCAException(const std::string, std::source_location = std::source_location::current()) noexcept;
};

DCAException::DCAException(std::string error, std::source_location location) noexcept  {
	std::stringstream stream{};
	stream << "Error Report: \n"
		   << "Caught in File: " << location.file_name() << " (" << std::to_string(location.line()) << ":" << std::to_string(location.column()) << ")"
		   << "\nThe Error: \n"
		   << error << std::endl
		   << std::endl;
	*static_cast<std::exception*>(this) = std::exception{ stream.str().c_str() };
}

int32_t main() noexcept {
	try {

		throw DCAException{ "THE ERROR IS" };
		StopWatch<std::chrono::nanoseconds> stopWatchNew{ std::chrono::microseconds{ 10000000 } };
		StopWatch<std::chrono::duration<double, std::micro>> stopWatch{ 1000000 };
		std::vector<std::string> vector{};
		std::chrono::duration<double, std::nano> totalTime{};
		size_t size{};
		WebSocketIdentifyDataTwo dataOne{};
		nlohmann::json stringBufferTwo = dataOne;
		stopWatch.resetTimer();
		
		for (uint32_t x = 0; x < 50; ++x) {
			stopWatch.resetTimer();
			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				if (stopWatchNew.hasTimePassed()) {
					stopWatchNew.resetTimer();
					std::cout << "IT'S PASSED!" << std::endl;
				}
				stringBufferTwo["d"]["intents"] = x;
				vector.push_back(stringBufferTwo.dump());
				size += vector.back().size();
			}
			totalTime += stopWatch.totalTimePassed();
		}
		std::cout << vector.back() << std::endl;
		std::cout << "The time it took (In milliseconds, on average): " << totalTime / 50 << ", with a total number of bytes serialized: " << size << std::endl;

		vector.clear();
		totalTime = totalTime.zero();
		size = 0;
		WebSocketIdentifyDataThree dataTwo{};
		
		stopWatch.resetTimer();

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
		int32_t x{ 0 };
		std::cout << vector.back() << std::endl;
		std::cout << "The time it took (In milliseconds, on average): " << totalTime / 50 << ", with a total number of bytes serialized: " << size << std::endl;

		vector.clear();
		totalTime = totalTime.zero();
		size = 0;
		

		
		for (uint32_t x = 0; x < 50; ++x) {			
			stopWatch.resetTimer();
			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				dataTwo.intents = x;
				rapidjson::StringBuffer stringBuffer = dataTwo;
				vector.push_back(stringBuffer.GetString());
				size += vector.back().size();
				
			}
			totalTime += stopWatch.totalTimePassed();
		}
		std::cout << vector.back() << std::endl;
		std::cout << "The time it took (In milliseconds, on average): " << totalTime / 50 << ", with a total number of bytes serialized: " << size << std::endl;

	} catch (std::exception&e) {
		std::cout << e.what() << std::endl;
	};

	return 0;
}