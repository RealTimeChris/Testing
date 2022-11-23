#include <jsonifier/Jsonifier.hpp>
#include <source_location>
#include <immintrin.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <bitset>
#include <atomic>
#include <deque>

struct DCAException : public std::runtime_error, std::string {
	DCAException(const std::string&, std::source_location = std::source_location::current()) noexcept;
};

DCAException::DCAException(const std::string& error, std::source_location location) noexcept : std::runtime_error(error) {
	std::stringstream stream{};
	stream << "Error Report: \n"
		   << "Caught in File: " << location.file_name() << " (" << std::to_string(location.line()) << ":" << std::to_string(location.column()) << ")"
		   << "\nThe Error: \n"
		   << error << std::endl
		   << std::endl;
	*static_cast<std::runtime_error*>(this) = std::runtime_error{ stream.str() };
}

template<typename TTy> class StopWatch {
  public:
	using HRClock = std::chrono::high_resolution_clock;

	inline StopWatch() = delete;

	inline StopWatch<TTy>& operator=(const StopWatch<TTy>& data) {
		this->maxNumberOfMs.store(data.maxNumberOfMs.load());
		this->startTime.store(data.startTime.load());
		return *this;
	}

	inline StopWatch(const StopWatch<TTy>& data) {
		*this = data;
	}

	inline StopWatch(TTy maxNumberOfMsNew) {
		this->maxNumberOfMs.store(maxNumberOfMsNew);
		this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
	}

	inline TTy totalTimePassed() {
		TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
		TTy elapsedTime = currentTime - this->startTime.load();
		return elapsedTime;
	}

	inline TTy getTotalWaitTime() {
		return this->maxNumberOfMs.load();
	}

	inline bool hasTimePassed() {
		TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
		TTy elapsedTime = currentTime - this->startTime.load();
		if (elapsedTime >= this->maxNumberOfMs.load()) {
			return true;
		} else {
			return false;
		}
	}

	inline void resetTimer() {
		this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
	}

  protected:
	std::atomic<TTy> maxNumberOfMs{ TTy{ 0 } };
	std::atomic<TTy> startTime{ TTy{ 0 } };
};

inline uint64_t convertSimd256To64BitUint(const __m256i inputA, const __m256i inputB) {
	uint64_t r_lo = _mm256_movemask_epi8(inputA);
	uint64_t r_hi = _mm256_movemask_epi8(inputB);
	return r_lo | (r_hi << 32);
}

class SimdBase128 {
  public:
	inline SimdBase128() noexcept = default;

	inline SimdBase128& operator=(char other) {
		this->value = _mm_set1_epi8(other);
		return *this;
	}

	inline SimdBase128(char other) {
		*this = other;
	}

	inline SimdBase128(int64_t value00, int64_t value01) {
		this->value = _mm_insert_epi64(this->value, value00, 0);
		this->value = _mm_insert_epi64(this->value, value01, 1);
	}

	inline SimdBase128(uint64_t value00, uint64_t value01) {
		this->value = _mm_insert_epi64(this->value, static_cast<int64_t>(value00), 0);
		this->value = _mm_insert_epi64(this->value, static_cast<int64_t>(value00), 1);
	}

	inline SimdBase128& operator=(const __m128i other) {
		this->value = other;
		return *this;
	}

	inline SimdBase128(const __m128i other) {
		*this = other;
	}

	inline operator __m128i() {
		return this->value;
	}

	inline SimdBase128 operator|(SimdBase128 other) {
		return _mm_or_si128(*this, other);
	}

	inline SimdBase128 operator&(SimdBase128 other) {
		return _mm_and_si128(*this, other);
	}

	inline SimdBase128 operator^(SimdBase128 other) {
		return _mm_xor_si128(*this, other);
	}

	inline SimdBase128 operator+(SimdBase128 other) {
		return _mm_add_epi8(*this, other);
	}

