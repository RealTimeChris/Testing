#include "Build/Release/_deps/jsonifier-src/Include/jsonifier/Jsonifier.hpp"
#include <nlohmann/json.hpp>
#include <scoped_allocator>
#include <source_location>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <simdjson.h>
#include <iostream>
#include <bitset>
#include <immintrin.h>

inline uint64_t convertSimd256To64BitUint(const __m256i& inputA, __m256i inputB) {
	uint64_t r_lo = uint32_t(_mm256_movemask_epi8(inputA));
	uint64_t r_hi = _mm256_movemask_epi8(inputB);
	return r_lo | (r_hi << 32);
}

template<typename ITy> struct SimdBase {
	ITy value{};

	inline SimdBase() noexcept = default;

};

template<>
class SimdBase<__m128i> {
  public:

	inline SimdBase() noexcept = default;

	inline SimdBase<__m128i>& operator=(char other) {
		this->value = _mm_set1_epi8(other);
		return *this;
	}

	inline SimdBase(char other) {
		*this = other;
	}

	inline SimdBase(int64_t value00, int64_t value01) {
		this->value = _mm_insert_epi64(this->value, value00, 0);
		this->value = _mm_insert_epi64(this->value, value01, 1);
	}

	inline SimdBase(uint64_t value00, uint64_t value01) {
		this->value = _mm_insert_epi64(this->value, value00, 0);
		this->value = _mm_insert_epi64(this->value, value01, 1);
	}

	inline SimdBase<__m128i>& operator=(__m128i other) {
		this->value = other;
		return *this;
	}

	inline SimdBase(__m128i other) {
		*this = other;
	}

	inline operator __m128i*() {
		return &this->value;
	}

	inline operator __m128i&() {
		return this->value;
	}

	inline SimdBase<__m128i> operator|(__m128i other) {
		return _mm_or_si128(*this, other);
	}

	inline SimdBase<__m128i> operator&(__m128i other) {
		return _mm_and_si128(*this, other);
	}

	inline SimdBase<__m128i> operator^(__m128i other) {
		return _mm_xor_si128(*this, other);
	}

	inline SimdBase<__m128i> bit_andnot(__m128i other) {
		return _mm_andnot_si128(other, *this);
	}

	inline SimdBase<__m128i> operator+(__m128i other) {
		return _mm_add_epi8(*this, other);
	}

	inline SimdBase<__m128i> operator|=(__m128i other) {
		*this = *this | other;
		return *this;
	}

	inline SimdBase<__m128i> operator&=(__m128i other) {
		*this = *this & other;
		return *this;
	}

	inline SimdBase<__m128i> operator^=(__m128i other) {
		*this = *this ^ other;
		return *this;
	}

	friend inline SimdBase<__m128i> operator==(__m128i lhs, __m128i rhs) {
		return _mm_cmpeq_epi8(lhs, rhs);
	}

	inline SimdBase<__m128i> operator<<(size_t amount) {
		__m128i newValue{};
		for (size_t x = 0; x < 2; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) << (amount % 64));
			if (x > 0) {
				*(reinterpret_cast<int64_t*>(&newValue) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 0x00000001);
			}
		}
		return newValue;
	}

	inline SimdBase<__m128i> operator~() {
		__m128i newValue{};
		for (size_t x = 0; x < 4; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) = ~*(reinterpret_cast<int64_t*>(&this->value) + x);
		}
		return newValue;
	}

	inline SimdBase<__m128i> shuffle(__m128i indices) {
		return _mm_shuffle_epi8(indices, *this);
	}

	inline void printBits(std::string valuesTitle) {
		std::cout << valuesTitle;
		for (size_t x = 0; x < 16; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				std::cout << std::bitset<1>{ static_cast<uint64_t>(*(reinterpret_cast<int8_t*>(&this->value) + x)) >> y };
			}
		}
		std::cout << std::endl;
	};

  protected:
	__m128i value{};

};

template<>
class SimdBase<__m256i> {
  public:
	
	SimdBase() noexcept = default;

