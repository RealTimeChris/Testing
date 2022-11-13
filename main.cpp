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



std::string reverseString(std::string inputString) {
	std::string newString{};
	for (size_t x = 0; x < inputString.size(); ++x) {
		newString.push_back(inputString[inputString.size() - x - 1]);
	}
	return newString;
}

void packStringIntoValue(__m256i*theValue, const char* string) {
	for (size_t x = 0; x < 32; ++x) {
		theValue->m256i_i8[x] = string[x];
	}
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

uint64_t convertTo64BitUint(const __m256i& inputA, const __m256i& inputB) {
	uint64_t r_lo = uint32_t(_mm256_movemask_epi8(inputA));
	uint64_t r_hi = _mm256_movemask_epi8(inputB);
	return r_lo | (r_hi << 32);
}


void printBits(std::string valuesTitle,__m256i values) {
	std::cout << valuesTitle;
	for (size_t x = 0; x < 32; ++x) {
		for (size_t y = 0; y < 8; ++y) {
			std::cout << std::bitset<1>{ static_cast<uint64_t>(values.m256i_i8[x]) >> y };
		}
	}
	std::cout << std::endl;
};

struct Simd256 {
	__m256i value{};

	Simd256() noexcept = default;

	inline Simd256& operator=(char other) {
		this->value = _mm256_set1_epi8(other);
		return *this;
	}

	inline Simd256(char other) {
		*this = other;
	}

	inline Simd256(int64_t value00, int64_t value01, int64_t value02, int64_t value03) {
		this->value = _mm256_insert_epi64(this->value, value00, 0);
		this->value = _mm256_insert_epi64(this->value, value01, 1);
		this->value = _mm256_insert_epi64(this->value, value02, 2);
		this->value = _mm256_insert_epi64(this->value, value03, 3);
	}

	inline Simd256(uint64_t value00, uint64_t value01, uint64_t value02, uint64_t value03) {
		this->value = _mm256_insert_epi64(this->value, value00, 0);
		this->value = _mm256_insert_epi64(this->value, value01, 1);
		this->value = _mm256_insert_epi64(this->value, value02, 2);
		this->value = _mm256_insert_epi64(this->value, value03, 3);
	}

	operator std::vector<uint64_t>() const  {
		std::vector<uint64_t> returnValue{};
		uint64_t returnValue6400 = _mm256_extract_epi64(this->value, 0) ;
		uint64_t returnValue6401 = _mm256_extract_epi64(this->value, 1) ;
		uint64_t returnValue6402 = _mm256_extract_epi64(this->value, 2) ;
		uint64_t returnValue6403 = _mm256_extract_epi64(this->value, 3) ;
		returnValue.push_back(returnValue6400);
		returnValue.push_back(returnValue6401);
		returnValue.push_back(returnValue6402);
		returnValue.push_back(returnValue6403);
		return returnValue;
	}

	inline Simd256(__m256i&& other) {
		this->value = other;
	}

	inline Simd256(__m256i& other) {
		this->value = other;
	}

	inline operator __m256i&() {
		return this->value;
	}

	inline __m256i operator | (const __m256i& other) {
		return _mm256_or_si256(*this, other);
	}

	inline __m256i operator&(const __m256i& other) {
		return _mm256_and_si256(*this, other);
	}

	inline __m256i operator^(const __m256i& other) {
		return _mm256_xor_si256(*this, other);
	}

	inline __m256i bit_andnot(const __m256i& other) {
		return _mm256_andnot_si256(other, *this);
	}

	inline __m256i operator+(const __m256i& other) {
		return _mm256_add_epi8(*this, other);
	}

	inline __m256i operator|=(const __m256i& other) {
		*this = *this | other;
		return *this;
	}

	inline __m256i operator&=(const __m256i& other) {
		*this = *this & other;
		return *this;
	}

	inline __m256i operator^=(const __m256i& other) {
		*this = *this ^ other;
		return *this;
	}

	inline __m256i operator==(Simd256& rhs) {
		return _mm256_cmpeq_epi8(*this, rhs);
	}

	inline __m256i operator<<(size_t amount) {
		__m256i this_cast{};
		for (size_t x = 0; x < 32; ++x) {
			this_cast.m256i_i8[x] |= this->value.m256i_i8[x] << (amount % 8);
			if (x > 0) {
				this_cast.m256i_i8[x] |= (this->value.m256i_i8[x - 1] >> 7) & 0b00000001;
			}
		}
		return this_cast;
	}

	inline __m256i operator~() {
		__m256i newValue{};
		for (size_t x = 0; x < 32; ++x) {
			newValue.m256i_i8[x] = ~this->value.m256i_i8[x];
		}
		return newValue;
	}

	void printBits(std::string valuesTitle) {
		std::cout << valuesTitle;
		for (size_t x = 0; x < 32; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				std::cout << std::bitset<1>{ static_cast<uint64_t>(this->value.m256i_i8[x]) >> y };
			}
		}
		std::cout << std::endl;
	};

};

