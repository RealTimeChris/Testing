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
	printf(
		std::string{ values.c_str() + std::string{ " (DIGITS) v32_u8: %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n" } }.c_str(),
		in.m256i_i8[0], in.m256i_i8[1], in.m256i_i8[2], in.m256i_i8[3], in.m256i_i8[4], in.m256i_i8[5], in.m256i_i8[6], in.m256i_i8[7], in.m256i_i8[8], in.m256i_i8[9],
		in.m256i_i8[10], in.m256i_i8[11], in.m256i_i8[12], in.m256i_i8[13], in.m256i_i8[14], in.m256i_i8[15], in.m256i_i8[16], in.m256i_i8[17], in.m256i_i8[18], in.m256i_i8[19],
		in.m256i_i8[20], in.m256i_i8[21], in.m256i_i8[22], in.m256i_i8[23], in.m256i_i8[24], in.m256i_i8[25], in.m256i_i8[26], in.m256i_i8[27], in.m256i_i8[28], in.m256i_i8[29],
		in.m256i_i8[30], in.m256i_i8[31]);
}

void printValueAsString(uint64_t inA, std::string values) {
	alignas(32) uint8_t v[64]{};
	for (size_t x = 0; x < 64; ++x) {
		if ((static_cast<uint64_t>(inA >> x) & 0x01) == 1 << 0) {
			v[x] = 1;
		}
	}
	printf(std::string{ values.c_str() +
			   std::string{ " (DIGITS) v64_u8: "
							"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
							"%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n" } }
			   .c_str(),
		v[32], v[33], v[34], v[35], v[36], v[37], v[38], v[39], v[40], v[41], v[42], v[43], v[44], v[45], v[46], v[47], v[48], v[49], v[50], v[51], v[52], v[53], v[54], v[55],
		v[56], v[57], v[58], v[59], v[60], v[61], v[62], v[63], v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17],
		v[18], v[19], v[20], v[21], v[22], v[23], v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31]);
}

void printValueAsString(uint32_t inA, std::string values) {
	alignas(32) uint8_t v[32]{};
	for (size_t x = 0; x < 32; ++x) {
		if ((static_cast<uint32_t>(inA >> x) & 0x01) == 1 << 0) {
			v[x] = 1;
		}
	}
	printf(std::string{ values.c_str() +
			   std::string{ " (DIGITS) v32_u8: "
							"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n" } }
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

__m256i packStringIntoValue(const char* string) {
	__m256i value{};
	for (size_t x = 0; x < 32; ++x) {
		value.m256i_i8[x] = string[x];
	}
	return value;
}

uint64_t collectCarries(uint64_t inputA, uint64_t inputB) {
	uint64_t returnValue{};
	_addcarry_u64(0, inputB, inputA, reinterpret_cast<unsigned __int64*>(&returnValue));
	return returnValue;
}

class Simd8 {
  public:
	Simd8(std::string& stringNewer) {
		auto stringNew = stringNewer;
		this->string = stringNew;
		this->backslashes[0] = _mm256_set1_epi8('\\');
		this->backslashes[1] = _mm256_set1_epi8('\\');
		this->StructuralTest[0] = _mm256_set1_epi8(0b00000111);
		this->StructuralTest[1] = _mm256_set1_epi8(0b00000111);
		this->quotes[0] = _mm256_set1_epi8('"');
		this->quotes[1] = _mm256_set1_epi8('"');
		this->values[0] = packStringIntoValue(stringNewer.data());
		this->values[1] = packStringIntoValue(stringNewer.data() + 32);
		this->Structural[0] = _mm256_and_si256(this->values[0], this->StructuralTest[0]);
		this->Structural[1] = _mm256_and_si256(this->values[1], this->StructuralTest[1]);
		this->S64 = convertTo64BitUint(this->Structural[1], this->Structural[0]);
		printValueAsString(this->values[1], "THE VALUES: ");
		printValueAsString(this->values[0], "THE VALUES: ");
		printValueAsString(this->Structural[1], "STRUCTURAL VALUES: ");
		printValueAsString(this->Structural[0], "STRUCTURAL VALUES: ");
		printValueAsString(this->S64, "STRUCTURAL VALUES: ");
		this->B[0] = _mm256_cmpeq_epi8(this->values[0], this->backslashes[0]);
		this->B[1] = _mm256_cmpeq_epi8(this->values[1], this->backslashes[1]);
		this->B64 = convertTo64BitUint(this->B[1], this->B[0]);
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
		this->Q[0] = _mm256_cmpeq_epi8(this->quotes[0], this->values[0]);
		this->Q[1] = _mm256_cmpeq_epi8(this->quotes[1], this->values[1]);
		this->Q64 = convertTo64BitUint(this->Q[1], this->Q[0]);
		printValueAsString(this->Q64, "Q VALUES: ");
		this->R64 = this->Q64 & ~this->OD;
		this->R64 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->R64), _mm_set1_epi8('\xFF'), 0));
		printValueAsString(this->R64, "R VALUES: ");




	}
	operator std::string() {
		return string;
	}

  protected:
	std::string string{};
	__m256i StructuralTest[2]{};
	__m256i backslashes[2]{};
	__m256i Structural[2]{};
	__m256i quotes[2]{};
	__m256i values[2]{};
	__m256i B[2]{};
	__m256i Q[2]{};
	uint64_t E{ 0b0101010101010101010101010101010101010101010101010101010101010101 };
	uint64_t O{ 0b1010101010101010101010101010101010101010101010101010101010101010 };
	uint64_t B64{};
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
	uint64_t W{};
	uint64_t OD{};
	uint64_t Q64{};
	uint64_t R64{};
	uint64_t S64{};

};

int32_t main() noexcept {
	std::string string{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
	Simd8 simd8Test{ string };
	std::cout << "A VALUES:  (DIGITS) v64_u8: " << simd8Test.operator std::string() << std::endl;

	return 0;
}