	inline SimdBase<__m256i>& operator=(char other) {
		this->value = _mm256_set1_epi8(other);
		return *this;
	}

	inline SimdBase(char other) {
		*this = other;
	}

	inline SimdBase(int64_t value00, int64_t value01, int64_t value02, int64_t value03) {
		this->value = _mm256_insert_epi64(this->value, value00, 0);
		this->value = _mm256_insert_epi64(this->value, value01, 1);
		this->value = _mm256_insert_epi64(this->value, value02, 2);
		this->value = _mm256_insert_epi64(this->value, value03, 3);
	}

	inline SimdBase& operator=(const __m256i& other) {
		this->value = other;
		return *this;
	}

	inline SimdBase(const __m256i& other) {
		*this = other;
	}

	inline operator __m256i*() {
		return &this->value;
	}

	inline operator __m256i&() {
		return this->value;
	}

	inline SimdBase<__m256i> operator|(__m256i other) {
		return _mm256_or_si256(this->value, other);
	}

	inline SimdBase<__m256i> operator&(__m256i other) {
		return _mm256_and_si256(this->value, other);
	}

	inline SimdBase<__m256i> operator^(__m256i other) {
		return _mm256_xor_si256(this->value, other);
	}

	inline SimdBase<__m256i> bit_andnot(__m256i other) {
		return _mm256_andnot_si256(other, this->value);
	}

	inline SimdBase<__m256i> operator+(__m256i other) {
		return _mm256_add_epi8(this->value, other);
	}

	inline SimdBase<__m256i> operator|=(__m256i other) {
		*this = *this | other;
		return *this;
	}

	inline SimdBase<__m256i> operator&=(__m256i other) {
		*this = *this & other;
		return *this;
	}

	inline SimdBase<__m256i> operator^=(__m256i other) {
		*this = *this ^ other;
		return *this;
	}

	inline SimdBase<__m256i> operator==(SimdBase<__m256i> rhs) {
		return _mm256_cmpeq_epi8(this->value, rhs.value);
	}

