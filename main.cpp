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

void printValueAsString(__m256i in, std::string values) {
	printf(std::string{ values.c_str() + std::string{ " (CHARACTERS) v32_u8: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n" } }.c_str(), in.m256i_i8[0],
		in.m256i_i8[1], in.m256i_i8[2], in.m256i_i8[3], in.m256i_i8[4], in.m256i_i8[5], in.m256i_i8[6], in.m256i_i8[7], in.m256i_i8[8], in.m256i_i8[9], in.m256i_i8[10],
		in.m256i_i8[11], in.m256i_i8[12], in.m256i_i8[13], in.m256i_i8[14], in.m256i_i8[15], in.m256i_i8[16], in.m256i_i8[17], in.m256i_i8[18], in.m256i_i8[19], in.m256i_i8[20],
		in.m256i_i8[21], in.m256i_i8[22], in.m256i_i8[23], in.m256i_i8[24], in.m256i_i8[25], in.m256i_i8[26], in.m256i_i8[27], in.m256i_i8[28], in.m256i_i8[29], in.m256i_i8[30],
		in.m256i_i8[31]);
	printf(std::string{ values.c_str() + std::string{ " (DIGITS) v32_u8: '%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }
			   .c_str(),
		in.m256i_i8[0], in.m256i_i8[1], in.m256i_i8[2], in.m256i_i8[3], in.m256i_i8[4], in.m256i_i8[5], in.m256i_i8[6], in.m256i_i8[7], in.m256i_i8[8], in.m256i_i8[9],
		in.m256i_i8[10], in.m256i_i8[11], in.m256i_i8[12], in.m256i_i8[13], in.m256i_i8[14], in.m256i_i8[15], in.m256i_i8[16], in.m256i_i8[17], in.m256i_i8[18], in.m256i_i8[19],
		in.m256i_i8[20], in.m256i_i8[21], in.m256i_i8[22], in.m256i_i8[23], in.m256i_i8[24], in.m256i_i8[25], in.m256i_i8[26], in.m256i_i8[27], in.m256i_i8[28], in.m256i_i8[29],
		in.m256i_i8[30], in.m256i_i8[31]);
}

void printValueAsString(uint64_t inA, std::string values) {
	alignas(32) uint8_t v[64]{};
	for (size_t x = 0; x < 64; ++x) {
		if ((static_cast<uint64_t>(inA >> x) & 0x01) == static_cast<uint64_t>(1 << 0)) {
			v[x] = 1;
		}
	}
	printf(std::string{ values.c_str() +
			   std::string{ " (DIGITS) v64_u8: "
							"'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%"
							"d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }
			   .c_str(),
		v[32], v[33], v[34], v[35], v[36], v[37], v[38], v[39], v[40], v[41], v[42], v[43], v[44], v[45], v[46], v[47], v[48], v[49], v[50], v[51], v[52], v[53], v[54], v[55],
		v[56], v[57], v[58], v[59], v[60], v[61], v[62], v[63], v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17],
		v[18], v[19], v[20], v[21], v[22], v[23], v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31]);
}

void printBits(__m256i value, std::string valuesTitle){
	std::cout << valuesTitle;
	for (size_t x = 0; x < 32; ++x) {
		for (size_t y = 0; y < 8; ++y) {
			std::cout << std::bitset<1>{ static_cast<uint64_t>(value.m256i_i8[x]) >> y };
		}
	}
	std::cout << std::endl;
};

std::string reverseString(std::string inputString) {
	std::string newString{};
	for (size_t x = 0; x < inputString.size(); ++x) {
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

void printValueAsString(uint32_t inA, std::string values) {
	alignas(32) uint8_t v[32]{};
	for (size_t x = 0; x < 32; ++x) {
		if ((static_cast<uint32_t>(inA >> x) & 0x01) == 1 << 0) {
			v[x] = 1;
		}
	}
	printf(std::string{ values.c_str() +
			   std::string{ " (DIGITS) v32_u8: "
							"'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d'%d\n" } }
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

uint64_t convertStringToUint64(char* string) {
	uint64_t value = *reinterpret_cast<uint64_t*>(string);
	return value;
}

struct Simd256Base {
	__m256i value{};
	__m256i values[8]{};
	inline Simd256Base() : value{ __m256i{} } {};

	inline Simd256Base(const __m256i _value){};

	inline Simd256Base(uint64_t value01, uint64_t value02, uint64_t value03, uint64_t value04) {
		this->value = _mm256_insert_epi64(this->value, value01, 0);
		this->value = _mm256_insert_epi64(this->value, value02, 1);
		this->value = _mm256_insert_epi64(this->value, value03, 2);
		this->value = _mm256_insert_epi64(this->value, value04, 3);
		printBits(this->value, "THE TESTING VALUES: ");
	}

	inline Simd256Base(std::string& valueNew) {
		__m256i backslashes{ _mm256_set1_epi8('\\') };
		__m256i StructuralTest{ _mm256_set1_epi8(0b00000111) };
		__m256i quotes{ _mm256_set1_epi8('"') };
		this->values[0] = packStringIntoValue(valueNew.data());
		this->values[1] = packStringIntoValue(valueNew.data() + 32);
		this->values[2] = packStringIntoValue(valueNew.data() + 64);
		this->values[3] = packStringIntoValue(valueNew.data() + 96);
		this->values[4] = packStringIntoValue(valueNew.data() + 128);
		this->values[5] = packStringIntoValue(valueNew.data() + 160);
		this->values[6] = packStringIntoValue(valueNew.data() + 192);
		this->values[7] = packStringIntoValue(valueNew.data() + 224);

		__m256i Structural[8]{};
		__m256i B[8]{};
		__m256i S256{};

		Structural[0] = _mm256_and_si256(this->values[0], StructuralTest);
		Structural[1] = _mm256_and_si256(this->values[1], StructuralTest);
		Structural[2] = _mm256_and_si256(this->values[2], StructuralTest);
		Structural[3] = _mm256_and_si256(this->values[3], StructuralTest);
		Structural[4] = _mm256_and_si256(this->values[4], StructuralTest);
		Structural[5] = _mm256_and_si256(this->values[5], StructuralTest);
		Structural[6] = _mm256_and_si256(this->values[6], StructuralTest);
		Structural[7] = _mm256_and_si256(this->values[7], StructuralTest);
		auto S64_01 = convertTo64BitUint(Structural[1], Structural[0]);
		auto S64_02 = convertTo64BitUint(Structural[3], Structural[2]);
		auto S64_03 = convertTo64BitUint(Structural[5], Structural[4]);
		auto S64_04 = convertTo64BitUint(Structural[7], Structural[6]);
		printValueAsString(this->values[1], "THE VALUES: ");
		printValueAsString(this->values[0], "THE VALUES: ");
		printValueAsString(Structural[1], "STRUCTURAL VALUES: ");
		printValueAsString(Structural[0], "STRUCTURAL VALUES: ");
		printValueAsString(S64_01, "STRUCTURAL VALUES: ");
		
		B[0] = _mm256_cmpeq_epi8(this->values[0], backslashes);
		B[1] = _mm256_cmpeq_epi8(this->values[1], backslashes);
		auto B64_01 = convertTo64BitUint(B[1], B[0]);
		B[2] = _mm256_cmpeq_epi8(this->values[2], backslashes);
		B[3] = _mm256_cmpeq_epi8(this->values[3], backslashes);
		auto B64_02 = convertTo64BitUint(B[3], B[2]);
		B[4] = _mm256_cmpeq_epi8(this->values[4], backslashes);
		B[5] = _mm256_cmpeq_epi8(this->values[5], backslashes);
		auto B64_03 = convertTo64BitUint(B[5], B[4]);
		B[6] = _mm256_cmpeq_epi8(this->values[6], backslashes);
		B[7] = _mm256_cmpeq_epi8(this->values[7], backslashes);
		auto B64_04 = convertTo64BitUint(B[7], B[6]);
		printValueAsString(B64_01, "THE B VALUES 00: ");
		printValueAsString(B64_02, "THE B VALUES 01: ");
		printValueAsString(B64_03, "THE B VALUES 02: ");
		printValueAsString(B64_04, "THE B VALUES 03: ");
		/*
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
		*/

	}

	inline operator const __m256i&() const {
		return this->value;
	}

	inline operator __m256i&() {
		return this->value;
	}

	inline Simd256Base operator|(const Simd256Base other) const {
		return _mm256_or_si256(*this, other);
	}
	inline Simd256Base operator&(const Simd256Base other) const {
		return _mm256_and_si256(*this, other);
	}
	inline Simd256Base operator^(const Simd256Base other) const {
		return _mm256_xor_si256(*this, other);
	}
	inline Simd256Base bitAndNot(const Simd256Base other) const {
		return _mm256_andnot_si256(other, *this);
	}
	inline Simd256Base& operator|=(const Simd256Base other) {
		auto thisCasted = static_cast<Simd256Base*>(this);
		*thisCasted = *thisCasted | other;
		return *thisCasted;
	}
	inline Simd256Base& operator&=(const Simd256Base other) {
		auto thisCasted = static_cast<Simd256Base*>(this);
		*thisCasted = *thisCasted & other;
		return *thisCasted;
	}
	inline Simd256Base& operator^=(const Simd256Base other) {
		auto thisCasted = static_cast<Simd256Base*>(this);
		*thisCasted = *thisCasted ^ other;
		return *thisCasted;
	}
};

class Simd64Base {
  public:
	Simd64Base(std::string& stringNewer) {
		std::string& stringNew = stringNewer;
		this->string = stringNew;
		this->backslashes = _mm256_set1_epi8('\\');
		this->StructuralTest = _mm256_set1_epi8(0b00000111);
		this->quotes = _mm256_set1_epi8('"');
		this->values[0] = packStringIntoValue(stringNewer.data());
		this->values[1] = packStringIntoValue(stringNewer.data() + 32);
		this->Structural[0] = _mm256_and_si256(this->values[0], this->StructuralTest);
		this->Structural[1] = _mm256_and_si256(this->values[1], this->StructuralTest);
		this->S64 = convertTo64BitUint(this->Structural[1], this->Structural[0]);
		printValueAsString(this->values[1], "THE VALUES: ");
		printValueAsString(this->values[0], "THE VALUES: ");
		printValueAsString(this->Structural[1], "STRUCTURAL VALUES: ");
		printValueAsString(this->Structural[0], "STRUCTURAL VALUES: ");
		printValueAsString(this->S64, "STRUCTURAL VALUES: ");
		this->B[0] = _mm256_cmpeq_epi8(this->values[0], this->backslashes);
		this->B[1] = _mm256_cmpeq_epi8(this->values[1], this->backslashes);
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
		this->Q[0] = _mm256_cmpeq_epi8(this->quotes, this->values[0]);
		this->Q[1] = _mm256_cmpeq_epi8(this->quotes, this->values[1]);
		this->Q64 = convertTo64BitUint(this->Q[1], this->Q[0]);
		printValueAsString(this->Q64, "Q VALUES: ");
		this->R64 = this->Q64 & ~this->OD;
		this->R64 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->R64), _mm_set1_epi8('\xFF'), 0));
		printValueAsString(this->R64, "R VALUES: ");

		const auto whitespaceTable = _mm256_setr_epi8(' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2, 100,
			'\t', '\n', 112, 100, '\r', 100, 100);
		printValueAsString(whitespaceTable, "WHITESPACE TABLE: ");
		auto whiteSpace00 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(whitespaceTable, this->values[0]), this->values[0]);
		auto whiteSpace01 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(whitespaceTable, this->values[1]), this->values[1]);
		printValueAsString(whiteSpace00, "WHITESPACE VALUES 00: ");
		printValueAsString(whiteSpace01, "WHITESPACE VALUES 01: ");
		auto whiteSpaceFinal = convertTo64BitUint(whiteSpace01, whiteSpace00);
		printValueAsString(whiteSpaceFinal, "WHITESPACE FINAL VALUES: ");
		const auto opTable = _mm256_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0);
		printValueAsString(opTable, "STRUCTURAL TABLE: ");
		auto valuesNew00 = _mm256_or_si256(_mm256_set1_epi8('0x20'), this->values[0]);
		auto valuesNew01 = _mm256_or_si256(_mm256_set1_epi8('0x20'), this->values[1]);
		printValueAsString(valuesNew00, "STRUCTURAL VALUES NEW 00: ");
		printValueAsString(valuesNew01, "STRUCTURAL VALUES NEW 01: ");
		auto structural00 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(opTable, this->values[0]), this->values[0]);
		auto structural01 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(opTable, this->values[1]), this->values[1]);
		printValueAsString(structural00, "STRUCTURAL VALUES 00: ");
		printValueAsString(structural01, "STRUCTURAL VALUES 01: ");
		auto structuralFinal = convertTo64BitUint(structural01, structural00);
		printValueAsString(structuralFinal, "STRUCTURAL FINAL VALUES: ");


	}
	operator std::string() {
		return string;
	}

  protected:
	std::string string{};
	__m256i StructuralTest{};
	__m256i backslashes{};
	__m256i Structural[2]{};
	__m256i quotes{};
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
	std::string string{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
	Simd64Base simd8Test{ string };
	std::cout << "A VALUES:  (DIGITS) v64_u8: " << simd8Test.operator std::string() << std::endl;
	//Simd256Base value{ string };
	std::cout << "THE STRING: " << string << std::endl;
	
	return 0;
}