	inline SimdBase128 operator|=(SimdBase128 other) {
		*this = *this | other;
		return *this;
	}

	inline SimdBase128 operator&=(SimdBase128 other) {
		*this = *this & other;
		return *this;
	}

	inline SimdBase128 operator^=(SimdBase128 other) {
		*this = *this ^ other;
		return *this;
	}

	inline SimdBase128 operator==(SimdBase128 other) {
		return _mm_cmpeq_epi8(this->value, other);
	}

	inline SimdBase128 operator<<(size_t amount) {
		SimdBase128 newValue{};
		for (size_t x = 0; x < 2; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) << (amount % 64));
			if (x > 0) {
				*(reinterpret_cast<int64_t*>(&newValue) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 1);
			}
		}
		return newValue;
	}

	inline SimdBase128 operator~() {
		SimdBase128 newValue{};
		for (size_t x = 0; x < 2; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) = ~*(reinterpret_cast<int64_t*>(&this->value) + x);
		}
		return newValue;
	}

	inline SimdBase128 bitAndNot(SimdBase128 other) {
		return _mm_andnot_si128(other, *this);
	}

	inline SimdBase128 shuffle(SimdBase128 other) {
		return _mm_shuffle_epi8(other, *this);
	}

  protected:
	__m128i value{};
};

class SimdBase256 {
  public:
	inline SimdBase256() noexcept = default;

	inline SimdBase256& operator=(char other) {
		this->value = _mm256_set1_epi8(other);
		return *this;
	}

	inline SimdBase256(char values[32]) {
		*this = _mm256_loadu_si256(reinterpret_cast<__m256i*>(values));
	}

	inline SimdBase256(char other) {
		*this = other;
	}

	inline SimdBase256(int64_t value00, int64_t value01, int64_t value02, int64_t value03) {
		this->value = _mm256_insert_epi64(this->value, value00, 0);
		this->value = _mm256_insert_epi64(this->value, value01, 1);
		this->value = _mm256_insert_epi64(this->value, value02, 2);
		this->value = _mm256_insert_epi64(this->value, value03, 3);
	}

	inline SimdBase256(uint64_t value00, uint64_t value01, uint64_t value02, uint64_t value03) {
		this->value = _mm256_insert_epi64(this->value, static_cast<int64_t>(value00), 0);
		this->value = _mm256_insert_epi64(this->value, static_cast<int64_t>(value01), 1);
		this->value = _mm256_insert_epi64(this->value, static_cast<int64_t>(value02), 2);
		this->value = _mm256_insert_epi64(this->value, static_cast<int64_t>(value03), 3);
	}

	inline SimdBase256& operator=(const __m256i other) {
		this->value = other;
		return *this;
	}

	inline SimdBase256(const __m256i other) {
		*this = other;
	}

	inline operator __m256i() {
		return this->value;
	}

	inline SimdBase256 operator|(SimdBase256 other) {
		return _mm256_or_si256(this->value, other);
	}

	inline SimdBase256 operator&(SimdBase256 other) {
		return _mm256_and_si256(this->value, other);
	}

	inline SimdBase256 operator^(SimdBase256 other) {
		return _mm256_xor_si256(this->value, other);
	}

	inline SimdBase256 operator+(SimdBase256 other) {
		return _mm256_add_epi8(this->value, other);
	}

	inline SimdBase256 operator|=(SimdBase256 other) {
		*this = *this | other;
		return *this;
	}

	inline SimdBase256 operator&=(SimdBase256 other) {
		*this = *this & other;
		return *this;
	}

	inline SimdBase256 operator^=(SimdBase256 other) {
		*this = *this ^ other;
		return *this;
	}

	inline SimdBase256 operator==(SimdBase256 other) {
		return _mm256_cmpeq_epi8(this->value, other);
	}

