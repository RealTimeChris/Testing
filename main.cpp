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

void printValueAsString(__m256i in,std::string values) {
	alignas(16) uint8_t v[32];
	_mm256_store_si256(( __m256i* )v, in);
	printf(std::string{ values.c_str() + std::string{ " v32_u8: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n" } }.c_str(),
		v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[0],
		v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
	printf(std::string{ values.c_str() + std::string{ " (DIGITS) v32_u8: %d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }.c_str(), v[24], v[25], v[26], v[27], v[28],
		v[29], v[30], v[31], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[0], v[1], v[2], v[3], v[4], v[5], v[6],
		v[7]);
}

void printValueAsString(uint64_t in, std::string values) {
	alignas(32) uint8_t v[64]{};
	for (size_t x = 0; x < 64; ++x) {
		if ((static_cast<uint64_t>(in >> x) & 0x01) == 1 << 0) {
			v[x] = 1;
		}
	}
	printf(std::string{ values.c_str() + std::string{ " v32_u8: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n" } }.c_str(), v[24], v[25], v[26], v[27], v[28],
		v[29], v[30], v[31], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[0], v[1], v[2], v[3], v[4], v[5], v[6],
		v[7]);
	printf(std::string{ values.c_str() + std::string{ " (DIGITS) v32_u8: %d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }
			   .c_str(),
		v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[0],
		v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
}

uint32_t convertTo32BitUint(__m256i inputA){
	uint32_t value{};
	for (size_t x = 0; x < 32; ++x) {
		if (static_cast<uint8_t>(inputA.m256i_i8[x]) == 0xff) {
			value |= 1 << x;
		}
	}
	return value;
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

void collectCarries(uint32_t inputA, uint32_t inputB, uint32_t* outputValue) {
	_addcarry_u32(0, inputA, inputB, reinterpret_cast<unsigned __int32*>(outputValue));
	printValueAsString(*outputValue, "TESTING VALUES: ");
}


class Simd8 {
  public:
	Simd8(std::string& stringNew) {
		for (int32_t x = 0; x < 32; ++x){
			this->string.push_back(stringNew[x]);
		}
		this->backslashes = _mm256_set1_epi8('\\');
		this->quotes = _mm256_set1_epi8('"');
		this->values = _mm256_set_epi64x(*reinterpret_cast<int64_t*>(stringNew.data()), *reinterpret_cast<int64_t*>(stringNew.data() + 8),
			*reinterpret_cast<int64_t*>(stringNew.data() + 16), *reinterpret_cast<int64_t*>(stringNew.data() + 24));
		this->B = _mm256_cmpeq_epi8(this->values, this->backslashes);
		convertTo32BitUint(this->B);
		printValueAsString(this->values, "VALUES:");
		printValueAsString(this->B, "B VALUES");
		printValueAsString(convertTo32BitUint(this->B), "B VALUES");
		printValueAsString(convertToM256(convertTo32BitUint(this->B)), "B VALUES");
		this->E = _mm256_set_epi8(0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff,
			0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff);
		this->O = _mm256_set_epi8(0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00,
			0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00);
		this->BShift = _mm256_set_epi8(this->B.m256i_i8[30], this->B.m256i_i8[29], this->B.m256i_i8[28], this->B.m256i_i8[27], this->B.m256i_i8[26], this->B.m256i_i8[25],
			this->B.m256i_i8[24], this->B.m256i_i8[23], this->B.m256i_i8[22], this->B.m256i_i8[21], this->B.m256i_i8[20], this->B.m256i_i8[19], this->B.m256i_i8[18],
			this->B.m256i_i8[17], this->B.m256i_i8[31], this->B.m256i_i8[16], this->B.m256i_i8[14], this->B.m256i_i8[13], this->B.m256i_i8[12], this->B.m256i_i8[11],
			this->B.m256i_i8[10], this->B.m256i_i8[9], this->B.m256i_i8[8], this->B.m256i_i8[7], this->B.m256i_i8[6], this->B.m256i_i8[5], this->B.m256i_i8[4], this->B.m256i_i8[3],
			this->B.m256i_i8[2], this->B.m256i_i8[1], this->B.m256i_i8[0], this->B.m256i_i8[15]);
		this->S = _mm256_andnot_si256(this->BShift, this->B);
		

		this->ES = _mm256_and_si256(this->E, this->S);
		auto valueEC = convertTo32BitUint(this->EC);
		collectCarries(convertTo32BitUint(this->B), convertTo32BitUint(this->ES), &valueEC);

	}
	operator std::string() {
		return string;
	}

  protected:
	std::string string{};
	__mmask32 valueMask{};
	__m256i backslashes{};
	__m256i quotes{};
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
	__m256i OS{};
	__m256i ECE{};
	__m256i OD2{};
	__m256i OD{};
	__m256i Q{};

};

int32_t main() noexcept {
	std::string string{ "{ \"\\\\\\\"Nam[{\":[ 116, \"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
	Simd8 simd8Test{ string };

	return 0;
}