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
		this->maxNumberOfMs.store(maxNumberOfMsNew.count());
		this->startTime.store(static_cast<int64_t>(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()).count()));
	}

	int64_t totalTimePassed() {
		int64_t currentTime = static_cast<int64_t>(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()).count());
		int64_t elapsedTime = currentTime - this->startTime.load();
		return elapsedTime;
	}

	int64_t getTotalWaitTime() {
		return this->maxNumberOfMs.load();
	}

	bool hasTimePassed() {
		int64_t currentTime = static_cast<int64_t>(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()).count());
		int64_t elapsedTime = currentTime - this->startTime.load();
		if (elapsedTime >= this->maxNumberOfMs.load()) {
			return true;
		} else {
			return false;
		}
	}

	void resetTimer() {
		this->startTime.store(static_cast<int64_t>(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()).count()));
	}

  protected:
	std::atomic_int64_t maxNumberOfMs{ 0 };
	std::atomic_int64_t startTime{ 0 };
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
		this->value = _mm_insert_epi64(this->value, value00, 0);
		this->value = _mm_insert_epi64(this->value, value01, 1);
	}

	inline SimdBase128& operator=(const __m128i other) {
		this->value = other;
		return *this;
	}

	inline SimdBase128(const __m128i other) {
		*this = other;
	}

	inline operator const __m128i&() const {
		return this->value;
	}

	inline operator __m128i&() {
		return this->value;
	}

	inline SimdBase128 operator|(__m128i other) {
		return _mm_or_si128(*this, other);
	}

	inline SimdBase128 operator&(__m128i other) {
		return _mm_and_si128(*this, other);
	}

	inline SimdBase128 operator^(__m128i other) {
		return _mm_xor_si128(*this, other);
	}

	inline SimdBase128 operator+(__m128i other) {
		return _mm_add_epi8(*this, other);
	}

	inline SimdBase128 operator|=(__m128i other) {
		*this = *this | other;
		return *this;
	}

	inline SimdBase128 operator&=(__m128i other) {
		*this = *this & other;
		return *this;
	}

	inline SimdBase128 operator^=(__m128i other) {
		*this = *this ^ other;
		return *this;
	}

	inline SimdBase128 operator==(__m128i rhs) {
		return _mm_cmpeq_epi8(this->value, rhs);
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

	inline SimdBase128 bitAndNot(__m128i other) {
		return _mm_andnot_si128(other, *this);
	}

	inline SimdBase128 shuffle(__m128i indices) {
		return _mm_shuffle_epi8(indices, *this);
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

	inline SimdBase256(char other) {
		*this = other;
	}

	inline SimdBase256(uint64_t value00, uint64_t value01, uint64_t value02, uint64_t value03) {
		this->value = _mm256_insert_epi64(this->value, value00, 0);
		this->value = _mm256_insert_epi64(this->value, value01, 1);
		this->value = _mm256_insert_epi64(this->value, value02, 2);
		this->value = _mm256_insert_epi64(this->value, value03, 3);
	}

	inline SimdBase256& operator=(const __m256i other) {
		this->value = other;
		return *this;
	}

	inline SimdBase256(const __m256i other) {
		*this = other;
	}

	inline operator const __m256i&() const {
		return this->value;
	}

	inline operator __m256i&() {
		return this->value;
	}

	inline SimdBase256 operator|(__m256i other) {
		return _mm256_or_si256(this->value, other);
	}

	inline SimdBase256 operator&(__m256i other) {
		return _mm256_and_si256(this->value, other);
	}

	inline SimdBase256 operator^(__m256i other) {
		return _mm256_xor_si256(this->value, other);
	}

	inline SimdBase256 operator+(__m256i other) {
		return _mm256_add_epi8(this->value, other);
	}

	inline SimdBase256 operator|=(__m256i other) {
		*this = *this | other;
		return *this;
	}

	inline SimdBase256 operator&=(__m256i other) {
		*this = *this & other;
		return *this;
	}

	inline SimdBase256 operator^=(__m256i other) {
		*this = *this ^ other;
		return *this;
	}

	inline SimdBase256 operator==(__m256i rhs) {
		return _mm256_cmpeq_epi8(this->value, rhs);
	}

	inline SimdBase256 operator<<(size_t amount) {
		__m256i newValue{};
		for (size_t x = 0; x < 4; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) << (amount % 64));
			if (x > 0) {
				*(reinterpret_cast<int64_t*>(&newValue) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 0x00000001);
			}
		}
		return newValue;
	}

	inline SimdBase256 operator~() {
		__m256i newValue{};
		for (size_t x = 0; x < 4; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) = ~*(reinterpret_cast<int64_t*>(&this->value) + x);
		}
		return newValue;
	}

	inline SimdBase256 carrylessMultiplication(char operand) {
		return SimdBase256{ static_cast<uint64_t>(_mm_cvtsi128_si64(_mm_clmulepi64_si128(
								_mm_set_epi64x(0ULL, *(reinterpret_cast<uint64_t*>(&this->value) + 0)), SimdBase128{ operand }, 0))),
			static_cast<uint64_t>(_mm_cvtsi128_si64(
				_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<uint64_t*>(&this->value) + 1)), SimdBase128{ operand }, 0))),
			static_cast<uint64_t>(_mm_cvtsi128_si64(
				_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<uint64_t*>(&this->value) + 2)), SimdBase128{ operand }, 0))),
			static_cast<uint64_t>(_mm_cvtsi128_si64(
				_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<uint64_t*>(&this->value) + 3)), SimdBase128{ operand }, 0))) };
	}

	inline SimdBase256 collectCarries(__m256i inputB) {
		SimdBase256 returnValue{};
		for (size_t x = 0; x < 4; ++x) {
			uint64_t returnValue64{};
			_addcarry_u64(0, *(reinterpret_cast<int64_t*>(&inputB) + x), *(reinterpret_cast<int64_t*>(&this->value) + x),
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

	inline SimdBase256 bitAndNot(__m256i other) {
		return _mm256_andnot_si256(other, this->value);
	}

	inline SimdBase256 shuffle(__m256i indices) {
		return _mm256_shuffle_epi8(indices, *this);
	}
	
	inline std::vector<int16_t> getSetBitIndices() {
		std::vector<int16_t> returnVector{};
		for (int64_t x = 0; x < 255; ++x) {
			if ((*reinterpret_cast<uint64_t*>(&this->value) >> x ) & 1) {
				returnVector.push_back(x);
			}
		}
		return returnVector;
	}

  protected:
	__m256i value{};
};

class SimdStringSection {
  public:
	inline SimdStringSection() noexcept = default;

	inline void packStringIntoValue(__m256i& theValue, const char* string) {
		for (size_t x = 0; x < 32; ++x) {
			*(reinterpret_cast<int8_t*>(&theValue) + x) = string[x];
		}
	}

	inline SimdBase256& getQuotedStringRange() {
		return this->Q256;
	}

	inline SimdBase256& getLeftSquareBracketRange() {
		return this->LSB256;
	}

	inline SimdBase256& getRightSquareBracketRange() {
		return this->RSB256;
	}

	inline SimdBase256& getWhiteSpaceCharacters() {
		return this->W256;
	}

	inline SimdBase256& getBackslashesRange() {
		return this->B256;
	}

	inline SimdBase256& getCommasRange() {
		return this->C256;
	}

	inline SimdBase256& getStructuralCharacters() {
		return this->S256;
	}

	inline SimdBase256& getLeftCurlyBracketRange() {
		return this->LCB256;
	}

	inline SimdBase256& getRightCurlyBracketRange() {
		return this->RCB256;
	}

	inline std::vector<int16_t> getStructuralIndices() {
		return this->S256.getSetBitIndices();
	}

	inline SimdBase256 collectColons() {
		SimdBase256 colons = _mm256_set1_epi8(':');
		SimdBase256 colonsReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			colonsReal[x] = this->values[x] == colons;
		}
		return { convertSimd256To64BitUint(colonsReal[0], colonsReal[1]),
			convertSimd256To64BitUint(colonsReal[2], colonsReal[3]),
			convertSimd256To64BitUint(colonsReal[4], colonsReal[5]),
			convertSimd256To64BitUint(colonsReal[6], colonsReal[7]) };
	}

	inline SimdBase256 collectRightSquareBrackets() {
		SimdBase256 rightSquareBrackets = _mm256_set1_epi8(']');
		SimdBase256 rightSquareBracketsReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			rightSquareBracketsReal[x] = this->values[x] == rightSquareBrackets;
		}
		return { convertSimd256To64BitUint(rightSquareBracketsReal[0], rightSquareBracketsReal[1]),
			convertSimd256To64BitUint(rightSquareBracketsReal[2], rightSquareBracketsReal[3]),
			convertSimd256To64BitUint(rightSquareBracketsReal[4], rightSquareBracketsReal[5]),
			convertSimd256To64BitUint(rightSquareBracketsReal[6], rightSquareBracketsReal[7]) };
	}

	inline SimdBase256 collectWhiteSpace() {
		SimdBase256 whitespaceTable{ _mm256_setr_epi8(' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100,
			100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100) };
		SimdBase256 whiteSpaceReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			whiteSpaceReal[x] = this->values[x].shuffle(whitespaceTable) == this->values[x];
		}
		return { convertSimd256To64BitUint(whiteSpaceReal[0], whiteSpaceReal[1]), convertSimd256To64BitUint(whiteSpaceReal[2], whiteSpaceReal[3]),
			convertSimd256To64BitUint(whiteSpaceReal[4], whiteSpaceReal[5]), convertSimd256To64BitUint(whiteSpaceReal[6], whiteSpaceReal[7]) };
	}

	inline SimdBase256 collectLeftSquareBrackets() {
		SimdBase256 leftSquareBrackets = _mm256_set1_epi8('[');
		SimdBase256 leftSquareBracketsReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			leftSquareBracketsReal[x] = this->values[x] == leftSquareBrackets;
		}
		return { convertSimd256To64BitUint(leftSquareBracketsReal[0], leftSquareBracketsReal[1]),
			convertSimd256To64BitUint(leftSquareBracketsReal[2], leftSquareBracketsReal[3]),
			convertSimd256To64BitUint(leftSquareBracketsReal[4], leftSquareBracketsReal[5]),
			convertSimd256To64BitUint(leftSquareBracketsReal[6], leftSquareBracketsReal[7]) };
	}

	inline SimdBase256 collectRightCurlyBrackets() {
		SimdBase256 rightCurlyBrackets = _mm256_set1_epi8('}');
		SimdBase256 rightCurlyBracketsReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			rightCurlyBracketsReal[x] = this->values[x] == rightCurlyBrackets;
		}
		return { convertSimd256To64BitUint(rightCurlyBracketsReal[0], rightCurlyBracketsReal[1]),
			convertSimd256To64BitUint(rightCurlyBracketsReal[2], rightCurlyBracketsReal[3]),
			convertSimd256To64BitUint(rightCurlyBracketsReal[4], rightCurlyBracketsReal[5]),
			convertSimd256To64BitUint(rightCurlyBracketsReal[6], rightCurlyBracketsReal[7]) };
	}

	inline SimdBase256 collectLeftCurlyBrackets() {
		SimdBase256 leftCurlyBrackets = _mm256_set1_epi8('{');
		SimdBase256 leftCurlyBracketsReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			leftCurlyBracketsReal[x] = this->values[x] == leftCurlyBrackets;
		}
		return { convertSimd256To64BitUint(leftCurlyBracketsReal[0], leftCurlyBracketsReal[1]),
			convertSimd256To64BitUint(leftCurlyBracketsReal[2], leftCurlyBracketsReal[3]),
			convertSimd256To64BitUint(leftCurlyBracketsReal[4], leftCurlyBracketsReal[5]),
			convertSimd256To64BitUint(leftCurlyBracketsReal[6], leftCurlyBracketsReal[7]) };
	}

	inline SimdBase256 collectCommas() {
		SimdBase256 commas = _mm256_set1_epi8(',');
		SimdBase256 commasReal[8]{};

		for (size_t x = 0; x < 8; ++x) {
			commasReal[x] = this->values[x] == commas;
		}

		return { convertSimd256To64BitUint(commasReal[0], commasReal[1]), convertSimd256To64BitUint(commasReal[2], commasReal[3]),
			convertSimd256To64BitUint(commasReal[4], commasReal[5]), convertSimd256To64BitUint(commasReal[6], commasReal[7]) };
	}

	inline SimdBase256 collectStructuralCharacters() {
		this->R256 = this->Q256;
		this->R256 = this->R256.carrylessMultiplication('\xFF');
		SimdBase256 opTable{ _mm256_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',',
			'}', 0, 0) };
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

	inline SimdBase256 collectBackslashes() {
		SimdBase256 backslashes = _mm256_set1_epi8('\\');
		SimdBase256 backslashesReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			backslashesReal[x] = this->values[x] == backslashes;
		}


		return { convertSimd256To64BitUint(backslashesReal[0], backslashesReal[1]), convertSimd256To64BitUint(backslashesReal[2], backslashesReal[3]),
			convertSimd256To64BitUint(backslashesReal[4], backslashesReal[5]), convertSimd256To64BitUint(backslashesReal[6], backslashesReal[7]) };
	}

	inline SimdBase256 collectQuotes() {
		SimdBase256 quotes = _mm256_set1_epi8('"');
		SimdBase256 quotesReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			quotesReal[x] = this->values[x] == quotes;
		}

		this->Q256 = SimdBase256{ convertSimd256To64BitUint(quotesReal[0], quotesReal[1]), convertSimd256To64BitUint(quotesReal[2], quotesReal[3]),
			convertSimd256To64BitUint(quotesReal[4], quotesReal[5]), convertSimd256To64BitUint(quotesReal[6], quotesReal[7]) };
		auto S = this->B256.bitAndNot(this->B256 << 1);
		SimdBase256 E{ _mm256_set1_epi8(0b01010101) };
		SimdBase256 O{ _mm256_set1_epi8(0b10101010) };
		auto ES = E & S;
		auto EC = ES.collectCarries(this->B256);
		auto ECE = EC.bitAndNot(this->B256);
		auto OD1 = ECE.bitAndNot(E);
		auto OS = S & O;
		auto OC = this->B256 + OS;
		auto OCE = OC.bitAndNot(this->B256);
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

		this->packStringIntoValue(this->values[0], this->stringView.data());
		this->packStringIntoValue(this->values[1], this->stringView.data() + 32);
		this->packStringIntoValue(this->values[2], this->stringView.data() + 64);
		this->packStringIntoValue(this->values[3], this->stringView.data() + 96);
		this->packStringIntoValue(this->values[4], this->stringView.data() + 128);
		this->packStringIntoValue(this->values[5], this->stringView.data() + 160);
		this->packStringIntoValue(this->values[6], this->stringView.data() + 192);
		this->packStringIntoValue(this->values[7], this->stringView.data() + 224);

		this->B256 = this->collectBackslashes();

		this->Q256 = this->collectQuotes();

		this->C256 = this->collectCommas();

		this->LCB256 = this->collectLeftCurlyBrackets();

		this->RCB256 = this->collectRightCurlyBrackets();

		this->LSB256 = this->collectLeftSquareBrackets();

		this->RSB256 = this->collectRightSquareBrackets();

		this->W256 = this->collectWhiteSpace();

		this->S256 = this->collectStructuralCharacters();
		this->S256.printBits("S FINAL VALUES (256) ");
		//this->W256.printBits("W FINAL VALUES (256) ");
		//this->R256.printBits("R FINAL VALUES (256) ");
		//this->Q256.printBits("Q FINAL VALUES (256): ");
		//this->LSB256.printBits("LSB FINAL VALUES (256): ");
		//this->RSB256.printBits("RSB FINAL VALUES (256) ");
		//this->LCB256.printBits("LCB FINAL VALUES (256): ");

		//this->RCB256.printBits("RCB FINAL VALUES (256) ");

		//this->R256.printBits("THE R VALUES: ");
		//this->C256.printBits("COMMAS FINAL VALUES (256) ");		
		std::cout << "THE STRING: " << this->stringView << std::endl;
	}

	operator std::string() {
		return this->string;
	}

  protected:
	SimdBase256 Q256{};
	SimdBase256 W256{};
	SimdBase256 R256{};
	SimdBase256 B256{};
	SimdBase256 S256{};
	SimdBase256 RSB256{};
	SimdBase256 LSB256{};
	SimdBase256 RCB256{};
	SimdBase256 LCB256{};
	SimdBase256 C256{};
	SimdBase256 values[8]{};
	std::string_view stringView{};
	std::string string{};
};