Simd256 collectCarries(const Simd256& inputA,const Simd256& inputB) {
	auto value01 = inputB.operator std::vector<uint64_t>();
	auto value02 = inputA.operator std::vector<uint64_t>();
	for (size_t x = 0; x < 4; ++x) {
		uint64_t returnValue64{};
		_addcarry_u64(0, value01[x], value02[x], reinterpret_cast<unsigned __int64*>(&returnValue64));
		value01[x] = returnValue64;
	}
	return { value01[0], value01[1], value01[2], value01[3] };
}

struct Simd256StringScanner {

	inline Simd256StringScanner(){};

	void printBits(std::string valuesTitle) {
		std::cout << valuesTitle;
		for (size_t x = 0; x < 32; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				std::cout << std::bitset<1>{ static_cast<uint64_t>(this->B256.operator __m256i&().m256i_i8[x]) >> y };
			}
		}
		std::cout << std::endl;
	};

	inline Simd256StringScanner(const std::string& valueNew, bool isItFirst) {
		this->backslashes = '\\';
		this->quotes = '"';
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		stopWatch.resetTimer();
		packStringIntoValue(&this->values[0], valueNew.data());
		packStringIntoValue(&this->values[1], valueNew.data() + 32);
		packStringIntoValue(&this->values[2], valueNew.data() + 64);
		packStringIntoValue(&this->values[3], valueNew.data() + 96);
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0101" << std::endl;
		}
		stopWatch.resetTimer();
		this->B[0] = _mm256_cmpeq_epi8(this->values[0], this->backslashes);
		this->B[1] = _mm256_cmpeq_epi8(this->values[1], this->backslashes);
		this->B[2] = _mm256_cmpeq_epi8(this->values[2], this->backslashes);
		this->B[3] = _mm256_cmpeq_epi8(this->values[3], this->backslashes);
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0202" << std::endl;
		}
		stopWatch.resetTimer();
		this->B256 = Simd256{ convertTo64BitUint(this->B[0], this->B[1]), convertTo64BitUint(this->B[2], this->B[3]), convertTo64BitUint(this->B[4], this->B[5]),
			convertTo64BitUint(this->B[6], this->B[7]) };
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0303" << std::endl;
		}
		stopWatch.resetTimer();
		this->S = this->B256 & ~Simd256{ (this->B256 << 1) };
		this->ES = this->E & this->S;
		this->EC = collectCarries(this->ES, this->B256);
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0404" << std::endl;
		}
		stopWatch.resetTimer();
		this->ECE = this->EC & ~this->B256;
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0505" << std::endl;
		}
		stopWatch.resetTimer();
		
		this->OD1 = this->ECE & ~this->E;
		this->OS = this->S & this->O;
		this->OC = this->B256 + this->OS;
		this->OCE = this->OC & ~this->B256;
		this->OD2 = this->OCE & this->E;
		this->OD = this->OD1 | this->OD2;
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0404" << std::endl;
		}
		stopWatch.resetTimer();
		this->Q[0] = _mm256_cmpeq_epi8(this->quotes, this->values[0]);
		this->Q[1] = _mm256_cmpeq_epi8(this->quotes, this->values[1]);
		this->Q[2] = _mm256_cmpeq_epi8(this->quotes, this->values[2]);
		this->Q[3] = _mm256_cmpeq_epi8(this->quotes, this->values[3]);
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0505" << std::endl;
		}
		stopWatch.resetTimer();
		this->Q256[0] = convertTo64BitUint(this->Q[1], this->Q[0]);
		this->Q256[1] = convertTo64BitUint(this->Q[2], this->Q[3]);
		this->Q256[2] = convertTo64BitUint(this->Q[4], this->Q[5]);
		this->Q256[3] = convertTo64BitUint(this->Q[6], this->Q[7]);
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0606" << std::endl;
		}
		stopWatch.resetTimer();
		this->R256[0] = this->Q256[0] & ~this->OD.operator std::vector<size_t, std::allocator<size_t>>()[0];
		this->R256[1] = this->Q256[1] & ~this->OD.operator std::vector<size_t, std::allocator<size_t>>()[1];
		this->R256[2] = this->Q256[2] & ~this->OD.operator std::vector<size_t, std::allocator<size_t>>()[2];
		this->R256[3] = this->Q256[2] & ~this->OD.operator std::vector<size_t, std::allocator<size_t>>()[3];
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0707" << std::endl;
		}
		stopWatch.resetTimer();
		this->R256[0] = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->R256[3]), _mm_set1_epi8('\xFF'), 0));
		this->R256[1] = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->R256[2]), _mm_set1_epi8('\xFF'), 0));
		this->R256[2] = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->R256[1]), _mm_set1_epi8('\xFF'), 0));
		this->R256[3] = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->R256[0]), _mm_set1_epi8('\xFF'), 0));
		if (isItFirst) {
		std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0808" << std::endl;
		}
		stopWatch.resetTimer();
		auto whiteSpace00 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->whitespaceTable, this->values[0]), this->values[0]);
		auto whiteSpace01 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->whitespaceTable, this->values[1]), this->values[1]);
		auto whiteSpace02 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->whitespaceTable, this->values[2]), this->values[2]);
		auto whiteSpace03 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->whitespaceTable, this->values[3]), this->values[3]);
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 0909" << std::endl;
		}
		stopWatch.resetTimer();
		this->W256[2] = convertTo64BitUint(whiteSpace02, whiteSpace03);
		this->W256[1] = convertTo64BitUint(whiteSpace00, whiteSpace01);
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 101010" << std::endl;
		}
		stopWatch.resetTimer();
		auto valuesNew00 = _mm256_or_si256(this->values[0], _mm256_set1_epi8(0x20));
		auto valuesNew01 = _mm256_or_si256(this->values[1], _mm256_set1_epi8(0x20));
		auto valuesNew02 = _mm256_or_si256(this->values[2], _mm256_set1_epi8(0x20));
		auto valuesNew03 = _mm256_or_si256(this->values[3], _mm256_set1_epi8(0x20));
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 11111" << std::endl;
		}
		stopWatch.resetTimer();
		auto structural00 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->opTable, this->values[0]), valuesNew00);
		auto structural01 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->opTable, this->values[1]), valuesNew01);
		auto structural02 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->opTable, this->values[2]), valuesNew02);
		auto structural03 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->opTable, this->values[3]), valuesNew03);
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 121212" << std::endl;
		}
		stopWatch.resetTimer();

		this->S256[0] = convertTo64BitUint(structural00, structural01);
		this->S256[1] = convertTo64BitUint(structural02, structural03);
		if (isItFirst) {
			std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << "ns TO PARSE THROUGH IT! 131313" << std::endl;
		}
		stopWatch.resetTimer();
		
		//this->Q256.printBits("Q FINAL VALUES: ");
		//this->R256.printBits("R FINAL VALUES: ");
		//this->S256.printBits("S FINAL VALUES: ");
		//this->W256.printBits("W FINAL VALUES: ");
	}

  protected:
	__m256i values[4]{};
	std::string string{};
	Simd256 backslashes{};
	uint64_t Q256[4]{};
	uint64_t R256[4]{};
	uint64_t W256[4]{};
	Simd256 quotes{};
	Simd256 B[8]{};
	Simd256 Q[8]{};
	__m256i whitespaceTable{ _mm256_setr_epi8(' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n',
		112, 100, '\r', 100, 100) };
	__m256i opTable{ _mm256_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0) };
	Simd256 E{ _mm256_set1_epi8(0b01010101) };
	Simd256 O{ _mm256_set1_epi8(0b10101010) };
	Simd256 B256{};
	Simd256 ES{};
	Simd256 EC{};
	Simd256 S{};
	Simd256 OD1{};
	Simd256 OS1{};
	Simd256 OC{};
	Simd256 OCE{};
	Simd256 OS{};
	Simd256 ECE{};
	Simd256 OD2{};
	Simd256 OD{};
	uint64_t S256[4]{};
};