	inline SimdBase256 operator<<(size_t amount) {
		SimdBase256 newValue{};
		for (size_t x = 0; x < 4; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) << (amount % 64));
			if (x > 0) {
				*(reinterpret_cast<int64_t*>(&newValue) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 1);
			}
		}
		return newValue;
	}

	inline SimdBase256 operator~() {
		SimdBase256 newValue{};
		for (size_t x = 0; x < 4; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) = ~*(reinterpret_cast<int64_t*>(&this->value) + x);
		}
		return newValue;
	}

	inline SimdBase256 carrylessMultiplication(char operand) {
		auto inString01 = static_cast<uint64_t>(_mm_cvtsi128_si64(
			_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<uint64_t*>(&this->value) + 0)), SimdBase128{ operand }, 0)));
		auto prevInString = uint64_t(static_cast<int64_t>(inString01) >> 63);
		auto inString02 =
			static_cast<uint64_t>(_mm_cvtsi128_si64(
				_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<uint64_t*>(&this->value) + 1)), SimdBase128{ operand }, 0))) ^
			prevInString;
		prevInString = uint64_t(static_cast<int64_t>(inString02) >> 63);
		auto inString03 =
			static_cast<uint64_t>(_mm_cvtsi128_si64(
				_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<uint64_t*>(&this->value) + 2)), SimdBase128{ operand }, 0))) ^
			prevInString;
		prevInString = uint64_t(static_cast<int64_t>(inString03) >> 63);
		auto inString04 =
			static_cast<uint64_t>(_mm_cvtsi128_si64(
				_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<uint64_t*>(&this->value) + 3)), SimdBase128{ operand }, 0))) ^
			prevInString;
		return SimdBase256{ inString01, inString02, inString03, inString04 };
	}

	inline SimdBase256 collectCarries(SimdBase256 other) {
		SimdBase256 returnValue{};
		for (size_t x = 0; x < 4; ++x) {
			uint64_t returnValue64{};
			_addcarry_u64(0, *(reinterpret_cast<int64_t*>(&other) + x), *(reinterpret_cast<int64_t*>(&this->value) + x),
				reinterpret_cast<unsigned long long*>(&returnValue64));
			*(reinterpret_cast<int64_t*>(&returnValue) + x) = returnValue64;
		}
		return returnValue;
	}

	inline void printBits(const std::string& valuesTitle) {
		std::cout << valuesTitle;
		for (size_t x = 0; x < 32; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				std::cout << std::bitset<1>{ static_cast<uint64_t>(*(reinterpret_cast<int8_t*>(&this->value) + x)) >> y };
			}
		}
		std::cout << std::endl;
	}

	inline SimdBase256 bitAndNot(SimdBase256 other) {
		return _mm256_andnot_si256(other, *this);
	}

	inline SimdBase256 shuffle(SimdBase256 other) {
		return _mm256_shuffle_epi8(other, *this);
	}

	inline std::vector<int16_t> getSetBitIndices() {
		std::vector<int16_t> returnVector{};
		std::cout << "GET SET BIT INDICES: " << std::endl;
		for (int64_t x = 0; x < 4; ++x) {
			for (int64_t y = 0; y < 64; ++y) {
				if (*(reinterpret_cast<uint64_t*>(&this->value) + x) >> y & 1) {
					returnVector.push_back(static_cast<int16_t>(y + (x * 64)));
					std::cout << "1";
				} else {
					std::cout << "0";
				}
			}
		}
		return returnVector;
	}

  protected:
	__m256i value{};
};

enum class IndexTypes { Whitespace = 0, Quotes = 1, Structural = 2 };

class SimdStringSection {
  public:
	inline SimdStringSection() noexcept = default;

	inline void packStringIntoValue(SimdBase256* theValue, const char* string) {
		for (size_t x = 0; x < 32; ++x) {
			*(reinterpret_cast<int8_t*>(theValue) + x) = string[x];
		}
	}

	inline std::vector<int16_t> getStructuralIndices() {
		return this->S256.getSetBitIndices();
	}

