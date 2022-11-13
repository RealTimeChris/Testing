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
template<typename OTy> class AtomicWrapper {
  public:

	AtomicWrapper& operator=(const AtomicWrapper& other) {
		this->object.store(other.object.load());
		return *this;
	}

	AtomicWrapper(const AtomicWrapper& other) {
		*this = other;
	}

	AtomicWrapper& operator=(OTy objectNew) noexcept {
		this->object.store(objectNew);
		return *this;
	};

	AtomicWrapper(OTy objectNew) noexcept {
		*this = objectNew;
	};

	operator OTy() {
		return this->object.load();
	}

  protected:
	std::atomic<OTy> object{};

};

class TSString {
  public:
	TSString& operator=(const TSString& other) {
		this->string = other.string;
		return *this;
	}

	TSString(const TSString& other) {
		*this = other;
	}

	TSString& operator=(const std::string& newString) {
		std::unique_lock lock{ this->accessMutex };
		this->string.clear();
		for (auto& value: newString) {
			this->string.emplace_back(value);
		}
		return *this;
	}

	TSString(const std::string& newString) {
		*this = newString;
	}

	const size_t size() {
		std::unique_lock lock{ this->accessMutex };
		size_t currentSize{ this->string.size() };
		return currentSize;
	}

	operator std::string() {
		std::unique_lock lock{ this->accessMutex };
		std::string returnValue{};
		for (auto& value: this->string) {
			returnValue.push_back(value.operator char());
		}
		return returnValue;
	}

  protected:
	std::vector<AtomicWrapper<char>> string{};
	std::mutex accessMutex{};
};

int32_t main() noexcept {
	try {
		TSString currentFile{ "testing" };
		;
		{ std::source_location location = std::source_location::current();
			currentFile = location.file_name();
		}
		std::cout << "CURRENT FILE: " << std::string{ currentFile } << std::endl;
		Jsonifier::StopWatch<std::chrono::milliseconds> stopWatchNew{ std::chrono::milliseconds{ 1000 } };
		Jsonifier::StopWatch<std::chrono::milliseconds> stopWatch{ std::chrono::milliseconds{ 1000 } };
		std::vector<TSString> vector{};
		std::chrono::duration<unsigned long long, std::milli> totalTime{};
		size_t size{};
		WebSocketIdentifyDataTwo dataOne{};
		nlohmann::json stringBufferTwo = dataOne;
		stopWatchNew.resetTimer();
		
		for (uint32_t x = 0; x < 50; ++x) {
			stopWatchNew.resetTimer();
			for (uint32_t x = 0; x < 1024 *128; ++x) {
				if (stopWatch.hasTimePassed()) {
					stopWatch.resetTimer();
					std::cout << "IT'S PASSED!" << std::endl;
				}
				stringBufferTwo["d"]["intents"] = x;
				vector.push_back(stringBufferTwo.dump());
				size += vector.back().size();
			}
			totalTime += stopWatchNew.totalTimePassed();
		}
		std::cout << std::string{ vector.back() } << std::endl;
		std::cout << "The time it took (In milliseconds, on average): " << totalTime / 50 << ", with a total number of bytes serialized: " << size << std::endl;

		vector.clear();
		totalTime = totalTime.zero();
		size = 0;
		WebSocketIdentifyDataThree dataTwo{};
		
		stopWatchNew.resetTimer();

		WebSocketIdentifyData data{};
		auto serializer = data.operator Jsonifier::Jsonifier();
		stopWatchNew.resetTimer();


		for (uint32_t x = 0; x < 50; ++x) {
			stopWatchNew.resetTimer();

			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				serializer["d"]["intents"] = x;
				serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
				vector.push_back(serializer.operator std::string());
				size += vector.back().size();
			}
			totalTime += stopWatchNew.totalTimePassed();
		}
		int32_t x{ 0 };
		std::cout << std::string{ vector.back() } << std::endl;
		std::cout << "The time it took (In milliseconds, on average): " << totalTime / 50 << ", with a total number of bytes serialized: " << size << std::endl;

		vector.clear();
		totalTime = totalTime.zero();
		size = 0;
		

		
		for (uint32_t x = 0; x < 50; ++x) {			
			stopWatchNew.resetTimer();
			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				dataTwo.intents = x;
				rapidjson::StringBuffer stringBuffer = dataTwo;
				vector.push_back(std::string{ stringBuffer.GetString() });
				size += vector.back().size();
				
			}
			totalTime += stopWatchNew.totalTimePassed();
		}
		std::cout << std::string{ vector.back() } << std::endl;
		std::cout << "The time it took (In milliseconds, on average): " << totalTime / 50 << ", with a total number of bytes serialized: " << size << std::endl;

	} catch (std::exception&e) {
		std::cout << e.what() << std::endl;
	};

	return 0;
}