class Simd64StringScanner {
  public:

	inline operator uint64_t() {
		return convertTo64BitUint(this->values[1], this->values[0]);
	}
	
	uint64_t collectCarries(uint64_t inputA, uint64_t inputB) {
		uint64_t returnValue{};
		_addcarry_u64(0, inputB, inputA, reinterpret_cast<unsigned __int64*>(&returnValue));
		return returnValue;
	}

	void printBits(std::string valuesTitle) {
		std::cout << valuesTitle;
		for (size_t x = 0; x < 32; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				std::cout << std::bitset<1>{ static_cast<uint64_t>(static_cast<__m256i>(values[0]).m256i_i8[x]) >> y };
			}
		}
		for (size_t x = 0; x < 32; ++x) {
			for (size_t y = 0; y < 8; ++y) {
				std::cout << std::bitset<1>{ static_cast<uint64_t>(static_cast<__m256i>(values[1]).m256i_i8[x]) >> y };
			}
		}
		std::cout << std::endl;

	};

	inline Simd64StringScanner(Simd256 value01, Simd256 value02) {
		this->values[0] = value01;
		this->values[1] = value02;
	}

	inline Simd64StringScanner(std::string& stringNewer) {
		this->string = stringNewer;
		this->backslashes = _mm256_set1_epi8('\\');
		this->quotes = _mm256_set1_epi8('"');
		packStringIntoValue(&this->values[0].operator __m256i&(), stringNewer.data());
		packStringIntoValue(&this->values[1].operator __m256i&(), stringNewer.data() + 32);
		this->B[0] = _mm256_cmpeq_epi8(this->values[0], this->backslashes);
		this->B[1] = _mm256_cmpeq_epi8(this->values[1],this->backslashes);
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
		this->R64 = this->Q64 & ~this->OD;
		this->R64 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->R64), _mm_set1_epi8('\xFF'), 0));
		auto whiteSpace00 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->whitespaceTable, this->values[0]), this->values[0]);
		auto whiteSpace01 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->whitespaceTable, this->values[1]), this->values[1]);
		this->W64 = convertTo64BitUint(whiteSpace01, whiteSpace00);
		auto valuesNew00 = _mm256_or_si256(this->values[0], _mm256_set1_epi8(0x20));
		auto valuesNew01 = _mm256_or_si256(this->values[1], _mm256_set1_epi8(0x20));
		auto structural00 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->opTable, this->values[0]), valuesNew00);
		auto structural01 = _mm256_cmpeq_epi8(_mm256_shuffle_epi8(this->opTable, this->values[1]), valuesNew01);
		this->S64 = convertTo64BitUint(structural01, structural00);
		//printValueAsString(this->Q64, "Q FINAL VALUES: ");
		//printValueAsString(this->R64, "R FINAL VALUES: ");
		//printValueAsString(this->S64, "S FINAL VALUES: ");
		//printValueAsString(this->W64, "W FINAL VALUES: ");
	}
	operator std::string() {
		return string;
	}

  protected:
	Simd256 values[2]{};
	std::string string{};
	Simd256 backslashes{};
	Simd256 whitespaceTable{ _mm256_setr_epi8(' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n',
		112, 100, '\r', 100, 100) };
	Simd256 opTable{ _mm256_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0) };
	Simd256 quotes{};
	Simd256 B[2]{};
	Simd256 Q[2]{};
	uint64_t E{ 0b0101010101010101010101010101010101010101010101010101010101010101 };
	uint64_t O{ 0b1010101010101010101010101010101010101010101010101010101010101010 };
	uint64_t B64{};
	uint64_t ES{};
	uint64_t EC{};
	uint64_t S{};
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
	std::string string256{
		"{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], "
		"\"t\":\"\\\\\\\"\" }{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , "
		"234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
	};
	Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
	stopWatch.resetTimer();
	size_t totalTime{};
	size_t totalSize{};
	
	for (size_t x = 0; x < 256 * 2048 / 4; ++x) {
		if (x == 0) {
			Simd256StringScanner value{ string256, true };
		} else {
			Simd256StringScanner value{ string256, false };
		}
		
		totalSize += string256.size();
	}
	
	totalTime += stopWatch.totalTimePassed();
	std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
	
	stopWatch.resetTimer();
	totalSize = 0;
	totalTime = 0;
	for (size_t x = 0; x < 256 * 2048; ++x) {
		Simd64StringScanner simd8Test{ string64 };
		totalSize += string64.size();
	}
	totalTime += stopWatch.totalTimePassed();
	std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
	
	
	return 0;
};