	inline SimdBase<__m256i> operator<<(size_t amount) {
		__m256i newValue{}; 
		for (size_t x = 0; x < 4; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) << (amount % 64));
			if (x > 0) {
				*(reinterpret_cast<int64_t*>(&newValue) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 0x00000001);
			}
		}
		return newValue;
	}

	inline SimdBase<__m256i> operator~() {
		__m256i newValue{};
		for (size_t x = 0; x < 4; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) = ~*(reinterpret_cast<int64_t*>(&this->value) + x);
		}
		return newValue;
	}

	inline SimdBase<__m256i> carrylessMultiplication(char operand) {
		return SimdBase<__m256i>{ _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<int64_t*>(&this->value) + 0)), SimdBase<__m128i>{ operand }, 0)),
			_mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<int64_t*>(&this->value) + 1)), SimdBase<__m128i>{ operand }, 0)),
			_mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<int64_t*>(&this->value) + 2)), SimdBase<__m128i>{ operand }, 0)),
			_mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<int64_t*>(&this->value) + 3)), SimdBase<__m128i>{ operand }, 0)) };
	}

	inline SimdBase<__m256i> collectCarries(__m256i inputB) {
		SimdBase<__m256i> returnValue{};
		for (size_t x = 0; x < 4; ++x) {
			uint64_t returnValue64{};
			_addcarry_u64(0, *(reinterpret_cast<int64_t*>(&inputB) + x), *(reinterpret_cast<int64_t*>(&this->value) + x), reinterpret_cast<unsigned long long*>(&returnValue64));
			*(reinterpret_cast<int64_t*>(&returnValue) + x) = returnValue64;
		}
		return returnValue;
	}

	inline SimdBase<__m256i> shuffle(__m256i indices) {
		return _mm256_shuffle_epi8(indices, *this);
	}

	void printBits(const std::string& valuesTitle) {
		std::cout << valuesTitle;
		for (size_t x = 0; x < 32; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				std::cout << std::bitset<1>{ static_cast<uint64_t>(*(reinterpret_cast<int8_t*>(&this->value) + x)) >> y };
			}
		}
		std::cout << std::endl;
	};

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

	inline SimdStringSection(const std::string& valueNew) {
		this->string = valueNew;
		this->backslashes = '\\';
		this->quotes = '"';
		packStringIntoValue(this->values[0], this->string.data());
		packStringIntoValue(this->values[1], this->string.data() + 32);
		packStringIntoValue(this->values[2], this->string.data() + 64);
		packStringIntoValue(this->values[3], this->string.data() + 96);
		packStringIntoValue(this->values[4], this->string.data() + 128);
		packStringIntoValue(this->values[5], this->string.data() + 160);
		packStringIntoValue(this->values[6], this->string.data() + 192);
		packStringIntoValue(this->values[7], this->string.data() + 224);
		
		this->Q[0] = this->values[0] == this->quotes;
		this->Q[1] = this->values[1] == this->quotes;
		this->Q[2] = this->values[2] == this->quotes;
		this->Q[3] = this->values[3] == this->quotes;
		this->Q[4] = this->values[4] == this->quotes;
		this->Q[5] = this->values[5] == this->quotes;
		this->Q[6] = this->values[6] == this->quotes;
		this->Q[7] = this->values[7] == this->quotes;

		this->B[0] = this->values[0] == this->backslashes;
		this->B[1] = this->values[1] == this->backslashes;
		this->B[2] = this->values[2] == this->backslashes;
		this->B[3] = this->values[3] == this->backslashes;
		this->B[4] = this->values[4] == this->backslashes;
		this->B[5] = this->values[5] == this->backslashes;
		this->B[6] = this->values[6] == this->backslashes;
		this->B[7] = this->values[7] == this->backslashes;

		this->B256 = SimdBase<__m256i>{ convertSimd256To64BitUint(this->B[0], this->B[1]), convertSimd256To64BitUint(this->B[2], this->B[3]),
			convertSimd256To64BitUint(this->B[4], this->B[5]),
			convertSimd256To64BitUint(this->B[6], this->B[7]) };
		this->S = this->B256.bit_andnot(this->B256 << 1);
		this->ES = this->E & this->S;
		this->EC = this->ES.collectCarries(this->B256);
		this->ECE = this->EC & ~this->B256;		
		this->OD1 = this->ECE & ~this->E;
		this->OS = this->S & this->O;
		this->OC = this->B256 + this->OS;
		this->OCE = this->OC & ~this->B256;
		this->OD2 = this->OCE & this->E;
		this->OD = this->OD1 | this->OD2;
		
		this->Q256 = SimdBase<__m256i>{ convertSimd256To64BitUint(this->Q[0], this->Q[1]), convertSimd256To64BitUint(this->Q[2], this->Q[3]),
			convertSimd256To64BitUint(this->Q[4], this->Q[5]), convertSimd256To64BitUint(this->Q[6], this->Q[7]) };
		this->Q256 = this->Q256 & ~this->OD;
		this->R256 = this->Q256;
		this->R256 = this->R256.carrylessMultiplication('\xFF');
		auto whiteSpace00 = this->values[0].shuffle(this->whitespaceTable) == this->values[0];
		auto whiteSpace01 = this->values[1].shuffle(this->whitespaceTable) == this->values[1];
		auto whiteSpace02 = this->values[2].shuffle(this->whitespaceTable) == this->values[2];
		auto whiteSpace03 = this->values[3].shuffle(this->whitespaceTable) == this->values[3];
		auto whiteSpace04 = this->values[4].shuffle(this->whitespaceTable) == this->values[4];
		auto whiteSpace05 = this->values[5].shuffle(this->whitespaceTable) == this->values[5];
		auto whiteSpace06 = this->values[6].shuffle(this->whitespaceTable) == this->values[6];
		auto whiteSpace07 = this->values[7].shuffle(this->whitespaceTable) == this->values[7];

		this->W256 = SimdBase<__m256i>{ convertSimd256To64BitUint(whiteSpace00, whiteSpace01), convertSimd256To64BitUint(whiteSpace02, whiteSpace03),
			convertSimd256To64BitUint(whiteSpace04, whiteSpace05), convertSimd256To64BitUint(whiteSpace06, whiteSpace07) };
		auto valuesNew00 = this->values[0] | SimdBase<__m256i>{ 0x20 };
		auto valuesNew01 = this->values[1] | SimdBase<__m256i>{ 0x20 };
		auto valuesNew02 = this->values[2] | SimdBase<__m256i>{ 0x20 };
		auto valuesNew03 = this->values[3] | SimdBase<__m256i>{ 0x20 };
		auto valuesNew04 = this->values[4] | SimdBase<__m256i>{ 0x20 };
		auto valuesNew05 = this->values[5] | SimdBase<__m256i>{ 0x20 };
		auto valuesNew06 = this->values[6] | SimdBase<__m256i>{ 0x20 };
		auto valuesNew07 = this->values[7] | SimdBase<__m256i>{ 0x20 };
		auto structural00 = this->values[0].shuffle(this->opTable) == valuesNew00;
		auto structural01 = this->values[1].shuffle(this->opTable) == valuesNew01;
		auto structural02 = this->values[2].shuffle(this->opTable) == valuesNew02;
		auto structural03 = this->values[3].shuffle(this->opTable) == valuesNew03;
		auto structural04 = this->values[4].shuffle(this->opTable) == valuesNew04;
		auto structural05 = this->values[5].shuffle(this->opTable) == valuesNew05;
		auto structural06 = this->values[6].shuffle(this->opTable) == valuesNew06;
		auto structural07 = this->values[7].shuffle(this->opTable) == valuesNew07;

		this->S256 = SimdBase<__m256i>{ convertSimd256To64BitUint(structural00, structural01), convertSimd256To64BitUint(structural02, structural03),
			convertSimd256To64BitUint(structural04, structural05),
			convertSimd256To64BitUint(structural06, structural07) };
		
		this->S256 = this->S256 & ~this->R256;
		this->S256 = this->S256 | this->Q256;
		this->P256 = this->S256 | this->W256;
		this->P256 = this->P256 << 1;
		this->P256 &= ~this->W256 & ~this->R256;
		
		this->S256 = this->S256 | this->P256;
		this->S256 = this->S256 & ~(this->Q256 & ~this->R256);

		//this->S256.printBits("S FINAL VALUES (256) ");
		//this->W256.printBits("W FINAL VALUES (256) ");
		//this->R256.printBits("R FINAL VALUES (256) ");
		//this->Q256.printBits("Q FINAL VALUES (256): ");
	}

	operator std::string() {
		return this->string;
	}

  protected:
	SimdBase<__m256i> whitespaceTable{ _mm256_setr_epi8(' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2, 100,
		'\t', '\n', 112, 100, '\r', 100, 100) };
	SimdBase<__m256i> opTable{ _mm256_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0) };
	SimdBase<__m256i> E{ _mm256_set1_epi8(0b01010101) };
	SimdBase<__m256i> O{ _mm256_set1_epi8(0b10101010) };
	SimdBase<__m256i> values[8]{};
	SimdBase<__m256i> B[8]{};
	SimdBase<__m256i> Q[8]{};
	SimdBase<__m256i> backslashes{};
	SimdBase<__m256i> quotes{};
	SimdBase<__m256i> Q256{};
	SimdBase<__m256i> R256{};
	SimdBase<__m256i> W256{};
	SimdBase<__m256i> P256{};
	SimdBase<__m256i> B256{};
	SimdBase<__m256i> S256{};
	SimdBase<__m256i> ES{};
	SimdBase<__m256i> EC{};
	SimdBase<__m256i> S{};
	SimdBase<__m256i> OD1{};
	SimdBase<__m256i> OS1{};
	SimdBase<__m256i> OC{};
	SimdBase<__m256i> OCE{};
	SimdBase<__m256i> OS{};
	SimdBase<__m256i> ECE{};
	SimdBase<__m256i> OD2{};
	SimdBase<__m256i> OD{};
	std::string string{};
};

