#include "Jsonifier/Jsonifier.hpp"
#include <nlohmann/json.hpp>
#include <scoped_allocator>
#include <source_location>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <simdjson.h>
#include <iostream>
#include <bitset>
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
/*
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

class Simd8 {
  public:
	Simd8(const void* ptr) {
		this->backslashes = _mm256_set1_epi8('\\');
		this->values = _mm256_loadu_si256(static_cast<const __m256i*>(ptr));
		this->B = _mm256_cmpeq_epi8(this->values, this->backslashes);
		auto negatives = _mm256_set1_epi8(-1);
		this->B = _mm256_sign_epi8(this->B, negatives);
		this->O = _mm256_set_epi8(0xff,0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff,
			0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00);
		this->E = _mm256_set_epi8(0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff,
			0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff);
		
		auto shiftValue = _mm256_set_epi8(30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -1);
		
		this->BShift = _mm256_shuffle_epi8(this->B, shiftValue);
		this->S = _mm256_andnot_si256(this->BShift, this->B);
		
		this->ES = _mm256_and_si256(this->E, this->S);
		this->EC = _mm256_add_epi8(this->B, this->ES);
		
	}
	operator std::string() {
		std::string string{};
		for (size_t x = 0; x < 32; ++x) {
			std::cout << "VALUE B: " << x << " " << +static_cast<uint8_t>(this->B.m256i_i8[x]) << std::endl;
			
		}
		for (size_t x = 0; x < 32; ++x) {
			std::cout << "VALUE B SHIFT: " << x << " " << +static_cast<uint8_t>(this->BShift.m256i_i8[x]) << std::endl;
		} 
		for (size_t x = 0; x < 32; ++x) {
			std::cout << "VALUE S: " << x << " " << +static_cast<uint8_t>(this->S.m256i_i8[x]) << std::endl;
		}
		for (size_t x = 0; x < 32; ++x) {
			std::cout << "VALUE ES: " << x << " " << +static_cast<uint8_t>(this->ES.m256i_i8[x]) << std::endl;
		} 
		/*
		for (size_t x = 0; x < 32; ++x) {
			std::cout << "VALUE E: " << x << " " << +static_cast<uint8_t>(this->E.m256i_i8[x]) << std::endl;
		}
		for (size_t x = 0; x < 32; ++x) {
			std::cout << "VALUE O: " << x << " " << +static_cast<uint8_t>(this->O.m256i_i8[x]) << std::endl;
		}
		
		
		
		
		for (size_t x = 0; x < 32; ++x) {
			std::cout << "VALUE EC: " << x << " " << +static_cast<uint8_t>(this->EC.m256i_i8[x]) << std::endl;
		}
		*/
		return string;
	}

  protected:
	__mmask32 valueMask{};
	__m256i backslashes{};
	__m256i values{};
	__m256i BShift{};
	__m256i B{};
	__m256i E{};
	__m256i O{};
	__m256i S{};
	__m256i ES{};
	__m256i EC{};
	__m256i OD1{};
	__m256i OS1{};
	__m256i OC{};
	__m256i OCE{};
	__m256i OD2{};
	__m256i OD{};

};


void copy256Data(void*dst, const void*src,size_t length) {
	if (length > 32) {
		_mm256_store_si256(static_cast<__m256i*>(dst), _mm256_load_si256(static_cast<const __m256i*>(src)));
	} else {
		for (size_t x = 0; x < length; ++x) {
			static_cast<uint8_t*>(dst)[x] = static_cast<const uint8_t*>(src)[x];
		}
	}
}

struct LengthData {
	uint64_t offSet{};
	uint64_t length{};
};

class StringBuffer {
  public:
	StringBuffer() noexcept;

	std::string_view operator[](LengthData);

	char operator[](uint64_t);

	void writeDataReal(const char*, uint64_t);

	void writeData(const char*, uint64_t);

	std::string::iterator begin();

	std::string::iterator end();

	void erase(uint64_t);

	uint64_t size();

	void clear();

	char* data();

  protected:
	std::string string01{};
	uint64_t sizeValue{};
};

StringBuffer::StringBuffer() noexcept {
	this->string01.resize(1024 * 16);
}