	inline SimdBase256 collectWhiteSpace() {
		char valuesNew[32]{ ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2, 100,
			'\t', '\n', 112, 100, '\r', 100, 100 };
		SimdBase256 whitespaceTable{ valuesNew };
		SimdBase256 whiteSpaceReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			whiteSpaceReal[x] = this->values[x].shuffle(whitespaceTable) == this->values[x];
		}
		return { convertSimd256To64BitUint(whiteSpaceReal[0], whiteSpaceReal[1]), convertSimd256To64BitUint(whiteSpaceReal[2], whiteSpaceReal[3]),
			convertSimd256To64BitUint(whiteSpaceReal[4], whiteSpaceReal[5]), convertSimd256To64BitUint(whiteSpaceReal[6], whiteSpaceReal[7]) };
	}

	inline SimdBase256 collectStructuralCharacters() {
		this->R256 = this->Q256;
		this->R256 = this->R256.carrylessMultiplication('\xFF');
		char valuesNew[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
		SimdBase256 opTable{ valuesNew };
		SimdBase256 structural[8]{};
		for (size_t x = 0; x < 8; ++x) {
			auto valuesNew00 = this->values[x] | SimdBase256{ 0x20 };
			structural[x] = this->values[x].shuffle(opTable) == valuesNew00;
		}

		this->S256 = SimdBase256{ convertSimd256To64BitUint(structural[0], structural[1]), convertSimd256To64BitUint(structural[2], structural[3]),
			convertSimd256To64BitUint(structural[4], structural[5]), convertSimd256To64BitUint(structural[6], structural[7]) };

		this->S256 = this->S256.bitAndNot(R256);
		this->S256 = this->S256 | this->Q256;
		auto P256 = this->S256 | this->W256;
		P256 = P256 << 1;
		P256 &= ~this->W256 & ~R256;

		this->S256 = this->S256 | P256;
		return this->S256.bitAndNot(this->Q256.bitAndNot(R256));
	}

	inline SimdBase256 collectQuotes() {
		SimdBase256 backslashes = _mm256_set1_epi8('\\');
		SimdBase256 backslashesReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			backslashesReal[x] = this->values[x] == backslashes;
		}

		auto B256 = SimdBase256{ convertSimd256To64BitUint(backslashesReal[0], backslashesReal[1]),
			convertSimd256To64BitUint(backslashesReal[2], backslashesReal[3]), convertSimd256To64BitUint(backslashesReal[4], backslashesReal[5]),
			convertSimd256To64BitUint(backslashesReal[6], backslashesReal[7]) };

		SimdBase256 quotes = _mm256_set1_epi8('"');
		SimdBase256 quotesReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			quotesReal[x] = this->values[x] == quotes;
		}

		this->Q256 = SimdBase256{ convertSimd256To64BitUint(quotesReal[0], quotesReal[1]), convertSimd256To64BitUint(quotesReal[2], quotesReal[3]),
			convertSimd256To64BitUint(quotesReal[4], quotesReal[5]), convertSimd256To64BitUint(quotesReal[6], quotesReal[7]) };
		auto S = B256.bitAndNot(B256 << 1);
		SimdBase256 E{ _mm256_set1_epi8(0b01010101) };
		SimdBase256 O{ _mm256_set1_epi8(0b10101010) };
		auto ES = E & S;
		auto EC = ES.collectCarries(B256);
		auto ECE = EC.bitAndNot(B256);
		auto OD1 = ECE.bitAndNot(E);
		auto OS = S & O;
		auto OC = B256 + OS;
		auto OCE = OC.bitAndNot(B256);
		auto OD2 = OCE & E;
		auto OD = OD1 | OD2;

		return this->Q256 & ~OD;
	}

	inline SimdStringSection(std::string_view valueNew) {
		if (valueNew.size() % 256 != 0) {
			this->string = valueNew;
			this->string.resize(this->string.size() + 256 - (this->string.size() % 256));
			this->stringView = this->string;

		} else {
			this->stringView = valueNew;
		}

		this->packStringIntoValue(&this->values[0], this->stringView.data());
		this->packStringIntoValue(&this->values[1], this->stringView.data() + 32);
		this->packStringIntoValue(&this->values[2], this->stringView.data() + 64);
		this->packStringIntoValue(&this->values[3], this->stringView.data() + 96);
		this->packStringIntoValue(&this->values[4], this->stringView.data() + 128);
		this->packStringIntoValue(&this->values[5], this->stringView.data() + 160);
		this->packStringIntoValue(&this->values[6], this->stringView.data() + 192);
		this->packStringIntoValue(&this->values[7], this->stringView.data() + 224);

		this->Q256 = this->collectQuotes();
		this->W256 = this->collectWhiteSpace();
		this->S256 = this->collectStructuralCharacters();
		this->S256.printBits("S FINAL VALUES (256) ");
		this->W256.printBits("W FINAL VALUES (256) ");
		this->R256.printBits("R FINAL VALUES (256) ");
		this->Q256.printBits("Q FINAL VALUES (256): ");
		std::cout << "THE STRING: " << this->stringView << std::endl;
	}

  protected:
	SimdBase256 Q256{};
	SimdBase256 W256{};
	SimdBase256 R256{};
	SimdBase256 S256{};
	SimdBase256 values[8]{};
	std::string_view stringView{};
	std::string string{};
};