class StringScanner {
  public:
	StringScanner(const std::string&) noexcept;
  protected:
	std::vector<SimdStringSection> stringSections{};

};

class Simd64Base {
  public:

	inline operator uint64_t() {
		return convertSimd256To64BitUint(this->values[1], this->values[0]);
	}

	void packStringIntoValue(__m256i& theValue, const char* string) {
		for (size_t x = 0; x < 32; ++x) {
			*(reinterpret_cast<int8_t*>(&theValue) + x) = string[x];
		}
	}

	void printBits(uint64_t inA, std::string values) {
		alignas(32) uint8_t v[64]{};
		for (size_t x = 0; x < 64; ++x) {
			if ((static_cast<uint64_t>(inA >> x) & 0x01) == static_cast<uint64_t>(1 << 0)) {
				v[x] = 1;
			}
		}
		printf(std::string{ values.c_str() +
				   std::string{ " (DIGITS) v64_u8: "
								"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%"
								"d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n" } }
				   .c_str(),
			v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[24],
			v[25], v[26], v[27], v[28], v[29], v[30], v[31], v[32], v[33], v[34], v[35], v[36], v[37], v[38], v[39], v[40], v[41], v[42], v[43], v[44], v[45], v[46], v[47], v[48],
			v[49], v[50], v[51], v[52], v[53], v[54], v[55], v[56], v[57], v[58], v[59], v[60], v[61], v[62], v[63]);
	}

