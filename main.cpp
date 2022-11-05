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
	inline OTy bitAndNot(const OTy other) const {
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
	printf(std::string{ values.c_str() + std::string{ " v32_u8: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n" } }.c_str(), in.m256i_i8[31],
		in.m256i_i8[30], in.m256i_i8[29], in.m256i_i8[28], in.m256i_i8[27], in.m256i_i8[26], in.m256i_i8[25], in.m256i_i8[24], in.m256i_i8[23], in.m256i_i8[22], in.m256i_i8[21],
		in.m256i_i8[20], in.m256i_i8[19], in.m256i_i8[18], in.m256i_i8[17], in.m256i_i8[16], in.m256i_i8[15], in.m256i_i8[14], in.m256i_i8[13], in.m256i_i8[12], in.m256i_i8[11],
		in.m256i_i8[10], in.m256i_i8[9], in.m256i_i8[8], in.m256i_i8[7], in.m256i_i8[6], in.m256i_i8[5], in.m256i_i8[4], in.m256i_i8[3], in.m256i_i8[2], in.m256i_i8[1],
		in.m256i_i8[0]);
	printf(
		std::string{ values.c_str() + std::string{ " (DIGITS) v32_u8: %d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }.c_str(),
		in.m256i_i8[31], in.m256i_i8[30], in.m256i_i8[29], in.m256i_i8[28], in.m256i_i8[27], in.m256i_i8[26], in.m256i_i8[25], in.m256i_i8[24], in.m256i_i8[23], in.m256i_i8[22],
		in.m256i_i8[21], in.m256i_i8[20], in.m256i_i8[19], in.m256i_i8[18], in.m256i_i8[17], in.m256i_i8[16], in.m256i_i8[15], in.m256i_i8[14], in.m256i_i8[13], in.m256i_i8[12],
		in.m256i_i8[11], in.m256i_i8[10], in.m256i_i8[9], in.m256i_i8[8], in.m256i_i8[7], in.m256i_i8[6], in.m256i_i8[5], in.m256i_i8[4], in.m256i_i8[3], in.m256i_i8[2],
		in.m256i_i8[1], in.m256i_i8[0]);
}

void printValueAsString(uint64_t inA, std::string values) {
	alignas(32) uint8_t v[64]{};
	for (size_t x = 0; x < 64; ++x) {
		if ((static_cast<uint64_t>(inA >> x) & 0x01) == 1 << 0) {
			v[x] = 1;
		}
	}
	printf(std::string{ values.c_str() + std::string{ " v64_u8: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n" } }.c_str(), v[63], v[62], v[61], v[60],
		v[59], v[58], v[57], v[56], v[55], v[54], v[53], v[52], v[51], v[50], v[49], v[48], v[47], v[46], v[45], v[44], v[43], v[42], v[41], v[40], v[39], v[38], v[37], v[36],
		v[35], v[34], v[33], v[32], v[31], v[30], v[29], v[28], v[27], v[26], v[25], v[24], v[23], v[22], v[21], v[20], v[19], v[18], v[17], v[16], v[15], v[14], v[13], v[12],
		v[11], v[10], v[9], v[8], v[7], v[6], v[5], v[4], v[3], v[2], v[1], v[0]);
	printf(std::string{ values.c_str() +
			   std::string{ " (DIGITS) v64_u8: "
							"%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }
			   .c_str(),
		v[63], v[62], v[61], v[60], v[59], v[58], v[57], v[56], v[55], v[54], v[53], v[52], v[51], v[50], v[49], v[48], v[47], v[46], v[45], v[44], v[43], v[42], v[41], v[40], v[39],
		v[38], v[37], v[36], v[35], v[34], v[33], v[32] ,v[31], v[30], v[29], v[28], v[27], v[26], v[25], v[24], v[23], v[22], v[21], v[20], v[19], v[18], v[17], v[16], v[15], v[14],
		v[13], v[12], v[11], v[10], v[9], v[8], v[7],
		v[6], v[5], v[4], v[3], v[2], v[1], v[0]);
}

void printValueAsString(uint32_t inA, std::string values) {
	alignas(32) uint8_t v[32]{};
	for (size_t x = 0; x < 32; ++x) {
		if ((static_cast<uint32_t>(inA >> x) & 0x01) == 1 << 0) {
			v[x] = 1;
		}
	}
	printf(std::string{ values.c_str() + std::string{ " v32_u8: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n" } }.c_str(), v[31], v[30], v[29], v[28],
		v[27], v[26], v[25], v[24], v[23], v[22], v[21], v[20], v[19], v[18], v[17], v[16], v[15], v[14], v[13], v[12], v[11], v[10], v[9], v[8], v[7], v[6], v[5], v[4], v[3],
		v[2], v[1], v[0]);
	printf(std::string{ values.c_str() +
			   std::string{ " (DIGITS) v32_u8: "
							"%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }
			   .c_str(),
		v[31], v[30], v[29], v[28], v[27], v[26], v[25], v[24], v[23], v[22], v[21], v[20], v[19], v[18], v[17], v[16], v[15], v[14], v[13], v[12], v[11], v[10], v[9], v[8], v[7],
		v[6], v[5], v[4], v[3], v[2], v[1], v[0]);
}

uint32_t convertTo32BitUint(__m256i inputA) {
	return static_cast<uint32_t>(_mm256_movemask_epi8(inputA));
}

uint64_t convertTo64BitUint(__m256i inputA, __m256i inputB) {
	uint64_t r_lo = static_cast<uint64_t>(_mm256_movemask_epi8(inputA));
	uint64_t r_hi = static_cast<uint64_t>(_mm256_movemask_epi8(inputB));
	return r_lo | (r_hi << 32);
}

std::string reverseString(std::string inputString) {
	std::string newString{};
	for (size_t x = 0; x < inputString.size() ; ++x) {
		newString.push_back(inputString[inputString.size() - x - 1]);
	}
	return newString;
}

class Simd8 {
  public:
	Simd8(std::string& stringNewer) {
		auto stringNew = reverseString(stringNewer);
		std::cout << "THE NEW STRING: " << stringNew << std::endl;
		this->string = stringNew;
		this->backslashes[0] = _mm256_set1_epi8('\\');
		this->backslashes[1] = _mm256_set1_epi8('\\');
		this->quotes[0] = _mm256_set1_epi8('"');
		this->quotes[1] = _mm256_set1_epi8('"');
		this->values[0] = _mm256_set_epi64x(*reinterpret_cast<int64_t*>(stringNew.data() + 24), *reinterpret_cast<int64_t*>(stringNew.data() + 16),
			*reinterpret_cast<int64_t*>(stringNew.data() + 8), *reinterpret_cast<int64_t*>(stringNew.data() ));
		this->values[1] = _mm256_set_epi64x(*reinterpret_cast<int64_t*>(stringNew.data() + 56), *reinterpret_cast<int64_t*>(stringNew.data() + 48),
			*reinterpret_cast<int64_t*>(stringNew.data() + 40), *reinterpret_cast<int64_t*>(stringNew.data() + 32));
		printValueAsString(this->values[0], "VALUES 0: ");
		printValueAsString(this->values[1], "VALUES 1: ");
		this->B[0] = _mm256_cmpeq_epi8(this->values[0], this->backslashes[0]);
		this->B[1] = _mm256_cmpeq_epi8(this->values[1], this->backslashes[1]);
		printValueAsString(convertTo32BitUint(this->B[0]), "B0 VALUES: ");
		printValueAsString(convertTo32BitUint(this->B[1]), "B1 VALUES: ");
		printValueAsString(convertTo64BitUint(this->B[0], this->B[1]), "TEST VALUES REAL: ");
		this->O[0] = _mm256_set1_epi16(0xff);
		this->O[1] = _mm256_set1_epi16(0xff00);
		this->E[0] = _mm256_set1_epi16(0x00ff);
		this->E[1] = _mm256_set1_epi16(0x00ff);
		printValueAsString(convertTo64BitUint(this->O[0], this->O[1]), "O VALUES: ");
		printValueAsString(convertTo64BitUint(this->E[0], this->E[1]), "E VALUES: ");
		
		this->B64 = convertTo64BitUint(this->B[0], this->B[1]);
		printValueAsString(this->B64, "B64 VALUES: ");
		printValueAsString(this->B64 << 1, "B64-SHIFT VALUES: ");

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
	std::cout << "THE STRING: " << simd8Test.operator std::string() << std::endl;

	return 0;
}