enum class ErrorCode { Empty = 0, TapeError = 1, DepthError = 2, Success = 3, ParseError = 4 };

enum class JsonTapeEventStates {
	ObjectBegin = 0,
	ObjectField = 1,
	ObjectContinue = 2,
	ScopeEnd = 3,
	ArrayBegin = 4,
	ArrayValue = 5,
	ArrayContinue = 6,
	DocumentEnd = 7
};

class SimdStringScanner {
  public:
	inline SimdStringScanner(std::string_view stringNew) noexcept {
		this->string = stringNew;
		size_t stringSize = this->string.size();
		size_t collectedSize{};
		while (stringSize > 256) {
			this->stringSections.emplace_back(std::string_view{ this->string.data() + collectedSize, 256 });
			stringSize -= 256;
			collectedSize += 256;
		}
		if (this->string.size() - collectedSize > 0) {
			this->stringSections.emplace_back(std::string_view{ this->string.data() + collectedSize, this->string.size() - collectedSize });
		}
		this->generateTapeRecord();
	}

	inline void generateTapeRecord() {
		for (auto& value: this->stringSections) {
			std::vector<int16_t> setBitIndices{ value.getStructuralIndices() };
			this->jsonTape.insert(this->jsonTape.end(), setBitIndices.begin(), setBitIndices.end());
		}
		this->next_structural = this->jsonTape.data();
	}

	ErrorCode visitTrueAtom(char* value) {
		if (strcmp(reinterpret_cast<char*>(value), "true")) {
			this->jsonData[this->currentKey] = true;
			return ErrorCode::Success;
		} else {
			return ErrorCode::ParseError;
		}
	}

	ErrorCode visitObjectStart() {
		this->jsonData[this->currentKey] = Jsonifier::Jsonifier::ObjectType{};
		return ErrorCode::Success;
	}

	ErrorCode visitArrayStart() {
		this->jsonData[this->currentKey] = Jsonifier::Jsonifier::ArrayType{};
		return ErrorCode::Success;
	}

	ErrorCode visitFalseAtom(char* value) {
		if (strcmp(reinterpret_cast<char*>(value), "false")) {
			this->jsonData[this->currentKey] = false;
			return ErrorCode::Success;
		} else {
			return ErrorCode::ParseError;
		}
	}

	ErrorCode visitNullAtom(char* value) {
		if (strcmp(reinterpret_cast<char*>(value), "null")) {
			this->jsonData[this->currentKey] = nullptr;
			return ErrorCode::Success;
		} else {
			return ErrorCode::ParseError;
		}
	}

