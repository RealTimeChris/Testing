#include <jsonifier/Jsonifier.hpp>
#include <immintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <wmmintrin.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <bitset>
#include <atomic>
#include <deque>

template<typename TTy> class StopWatch {
  public:
	using HRClock = std::chrono::high_resolution_clock;

	StopWatch() = delete;

	StopWatch<TTy>& operator=(const StopWatch<TTy>& data) {
		this->maxNumberOfMs.store(data.maxNumberOfMs.load());
		this->startTime.store(data.startTime.load());
		return *this;
	}

	StopWatch(const StopWatch<TTy>& data) {
		*this = data;
	}

	StopWatch(TTy maxNumberOfMsNew) {
		this->maxNumberOfMs.store(maxNumberOfMsNew);
		this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
	}

	TTy totalTimePassed() {
		TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
		TTy elapsedTime = currentTime - this->startTime.load();
		return elapsedTime;
	}

	TTy getTotalWaitTime() {
		return this->maxNumberOfMs.load();
	}

	bool hasTimePassed() {
		TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
		TTy elapsedTime = currentTime - this->startTime.load();
		if (elapsedTime >= this->maxNumberOfMs.load()) {
			return true;
		}else {
			return false;
		}
	}

	void resetTimer() {
		this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
	}

  protected:
	std::atomic<TTy> maxNumberOfMs{ TTy{ 0 }};
	std::atomic<TTy> startTime{ TTy{ 0 }};
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

	inline operator const __m128i() const {
		return this->value;
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

	inline SimdBase128 operator^(SimdBase128 other) const {
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
		__m128i newValue{};
		for (size_t x = 0; x < 2; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) << (amount % 64));
			if (x > 0) {
				*(reinterpret_cast<int64_t*>(&newValue) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 0x00000001);
			}
		}
		return newValue;
	}

	inline SimdBase128 operator~() {
		__m128i newValue{};
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

	inline void printBits(std::string valuesTitle) {
		std::cout << valuesTitle;
		for (size_t x = 0; x < 16; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				//std::cout << std::bitset<1>{ static_cast<uint64_t>(*(reinterpret_cast<int8_t*>(&this->value) + x)) >> y };
			}
		}
		std::cout << std::endl;
	};

  protected:
	__m128i value{};
};

class SimdBase256 {
  public:
	SimdBase256() noexcept = default;

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

	inline operator const __m256i() const {
		return this->value;
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
				*(reinterpret_cast<int64_t*>(&newValue) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 0x00000001);
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
		return _mm256_andnot_si256(other.value, this->value);
	}

	inline SimdBase256 shuffle(SimdBase256 other) {
		return _mm256_shuffle_epi8(other, *this);
	}

