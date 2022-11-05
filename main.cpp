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

template<typename OTy> struct SimdBase {
	__m256i value{};
	inline SimdBase() : value{ __m256i() } {}

	inline SimdBase(const __m256i _value) : value(_value) {
	}

	inline operator const __m256i&() const {
		return this->value;
	}

	inline operator __m256i&() {
		return this->value;
	}

	inline OTy operator|(const OTy other) const {
		return _mm256_or_si256(*this, other);
	}
	inline OTy operator&(const OTy other) const {
		return _mm256_and_si256(*this, other);
	}
	inline OTy operator^(const OTy other) const {
		return _mm256_xor_si256(*this, other);
	}
	inline OTy bit_andnot(const OTy other) const {
		return _mm256_andnot_si256(other, *this);
	}
	inline OTy& operator|=(const OTy other) {
		auto thisCasted = static_cast<OTy*>(this);
		*thisCasted = *thisCasted | other;
		return *thisCasted;
	}
	inline OTy& operator&=(const OTy other) {
		auto thisCasted = static_cast<OTy*>(this);
		*thisCasted = *thisCasted & other;
		return *thisCasted;
	}
	inline OTy& operator^=(const OTy other) {
		auto thisCasted = static_cast<OTy*>(this);
		*thisCasted = *thisCasted ^ other;
		return *thisCasted;
	}
};
void printValueAsString(__m256i in,std::string values) {
	alignas(16) uint8_t v[32];
	_mm256_store_si256(( __m256i* )v, in);
	printf(std::string{ values.c_str() + std::string{ " v32_u8: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n" } }.c_str(), v[0], v[1], v[2], v[3], v[4],
		v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[24], v[25], v[26], v[27], v[28], v[29],
		v[30], v[31]);
	printf(std::string{ values.c_str() + std::string{ " (DIGITS) v32_u8: %d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }
			   .c_str(),
		v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[24], v[25],
		v[26], v[27], v[28], v[29], v[30], v[31]);
}

void printValueAsString(uint32_t inA, std::string values) {
	std::cout << "RAW VALUES: " << std::bitset<32>{ inA } << std::endl;
	alignas(32) uint8_t v[32]{};
	for (size_t x = 0; x < 32; ++x) {
		if ((static_cast<uint32_t>(inA >> x) & 0x01) == 1 << 0) {
			v[x] = 1;
		}
	}
	printf(std::string{ values.c_str() + std::string{ " v32_u8: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n" } }.c_str(), v[24], v[25], v[26], v[27],
		v[28], v[29], v[30], v[31], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[0], v[1], v[2], v[3], v[4],
		v[5], v[6], v[7]);
	printf(std::string{ values.c_str() +
			   std::string{ " (DIGITS) v32_u8: "
							"%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }
			   .c_str(),
		v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[0],
		v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
}

void printValueAsString(uint64_t inA,std::string values) {
	std::cout << "RAW VALUES: " << std::bitset<64>{ inA } << std::endl;
	alignas(32) uint8_t v[64]{};
	for (size_t x = 0; x < 64; ++x) {
		if ((static_cast<uint64_t>(inA >> x) & 0x01) == 1 << 0) {
			v[x] = 1;
		}
	}
	printf(std::string{ values.c_str() +
			   std::string{ " v64_u8: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n" } }
			   .c_str(),
		v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[0],
		v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[56], v[57], v[58], v[59], v[60], v[61], v[62], v[63], v[48], v[49], v[50], v[51], v[52], v[53], v[54], v[55], v[40], v[41],
		v[42], v[43], v[44], v[45], v[46], v[47], v[32], v[33], v[34], v[35], v[36], v[37], v[38], v[39]);
	printf(
		std::string{ values.c_str() +
			std::string{ " (DIGITS) v64_u8: "
						 "%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%"
						 "d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }
			.c_str(),
		v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[0],
		v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[56], v[57], v[58], v[59], v[60], v[61], v[62], v[63], v[48], v[49], v[50], v[51], v[52], v[53], v[54], v[55], v[40], v[41],
		v[42], v[43], v[44], v[45], v[46], v[47], v[32], v[33], v[34], v[35], v[36], v[37], v[38], v[39]);
}

uint32_t convertTo32BitUint(__m256i inputA) {
	uint32_t value{};
	for (size_t x = 0; x < 32; ++x) {
		if (static_cast<uint8_t>(inputA.m256i_i8[x]) == 0xff) {
			value |= 1 << static_cast<uint64_t>(x);
		}
	}
	return value;
}

uint64_t convertTo64BitUint(__m256i inputA, __m256i inputB) {
	uint64_t r_lo = uint32_t(_mm256_movemask_epi8(inputA));
	uint64_t r_hi = _mm256_movemask_epi8(inputB);
	return r_lo | (r_hi << 32);
}

__m256i convertToM256(uint32_t inputA) {
	__m256i value{};
	for (size_t x = 0; x < 32; ++x) {
		if ((static_cast<uint32_t>(inputA >> x) & 0x01) == 0x01) {
			value.m256i_i8[x] = 255;
		}
	}
	return value;
}

void collectCarries(uint64_t inputA, uint64_t inputB, uint64_t* outputValue) {
	_addcarry_u64(0, inputA, inputB, reinterpret_cast<unsigned __int64*>(outputValue));
}


class Simd8 {
  public:
	Simd8(std::string& stringNew) {
		this->backslashes[0] = _mm256_set1_epi8('\\');
		this->backslashes[1] = _mm256_set1_epi8('\\');
		this->quotes[0] = _mm256_set1_epi8('"');
		this->quotes[1] = _mm256_set1_epi8('"');
		this->values[0] = _mm256_set_epi64x(*reinterpret_cast<int64_t*>(stringNew.data()), *reinterpret_cast<int64_t*>(stringNew.data() + 8),
			*reinterpret_cast<int64_t*>(stringNew.data() + 16), *reinterpret_cast<int64_t*>(stringNew.data() + 24));
		this->values[1] = _mm256_set_epi64x(*reinterpret_cast<int64_t*>(stringNew.data() + 32), *reinterpret_cast<int64_t*>(stringNew.data() + 40),
			*reinterpret_cast<int64_t*>(stringNew.data() + 48), *reinterpret_cast<int64_t*>(stringNew.data() + 56));
		this->B[0] = _mm256_cmpeq_epi8(this->values[0], this->backslashes[0]);
		this->B[1] = _mm256_cmpeq_epi8(this->values[1], this->backslashes[1]);
		this->O[0] = _mm256_set1_epi16(0xff00);
		this->O[1] = _mm256_set1_epi16(0xff00);
		printValueAsString(this->E[0], "E0 VALUES: ");
		printValueAsString(this->E[1], "E1 VALUES: ");
		this->E[0] = _mm256_set1_epi16(0x00ff);
		this->E[1] = _mm256_set1_epi16(0x00ff);
		printValueAsString(this->O[0], "O0 VALUES: ");
		printValueAsString(this->O[1], "O1 VALUES: ");
		
		this->B64 = convertTo64BitUint(this->B[0], this->B[1]);
		
		printValueAsString(this->B64, "B VALUES: ");
		printValueAsString(static_cast<uint32_t>(this->B64), "B VALUES: ");
		printValueAsString(static_cast<uint32_t>(this->B64 >> 32), "B VALUES: ");
		printValueAsString(static_cast<uint32_t>(this->B64 << 1), "B-SHIFT VALUES: ");
		printValueAsString(static_cast<uint32_t>(this->B64 << 1 >> 32), "B-SHIFT VALUES: ");
		this->S = this->B64 & ~(this->B64 << 1);
		printValueAsString(this->S, "S VALUES: ");
		
		__m256i valuesNew = _mm256_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
		printValueAsString(valuesNew, "TEST VALUES: ");

	}
	operator std::string() {
		return string;
	}

  protected:
	std::string string{};
	__m256i backslashes[2]{};
	__m256i quotes[2]{};
	__m256i values[2]{};
	__m256i B[2]{};
	__m256i E[2]{};
	__m256i O[2]{};
	uint64_t B64{};
	uint64_t BShift{};
	uint64_t S{};
	uint64_t ES{};
	uint64_t EC{};
	uint64_t OD1{};
	uint64_t OS1{};
	uint64_t OC{};
	uint64_t OCE{};
	uint64_t OS{};
	uint64_t ECE{};
	uint64_t OD2{};
	uint64_t OD{};
	uint64_t Q{};

};

int32_t main() noexcept {
	std::string string{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
	Simd8 simd8Test{ string };

	return 0;
}