	ErrorCode visitNumber(char* value) {
		if (strcmp(reinterpret_cast<char*>(value), "null")) {
			return ErrorCode::Success;
		} else {
			return ErrorCode::ParseError;
		}
	}

	ErrorCode visitKey(char* value) {
		this->currentKey.clear();
		this->currentKey.insert(this->currentKey.begin(), value + 1, this->peek() - 1);
		std::cout << "THE CURRENT KEY: " << this->currentKey << std::endl;
		std::cout << "THE CURRENT INDEX 01: " << *value << std::endl;
		std::cout << "THE CURRENT INDEX 02: " << *this->peek() << std::endl;
		std::cout << "THE CURRENT STRING: " << this->currentString << std::endl;
		return ErrorCode::Success;
	}

	ErrorCode visitEmptyObject() {
		this->jsonData[this->currentKey] = Jsonifier::Jsonifier::ObjectType{};
		return ErrorCode::Success;
	}

	ErrorCode visitEmptyArray() {
		this->jsonData[this->currentKey] = Jsonifier::Jsonifier::ArrayType{};
		return ErrorCode::Success;
	}

	ErrorCode visitString(char* value) {
		this->currentString.clear();
		this->currentString.insert(this->currentString.begin(), value, this->peek());
		std::cout << "THE CURRENT KEY: " << this->currentKey << std::endl;
		std::cout << "THE CURRENT INDEX 01: " << *value  << std::endl;
		std::cout << "THE CURRENT INDEX 02: " << *this->peek() << std::endl;
		std::cout << "THE CURRENT STRING: " << this->currentString << std::endl;
		this->jsonData[this->currentKey] = this->currentString;
		return ErrorCode::Success;
	}