	uint64_t collectCarries(uint64_t inputA, uint64_t inputB) {
		uint64_t returnValue{};
		_addcarry_u64(0, inputB, inputA, reinterpret_cast<unsigned long long*>(&returnValue));
		return returnValue;
	}

	void printBits(std::string valuesTitle) {
		std::cout << valuesTitle;
		for (size_t x = 0; x < 32; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				//std::cout << std::bitset<1>{ static_cast<uint64_t>(static_cast<__m256i>(values[0]).m256i_i8[x]) >> y };
			}
		}
		for (size_t x = 0; x < 32; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				//std::cout << std::bitset<1>{ static_cast<uint64_t>(static_cast<__m256i>(values[1]).m256i_i8[x]) >> y };
			}
		}
		std::cout << std::endl;
	};

	inline Simd64Base(const __m256i&value01, __m256i value02) {
		this->values[0] = value01;
		this->values[1] = value02;
	}

	inline Simd64Base(std::string& stringNewer) {
		this->string = stringNewer;
		this->backslashes = _mm256_set1_epi8('\\');
		this->quotes = _mm256_set1_epi8('"');
		packStringIntoValue(this->values[0], stringNewer.data());
		packStringIntoValue(this->values[1], stringNewer.data() + 32);
		this->S64 = this->B64 & ~(this->B64 << 1);
		this->ES = this->S & this->E;
		this->EC = this->B64 + this->ES;
		this->ECE = this->EC & ~this->B64;
		this->OD1 = this->ECE & ~this->E;
		this->OS = this->S & this->O;
		this->OC = this->B64 + this->OS;
		this->OC = this->B64+ this->OS;
		this->OCE = this->OC = this->B64 + this->OS;
		this->OC = this->B64 + this->OS;
		this->OC = this->B64 + this->OS;
		this->OCE = this->B64 + this->OS;
		this->OC = this->B64 + this->OS;
		this->OC = this->B64 + this->OS;
		this->OC = this->B64 + this->OS;
		this->B[0] = _mm256_cmpeq_epi8(this->values[0], this->backslashes);
		this->B[1] = _mm256_cmpeq_epi8(this->values[1], this->backslashes);
		this->B64 = convertSimd256To64BitUint(this->B[0], this->B[1]);
		this->S = this->B64 & ~(this->B64 << 1);
		this->ES = this->S & this->E;
		this->EC = collectCarries(this->ES, this->B64);
		this->ECE = this->EC & ~this->B64;
		this->OD1 = this->ECE & ~this->E;
		this->OS = this->S & this->O;
		this->OC = this->B64 + this->OS;
		this->OCE = this->OC & ~this->B64;
		this->OD2 = this->OCE & this->E;
		this->OD = this->OD1 | this->OD2;
		this->Q[0] = _mm256_cmpeq_epi8(this->values[0], this->quotes);
		this->Q[1] = _mm256_cmpeq_epi8(this->values[1], this->quotes);
		this->Q64 = convertSimd256To64BitUint(this->Q[0], this->Q[1]);
		this->Q64 = this->Q64 & ~this->OD;
		this->R64 = this->Q64;
		this->R64 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->R64), _mm_set1_epi8('\xFF'), 0));
		auto whiteSpace00 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->whitespaceTable, this->values[0]), this->values[0]);
		auto whiteSpace01 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->whitespaceTable, this->values[1]), this->values[1]);
		this->W64 = convertSimd256To64BitUint(whiteSpace00, whiteSpace01);
		auto valuesNew00 = _mm256_or_si256(this->values[0], _mm256_set1_epi8(0x20));
		auto valuesNew01 = _mm256_or_si256(this->values[1], _mm256_set1_epi8(0x20));
		auto structural00 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->opTable, this->values[0]), valuesNew00);
		auto structural01 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->opTable, this->values[1]), valuesNew01);
		this->S64 = convertSimd256To64BitUint(structural00, structural01);
		this->S64 = this->S64 & ~this->R64;
		this->S64 = this->S64 | this->Q64;
		this->P64 = this->S64 | this->W64;
		this->P64 = this->P64 << 1;
		this->P64 &= ~this->W64 & ~this->R64;
		this->S64 = this->S64 | this->P64;
		this->S64 = this->S64 & ~(this->Q64 & ~this->R64);
		//printBits(this->Q64, "Q FINAL VALUES: ");
		//printBits(this->R64, "R FINAL VALUES: ");
		//printBits(this->S64, "S FINAL VALUES: ");
		//printBits(this->W64, "W FINAL VALUES: ");
	}

	operator std::string() {
		return string;
	}

  protected:
	__m256i values[2]{};
	std::string string{};
	__m256i backslashes{};
	__m256i whitespaceTable{ _mm256_setr_epi8(' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n',
		112, 100, '\r', 100, 100) };
	__m256i opTable{ _mm256_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0) };
	__m256i quotes{};
	__m256i B[2]{};
	__m256i Q[2]{};
	uint64_t E{ 0b0101010101010101010101010101010101010101010101010101010101010101 };
	uint64_t O{ 0b1010101010101010101010101010101010101010101010101010101010101010 };
	uint64_t B64{};
	uint64_t ES{};
	uint64_t EC{};
	uint64_t S{};
	uint64_t P64{};
	uint64_t OD1{};
	uint64_t OS1{};
	uint64_t OC{};
	uint64_t OCE{};
	uint64_t OS{};
	uint64_t ECE{};
	uint64_t OD2{};
	uint64_t OD{};
	uint64_t Q64{};
	uint64_t R64{};
	uint64_t S64{};
	uint64_t W64{};
};

int32_t main() noexcept {
	std::string string64{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
	std::string string256{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
						   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
						   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
						   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
	Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
	size_t totalTime{};
	size_t totalSize{};

	stopWatch.resetTimer();
	for (size_t x = 0; x < 256 * 16384 / 4; ++x) {
		SimdStringSection simd8Test{ string256 };
		totalSize += string256.size();
	}
	totalTime += stopWatch.totalTimePassed();
	
	std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;

	totalSize = 0;
	totalTime = 0;
	stopWatch.resetTimer();
	for (size_t x = 0; x < 256 * 16384; ++x) {
		Simd64Base simd8Test{ string64 };
		totalSize += string64.size();
	}
	totalTime += stopWatch.totalTimePassed();
	std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
	
	
	
	return 0;
};