enum class JsonEventType : int8_t {
	Object_Start = 1 << 0,
	Array_Start = 1 << 1,
	String_Start = 1 << 2,
	Primitive_Start = 1 << 3,
	Int_Start = 1 << 4,
	Float_Start = 1 << 5,
	Bool_Start = 1 << 6
};

struct JsonTapeRecord {
	JsonEventType eventType{};
	size_t startingIndex{};
};

class SimdStringScanner {
  public:
	SimdStringScanner(std::string_view string) noexcept {
		size_t stringSize = string.size();
		size_t collectedSize{};
		while (stringSize > 256) {
			this->stringSections.emplace_back(std::string_view{ string.data() + collectedSize, 256 });
			stringSize -= 256;
			collectedSize += 256;
		}
		this->stringSections.emplace_back(std::string_view{ string.data() + collectedSize, string.size() - collectedSize });
	}

	void generateTapeRecord() {
		for (auto& value: this->stringSections) {
			for (size_t x = 0; x < 4; ++x) {
				for (size_t y = 0; y < 64; ++y) {
					std::cout << +((*reinterpret_cast<uint64_t*>(&value.getStructuralCharacters()) + x) >> y & 1);
				}
			}
			std::cout << std::endl;
		}
	}

  protected:
	std::vector<SimdStringSection> stringSections{};
	std::vector<JsonTapeRecord> jsonTape{};
	bool haveWeStarted{ false };
	std::string finalString{};
};

int32_t main() noexcept {
	std::string string64{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
	std::string string256{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
						   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
						   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
						   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
	std::string stringNew{
		"{\"d\":{\"activities\":[{\"created_at\":\"1668496069331\",\"emoji\":{\"name\":\" ≡ ƒÑ╖\"},\"id\":\"custom\",\"name\":\"testing\"}]}}"
	};
	::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
	size_t totalTime{};
	size_t totalSize{};
	SimdStringScanner stringScanner{ stringNew };
	stringScanner.generateTapeRecord();
	stopWatch.resetTimer();
	for (size_t x = 0; x < 256 * 16384 / 4; ++x) {
		SimdStringSection simd8Test{ string256 };
		stringScanner.generateTapeRecord();
		totalSize += string256.size();
	}
	totalTime += stopWatch.totalTimePassed();

	std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;

	totalSize = 0;
	totalTime = 0;
	stopWatch.resetTimer();
	for (size_t x = 0; x < 256 * 16384; ++x) {
		//SimdBase64 simd8Test{ string64 };
		totalSize += string64.size();
	}
	totalTime += stopWatch.totalTimePassed();
	std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;



	return 0;
};