	inline ErrorCode visitPrimitive(char* value) {
		switch (*value) {
			case '"':
				return this->visitString(value);
			case 't':
				return this->visitTrueAtom(value);
			case 'f':
				return this->visitFalseAtom(value);
			case 'n':
				return this->visitNullAtom(value);
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return this->visitNumber(value);
			default:
				throw DCAException{"Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(ErrorCode::TapeError))};
		}
	}

	uint16_t* next_structural{ nullptr };
	uint32_t depth{ 0 };

	inline char* peek() noexcept {
		std::cout << "CURRENT NEXT STRUCTURAL: " << *(next_structural) << std::endl;
		auto returnValue = &this->string[*(this->next_structural)];
		std::cout << "CURRENT NEXT VALUEL: " << *returnValue << std::endl;
		std::cout << *returnValue << std::endl;
		return returnValue;
	}

	inline char* advance(std::source_location location=std::source_location::current()) noexcept {
		auto returnValue = &this->string[*(this->next_structural++)];
		std::cout << "CURRENT NEXT STRUCTURAL: " << location.line() << *(next_structural) << std::endl;
		std::cout << "CURRENT NEXT VALUEL: " << *returnValue << std::endl;
		std::cout << *returnValue << std::endl;
		return returnValue;
	}

	inline ErrorCode generateJsonData() {
		switch (this->currentState) {
			case JsonTapeEventStates::ObjectBegin:{
				this->depth++;
				this->visitObjectStart();
				auto key = advance();
				if (*key != '"') {
					throw DCAException{ "Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
				}
				this->visitKey(key);
				this->currentState = JsonTapeEventStates::ObjectField;
				return this->generateJsonData();
			}
			case JsonTapeEventStates::ObjectField: {
				if (*advance() != ':') {
					throw DCAException{ "Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
				}
				auto value = advance();
				switch (*value) {
					case '{':
						if (*peek() == '}') {
							advance();
							this->visitEmptyObject();
							break;
						}
						this->currentState = JsonTapeEventStates::ObjectBegin;
						return this->generateJsonData();
					case '[':
						if (*peek() == ']') {
							advance();
							this->visitEmptyArray();
							break;
						}
						this->currentState = JsonTapeEventStates::ArrayBegin;
						return this->generateJsonData();

					default:
						this->visitPrimitive(value);
						break;
				}
				this->currentState = JsonTapeEventStates::ObjectContinue;
				return this->generateJsonData();
			}
			case JsonTapeEventStates::ObjectContinue: {
				switch (*advance()) {
					case ',': {
						auto key = advance();
						if (*key != '"') {
							throw DCAException{ "Failed to generate Json data: Reason: " +
								std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
						}
						this->visitKey(key);
						this->currentState = JsonTapeEventStates::ObjectField;
						return this->generateJsonData();
					}
					case '}': {
						this->currentState = JsonTapeEventStates::ScopeEnd;
						return this->generateJsonData();
					}
					default: {
						throw DCAException{ "Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
					}
						
				}
			}
			case JsonTapeEventStates::ScopeEnd: {
				this->depth--;
				if (this->depth == 0) {
					this->currentState = JsonTapeEventStates::DocumentEnd;
					return this->generateJsonData();
				}
				this->currentState = JsonTapeEventStates::ObjectContinue;
				return this->generateJsonData();
			}
			case JsonTapeEventStates::ArrayBegin: {
				this->depth++;
				this->visitArrayStart();
				this->currentState = JsonTapeEventStates::ArrayValue;
				return this->generateJsonData();
			}
			case JsonTapeEventStates::ArrayValue: {
				auto value = this->advance();
				switch (*value) {
					case '{':
						if (*this->peek() == '}') {
							this->advance();
							this->visitEmptyObject();
							break;
						}
						this->currentState = JsonTapeEventStates::ObjectBegin;
						return this->generateJsonData();
					case '[':
						if (*this->peek() == ']') {
							this->advance();
							this->visitEmptyArray();
							break;
						}
						this->currentState = JsonTapeEventStates::ArrayBegin;
						return this->generateJsonData();
					default:
						this->visitPrimitive(value);
						break;
				}
			}
			case JsonTapeEventStates::ArrayContinue: {
				switch (*advance()) {
					case ',':
						this->currentState = JsonTapeEventStates::ArrayValue;
						return this->generateJsonData();
					case ']':
						this->currentState = JsonTapeEventStates::ScopeEnd;
						return this->generateJsonData();
					default:
						throw DCAException{ "Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
				}
				break;
			}
			case JsonTapeEventStates::DocumentEnd: {
				break;
			}
			default: {
				break;
			}
		}
		return ErrorCode::Success;
	}

	inline Jsonifier::Jsonifier getJsonData() {
		auto value = advance();
		ErrorCode resultCode{};
		switch (*value) {
			case '{':
				if (*this->peek() == '}') {
					this->advance();
					this->visitEmptyObject();
					break;
				}
				this->currentState = JsonTapeEventStates::ObjectBegin;
				resultCode = this->generateJsonData();
				break;
		  case '[':
				if (*this->peek() == ']') {
					this->advance();
					this->visitEmptyArray();
					break;
				}
			  this->currentState = JsonTapeEventStates::ArrayBegin;
			  resultCode = this->generateJsonData();
			  break;
		  default:
			  resultCode = this->visitPrimitive(value);
			  break;
		}
 
		if (resultCode!= ErrorCode::Success) {
			throw std::runtime_error{ "Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(resultCode)) };
		}
		return this->jsonData;
	}

  protected:
	JsonTapeEventStates currentState{ JsonTapeEventStates::ObjectBegin };
	std::vector<SimdStringSection> stringSections{};
	bool areWeWaitingForAKey{ true };
	std::vector<uint16_t> jsonTape{};
	Jsonifier::Jsonifier jsonData{};
	bool haveWeStarted{ false };
	std::string currentString{};
	std::string currentKey{};
	std::string string{};
};