	inline std::vector<int16_t> getSetBitIndices() {
		std::vector<int16_t> returnVector{};
		for (int64_t x = 0; x < 255; ++x) {
			if ((*reinterpret_cast<uint64_t*>(&this->value) >> x) & 1) {
				returnVector.push_back(static_cast<int16_t>(x));
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
		return (this->S256 | this->Q256).getSetBitIndices();
	}

	inline bool checkIfIndexIsSet(size_t index, IndexTypes type) {
		switch (type) {
			case IndexTypes::Whitespace: {
				if ((*reinterpret_cast<uint64_t*>(&this->W256)) << index) {
					return true;
				}
			}
			case IndexTypes::Quotes: {
				if ((*reinterpret_cast<uint64_t*>(&this->Q256)) << index) {
					return true;
				}
			}
			case IndexTypes::Structural: {
				if ((*reinterpret_cast<uint64_t*>(&this->S256)) << index) {
					return true;
				}
			}
		}
		return false;
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

		}else {
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
		(this->Q256 | this->S256).printBits("S | Q FINAL VALUES: ");
		this->S256.printBits("S FINAL VALUES (256) ");
		this->W256.printBits("W FINAL VALUES (256) ");
		this->R256.printBits("R FINAL VALUES (256) ");
		this->Q256.printBits("Q FINAL VALUES (256): ");
		std::cout << "THE STRING: " << this->stringView << std::endl;
	}

	operator std::string() {
		return this->string;
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

enum class JsonTapeEventStates {
	Object_Start = 0,
	Object_End = 1,
	Array_Start = 2,
	Array_End = 3,
	String_Start = 4,
	String_End = 5,
	Primitive_Start = 6,
	Primitive_End = 7
};

class SimdStringScanner {
  public:
	inline SimdStringScanner(std::string_view stringNew) noexcept {
		this->string = stringNew;
		size_t stringSize =this->string.size();
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
	}

	inline std::string_view collectKey(size_t index01, size_t index02) {
		return this->string.substr(index01, index02);
	}

	inline void generateJsonData(Jsonifier::Jsonifier& jsonDataNew , size_t currentIndex01 = 0) {
		std::string currentKey{};
		std::unordered_map<std::string, Jsonifier::Jsonifier> newData{};
		for (size_t x = currentIndex01; x < this->jsonTape.size(); ++x) {
			std::cout << "THE INDEX: " << +this->jsonTape[x] << std::endl;
			std::cout << "THE VALUE: " << this->string[this->jsonTape[x]] << std::endl;
			if (!this->haveWeStarted) {
				this->areWeWaitingForAKey = true;
			}
			switch (this->string[this->jsonTape[x]]) {
				case '{': {
					if (!this->haveWeStarted) {
						this->haveWeStarted = true;
						this->areWeWaitingForAKey = true;
						this->objectCount++;
						continue;
					}
					this->areWeWaitingForAKey = true;
					this->objectCount++;
					break;
				}
				case '[': {
					this->arrayCount++;
					break;
				}
				case ']': {
					this->arrayCount--;
					break;
				}
				case '"': {
					if (this->areWeWaitingForAKey) {
						currentKey = this->string.substr(this->jsonTape[x] + 1, this->jsonTape[x + 1] - this->jsonTape[x] - 1);
						std::cout << "CURRENT KEY INDEX: (REAL) 0101: " << this->jsonTape[x] << std::endl;
						std::cout << "CURRENT KEY INDEX VALUE: (REAL) 0101: " << this->string[this->jsonTape[x]] << std::endl;
						std::cout << "CURRENT KEY INDEX: (REAL) 0202: " << this->jsonTape[x + 1] << std::endl;
						std::cout << "CURRENT KEY INDEX VALUE: (REAL) 0101: " << this->string[this->jsonTape[x + 1]] << std::endl;
						std::cout << "CURRENT KEY INDEX: (REAL) 0101: " << this->jsonTape[x] + 1 << std::endl;
						std::cout << "CURRENT KEY INDEX: (REAL) 0202: " << this->jsonTape[x + 1] - this->jsonTape[x] - 2 << std::endl;
						std::cout << "CURRENT KEY (REAL): " << currentKey << std::endl;
						this->areWeWaitingForAKey = false;
					} else {
						auto stringNew = this->string.substr(this->jsonTape[x] + 1, this->jsonTape[x + 1] - this->jsonTape[x] - 2);
						jsonDataNew[currentKey] = static_cast<std::string>(stringNew);
						std::cout << "CURRENT STRING: " << jsonDataNew[currentKey].getValue<std::string>() << std::endl;
					}
					break;
				}
				case '}': {
					this->objectCount--;
					break;
				}
				case ':': {
					jsonDataNew[currentKey] =
						static_cast<std::string>(this->string.substr(this->jsonTape[x] + 1, this->jsonTape[x + 1] - this->jsonTape[x] - 2));
					std::cout << "CURRENT STRING: 0303: " << jsonDataNew[currentKey].getValue<std::string>() << std::endl;
					break;
				}
				default: {
					break;
				}
			}
		}
		if (this->arrayCount != 0) {
			throw std::runtime_error{ "Arrays were not all closed: " + std::to_string(this->arrayCount) };
		}
		if (this->objectCount!= 0) {
			throw std::runtime_error{ "Objects were not all closed: " + std::to_string(this->objectCount) };
		}
		this->jsonData = std::move(jsonDataNew);
	}

	inline Jsonifier::Jsonifier getJsonData() {
		return this->jsonData;
	}

  protected:
	std::vector<SimdStringSection> stringSections{};
	JsonTapeEventStates currentState{};
	bool areWeWaitingForAKey{ true };
	std::vector<int16_t> jsonTape{};
	Jsonifier::Jsonifier jsonData{};
	bool haveWeStarted{ false };
	std::string_view string{};
	int64_t objectCount{};
	int64_t arrayCount{};
};

class SimdBase64 {
  public:
	inline operator uint64_t() {
		return convertSimd256To64BitUint(this->values[1], this->values[0]);
	}

	inline void packStringIntoValue(__m256i& theValue, const char* string) {
		for (size_t x = 0; x < 32; ++x) {
			*(reinterpret_cast<int8_t*>(&theValue) + x) = string[x];
		}
	}

	inline void printBits(uint64_t inA, std::string values) {
		alignas(32) uint8_t v[64]{};
		for (size_t x = 0; x < 64; ++x) {
			if ((static_cast<uint64_t>(inA >> x) & 0x01) == static_cast<uint64_t>(1 << 0)) {
				v[x] = 1;
			}
		}
		printf(std::string{ values.c_str() +
				   std::string{ " (DIGITS) v64_u8: "
								"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%"
								"d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n" }}
				   .c_str(),
			v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17], v[18], v[19], v[20],
			v[21], v[22], v[23], v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31], v[32], v[33], v[34], v[35], v[36], v[37], v[38], v[39],
			v[40], v[41], v[42], v[43], v[44], v[45], v[46], v[47], v[48], v[49], v[50], v[51], v[52], v[53], v[54], v[55], v[56], v[57], v[58],
			v[59], v[60], v[61], v[62], v[63]);
	}

	inline uint64_t collectCarries(uint64_t inputA, uint64_t inputB) {
		uint64_t returnValue{};
		_addcarry_u64(0, inputB, inputA, reinterpret_cast<unsigned long long*>(&returnValue));
		return returnValue;
	}

	inline void printBits(std::string valuesTitle) {
		std::cout << valuesTitle;
		for (size_t x = 0; x < 32; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				//std::cout << std::bitset<1>{ reinterpret_cast<uint64_t*>(&values[0])[x] >> y };
			}
		}
		for (size_t x = 0; x < 32; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				//std::cout << std::bitset<1>{reinterpret_cast<uint64_t*>(&values[1])[x] >> y };
			}
		}
		std::cout << std::endl;
	};

