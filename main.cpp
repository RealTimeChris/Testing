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

void printValueAsString(uint32_t inA, std::string values) {
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
	alignas(32) uint8_t v[64]{};
	for (size_t x = 0; x < 64; ++x) {
		if ((static_cast<uint64_t>(inA >> x) & 0x01) == 1 << 0) {
			v[x] = 1;
		}
	}
	printf(std::string{ values.c_str() +
			   std::string{ " v32_u8: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n" } }
			   .c_str(),
		v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31], v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[0],
		v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[56], v[57], v[58], v[59], v[60], v[61], v[62], v[63], v[48], v[49], v[50], v[51], v[52], v[53], v[54], v[55], v[40], v[41],
		v[42], v[43], v[44], v[45], v[46], v[47], v[32], v[33], v[34], v[35], v[36], v[37], v[38], v[39]);
	printf(std::string{ values.c_str() +
			   std::string{ " (DIGITS) v32_u8: "
							"%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%"
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
	uint64_t value{};
	value |= _mm256_movemask_epi8(inputA);
	std::cout << std::bitset<64>{ static_cast<uint64_t>(_mm256_movemask_epi8(inputA)) } << std::endl;
	value |= static_cast<uint64_t>(_mm256_movemask_epi8(inputB)) << 32;
	std::cout << std::bitset<64>{ static_cast<uint64_t>(_mm256_movemask_epi8(inputB)) << 32 } << std::endl;
	std::cout << std::bitset<64>{ value } << std::endl;
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

	}
	operator std::string() {
		return string;
	}

  protected:
	std::string string{};
	__mmask32 valueMask{};
	__m256i backslashes[2]{};
	__m256i quotes[2]{};
	__m256i values[2]{};
	__m256i BShift{};
	__m256i B[2]{};
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
	std::string string{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
	Simd8 simd8Test{ string };

	return 0;
}