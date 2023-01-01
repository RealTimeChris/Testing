#pragma once

#include <immintrin.h>

namespace Jsonifier {

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

		inline operator __m128i() {
			return this->value;
		}

	  protected:
		__m128i value{};
	};

	class SimdBase256 {
	  public:
		inline SimdBase256() noexcept {};

		inline explicit operator bool() {
			for (size_t x = 0; x < 4; ++x) {
				if (this->getUint64(x) != 0) {
					return true;
				}
			}
			return false;
		}

		void fromUint64(uint64_t data) {
			this->value = _mm256_set1_epi64x(static_cast<int64_t>(data));
		}

		explicit inline SimdBase256(int64_t other) {
			this->fromUint64(other);
		}

		inline SimdBase256& operator=(char other) {
			this->value = _mm256_set1_epi8(other);
			return *this;
		}

		inline SimdBase256(char other) {
			*this = other;
		}

		inline SimdBase256& operator=(const char* values) {
			*this = _mm256_loadu_epi8(values);
			return *this;
		}

		explicit inline SimdBase256(const char* values) {
			*this = values;
		}

		explicit inline SimdBase256(int64_t value00, int64_t value01, int64_t value02, int64_t value03) {
			this->value = _mm256_set_epi64x(value03, value02, value01, value00);
		}

		explicit inline SimdBase256(uint64_t value00, uint64_t value01, uint64_t value02, uint64_t value03) {
			this->value = _mm256_set_epi64x(static_cast<int64_t>(value03), static_cast<int64_t>(value02), static_cast<int64_t>(value01),
				static_cast<int64_t>(value00));
		}

		inline SimdBase256& operator=(__m256i other) {
			this->value = other;
			return *this;
		}

		inline SimdBase256(__m256i other) {
			*this = other;
		}

		inline void store(uint8_t dst[32]) const {
			return _mm256_storeu_epi8(dst, this->value);
		}

		inline uint64_t getUint64(size_t index) {
			switch (index) {
				case 0: {
					return static_cast<size_t>(_mm256_extract_epi64(this->value, 0));
				}
				case 1: {
					return static_cast<size_t>(_mm256_extract_epi64(this->value, 1));
				}
				case 2: {
					return static_cast<size_t>(_mm256_extract_epi64(this->value, 2));
				}
				case 3: {
					return static_cast<size_t>(_mm256_extract_epi64(this->value, 3));
				}
				default: {
					return static_cast<size_t>(_mm256_extract_epi64(this->value, 0));
				}
			}
		}

		inline int64_t getInt64(size_t index) {
			switch (index) {
				case 0: {
					return _mm256_extract_epi64(this->value, 0);
				}
				case 1: {
					return _mm256_extract_epi64(this->value, 1);
				}
				case 2: {
					return _mm256_extract_epi64(this->value, 2);
				}
				case 3: {
					return _mm256_extract_epi64(this->value, 3);
				}
				default: {
					return _mm256_extract_epi64(this->value, 0);
				}
			}
		}

		inline void insertInt64(int64_t value, size_t index) {
			switch (index) {
				case 0: {
					this->value = _mm256_insert_epi64(this->value, value, 0);
					break;
				}
				case 1: {
					this->value = _mm256_insert_epi64(this->value, value, 1);
					break;
				}
				case 2: {
					this->value = _mm256_insert_epi64(this->value, value, 2);
					break;
				}
				case 3: {
					this->value = _mm256_insert_epi64(this->value, value, 3);
					break;
				}
				default: {
					this->value = _mm256_insert_epi64(this->value, value, 0);
					break;
				}
			}
		}

		inline operator __m256i&() {
			return this->value;
		}

		inline SimdBase256 operator|(SimdBase256& other) {
			return _mm256_or_si256(this->value, other);
		}

		inline SimdBase256 operator&(SimdBase256& other) {
			return _mm256_and_si256(this->value, other);
		}

		inline SimdBase256 operator^(SimdBase256& other) {
			return _mm256_xor_si256(this->value, other);
		}

		inline SimdBase256 operator+(SimdBase256& other) {
			return _mm256_add_epi8(this->value, other);
		}

		inline SimdBase256& operator|=(SimdBase256& other) {
			*this = *this | other;
			return *this;
		}

		inline SimdBase256& operator&=(SimdBase256& other) {
			*this = *this & other;
			return *this;
		}

		inline SimdBase256& operator^=(SimdBase256& other) {
			*this = *this ^ other;
			return *this;
		}

		inline SimdBase256 operator==(SimdBase256& other) {
			return _mm256_cmpeq_epi8(this->value, other);
		}

		inline SimdBase256 operator|(SimdBase256&& other) {
			return _mm256_or_si256(this->value, other);
		}

		inline SimdBase256 operator&(SimdBase256&& other) {
			return _mm256_and_si256(this->value, other);
		}

		inline SimdBase256 operator^(SimdBase256&& other) {
			return _mm256_xor_si256(this->value, other);
		}

		inline SimdBase256 operator+(SimdBase256&& other) {
			return _mm256_add_epi8(this->value, other);
		}

		inline SimdBase256& operator|=(SimdBase256&& other) {
			*this = *this | other;
			return *this;
		}

		inline SimdBase256& operator&=(SimdBase256&& other) {
			*this = *this & other;
			return *this;
		}

		inline SimdBase256& operator^=(SimdBase256&& other) {
			*this = *this ^ other;
			return *this;
		}

		inline SimdBase256 operator==(SimdBase256&& other) {
			return _mm256_cmpeq_epi8(this->value, other);
		}

		inline SimdBase256 operator==(uint8_t other) {
			return _mm256_cmpeq_epi8(this->value, _mm256_set1_epi8(other));
		}

		template<size_t amount> inline SimdBase256 shl() {
			SimdBase256 returnValueReal{};
			SimdBase256 returnValue{};
			returnValue = _mm256_slli_epi64(*this, (amount % 64));
			returnValueReal |= returnValue;
			returnValue = _mm256_permute4x64_epi64(*this, 0b10010011);
			returnValue = _mm256_srli_epi64(returnValue, 64 - amount);
			returnValueReal |= returnValue;
			returnValue = _mm256_set_epi64x(0, 0, 0, (1ll << amount) - (1ll << 0));
			returnValueReal &= ~returnValue;
			return returnValueReal;
		}

		template<size_t amount> inline SimdBase256 shr() {
			SimdBase256 returnValueReal{};
			SimdBase256 returnValue{};
			returnValue = _mm256_srli_epi64(*this, (amount % 64));
			returnValueReal |= returnValue;
			returnValue = _mm256_permute4x64_epi64(*this, 0b00111001);
			returnValue = _mm256_slli_epi64(returnValue, 64 - amount);
			returnValueReal |= returnValue;
			returnValue = _mm256_set_epi64x(0, 0, 0, (1ull << 64 - amount) - (1ull << 0));
			returnValueReal &= returnValue;
			return returnValueReal;
		}

		inline SimdBase256 operator~() {
			SimdBase256 newValues{};
			newValues = _mm256_xor_si256(*this, _mm256_set1_epi64x(-1ll));
			return newValues;
		}

		inline SimdBase256 carrylessMultiplication(int64_t& prevInString) {
			SimdBase128 allOnes{ '\xFF' };
			auto inString00 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(0)), allOnes, 0)) ^ prevInString;
			prevInString = static_cast<uint64_t>(inString00 >> 63);
			auto inString01 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(1)), allOnes, 0)) ^ prevInString;
			prevInString = static_cast<uint64_t>(inString01 >> 63);
			auto inString02 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(2)), allOnes, 0)) ^ prevInString;
			prevInString = static_cast<uint64_t>(inString02 >> 63);
			auto inString03 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(3)), allOnes, 0)) ^ prevInString;
			prevInString = static_cast<uint64_t>(inString03 >> 63);
			return SimdBase256{ inString00, inString01, inString02, inString03 };
		}

		inline bool collectCarries(SimdBase256 other1, SimdBase256* result) {
			bool returnValue{};
			uint64_t returnValue64{};
			for (size_t x = 0; x < 4; ++x) {
				if (_addcarry_u64(0, this->getUint64(x), other1.getUint64(x), &returnValue64)) {
					returnValue = true;
				}
				result->insertInt64(returnValue64, x);
			}
			return returnValue;
		}

		inline void printBits(uint64_t values, const std::string& valuesTitle) {
			using std::cout;
			cout << valuesTitle;
			cout << std::bitset<64>{ values };
			cout << std::endl;
		}

		inline void printBits(const std::string& valuesTitle) {
			using std::cout;
			cout << valuesTitle;
			for (size_t x = 0; x < 32; ++x) {
				for (size_t y = 0; y < 8; ++y) {
					cout << std::bitset<1>{ static_cast<size_t>(*(reinterpret_cast<int8_t*>(&this->value) + x)) >> y };
				}
			}
			cout << std::endl;
		}

		inline SimdBase256 bitAndNot(SimdBase256 other) {
			return _mm256_andnot_si256(other, this->value);
		}

		inline SimdBase256 shuffle(SimdBase256 other) {
			return _mm256_shuffle_epi8(other, this->value);
		}

	  protected:
		__m256i value{};
	};

	inline SimdBase256 convertSimdBytesToBits(SimdBase256 input00[8]) {
		SimdBase256 returnValue{};
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[0]), 0);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[1]), 1);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[2]), 2);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[3]), 3);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[4]), 4);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[5]), 5);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[6]), 6);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[7]), 7);
		return returnValue;
	}

	template<size_t StepSize> struct StringBlockReader {
	  public:
		inline StringBlockReader(const uint8_t* _buf, size_t _len);
		inline size_t getRemainder(char* dst) const;
		inline const char* fullBlock() const;
		inline bool hasFullBlock() const;
		inline size_t blockIndex();
		inline void advance();

	  private:
		const uint8_t* stringBuffer{};
		const size_t length{};
		const size_t lengthMinusStep{};
		size_t index{};
	};

	template<size_t StepSize>
	inline StringBlockReader<StepSize>::StringBlockReader(const uint8_t* _buf, size_t _len)
		: stringBuffer{ _buf }, length{ _len }, lengthMinusStep{ length < StepSize ? 0 : length - StepSize }, index{ 0 } {
	}

	template<size_t StepSize> inline size_t StringBlockReader<StepSize>::blockIndex() {
		return index;
	}

	template<size_t StepSize> inline bool StringBlockReader<StepSize>::hasFullBlock() const {
		return index < lengthMinusStep;
	}

	template<size_t StepSize> inline const char* StringBlockReader<StepSize>::fullBlock() const {
		return reinterpret_cast<const char*>(&stringBuffer[index]);
	}

	template<size_t StepSize> inline size_t StringBlockReader<StepSize>::getRemainder(char* dst) const {
		if (length == index) {
			return 0;
		}
		std::memset(dst, 0x20, StepSize);
		std::memcpy(dst, stringBuffer + index, length - index);
		return length - index;
	}

	template<size_t StepSize> inline void StringBlockReader<StepSize>::advance() {
		index += StepSize;
	}

	class SimdStringSection {
	  public:
		inline SimdStringSection() noexcept = default;

		inline void packStringIntoValue(SimdBase256* theValue, const char string[32]) {
			*theValue = string;
		}

		inline uint64_t addTapeValues(uint32_t* tapePtrs, uint64_t* theBits, size_t currentIndexNew, size_t& currentIndexIntoTape,
			size_t stringLength) {
			int cnt = static_cast<int>(__popcnt64(*theBits));
			int64_t newValue{};
			for (int i = 0; i < cnt; i++) {
				newValue = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString;

				if (newValue > stringLength) {
					currentIndexIntoTape += i;
					return i;

				} else {
					tapePtrs[i + currentIndexIntoTape] = newValue;
					*theBits = _blsr_u64(*theBits);
				}
			}
			currentIndexIntoTape += cnt;
			return cnt;
		}

		inline SimdBase256 follows(SimdBase256 match, SimdBase256 overflow) {
			SimdBase256 result = match.shl<1>() | overflow;
			overflow = match.shr<63>();
			return result;
		}

		inline size_t getStructuralIndices(uint32_t* currentPtr, size_t& currentIndexIntoTape, size_t stringLength) {
			size_t returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				auto newValue = this->S256.getUint64(x);
				returnValue += this->addTapeValues(currentPtr, &newValue, x, currentIndexIntoTape, stringLength);
			}
			this->currentIndexIntoString += 256;
			return returnValue;
		}

		inline SimdBase256 collectWhiteSpace() {
			char valuesNew[32]{ ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2,
				100, '\t', '\n', 112, 100, '\r', 100, 100 };
			SimdBase256 whitespaceTable{ valuesNew };
			SimdBase256 whiteSpaceReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				whiteSpaceReal[x] = this->values[x].shuffle(whitespaceTable) == this->values[x];
			}
			return convertSimdBytesToBits(whiteSpaceReal);
		}

		inline SimdBase256 collectStructuralCharacters() {
			char newValues[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
			SimdBase256 opTable{ newValues };
			SimdBase256 structural[8]{};
			for (size_t x = 0; x < 8; ++x) {
				auto valuesNew00 = this->values[x] | char{ 0x20 };
				structural[x] = this->values[x].shuffle(opTable) == valuesNew00;
			}

			return convertSimdBytesToBits(structural);
		}

		inline SimdBase256 collectQuotedRange() {
			SimdBase256 backslashes = _mm256_set1_epi8('\\');
			SimdBase256 backslashesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				backslashesReal[x] = this->values[x] == backslashes;
			}

			auto B256 = convertSimdBytesToBits(backslashesReal);

			SimdBase256 E{ _mm256_set1_epi8(0b01010101) };
			SimdBase256 O{ _mm256_set1_epi8(0b10101010) };

			auto S = B256 & ~(B256.shl<1>());
			auto ES = S & E;
			SimdBase256 EC{};
			B256.collectCarries(ES, &EC);
			auto ECE = EC.bitAndNot(B256);
			auto OD1 = ECE & ~E;
			auto OS = S & O;
			auto OC = B256 + OS;
			auto OCE = OC & ~B256;
			auto OD2 = OCE & E;
			auto OD = OD1 | OD2;
			auto R = Q256.bitAndNot(OD);
			return R.carrylessMultiplication(this->prevInString);
		}

		inline SimdBase256 collectQuotes() {
			SimdBase256 quotes = _mm256_set1_epi8('"');
			SimdBase256 quotesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				quotesReal[x] = (this->values[x] == quotes);
			}

			return convertSimdBytesToBits(quotesReal);
		}

		inline SimdBase256 collectFinalStructurals() {
			auto nonquoteScalar = ~(this->S256 | this->W256).bitAndNot(this->Q256);
			this->followsPotentialNonquoteScalar = follows(nonquoteScalar, this->prevInScalar);
			auto string_tail = this->R256 ^ this->Q256;
			auto scalar = ~(this->S256 | this->W256);
			auto potential_scalar_start = scalar & ~this->followsPotentialNonquoteScalar;
			auto op = this->S256;
			auto potential_structural_start = op | potential_scalar_start;
			auto structuralStart = potential_structural_start & ~string_tail;
			return structuralStart;
		}

		void submitDataForProcessing(const char* valueNew) {
			this->packStringIntoValue(&this->values[0], valueNew);
			this->packStringIntoValue(&this->values[1], valueNew + 32);
			this->packStringIntoValue(&this->values[2], valueNew + 64);
			this->packStringIntoValue(&this->values[3], valueNew + 96);
			this->packStringIntoValue(&this->values[4], valueNew + 128);
			this->packStringIntoValue(&this->values[5], valueNew + 160);
			this->packStringIntoValue(&this->values[6], valueNew + 192);
			this->packStringIntoValue(&this->values[7], valueNew + 224);
			this->Q256 = this->collectQuotes();
			this->R256 = this->collectQuotedRange();
			this->W256 = this->collectWhiteSpace();
			this->S256 = this->collectStructuralCharacters();
			this->S256 = this->collectFinalStructurals();
			//this->S256.printBits("FINAL BITS: ");
		}

	  protected:
		SimdBase256 followsPotentialNonquoteScalar{};
		size_t currentIndexIntoString{};
		SimdBase256 prevInScalar{};
		SimdBase256 values[8]{};
		int64_t prevInString{};
		SimdBase256 Q256{};
		SimdBase256 W256{};
		SimdBase256 R256{};
		SimdBase256 S256{};
	};
}