	inline SimdBase64(const __m256i& value01, __m256i value02) {
		this->values[0] = value01;
		this->values[1] = value02;
	}

	inline void collectQuotes() {
		auto backslashes = _mm256_set1_epi8('\\');
		auto B0 = _mm256_cmpeq_epi8(this->values[0], backslashes);
		auto B1 = _mm256_cmpeq_epi8(this->values[1], backslashes);
		auto B64 = convertSimd256To64BitUint(B0, B1);
		this->S64 = B64 & ~(B64 << 1);
		uint64_t E{ 0b0101010101010101010101010101010101010101010101010101010101010101 };
		auto quotes = _mm256_set1_epi8('"');
		uint64_t O{ 0b1010101010101010101010101010101010101010101010101010101010101010 };
		auto ES = this->S64 & E;
		auto EC = B64 + ES;
		auto ECE = EC & ~B64;
		auto OD1 = ECE & ~E;
		auto OS = this->S64 & O;
		auto OC = B64 + OS;
		auto OCE = OC & ~B64;
		auto OD2 = OCE & E;
		auto OD = OD1 | OD2;
		auto Q0 = _mm256_cmpeq_epi8(this->values[0], quotes);
		auto Q1 = _mm256_cmpeq_epi8(this->values[1], quotes);
		this->Q64 = convertSimd256To64BitUint(Q0, Q1);
		this->Q64 &= ~OD;
	}