std::string_view StringBuffer::operator[](LengthData size) {
	std::string_view string{ this->string01.data() + size.offSet, size.length };
	return string;
}

char StringBuffer::operator[](uint64_t index) {
	return this->string01[index];
}

void StringBuffer::writeDataReal(const char* ptr, uint64_t size) {
	if (this->sizeValue + size > this->string01.size()) {
		this->string01.resize(this->string01.size() + size);
	}
	while (size > 0) {
		copy256Data(reinterpret_cast<void*>(this->string01.data() + size), reinterpret_cast<const void*>(ptr + size), static_cast<size_t>(size));
		if (size > 32) {
			size -= 32;
		} else {
			size = 0;
		}
	}
	this->sizeValue += size;
}

void StringBuffer::writeData(const char* ptr, uint64_t size) {
	if (this->sizeValue + size > this->string01.size()) {
		this->string01.resize(this->string01.size() + size);
	}
	std::copy(ptr, ptr + size, this->string01.data() + this->sizeValue);
	this->sizeValue += size;
}

std::string::iterator StringBuffer::begin() {
	return this->string01.begin();
}

std::string::iterator StringBuffer::end() {
	return this->string01.end();
}

void StringBuffer::erase(uint64_t amount) {
	std::copy(this->string01.data() + amount, this->string01.data() + this->sizeValue, this->string01.data());
	this->sizeValue = this->sizeValue - amount;
}

uint64_t StringBuffer::size() {
	return this->sizeValue;
}

void StringBuffer::clear() {
	this->sizeValue = 0;
}

char* StringBuffer::data() {
	return this->string01.data();
}

int32_t main() noexcept {
	try {

		Jsonifier::StopWatch<std::chrono::microseconds> stopWatch{ std::chrono::microseconds{ 1 } };
		std::vector<std::string> vector{};
		std::string string{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\"" };
		char values[24]{};
		Simd8 simd8Test{ string.data() };
		uint64_t totalTime{};
		std::cout << "THE STRING: " << simd8Test.operator std::string() << std::endl;
		std::cout << "THE STRING: " << string << std::endl;

		std::cout << "NOT 1: " << std::bitset<8>(~1) << std::endl;
		size_t size{};
		WebSocketIdentifyData data{};
		auto serializer = data.operator Jsonifier::Jsonifier();
		stopWatch.resetTimer();
		std::cout << "WERE HERE THIS SI IT!" << std::endl;
		StringBuffer stringBuffer{};
		for (uint32_t x = 0; x < 50; ++x) {
			stopWatch.resetTimer();

			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				stringBuffer.clear();
				stringBuffer.writeData(values, std::size(values));
			}
			totalTime += stopWatch.totalTimePassed();
		}
		int32_t x{ 0 };
		std::cout << std::string_view{ stringBuffer.data(), stringBuffer.size() } << std::endl;
		std::cout << "The time it took (In milliseconds, on average): " << totalTime / 50 << ", with a total number of bytes serialized: " << size << std::endl;
		
		vector.clear();
		totalTime = 0;
		size = 0;
		WebSocketIdentifyDataTwo dataOne{};
		nlohmann::json stringBufferTwo = dataOne;
		stopWatch.resetTimer();

		StringBuffer stringBuffer02{};
		for (uint32_t x = 0; x < 50; ++x) {
			stopWatch.resetTimer();
			for (uint32_t x = 0; x < 1024 * 128; ++x) {
				stringBuffer02.clear();
				stringBuffer02.writeDataReal(values, std::size(values));
			}
			totalTime += stopWatch.totalTimePassed();
		}
		std::cout << std::string_view{ stringBuffer02.data(), stringBuffer02.size() } << std::endl;
		std::cout << "The time it took (In milliseconds, on average): " << totalTime / 50 << ", with a total number of bytes serialized: " << size << std::endl;

		vector.clear();
		totalTime = 0;
		size = 0;
		WebSocketIdentifyDataThree dataTwo{};
		
		stopWatch.resetTimer();

		
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
			auto currentException = std::current_exception();
			if (currentException) {
				std::rethrow_exception(currentException);
			}
		}
	};

	return 0;
}