	inline void collectStructuralCharacters() {
		__m256i opTable{ _mm256_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0,
			0) };
		this->R64 = this->Q64;
		this->R64 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->R64), _mm_set1_epi8('\xFF'), 0));
		auto valuesNew00 = _mm256_or_si256(this->values[0], _mm256_set1_epi8(0x20));
		auto valuesNew01 = _mm256_or_si256(this->values[1], _mm256_set1_epi8(0x20));
		auto structural00 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(opTable, this->values[0]), valuesNew00);
		auto structural01 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(opTable, this->values[1]), valuesNew01);
		this->S64 = convertSimd256To64BitUint(structural00, structural01);
		this->S64 = this->S64 & ~this->R64;
		this->S64 = this->S64 | this->Q64;
		auto P64 = this->S64 | this->W64;
		P64 = P64 << 1;
		P64 &= ~this->W64 & ~this->R64;
		this->S64 = this->S64 | P64;
		this->S64 = this->S64 & ~(this->Q64 & ~this->R64);
	}

	inline void collectWhiteSpace() {
		__m256i whitespaceTable{ _mm256_setr_epi8(' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100,
			17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100) };
		auto whiteSpace00 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(whitespaceTable, this->values[0]), this->values[0]);
		auto whiteSpace01 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(whitespaceTable, this->values[1]), this->values[1]);
		this->W64 = convertSimd256To64BitUint(whiteSpace00, whiteSpace01);
	}

	inline SimdBase64(std::string& stringNewer) {
		this->string = stringNewer;
		packStringIntoValue(this->values[0], stringNewer.data());
		packStringIntoValue(this->values[1], stringNewer.data() + 32);
		this->collectQuotes();
		this->collectWhiteSpace();
		this->collectStructuralCharacters();

		//printBits(this->Q64, "Q FINAL VALUES: ");
		//printBits(this->R64, "R FINAL VALUES: ");
		//printBits(this->S64, "S FINAL VALUES: ");
		//printBits(this->W64, "W FINAL VALUES: ");
	}

	inline operator std::string() {
		return string;
	}

  protected:
	__m256i values[2]{};
	std::string string{};
	uint64_t Q64{};
	uint64_t R64{};
	uint64_t S64{};
	uint64_t W64{};
};

int32_t main() noexcept {
	try {
	
			std::string string64{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
		std::string string256{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
			std::string stringNew{
				"{\"d\":{\"activities\":null,\"client_status\":{\"mobile\":\"online\"},\"guild_id\":\"815087249556373516\",\"status\":"
				"\"online\",\"user\":{\"id\":\"381531043334717440\"}}}}"
			};
		::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		SimdStringScanner stringScanner{ stringNew };
		Jsonifier::Jsonifier theData{};
		stringScanner.generateJsonData(theData);
		auto newJsonData = stringScanner.getJsonData();
		newJsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::cout << "THE DATA: " << newJsonData.operator std::string&&() << std::endl;
		stopWatch.resetTimer();
		for (size_t x = 0; x < 256 * 16384 / 4; ++x) {
			SimdStringScanner simd8Test{ string256 };
			totalSize += string256.size();
		}
		totalTime += stopWatch.totalTimePassed().count();

		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;

		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		for (size_t x = 0; x < 256 * 16384; ++x) {
			SimdBase64 simd8Test{ string64 };
			totalSize += string64.size();
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;


	} catch (std::exception& e) {
		std::cout << "THE ERROR: " << e.what() << std::endl;
	}


	return 0;
};