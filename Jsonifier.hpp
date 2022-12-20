#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

#include "NumberParsingUtils.hpp"

namespace Jsonifier {

	inline int trailingZeroes(uint64_t input_num) {
		return ( int )_tzcnt_u64(input_num);
	}

	inline uint64_t clearLowestBit(uint64_t input_num) {
		return _blsr_u64(input_num);
	}

	inline int leadingZeroes(uint64_t  input_num) {
		return int(_lzcnt_u64(input_num));
	}

	inline long long int countOnes(uint64_t input_num) {
		return __popcnt64(input_num);
	}

	class SimdJsonValue;

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
		inline SimdBase256() noexcept {
			this->value = _mm256_set1_epi8(0x00);
		};

		explicit operator bool() {
			for (size_t x = 0; x < 4; ++x) {
				if (this->getUint64(x) != 0) {
					return true;
				}
			}
			return false;
		}

		inline static SimdBase256 repeat_16(int8_t v0, int8_t v1, int8_t v2, int8_t v3, int8_t v4, int8_t v5, int8_t v6, int8_t v7, int8_t v8,
			int8_t v9, int8_t v10, int8_t v11, int8_t v12, int8_t v13, int8_t v14, int8_t v15) {
			int8_t returnValue[32]{ v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10,
				v11, v12, v13, v14, v15 };
			return returnValue;
		}

		inline SimdBase256& operator=(char other) {
			this->value = _mm256_set1_epi8(other);
			return *this;
		}

		inline SimdBase256(char other) {
			*this = other;
		}

		inline SimdBase256& operator=(int8_t values[32]) {
			*this = _mm256_loadu_epi8(values);
			return *this;
		}

		inline SimdBase256(int8_t values[32]) {
			*this = values;
		}

		inline SimdBase256& operator=(const char* values) {
			*this = _mm256_loadu_epi8(values);
			return *this;
		}

		inline SimdBase256(const char* values) {
			*this = values;
		}

		inline SimdBase256(int64_t value00, int64_t value01, int64_t value02, int64_t value03) {
			this->value = _mm256_set_epi64x(value03, value02, value01, value00);
		}

		inline SimdBase256(uint64_t value00, uint64_t value01, uint64_t value02, uint64_t value03) {
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

		inline void store(char dst[32]) const {
			return _mm256_storeu_epi8(dst, this->value);
		}

		inline uint64_t getUint64(size_t index) {
			switch (index) {
				case 0: {
					return static_cast<uint64_t>(_mm256_extract_epi64(this->value, 0));
				}
				case 1: {
					return static_cast<uint64_t>(_mm256_extract_epi64(this->value, 1));
				}
				case 2: {
					return static_cast<uint64_t>(_mm256_extract_epi64(this->value, 2));
				}
				case 3: {
					return static_cast<uint64_t>(_mm256_extract_epi64(this->value, 3));
				}
				default: {
					return static_cast<uint64_t>(_mm256_extract_epi64(this->value, 0));
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
					*this = _mm256_insert_epi64(this->value, value, 0);
					break;
				}
				case 1: {
					*this = _mm256_insert_epi64(this->value, value, 1);
					break;
				}
				case 2: {
					*this = _mm256_insert_epi64(this->value, value, 2);
					break;
				}
				case 3: {
					*this = _mm256_insert_epi64(this->value, value, 3);
					break;
				}
				default: {
					*this = _mm256_insert_epi64(this->value, value, 0);
					break;
				}
			}
		}

		inline operator __m256i&() {
			return this->value;
		}

		inline SimdBase256 operator|(SimdBase256 other) {
			return _mm256_or_si256(this->value, other);
		}

		inline SimdBase256 operator&(SimdBase256 other) {
			return _mm256_and_si256(this->value, other);
		}

		inline SimdBase256 operator^(SimdBase256 other) {
			return _mm256_xor_si256(this->value, other);
		}

		inline SimdBase256 operator<=(SimdBase256 other) {
			return _mm256_cmple_epi8_mask(this->value, other);
		}

		inline SimdBase256 operator+(SimdBase256 other) {
			return _mm256_add_epi8(this->value, other);
		}

		inline SimdBase256& operator|=(SimdBase256 other) {
			*this = *this | other;
			return *this;
		}

		inline SimdBase256& operator&=(SimdBase256 other) {
			*this = *this & other;
			return *this;
		}

		inline SimdBase256& operator^=(SimdBase256 other) {
			*this = *this ^ other;
			return *this;
		}

		SimdBase256 operator-(int32_t other) {
			auto currentValue = *this;
			if (other == 1) {
				currentValue = ~currentValue & 1;
			}
			return currentValue;
		}

		inline SimdBase256 operator==(SimdBase256 other) {
			return _mm256_cmpeq_epi8(this->value, other);
		}

		inline SimdBase256 operator==(char other) {
			return _mm256_cmpeq_epi8(this->value, _mm256_set1_epi8(other));
		}

		inline SimdBase256 operator<<(size_t amount) {
			int64_t values[4]{};
			values[0] = _mm256_extract_epi64(this->value, 0);
			values[1] = _mm256_extract_epi64(this->value, 1);
			values[2] = _mm256_extract_epi64(this->value, 2);
			values[3] = _mm256_extract_epi64(this->value, 3);
			SimdBase256 newValues{};
			newValues = _mm256_insert_epi64(newValues, (values[0] << (amount % 64)), 0);
			newValues = _mm256_insert_epi64(newValues, (values[1] << (amount % 64)) | ((values[0] & 1ull) << 63), 1);
			newValues = _mm256_insert_epi64(newValues, (values[2] << (amount % 64)) | ((values[1] & 1ull) << 63), 2);
			newValues = _mm256_insert_epi64(newValues, (values[3] << (amount % 64)) | ((values[2] & 1ull) << 63), 3);
			return newValues;
		}

		inline SimdBase256 operator>>(size_t amount) {
			int64_t values[4]{};
			values[0] = _mm256_extract_epi64(this->value, 0);
			values[1] = _mm256_extract_epi64(this->value, 1);
			values[2] = _mm256_extract_epi64(this->value, 2);
			values[3] = _mm256_extract_epi64(this->value, 3);
			SimdBase256 newValues{};
			newValues = _mm256_insert_epi64(newValues, (values[0] >> (amount % 64)) | ((values[1] & 1ull) >> 63), 0);
			newValues = _mm256_insert_epi64(newValues, (values[1] >> (amount % 64)) | ((values[2] & 1ull) >> 63), 1);
			newValues = _mm256_insert_epi64(newValues, (values[2] >> (amount % 64)) | ((values[3] & 1ull) >> 63), 2);
			newValues = _mm256_insert_epi64(newValues, (values[3] >> (amount % 64)), 3);
			return newValues;
		}

		inline SimdBase256 operator~() {
			SimdBase256 newValues{};
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->value, 0), 0);
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->value, 1), 1);
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->value, 2), 2);
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->value, 3), 3);
			return newValues;
		}

		inline SimdBase256 carrylessMultiplication(int64_t& prevInString) {
			SimdBase128 allOnes{ '\xFF' };
			auto inString00 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(0)), allOnes, 0)) ^ prevInString;
			prevInString = inString00 >> 63;
			auto inString01 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(1)), allOnes, 0)) ^ prevInString;
			prevInString = inString01 >> 63;
			auto inString02 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(2)), allOnes, 0)) ^ prevInString;
			prevInString = inString02 >> 63;
			auto inString03 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(3)), allOnes, 0)) ^ prevInString;
			prevInString = inString03 >> 63;
			return SimdBase256{ inString00, inString01, inString02, inString03 };
		}

		inline bool collectCarries(SimdBase256 other1, SimdBase256* result) {
			bool returnValue{};
			uint64_t returnValue64{};
			for (size_t x = 0; x < 4; ++x) {
				if (_addcarry_u64(0, this->getUint64(x), other1.getUint64(x), reinterpret_cast<unsigned long long*>(&returnValue64))) {
					returnValue = true;
				}
				result->insertInt64(returnValue64, x);
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

		inline SimdBase256 bitAndNot(SimdBase256 other) {
			return _mm256_andnot_si256(other, this->value);
		}

		inline SimdBase256 shuffle(SimdBase256 other) {
			return _mm256_shuffle_epi8(other, this->value);
		}

	  protected:
		__m256i value{};
	};

	class SimdBase256;

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

	struct JsonCharacterBlock {
		static inline JsonCharacterBlock classify(SimdBase256 in[8]);
		//  ASCII white-space ('\r','\n','\t',' ')
		inline SimdBase256 whitespace() const noexcept;
		// non-quote structural characters (comma, colon, braces, brackets)
		inline SimdBase256 op() const noexcept;
		// neither a structural character nor a white-space, so letters, numbers and quotes
		inline SimdBase256 scalar() const noexcept;

		SimdBase256 _whitespace;// ASCII white-space ('\r','\n','\t',' ')
		SimdBase256 _op;// structural characters (comma, colon, braces, brackets but not quotes)
	};

	inline SimdBase256 JsonCharacterBlock::whitespace() const noexcept {
		return _whitespace;
	}
	inline SimdBase256 JsonCharacterBlock::op() const noexcept {
		return _op;
	}
	inline SimdBase256 JsonCharacterBlock::scalar() const noexcept {
		return ~(op() | whitespace());
	}

	// This identifies structural characters (comma, colon, braces, brackets),
	// and ASCII white-space ('\r','\n','\t',' ').
	inline JsonCharacterBlock JsonCharacterBlock::classify(SimdBase256 in[8]) {
		// These lookups rely on the fact that anything < 127 will match the lower 4 bits, which is why
		// we can't use the generic lookup_16.
		char valuesNewOne[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
		SimdBase256 opTable{ valuesNewOne };
		SimdBase256 structural[8]{};
		for (size_t x = 0; x < 8; ++x) {
			auto valuesNew00 = in[x] | SimdBase256{ 0x20 };
			structural[x] = in[x].shuffle(opTable) == valuesNew00;
		}
		// The 6 operators (:,[]{}) have these values:
		//
		// , 2C
		// : 3A
		// [ 5B
		// { 7B
		// ] 5D
		// } 7D
		//
		// If you use | 0x20 to turn [ and ] into { and }, the lower 4 bits of each character is unique.
		// We exploit this, using a simd 4-bit lookup to tell us which character match against, and then
		// match it (against | 0x20).
		//
		// To prevent recognizing other characters, everything else gets compared with 0, which cannot
		// match due to the | 0x20.
		//
		// NOTE: Due to the | 0x20, this ALSO treats <FF> and <SUB> (control characters 0C and 1A) like ,
		// and :. This gets caught in stage 2, which checks the actual character to ensure the right
		// operators are in the right places.

		char valuesNew[32]{ ' ', static_cast<char>(100), static_cast<char>(100), static_cast<char>(100), static_cast<char>(17),
			static_cast<char>(100), 113, 2, static_cast<char>(100), '\t', '\n', static_cast<char>(112), static_cast<char>(100), '\r',
			static_cast<char>(100), static_cast<char>(100), ' ', static_cast<char>(100), static_cast<char>(100), static_cast<char>(100),
			static_cast<char>(17), static_cast<char>(100), 113, 2, static_cast<char>(100), '\t', '\n', static_cast<char>(112), static_cast<char>(100),
			'\r', static_cast<char>(100), static_cast<char>(100) };
		SimdBase256 whitespaceTable{ valuesNew };
		SimdBase256 whiteSpaceReal[8]{};
		for (size_t x = 0; x < 8; ++x) {
			whiteSpaceReal[x] = in[x].shuffle(whitespaceTable) == in[x];
		}


		return { convertSimdBytesToBits(whiteSpaceReal), convertSimdBytesToBits(structural) };
		
	}

	inline SimdBase256 prefixXor(SimdBase256 bitmask, int64_t prevInString) {
		// There should be no such thing with a processor supporting avx2
		// but not clmul.
		for (size_t x = 0; x < 4; ++x) {
			SimdBase128 allOnes{ '\xFF' };
			auto inString00 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, bitmask.getInt64(0)), allOnes, 0)) ^ prevInString;
			prevInString = inString00 >> 63;
			auto inString01 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, bitmask.getInt64(1)), allOnes, 0)) ^ prevInString;
			prevInString = inString01 >> 63;
			auto inString02 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, bitmask.getInt64(2)), allOnes, 0)) ^ prevInString;
			prevInString = inString02 >> 63;
			auto inString03 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, bitmask.getInt64(3)), allOnes, 0)) ^ prevInString;
			prevInString = inString03 >> 63;
			return SimdBase256{ inString00, inString01, inString02, inString03 };
		}
	}

	struct JsonStringBlock {
		// We spell out the constructors in the hope of resolving inlining issues with Visual Studio 20static_cast<char>(17)
		inline JsonStringBlock(SimdBase256 backslash, SimdBase256 escaped, SimdBase256 quote, SimdBase256 in_string)
			: _backslash(backslash), _escaped(escaped), _quote(quote), _in_string(in_string) {
		}

		// Escaped characters (characters following an escape() character)
		inline SimdBase256 escaped() {
			return _escaped;
		}
		// Escape characters (backslashes that are not escaped--i.e. in \\, includes only the first \)
		inline SimdBase256 escape() {
			return _backslash & ~_escaped;
		}
		// Real (non-backslashed) quotes
		inline SimdBase256 quote() {
			return _quote;
		}
		// Start quotes of strings
		inline SimdBase256 string_start() {
			return _quote & _in_string;
		}
		// End quotes of strings
		inline SimdBase256 string_end() {
			return _quote & ~_in_string;
		}
		// Only characters inside the string (not including the quotes)
		inline SimdBase256 string_content() {
			return _in_string & ~_quote;
		}
		// Return a mask of whether the given characters are inside a string (only works on non-quotes)
		inline SimdBase256 non_quote_inside_string(SimdBase256 mask) {
			return mask & _in_string;
		}
		// Return a mask of whether the given characters are inside a string (only works on non-quotes)
		inline SimdBase256 non_quote_outside_string(SimdBase256 mask) {
			return mask & ~_in_string;
		}
		// Tail of string (everything except the start quote)
		inline SimdBase256 string_tail() {
			return _in_string ^ _quote;
		}

		// backslash characters
		SimdBase256 _backslash;
		// escaped characters (backslashed--does not include the hex characters after \u)
		SimdBase256 _escaped;
		// real quotes (non-backslashed ones)
		SimdBase256 _quote;
		// string characters (includes start quote but not end quote)
		SimdBase256 _in_string;
	};

	// Scans blocks for string characters, storing the state necessary to do so
	class JsonStringScanner {
	  public:
		inline JsonStringBlock next(SimdBase256 in[8]);
		// Returns either UNCLOSED_STRING or SUCCESS
		inline ErrorCode finish();

	  private:
		// Intended to be defined by the implementation

		inline SimdBase256 findEscapedBranchless(SimdBase256 escape);
		inline SimdBase256 findEscaped(SimdBase256 escape);

		// Whether the last iteration was still inside a string (all 1's = true, all 0's = false).
		uint64_t prevInString = '\0';
		// Whether the first character of the next iteration is escaped.
		SimdBase256 prev_escaped = '\0';
	};

	//
	// Finds escaped characters (characters following \).
	//
	// Handles runs of backslashes like \\\" and \\\\" correctly (yielding 0101 and 01010, respectively).
	//
	// Does this by:
	// - Shift the escape mask to get potentially escaped characters (characters after backslashes).
	// - Mask escaped sequences that start on *even* bits with 1010101010 (odd bits are escaped, even bits are not)
	// - Mask escaped sequences that start on *odd* bits with 0101010101 (even bits are escaped, odd bits are not)
	//
	// To distinguish between escaped sequences starting on even/odd bits, it finds the start of all
	// escape sequences, filters out the ones that start on even bits, and adds that to the mask of
	// escape sequences. This causes the addition to clear out the sequences starting on odd bits (since
	// the start bit causes a carry), and leaves even-bit sequences alone.
	//
	// Example:
	//
	// text           |  \\\ | \\\"\\\" \\\" \\"\\" |
	// escape         |  xxx |  xx xxx  xxx  xx xx  | Removed overflow backslash; will | it into follows_escape
	// odd_starts     |  x   |  x       x       x   | escape & ~even_bits & ~follows_escape
	// even_seq       |     c|    cxxx     c xx   c | c = carry bit -- will be masked out later
	// invert_mask    |      |     cxxx     c xx   c| even_seq << 1
	// follows_escape |   xx | x xx xxx  xxx  xx xx | Includes overflow bit
	// escaped        |   x  | x x  x x  x x  x  x  |
	// desired        |   x  | x x  x x  x x  x  x  |
	// text           |  \\\ | \\\"\\\" \\\" \\"\\" |
	//
	inline SimdBase256 JsonStringScanner::findEscapedBranchless(SimdBase256 backslash) {
		// If there was overflow, pretend the first character isn't a backslash
		backslash &= ~prev_escaped;
		SimdBase256 follows_escape = backslash << 1 | prev_escaped;

		// Get sequences starting on even bits by clearing out the odd series using +
		SimdBase256 even_bits = 0x5555555555555555ULL;
		SimdBase256 odd_sequence_starts = backslash & ~even_bits & ~follows_escape;
		SimdBase256 sequences_starting_on_even_bits;
		prev_escaped = prev_escaped.collectCarries(odd_sequence_starts, &sequences_starting_on_even_bits);
		SimdBase256 invert_mask = sequences_starting_on_even_bits << 1;// The mask we want to return is the *escaped* bits, not escapes.

		// Mask every other backslashed character as an escaped character
		// Flip the mask for sequences that start on even bits, to correct them
		return (even_bits ^ invert_mask) & follows_escape;
	}

	//
	// Return a mask of all string characters plus end quotes.
	//
	// prev_escaped is overflow saying whether the next character is escaped.
	// prevInString is overflow saying whether we're still in a string.
	//
	// Backslash sequences outside of quotes will be detected in stage 2.
	//
	inline JsonStringBlock JsonStringScanner::next(SimdBase256 in[8]) {
		SimdBase256 results[8]{};
		for (size_t x = 0; x < 8; ++x) {
			results[x] = in[x] == '\\';
		}
		SimdBase256 backslash = convertSimdBytesToBits(results);
		SimdBase256 escaped = findEscaped(backslash);
		for (size_t x = 0; x < 8; ++x) {
			results[x] = in [x]== '"' & ~escaped;
		}
		SimdBase256 quote = convertSimdBytesToBits(results);
		
		//
		// prefixXor flips on bits inside the string (and flips off the end quote).
		//
		// Then we xor with prevInString: if we were in a string already, its effect is flipped
		// (characters inside strings are outside, and characters outside strings are inside).
		//
		SimdBase256 in_string = prefixXor(quote, prevInString);

		//
		// Check if we're still in a string at the end of the box so the next block will know
		//
		// right shift of a signed value expected to be well-defined and standard
		// compliant as of C++20, John Regher from Utah U. says this is fine code
		//
		prevInString = uint64_t(static_cast<int64_t>(in_string.getInt64(0)) >> 63);

		// Use ^ to turn the beginning quote off, and the end quote on.

		// We are returning a function-local object so either we get a move constructor
		// or we get copy elision.
		return JsonStringBlock(backslash, escaped, quote, in_string);
	}

	inline ErrorCode JsonStringScanner::finish() {
		if (prevInString) {
			return ErrorCode::Unclosed_String;
		}
		return ErrorCode::Success;
	}

	struct JsonBlock {
	  public:
		// We spell out the constructors in the hope of resolving inlining issues with Visual Studio 20static_cast<char>(17)
		inline JsonBlock(JsonStringBlock&& string, JsonCharacterBlock characters, SimdBase256  follows_potential_nonquote_scalar)
			: _string(std::move(string)), _characters(characters), _follows_potential_nonquote_scalar(follows_potential_nonquote_scalar) {
		}
		inline JsonBlock(JsonStringBlock string, JsonCharacterBlock characters, SimdBase256 follows_potential_nonquote_scalar)
			: _string(string), _characters(characters), _follows_potential_nonquote_scalar(follows_potential_nonquote_scalar) {
		}

		/**
   * The start of structurals.
   * In simdjson prior to v0.3, these were called the pseudo-structural characters.
   **/
		inline SimdBase256 structural_start() noexcept {
			return potential_structural_start() & ~_string.string_tail();
		}
		/** All JSON whitespace (i.e. not in a string) */
		inline SimdBase256 whitespace() noexcept {
			return non_quote_outside_string(_characters.whitespace());
		}

		// Helpers

		/** Whether the given characters are inside a string (only works on non-quotes) */
		inline SimdBase256 non_quote_inside_string(SimdBase256 mask) noexcept {
			return _string.non_quote_inside_string(mask);
		}
		/** Whether the given characters are outside a string (only works on non-quotes) */
		inline SimdBase256 non_quote_outside_string(SimdBase256 mask) noexcept {
			return _string.non_quote_outside_string(mask);
		}

		// string and escape characters
		JsonStringBlock _string;
		// whitespace, structural characters ('operators'), scalars
		JsonCharacterBlock _characters;
		// whether the previous character was a scalar
		SimdBase256 _follows_potential_nonquote_scalar;

	  private:
		// Potential structurals (i.e. disregarding strings)

		/**
   * structural elements ([,],{,},:, comma) plus scalar starts like 123, true and "abc".
   * They may reside inside a string.
   **/
		inline SimdBase256 potential_structural_start() const noexcept {
			return _characters.op() | potential_scalar_start();
		}
		/**
   * The start of non-operator runs, like 123, true and "abc".
   * It main reside inside a string.
   **/
		inline SimdBase256 potential_scalar_start() const noexcept {
			// The term "scalar" refers to anything except structural characters and white space
			// (so letters, numbers, quotes).
			// Whenever it is preceded by something that is not a structural element ({,},[,],:, ") nor a white-space
			// then we know that it is irrelevant structurally.
			return _characters.scalar() & ~follows_potential_scalar();
		}
		/**
   * Whether the given character is immediately after a non-operator like 123, true.
   * The characters following a quote are not included.
   */
		inline SimdBase256 follows_potential_scalar() const noexcept {
			// _follows_potential_nonquote_scalar: is defined as marking any character that follows a character
			// that is not a structural element ({,},[,],:, comma) nor a quote (") and that is not a
			// white space.
			// It is understood that within quoted region, anything at all could be marked (irrelevant).
			return _follows_potential_nonquote_scalar;
		}
	};

	/**
 * Scans JSON for important bits: structural characters or 'operators', strings, and scalars.
 *
 * The scanner starts by calculating two distinct things:
 * - string characters (taking \" into account)
 * - structural characters or 'operators' ([]{},:, comma)
 *   and scalars (runs of non-operators like 123, true and "abc")
 *
 * To minimize data dependency (a key component of the scanner's speed), it finds these in parallel:
 * in particular, the operator/scalar bit will find plenty of things that are actually part of
 * strings. When we're done, JsonBlock will fuse the two together by masking out tokens that are
 * part of a string.
 */
	class JsonScanner {
	  public:
		JsonScanner() {
		}
		inline JsonBlock next(SimdBase256 in[8]);
		// Returns either UNCLOSED_STRING or SUCCESS
		inline ErrorCode finish();

	  private:
		// Whether the last character of the previous iteration is part of a scalar token
		// (anything except whitespace or a structural character/'operator').
		SimdBase256 prev_scalar = '\0';
		JsonStringScanner string_scanner{};
	};


	//
	// Check if the current character immediately follows a matching character.
	//
	// For example, this checks for quotes with backslashes in front of them:
	//
	//     const uint64_t backslashed_quote = in.eq('"') & immediately_follows(in.eq('\'), prev_backslash);
	//
	inline SimdBase256 follows(SimdBase256 match, SimdBase256& overflow) {
		SimdBase256 result = match << 1 | overflow;
		overflow = match >> 63;
		return result;
	}

	inline JsonBlock JsonScanner::next(SimdBase256 in[8]) {
		JsonStringBlock strings = string_scanner.next(in);
		// identifies the white-space and the structural characters
		JsonCharacterBlock characters = JsonCharacterBlock::classify(in);
		// The term "scalar" refers to anything except structural characters and white spac
		// (so letters, numbers, quotes).
		// We want follows_scalar to mark anything that follows a non-quote scalar (so letters and numbers).
		//
		// A terminal quote should either be followed by a structural character (comma, brace, bracket, colon)
		// or nothing. However, we still want ' "a string"true ' to mark the 't' of 'true' as a potential
		// pseudo-structural character just like we would if we had  ' "a string" true '; otherwise we
		// may need to add an extra check when parsing strings.
		//
		// Performance: there are many ways to skin this cat.
		SimdBase256 nonquote_scalar = characters.scalar() & ~strings.quote();
		SimdBase256 follows_nonquote_scalar = follows(nonquote_scalar, prev_scalar);
		// We are returning a function-local object so either we get a move constructor
		// or we get copy elision.
		return JsonBlock(strings,// strings is a function-local object so either it moves or the copy is elided.
			characters, follows_nonquote_scalar);
	}

	inline ErrorCode JsonScanner::finish() {
		return string_scanner.finish();
	}

	template<size_t STEP_SIZE> struct BufBlockReader {
	  public:
		inline BufBlockReader(const uint8_t* _buf, size_t _len);
		inline size_t blockIndex();
		inline bool has_full_block() const;
		inline const uint8_t* full_block() const;
		/**
   * Get the last block, padded with spaces.
   *
   * There will always be a last block, with at least 1 byte, unless len == 0 (in which case this
   * function fills the buffer with spaces and returns 0. In particular, if len == STEP_SIZE there
   * will be 0 full_blocks and 1 remainder block with STEP_SIZE bytes and no spaces for padding.
   *
   * @return the number of effective characters in the last block.
   */
		inline size_t get_remainder(uint8_t* dst) const;
		inline void advance();

	  private:
		const uint8_t* buf;
		const size_t len;
		const size_t lenminusstep;
		size_t idx;
	};

	template<size_t STEP_SIZE>
	inline BufBlockReader<STEP_SIZE>::BufBlockReader(const uint8_t* _buf, size_t _len)
		: buf{ _buf }, len{ _len }, lenminusstep{ len < STEP_SIZE ? 0 : len - STEP_SIZE }, idx{ 0 } {
	}

	template<size_t STEP_SIZE> inline size_t BufBlockReader<STEP_SIZE>::blockIndex() {
		return idx;
	}

	template<size_t STEP_SIZE> inline bool BufBlockReader<STEP_SIZE>::has_full_block() const {
		return idx < lenminusstep;
	}

	template<size_t STEP_SIZE> inline const uint8_t* BufBlockReader<STEP_SIZE>::full_block() const {
		return &buf[idx];
	}

	template<size_t STEP_SIZE> inline size_t BufBlockReader<STEP_SIZE>::get_remainder(uint8_t* dst) const {
		if (len == idx) {
			return 0;
		}// memcpy(dst, null, 0) will trigger an error with some sanitizers
		std::memset(dst, 0x20, STEP_SIZE);// std::memset STEP_SIZE because it's more efficient to write out 8 or 16 bytes at once.
		std::memcpy(dst, buf + idx, len - idx);
		return len - idx;
	}

	template<size_t STEP_SIZE> inline void BufBlockReader<STEP_SIZE>::advance() {
		idx += STEP_SIZE;
	}

	class BitIndexer {
	  public:
		uint32_t* tail;

		inline BitIndexer(uint32_t* index_buf) : tail(index_buf) {
		}

		// flatten out values in 'bits' assuming that they are are to have values of idx
		// plus their position in the bitvector, and store these indexes at
		// base_ptr[base] incrementing base as we go
		// will potentially store extra values beyond end of valid bits, so base_ptr
		// needs to be large enough to handle this
		//
		// If the kernel sets SIMDJSON_CUSTOM_BIT_INDEXER, then it will provide its own
		// version of the code.

		inline void write(uint32_t idx, SimdBase256 bits) {
			// In some instances, the next branch is expensive because it is mispredicted.
			// Unfortunately, in other cases,
			// it helps tremendously.
			if (bits == 0)
				return;
			/**
     * Under recent x64 systems, we often have both a fast trailing zero
     * instruction and a fast 'clear-lower-bit' instruction so the following
     * algorithm can be competitive.
     */
			int cnt{}; 
			for (size_t x = 0; x < 4; ++x) {
				cnt += static_cast<int>(countOnes(bits.getUint64(x)));
				// Do the first 8 all together
				for (int i = 0; i < 8; i++) {
					this->tail[i] = idx + trailingZeroes(bits.getUint64(x));
					bits = clearLowestBit(bits.getUint64(x));
				}

				// Do the next 8 all together (we hope in most cases it won't happen at all
				// and the branch is easily predicted).
				if (cnt > 8) {
					for (int i = 8; i < 16; i++) {
						this->tail[i] = idx + trailingZeroes(bits.getUint64(x));
						bits = clearLowestBit(bits.getUint64(x));
					}

					// Most files don't have 16+ structurals per block, so we take several basically guaranteed
					// branch mispredictions here. 16+ structurals per block means either punctuation ({} [] , :)
					// or the start of a value ("abc" true 123) every four characters.
					if (cnt > 16) {
						int i = 16;
						do {
							this->tail[i] = idx + trailingZeroes(bits.getUint64(x));
							bits = clearLowestBit(bits.getUint64(x));
							i++;
						} while (i < cnt);
					}
				}
			}
			

			this->tail += cnt;
		}
	};

	class JsonStructuralIndexer {
	  public:
		/**
   * Find the important bits of JSON in a 128-byte chunk, and add them to structuralIndexes.
   *
   * @param partial Setting the partial parameter to true allows the find_structural_bits to
   *   tolerate unclosed strings. The caller should still ensure that the input is valid UTF-8. If
   *   you are processing substrings, you may want to call on a function like trimmed_length_safe_utf8.
   */
		template<size_t STEP_SIZE, typename SimdJsonValue> static ErrorCode index(int8_t* buf, size_t len, SimdJsonValue& parser) noexcept;

	  private:
		inline JsonStructuralIndexer(uint32_t* structuralIndexes);
		template<size_t STEP_SIZE> inline void step(int8_t* block, BufBlockReader<STEP_SIZE>& reader) noexcept;
		inline void next(SimdBase256 in[8], JsonBlock& block, size_t idx);
		template<typename SimdJsonValue> inline ErrorCode finish(SimdJsonValue& parser, size_t idx, size_t len);

		JsonScanner scanner{};
		BitIndexer indexer;
		SimdBase256 prev_structurals = '\0';
		SimdBase256 unescaped_chars_error = '\0';
	};

	inline JsonStructuralIndexer::JsonStructuralIndexer(uint32_t* structuralIndexes) : indexer{ structuralIndexes } {
	}

	// Skip the last character if it is partial
	inline size_t trim_partial_utf8(int8_t* buf, size_t len) {
		if (len < 3) {
			switch (len) {
				case 2:
					if (buf[len - 1] >= 0xc0) {
						return len - 1;
					}// 2-, 3- and 4-byte characters with only 1 byte left
					if (buf[len - 2] >= 0xe0) {
						return len - 2;
					}// 3- and 4-byte characters with only 2 bytes left
					return len;
				case 1:
					if (buf[len - 1] >= 0xc0) {
						return len - 1;
					}// 2-, 3- and 4-byte characters with only 1 byte left
					return len;
				case 0:
					return len;
			}
		}
		if (buf[len - 1] >= 0xc0) {
			return len - 1;
		}// 2-, 3- and 4-byte characters with only 1 byte left
		if (buf[len - 2] >= 0xe0) {
			return len - 2;
		}// 3- and 4-byte characters with only 1 byte left
		if (buf[len - 3] >= 0xf0) {
			return len - 3;
		}// 4-byte characters with only 3 bytes left
		return len;
	}

	//
	// PERF NOTES:
	// We pipe 2 inputs through these stages:
	// 1. Load JSON into registers. This takes a long time and is highly parallelizable, so we load
	//    2 inputs' worth at once so that by the time step 2 is looking for them input, it's available.
	// 2. Scan the JSON for critical data: strings, scalars and operators. This is the critical path.
	//    The output of step 1 depends entirely on this information. These functions don't quite use
	//    up enough CPU: the second half of the functions is highly serial, only using 1 execution core
	//    at a time. The second input's scans has some dependency on the first ones finishing it, but
	//    they can make a lot of progress before they need that information.
	// 3. Step 1 doesn't use enough capacity, so we run some extra stuff while we're waiting for that
	//    to finish: utf-8 checks and generating the output from the last iteration.
	//
	// The reason we run 2 inputs at a time, is steps 2 and 3 are *still* not enough to soak up all
	// available capacity with just one input. Running 2 at a time seems to give the CPU a good enough
	// workout.
	//
	template<size_t STEP_SIZE, typename SimdJsonValue> ErrorCode JsonStructuralIndexer::index(int8_t* buf, size_t len, SimdJsonValue& parser) noexcept {
		if (simdjson_unlikely(len > parser.capacity())) {
			return CAPACITY;
		}
		// We guard the rest of the code so that we can assume that len > 0 throughout.
		if (len == 0) {
			return ErrorCode::Empty;
		}
		BufBlockReader<STEP_SIZE> reader(buf, len);
		JsonStructuralIndexer indexer(parser.structuralIndexes.get());

		// Read all but the last block
		while (reader.has_full_block()) {
			indexer.step<STEP_SIZE>(reader.full_block(), reader);
		}
		// Take care of the last block (will always be there unless file is empty which is
		// not supposed to happen.)
		uint8_t block[STEP_SIZE];
		if (simdjson_unlikely(reader.get_remainder(block) == 0)) {
			return ErrorCode::Unexpected_Error;
		}
		indexer.step<STEP_SIZE>(block, reader);
		return indexer.finish(parser, reader.blockIndex(), len);
	}

	template<> inline void JsonStructuralIndexer::step<256>(int8_t* block, BufBlockReader<256>& reader) noexcept {
		SimdBase256 in[8](block);
		for (size_t x = 0; x < 8; ++x) {
			in[x] = block + 32;
		}
		JsonBlock block_1 = scanner.next(in);
		this->next(in, block_1, reader.blockIndex());
		reader.advance();
	}

	inline void JsonStructuralIndexer::next(SimdBase256 in[8], JsonBlock& block, size_t idx) {
		SimdBase256 results[8]{};
		for (size_t x = 0; x < 8; ++x) {
			results[x] = in[x] <= SimdBase256{ static_cast<char>(0x1F) };
		}
	
		SimdBase256 unescaped = convertSimdBytesToBits(results);
		indexer.write(uint32_t(idx - 64), prev_structurals);// Output *last* iteration's structurals to the parser
		prev_structurals = block.structural_start();
		unescaped_chars_error |= block.non_quote_inside_string(unescaped);
	}
	template<typename SimdJsonValue>
	inline ErrorCode JsonStructuralIndexer::finish(SimdJsonValue& parser, size_t idx, size_t len) {
		// Write out the final iteration's structurals
		indexer.write(uint32_t(idx - 64), prev_structurals);
		ErrorCode error = scanner.finish();

		if (unescaped_chars_error) {
			return ErrorCode::Unclosed_String;
		}
		parser.nStructuralIndexes = uint32_t(indexer.tail - parser.structuralIndexes.get());
		/***
   * The On Demand API requires special padding.
   *
   * This is related to https://github.com/simdjson/simdjson/issues/906
   * Basically, we want to make sure that if the parsing continues beyond the last (valid)
   * structural character, it quickly stops.
   * Only three structural characters can be repeated without triggering an error in JSON:  [,] and }.
   * We repeat the padding character (at 'len'). We don't know what it is, but if the parsing
   * continues, then it must be [,] or }.
   * Suppose it is ] or }. We backtrack to the first character, what could it be that would
   * not trigger an error? It could be ] or } but no, because you can't start a document that way.
   * It can't be a comma, a colon or any simple value. So the only way we could continue is
   * if the repeated character is [. But if so, the document must start with [. But if the document
   * starts with [, it should end with ]. If we enforce that rule, then we would get
   * ][[ which is invalid.
   *
   * This is illustrated with the test array_iterate_unclosed_error() on the following input:
   * R"({ "a": [,,)"
   **/
		parser.structuralIndexes[parser.nStructuralIndexes] = uint32_t(len);// used later in partial == stage1_mode::streaming_final
		parser.structuralIndexes[parser.nStructuralIndexes + 1] = uint32_t(len);
		parser.structuralIndexes[parser.nStructuralIndexes + 2] = 0;
		parser.nextStructuralIndex = 0;
		// a valid JSON file cannot have zero structural indexes - we should have found something
		if (parser.nStructuralIndexes == 0u) {
			return ErrorCode::Empty;
		}
		if (parser.structuralIndexes[parser.nStructuralIndexes - 1] > len) {
			return ErrorCode::Unexpected_Error;
		} /*
		if (partial == stage1_mode::streaming_partial) {
			// If we have an unclosed string, then the last structural
			// will be the quote and we want to make sure to omit it.
			if (have_unclosed_string) {
				parser.nStructuralIndexes--;
				// a valid JSON file cannot have zero structural indexes - we should have found something
				if (simdjson_unlikely(parser.nStructuralIndexes == 0u)) {
					return CAPACITY;
				}
			}
			// We truncate the input to the end of the last complete document (or zero).
			auto new_structuralIndexes = find_next_document_index(parser);
			if (new_structuralIndexes == 0 && parser.nStructuralIndexes > 0) {
				if (parser.structuralIndexes[0] == 0) {
					// If the buffer is partial and we started at index 0 but the document is
					// incomplete, it's too big to parse.
					return CAPACITY;
				} else {
					// It is possible that the document could be parsed, we just had a lot
					// of white space.
					parser.nStructuralIndexes = 0;
					return ErrorCode::Empty;
				}
			}

			parser.nStructuralIndexes = new_structuralIndexes;
		} else if (partial == stage1_mode::streaming_final) {
			if (have_unclosed_string) {
				parser.nStructuralIndexes--;
			}
			// We truncate the input to the end of the last complete document (or zero).
			// Because partial == stage1_mode::streaming_final, it means that we may
			// silently ignore trailing garbage. Though it sounds bad, we do it
			// deliberately because many people who have streams of JSON documents
			// will truncate them for processing. E.g., imagine that you are uncompressing
			// the data from a size file or receiving it in chunks from the network. You
			// may not know where exactly the last document will be. Meanwhile the
			// document_stream instances allow people to know the JSON documents they are
			// parsing (see the iterator.source() method).
			parser.nStructuralIndexes = find_next_document_index(parser);
			// We store the initial nStructuralIndexes so that the client can see
			// whether we used truncation. If initial_nStructuralIndexes == parser.nStructuralIndexes,
			// then this will query parser.structuralIndexes[parser.nStructuralIndexes] which is len,
			// otherwise, it will copy some prior index.
			parser.structuralIndexes[parser.nStructuralIndexes + 1] = parser.structuralIndexes[parser.nStructuralIndexes];
			// This next line is critical, do not change it unless you understand what you are
			// doing.
			parser.structuralIndexes[parser.nStructuralIndexes] = uint32_t(len);
			if (parser.nStructuralIndexes == 0u) {
				// We tolerate an unclosed string at the very end of the stream. Indeed, users
				// often load their data in bulk without being careful and they want us to ignore
				// the trailing garbage.
				return ErrorCode::Empty;
			}
		}*/
		return ErrorCode::Success;
	}

	struct JsonifierException : public std::runtime_error, std::string {
		inline JsonifierException(const std::string&, std::source_location = std::source_location::current()) noexcept;
	};

	constexpr int64_t JSON_VALUE_MASK = 0x00FFFFFFFFFFFFFF;
	constexpr uint32_t JSON_COUNT_MASK = 0xFFFFFF;

	template<typename RTy> void reverseByteOrder(RTy& net) {
		if constexpr (std::endian::native == std::endian::little) {
			switch (sizeof(RTy)) {
				case 1: {
					return;
				}
				case 2: {
					net =
						_mm256_extract_epi16(_mm256_shuffle_epi8(_mm256_insert_epi16(__m256i{}, net, 0), _mm256_insert_epi16(__m256i{}, 0x01, 0)), 0);
				}
				case 4: {
					net = _mm256_extract_epi32(
						_mm256_shuffle_epi8(_mm256_insert_epi32(__m256i{}, net, 0), _mm256_insert_epi32(__m256i{}, 0x10203, 0)), 0);
				}
				case 8: {
					net = _mm256_extract_epi64(
						_mm256_shuffle_epi8(_mm256_insert_epi64(__m256i{}, net, 0), _mm256_insert_epi64(__m256i{}, 0x102030405060708, 0)), 0);
				}
				default:
					return;
			}
		}
	}

	template<typename RTy> void storeBits(char* to, RTy num) {
		uint8_t byteSize{ 8 };
		reverseByteOrder<RTy>(num);
		for (uint32_t x = 0; x < sizeof(RTy); ++x) {
			to[x] = static_cast<uint8_t>(num >> (byteSize * x));
		}
	}

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
			this->maxNumberOfMs.store(maxNumberOfMsNew);
			this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
		}

		TTy totalTimePassed() {
			TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
			TTy elapsedTime = currentTime - this->startTime.load();
			return elapsedTime;
		}

		TTy getTotalWaitTime() {
			return this->maxNumberOfMs.load();
		}

		bool hasTimePassed() {
			TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
			TTy elapsedTime = currentTime - this->startTime.load();
			if (elapsedTime >= this->maxNumberOfMs.load()) {
				return true;
			} else {
				return false;
			}
		}

		void resetTimer() {
			this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
		}

	  protected:
		std::atomic<TTy> maxNumberOfMs{ TTy{ 0 } };
		std::atomic<TTy> startTime{ TTy{ 0 } };
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class EtfType : uint8_t {
		New_Float_Ext = 70,
		Small_Integer_Ext = 97,
		Integer_Ext = 98,
		Atom_Ext = static_cast<char>(100),
		Nil_Ext = 106,
		String_Ext = 107,
		List_Ext = 108,
		Binary_Ext = 109,
		Small_Big_Ext = 110,
		Small_Atom_Ext = 115,
		Map_Ext = 116,
	};

	template<typename Ty>
	concept IsEnum = std::is_enum<Ty>::value;

	struct EnumConverter {
		template<IsEnum EnumType> EnumConverter& operator=(std::vector<EnumType> data) {
			for (auto& value: data) {
				this->vector.emplace_back(std::move(static_cast<uint64_t>(value)));
			}
			return *this;
		};

		template<IsEnum EnumType> EnumConverter(std::vector<EnumType> data) {
			*this = data;
		};

		template<IsEnum EnumType> EnumConverter& operator=(EnumType data) {
			this->integer = static_cast<uint64_t>(data);
			return *this;
		};

		template<IsEnum EnumType> EnumConverter(EnumType data) {
			*this = data;
		};

		operator std::vector<uint64_t>() const noexcept;

		operator uint64_t() const noexcept;

		bool isItAVector() const noexcept;

	  protected:
		std::vector<uint64_t> vector{};
		bool vectorType{ false };
		uint64_t integer{};
	};

	enum class JsonType : int8_t { Object = 1, Array = 2, String = 3, Float = 4, Uint64 = 5, Int64 = 6, Bool = 7, Null = 8 };

	enum class JsonifierSerializeType { Etf = 0, Json = 1 };

	class Jsonifier;

	class JsonSerializer;

	template<typename Ty>
	concept IsConvertibleToJsonifier = std::convertible_to<Ty, Jsonifier>;

	template<typename Ty>
	concept IsConvertibleToJsonSerializer = std::convertible_to<Ty, JsonSerializer>;

	class SimdJsonValue;

	class Jsonifier {
	  public:
		friend class JsonSerializer;
		using MapAllocatorType = std::allocator<std::pair<const std::string, Jsonifier>>;
		template<typename OTy> using AllocatorType = std::allocator<OTy>;
		template<typename OTy> using AllocatorTraits = std::allocator_traits<AllocatorType<OTy>>;
		using ObjectType = std::map<std::string, Jsonifier, std::less<>, MapAllocatorType>;
		using ArrayType = std::vector<Jsonifier, AllocatorType<Jsonifier>>;
		using StringType = std::string_view;
		using FloatType = double;
		using UintType = uint64_t;
		using IntType = int64_t;
		using BoolType = bool;

		union JsonValue {
			JsonValue() noexcept = default;
			JsonValue& operator=(JsonValue&&) noexcept = delete;
			JsonValue(JsonValue&&) noexcept = delete;
			JsonValue& operator=(const JsonValue&) noexcept = delete;
			JsonValue(const JsonValue&) noexcept = delete;
			ObjectType* object;
			StringType* string;
			ArrayType* array;
			FloatType numberDouble;
			UintType numberUint;
			IntType numberInt;
			BoolType boolean;
		};

		Jsonifier() noexcept = default;

		template<IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::vector<OTy>&& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& value: data) {
				this->jsonValue.array->emplace_back(std::move(value));
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier(std::vector<OTy>&& data) noexcept {
			*this = std::move(data);
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::vector<OTy>& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& value: data) {
				this->jsonValue.array->emplace_back(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier(std::vector<OTy>& data) noexcept {
			*this = data;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::unordered_map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = std::move(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::unordered_map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::unordered_map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = value;
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::unordered_map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = std::move(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = value;
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsEnum Ty> Jsonifier& operator=(Ty data) noexcept {
			this->jsonValue.numberUint = static_cast<uint64_t>(data);
			this->type = JsonType::Uint64;
			return *this;
		}

		template<IsEnum Ty> Jsonifier(Ty data) noexcept {
			*this = data;
		}

		Jsonifier& operator=(ErrorCode data);

		Jsonifier(ErrorCode data);

		Jsonifier& operator=(ObjectType&& data) noexcept;

		Jsonifier(ObjectType&& data) noexcept;

		Jsonifier& operator=(const ObjectType& data) noexcept;

		Jsonifier(const ObjectType& data) noexcept;

		Jsonifier& operator=(Jsonifier&& data) noexcept;

		Jsonifier(Jsonifier&& data) noexcept;

		Jsonifier& operator=(const Jsonifier& data) noexcept;

		Jsonifier(const Jsonifier& data) noexcept;

		operator std::string_view() noexcept;

		void refreshString(JsonifierSerializeType OpCode);

		bool contains(std::string& key);

		Jsonifier& operator=(EnumConverter&& data) noexcept;
		Jsonifier(EnumConverter&& data) noexcept;

		Jsonifier& operator=(const EnumConverter& data) noexcept;
		Jsonifier(const EnumConverter& data) noexcept;

		Jsonifier& operator=(std::string&& data) noexcept;
		Jsonifier(std::string&& data) noexcept;

		Jsonifier& operator=(const std::string& data) noexcept;
		Jsonifier(const std::string& data) noexcept;

		Jsonifier& operator=(std::string_view&& data) noexcept;
		Jsonifier(std::string_view&& data) noexcept;

		Jsonifier& operator=(std::string_view& data) noexcept;
		Jsonifier(std::string_view& data) noexcept;

		Jsonifier& operator=(const char* data) noexcept;
		Jsonifier(const char* data) noexcept;

		Jsonifier& operator=(double data) noexcept;
		Jsonifier(double data) noexcept;

		Jsonifier& operator=(float data) noexcept;
		Jsonifier(float data) noexcept;

		Jsonifier& operator=(uint64_t data) noexcept;
		Jsonifier(uint64_t data) noexcept;

		Jsonifier& operator=(uint32_t data) noexcept;
		Jsonifier(uint32_t data) noexcept;

		Jsonifier& operator=(uint16_t data) noexcept;
		Jsonifier(uint16_t data) noexcept;

		Jsonifier& operator=(uint8_t data) noexcept;
		Jsonifier(uint8_t data) noexcept;

		Jsonifier& operator=(int64_t data) noexcept;
		Jsonifier(int64_t data) noexcept;

		Jsonifier& operator=(int32_t data) noexcept;
		Jsonifier(int32_t data) noexcept;

		Jsonifier& operator=(int16_t data) noexcept;
		Jsonifier(int16_t data) noexcept;

		Jsonifier& operator=(int8_t data) noexcept;
		Jsonifier(int8_t data) noexcept;

		Jsonifier& operator=(bool data) noexcept;
		Jsonifier(bool data) noexcept;

		Jsonifier& operator=(JsonType TypeNew) noexcept;
		Jsonifier(JsonType type) noexcept;

		Jsonifier& operator=(std::nullptr_t) noexcept;
		Jsonifier(std::nullptr_t data) noexcept;

		Jsonifier& operator[](std::string_view);

		Jsonifier& operator[](typename ObjectType::key_type key);

		Jsonifier& operator[](const char*);

		Jsonifier& operator[](uint64_t index);

		template<typename Ty> Ty getValue() {
			return Ty{};
		}

		JsonType getType() noexcept;

		bool parseString(std::string&) noexcept;

		size_t size() noexcept;

		Jsonifier& emplaceBack(Jsonifier&& data) noexcept;
		Jsonifier& emplaceBack(Jsonifier& data) noexcept;

		~Jsonifier() noexcept;

	  protected:
		std::unique_ptr<SimdJsonValue> parser{};
		JsonType type{ JsonType::Null };
		JsonValue jsonValue{};
		std::string string{};

		void serializeJsonToEtfString(const Jsonifier* jsonData);

		void serializeJsonToJsonString(const Jsonifier* jsonData);

		void writeJsonObject(const ObjectType& ObjectNew);

		void writeJsonArray(const ArrayType& Array);

		void writeJsonString(const StringType& StringNew);

		void writeJsonFloat(const FloatType x);

		template<typename NumberType,
			std::enable_if_t<
				std::is_integral<NumberType>::value || std::is_same<NumberType, uint64_t>::value || std::is_same<NumberType, int64_t>::value, int> =
				0>
		void writeJsonInt(NumberType Int) {
			auto IntNew = std::to_string(Int);
			this->writeString(IntNew.data(), IntNew.size());
		}

		void writeJsonBool(const BoolType ValueNew);

		void writeJsonNull();

		void writeEtfObject(const ObjectType& jsonData);

		void writeEtfArray(const ArrayType& jsonData);

		void writeEtfString(const StringType& jsonData);

		void writeEtfUint(const UintType jsonData);

		void writeEtfInt(const IntType jsonData);

		void writeEtfFloat(const FloatType jsonData);

		void writeEtfBool(const BoolType jsonData);

		void writeEtfNull();

		void writeString(const char* data, std::size_t tapeLength);

		void writeCharacter(const char Char);

		void appendBinaryExt(std::string_view bytes, uint32_t sizeNew);

		void appendUnsignedLongLong(const uint64_t value);

		void appendNewFloatExt(const double FloatValue);

		void appendSmallIntegerExt(uint8_t value);

		void appendListHeader(const uint32_t sizeNew);

		void appendMapHeader(const uint32_t sizeNew);

		void appendIntegerExt(const uint32_t value);

		void appendBool(bool data);

		void appendVersion();

		void appendNilExt();

		void appendNil();

		void setValue(JsonType TypeNew);

		void destroy() noexcept;
	};

	template<> inline Jsonifier::ObjectType Jsonifier::getValue() {
		if (this->type != JsonType::Object) {
			return Jsonifier::ObjectType{};
		}
		return std::move(*this->jsonValue.object);
	}

	template<> inline Jsonifier::ArrayType Jsonifier::getValue() {
		if (this->type != JsonType::Array) {
			return Jsonifier::ArrayType{};
		}
		return std::move(*this->jsonValue.array);
	}

	template<> inline Jsonifier::StringType Jsonifier::getValue() {
		if (this->type != JsonType::String) {
			return Jsonifier::StringType{};
		}
		return std::move(*this->jsonValue.string);
	}

	template<> inline Jsonifier::FloatType Jsonifier::getValue() {
		return this->jsonValue.numberDouble;
	}

	template<> inline Jsonifier::UintType Jsonifier::getValue() {
		return this->jsonValue.numberUint;
	}

	template<> inline Jsonifier::IntType Jsonifier::getValue() {
		return this->jsonValue.numberInt;
	}

	template<> inline Jsonifier::BoolType Jsonifier::getValue() {
		return this->jsonValue.boolean;
	}

	class escapeJsonString;

	inline std::ostream& operator<<(std::ostream& out, const escapeJsonString& str);

	class escapeJsonString {
	  public:
		escapeJsonString(std::string_view _str) noexcept : str{ _str } {
		}
		operator std::string() const noexcept {
			std::stringstream s;
			s << *this;
			return s.str();
		}

	  private:
		std::string_view str;
		friend std::ostream& operator<<(std::ostream& out, const escapeJsonString& unescaped);
	};

	inline std::ostream& operator<<(std::ostream& out, const escapeJsonString& unescaped) {
		for (size_t i = 0; i < unescaped.str.length(); i++) {
			switch (unescaped.str[i]) {
				case '\b':
					out << "\\b";
					break;
				case '\f':
					out << "\\f";
					break;
				case '\n':
					out << "\\n";
					break;
				case '\r':
					out << "\\r";
					break;
				case '\"':
					out << "\\\"";
					break;
				case '\t':
					out << "\\t";
					break;
				case '\\':
					out << "\\\\";
					break;
				default:
					if (static_cast<unsigned char>(unescaped.str[i]) <= 0x1F) {
						// TODO can this be done once at the beginning, or will it mess up << char?
						std::ios::fmtflags f(out.flags());
						out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << int(unescaped.str[i]);
						out.flags(f);
					} else {
						out << unescaped.str[i];
					}
			}
		}
		return out;
	}

	inline bool dumpRawTape(std::ostream& os, uint64_t* tape, const uint8_t* stringBuffer) noexcept {
		uint32_t string_length;
		size_t tape_idx = 0;
		uint64_t tape_val = tape[tape_idx];
		uint8_t type = uint8_t(tape_val >> 56);
		os << tape_idx << " : " << type;
		tape_idx++;
		size_t how_many = 0;
		if (type == 'r') {
			how_many = size_t(tape_val & JSON_VALUE_MASK);
		} else {
			// Error: no starting root node?
			return false;
		}
		os << "\t// pointing to " << how_many << " (right after last node)\n";
		uint64_t payload;
		for (; tape_idx < how_many; tape_idx++) {
			os << tape_idx << " : ";
			tape_val = tape[tape_idx];
			payload = tape_val & JSON_VALUE_MASK;
			type = uint8_t(tape_val >> 56);
			switch (type) {
				case '"':// we have a string
					os << "string \"";
					std::memcpy(&string_length, stringBuffer + payload, sizeof(uint32_t));
					os << escapeJsonString(std::string_view(reinterpret_cast<const char*>(stringBuffer + payload + sizeof(uint32_t)), string_length));
					os << '"';
					os << '\n';
					break;
				case 'l':// we have a long int
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					os << "integer " << static_cast<int64_t>(tape[++tape_idx]) << "\n";
					break;
				case 'u':// we have a long uint
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					os << "unsigned integer " << tape[++tape_idx] << "\n";
					break;
				case 'd':// we have a double
					os << "float ";
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					double answer;
					std::memcpy(&answer, &tape[++tape_idx], sizeof(answer));
					os << answer << '\n';
					break;
				case 'n':// we have a null
					os << "null\n";
					break;
				case 't':// we have a true
					os << "true\n";
					break;
				case 'f':// we have a false
					os << "false\n";
					break;
				case '{':// we have an object
					os << "{\t// pointing to next tape location " << uint32_t(payload) << " (first node after the scope), "
					   << " saturated count " << ((payload >> 32) & JSON_COUNT_MASK) << "\n";
					break;
				case '}':// we end an object
					os << "}\t// pointing to previous tape location " << uint32_t(payload) << " (start of the scope)\n";
					break;
				case '[':// we start an array
					os << "[\t// pointing to next tape location " << uint32_t(payload) << " (first node after the scope), "
					   << " saturated count " << ((payload >> 32) & JSON_COUNT_MASK) << "\n";
					break;
				case ']':// we end an array
					os << "]\t// pointing to previous tape location " << uint32_t(payload) << " (start of the scope)\n";
					break;
				case 'r':// we start and end with the root node
					// should we be hitting the root node?
					return false;
				default:
					return false;
			}
		}
		tape_val = tape[tape_idx];
		payload = tape_val & JSON_VALUE_MASK;
		type = uint8_t(tape_val >> 56);
		os << tape_idx << " : " << type << "\t// pointing to " << payload << " (start root)\n";
		return true;
	}


	class JsonParser {
	  public:

		JsonParser& operator=(JsonParser&& other) {
			this->currentStructuralCount = other.currentStructuralCount;
			this->currenPositionInTape = other.currenPositionInTape;
			other.currenPositionInTape = 0;
			other.currentStructuralCount = 0;
			other.currentStringSize = 0;
			this->stringView = other.stringView;
			other.stringView = nullptr;
			this->currentStringSize = other.currentStringSize;
			this->stringBuffer = std::move(other.stringBuffer);
			this->ptrs = std::move(other.ptrs);
			return *this;
		}

		JsonParser& operator=(const JsonParser&) = delete;
		JsonParser(const JsonParser&) = delete;

		JsonParser(JsonParser&& other) {
			*this = std::move(other);
		}

		JsonParser() noexcept {};

		JsonParser(uint64_t* startingPtr, size_t currentStructuralCountNew, const char* stringViewNew) {
			this->ptrs = std::make_unique<uint64_t[]>(currentStructuralCountNew);
			this->currentStructuralCount = currentStructuralCountNew;
			for (size_t x = 0; x < currentStructuralCount; ++x) {
				this->ptrs[x] = startingPtr[x];
			}
			this->stringView = stringViewNew;
		}

		template<typename OTy> OTy getValue();

		template<> double getValue() {
			double returnValue{};
			return returnValue;
		}

		template<> int32_t getValue() {
			int32_t returnValue{};
			return returnValue;
		}

		template<> int8_t getValue() {
			int8_t returnValue{};
			return returnValue;
		}

		template<> std::string getValue() {
			std::string returnValue{ reinterpret_cast<char*>(this->stringView[(this->ptrs[this->currenPositionInTape - 1] & JSON_VALUE_MASK)]),
				static_cast<size_t>(
					(this->ptrs[this->currenPositionInTape] & JSON_VALUE_MASK) - (this->ptrs[this->currenPositionInTape - 1] & JSON_VALUE_MASK)) };
			//std::cout << "RETURN VALUE: " << returnValue << std::endl;
			return returnValue;
		}

		template<> std::vector<JsonParser> getValue() {
			std::vector<JsonParser> returnValue{};
			auto newValue = (this->ptrs[this->currenPositionInTape] >> 56);
			//std::cout << "CURRENT INDEX'S VALUE: 0202 " << newValue << std::endl;
			//std::cout << "CURRENT INDEX: 0202 " << (this->ptrs[this->currenPositionInTape - 1] & JSON_COUNT_MASK) << std::endl;
			return std::vector<JsonParser>{};
		}
		
		JsonParser& operator[](const std::string& key) {
			dumpRawTape(std::cout, this->ptrs.get(), reinterpret_cast<const uint8_t*>(this->stringView));
			
			auto newValue = (this->ptrs[this->currenPositionInTape++] >> 56);
			//std::cout << "CURRENT INDEX'S VALUE: " << newValue << std::endl;
			if (newValue == 'r') {
				//std::cout << "CURRENT INDEX: " << (this->ptrs[this->currenPositionInTape - 1] & JSON_VALUE_MASK) << std::endl;
				return *this;
			}
			if (newValue == '[') {
				//std::cout << "CURRENT INDEX: " << (this->ptrs[this->currenPositionInTape - 1] & JSON_COUNT_MASK) << std::endl;
				return *this;
			}
			if (newValue == '{') {
				//std::cout << "CURRENT INDEX: " << (this->ptrs[this->currenPositionInTape - 1] & JSON_COUNT_MASK) << std::endl;
				return *this;
			}
			if (newValue == '\"') {
				//std::cout << "CURRENT INDEX: " << (this->ptrs[this->currenPositionInTape - 1] & JSON_VALUE_MASK) << std::endl;
				return *this;
			}
			return *this;
		};

		const char* getStringView() {
			return this->stringView;
		}

		char* getStringViewNew() {
			return this->stringBuffer.get();
		}

		JsonParser(ErrorCode error) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " + std::to_string(( int32_t )error) };
		}

		void reset(size_t count, size_t stringSizeNew, const char* newStringView) {
			if (count > this->currentStructuralCount) {
				this->ptrs = std::make_unique<uint64_t[]>(count);
				this->currentStructuralCount = count;
			}
			if (stringSizeNew > this->currentStringSize) {
				this->stringBuffer = std::make_unique<char[]>(count);
				this->currentStringSize = stringSizeNew;
			}
			this->stringView = newStringView;
			this->currenPositionInTape = 0;
		}

		operator uint64_t*() {
			return this->ptrs.get();
		}

		void setTapeCount(size_t count) {
			this->currentStructuralCount = count;
		}

	  protected:
		std::unique_ptr<char[]> stringBuffer{ nullptr };
		std::unique_ptr<uint64_t[]> ptrs{};
		const char* stringView{ nullptr };
		size_t currentStructuralCount{};
		size_t currenPositionInTape{};
		size_t currentStringSize{};
	};

	class SimdStringSection {
	  public:
		inline SimdStringSection() noexcept = default;

		inline void packStringIntoValue(SimdBase256* theValue, const char string[32]) {
			for (size_t x = 0; x < 32; ++x) {
				*theValue = string;
			}
		}

		inline uint64_t addTapeValues(uint64_t* tapePtrs, uint64_t* theBits, size_t currentIndexNew, size_t currentIndexIntoString,
			size_t& currentIndexIntoTape) {
			uint64_t value = static_cast<uint64_t>(__popcnt64(*theBits));
			for (int i = 0; i < value; i++) {
				tapePtrs[currentIndexIntoTape++] = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString;
				*theBits = _blsr_u64(*theBits);
			}
			
			return value;
		}

		inline size_t getStructuralIndices(uint64_t* currentPtr, size_t currentIndex, size_t& currentIndexIntoTape) {
			size_t returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				auto newValue = this->S256.getUint64(x);
				returnValue += this->addTapeValues(currentPtr, &newValue, x, currentIndex, currentIndexIntoTape);
			}
			return returnValue;
		}

		inline SimdBase256 collectWhiteSpace() {
			char valuesNew[32]{ ' ', static_cast<char>(100), static_cast<char>(100), static_cast<char>(100), static_cast<char>(17), static_cast<char>(100), 113, 2, static_cast<char>(100), '\t', '\n', static_cast<char>(112), static_cast<char>(100), '\r', static_cast<char>(100), static_cast<char>(100), ' ', static_cast<char>(100), static_cast<char>(100), static_cast<char>(100), static_cast<char>(17), static_cast<char>(100), 113, 2,
				static_cast<char>(100), '\t', '\n', static_cast<char>(112), static_cast<char>(100), '\r', static_cast<char>(100), static_cast<char>(100) };
			SimdBase256 whitespaceTable{ valuesNew };
			SimdBase256 whiteSpaceReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				whiteSpaceReal[x] = this->values[x].shuffle(whitespaceTable) == this->values[x];
			}
			return convertSimdBytesToBits(whiteSpaceReal);
		}

		inline SimdBase256 collectStructuralCharacters() {
			char valuesNew[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
			SimdBase256 opTable{ valuesNew };
			SimdBase256 structural[8]{};
			for (size_t x = 0; x < 8; ++x) {
				auto valuesNew00 = this->values[x] | SimdBase256{ 0x20 };
				structural[x] = this->values[x].shuffle(opTable) == valuesNew00;
			}

			return convertSimdBytesToBits(structural);
		}

		inline SimdBase256 collectQuotedRange(int64_t& prevInString) {
			SimdBase256 backslashes = _mm256_set1_epi8('\\');
			SimdBase256 backslashesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				backslashesReal[x] = this->values[x] == backslashes;
			}

			auto B256 = convertSimdBytesToBits(backslashesReal);

			SimdBase256 E{ _mm256_set1_epi8(0b01010101) };
			SimdBase256 O{ _mm256_set1_epi8(0b10101010) };
			this->S256 = B256.bitAndNot(B256 << 1);
			auto ES = E & this->S256;
			SimdBase256 EC{};
			B256.collectCarries(ES, &EC);
			auto ECE = EC.bitAndNot(B256);
			auto OD1 = ECE.bitAndNot(E);
			auto OS = this->S256 & O;
			auto OC = B256 + OS;
			auto OCE = OC.bitAndNot(B256);
			auto OD2 = OCE & E;
			auto OD = OD1 | OD2;
			this->Q256 = this->Q256.bitAndNot(OD);
			return this->Q256.carrylessMultiplication(prevInString);
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
			this->S256 = this->S256.bitAndNot(this->R256);
			this->S256 = this->S256 | this->Q256;
			auto P = this->S256 | this->W256;
			P = P << 1;
			P &= (~W256).bitAndNot(this->R256);
			this->S256 = this->S256 | P;
			return S256.bitAndNot((this->Q256.bitAndNot(this->R256)));
		}

		inline SimdStringSection(const char* valueNew, int64_t& prevInString) {
			this->packStringIntoValue(&this->values[0], valueNew);
			this->packStringIntoValue(&this->values[1], valueNew + 32);
			this->packStringIntoValue(&this->values[2], valueNew + 64);
			this->packStringIntoValue(&this->values[3], valueNew + 96);
			this->packStringIntoValue(&this->values[4], valueNew + 128);
			this->packStringIntoValue(&this->values[5], valueNew + 160);
			this->packStringIntoValue(&this->values[6], valueNew + 192);
			this->packStringIntoValue(&this->values[7], valueNew + 224);

			this->Q256 = this->collectQuotes();
			this->R256 = this->collectQuotedRange(prevInString);
			this->W256 = this->collectWhiteSpace();
			this->S256 = this->collectStructuralCharacters();
			this->S256 = this->collectFinalStructurals();
			//this->S256.printBits("FINAL BITS: ");
		}

	  protected:
		SimdBase256 values[8]{};
		SimdBase256 Q256{};
		SimdBase256 W256{};
		SimdBase256 R256{};
		SimdBase256 S256{};
	};

	struct OpenContainer {
		uint32_t tapeIndex{};
		uint32_t count{};
	};

	class SimdJsonValue {
	  public:
		std::unique_ptr<uint32_t[]> structuralIndexes{};
		uint32_t nextStructuralIndex{};
		uint32_t nStructuralIndexes{};

		inline SimdJsonValue() {}

		int32_t round(int32_t a, int32_t n) {
			return (((a) + (( n )-1)) & ~(( n )-1));
		}

		inline ErrorCode allocate(size_t capacity, const char*stringViewNew) noexcept {
			if (capacity == 0) {
				this->tape.reset(0, 0, nullptr);
				this->allocatedCapacity = 0;
				return ErrorCode::Success;
			}

			
			if (!(this->tape.getStringViewNew() && this->tape.operator uint64_t*())) {
				this->allocatedCapacity = 0;
				this->tape.reset(0, 0, nullptr);
				return ErrorCode::MemAlloc;
			}
			this->isArray = std::make_unique<bool[]>(12);
			this->openContainers = std::make_unique<OpenContainer[]>(this->maxDepth);
			this->allocatedCapacity = capacity;
			return ErrorCode::Success;
		}

		inline void generateJsonEvents(const char* stringNew, size_t stringLength) {
			if (stringNew) {
				if (stringLength == 0) {
					throw JsonifierException{ "Failed to parse as the string size is 0." };
				}
				this->stringLengthRaw = stringLength;
				size_t tapeCapacity = round(stringLength + 3, 64);
				size_t stringCapacity = round(5 * stringLength / 3 + 64, 64);
				this->tape.reset(tapeCapacity, stringCapacity, stringNew);
				this->tapeLength = 0;
				if (this->allocatedCapacity < this->stringLengthRaw) {
					if (this->allocate(stringLength, stringNew) != ErrorCode::Success) {
						throw JsonifierException{ "Failed to allocate properly!" };
					}
				}
				int64_t stringSize = this->allocatedCapacity;
				uint32_t collectedSize{};
				size_t tapeCurrentIndex{ 0 };
				int64_t prevInString{};
				while (stringSize > 0) {
					SimdStringSection section(this->tape.getStringView() + collectedSize, prevInString);
					auto indexCount = section.getStructuralIndices(this->tape.operator uint64_t*(), collectedSize, tapeCurrentIndex);
					this->tapeLength += indexCount;
					stringSize -= 256;
					collectedSize += 256;
				}
				//this->tapeLength -= 2;
				this->tape.setTapeCount(this->tapeLength);
			}
		}

		inline ~SimdJsonValue() noexcept {};

		inline char* getStringView() {
			return ( char* )this->tape.getStringView();
		}

		inline uint8_t* getStringViewNew() {
			return reinterpret_cast<uint8_t*>(this->tape.getStringViewNew());
		}

		OpenContainer* getOpenContainers() {
			return this->openContainers.get();
		}

		inline uint32_t& getCurrentDepth() {
			return this->depth;
		}

		inline uint64_t* getStructuralIndexes() {
			return this->tape;
		}

		inline JsonParser getJsonData(std::string& string);

		inline uint32_t getMaxDepth() {
			return this->maxDepth;
		}

		inline size_t getTapeLength() {
			return this->tapeLength;
		}

		inline bool* getIsArray() {
			return this->isArray.get();
		}
		
	  protected:
		std::unique_ptr<OpenContainer[]> openContainers{};
		std::unique_ptr<bool[]> isArray{};
		JsonParser tape{ 0, 0, nullptr };
		size_t allocatedCapacity{};
		size_t stringLengthRaw{};
		uint32_t maxDepth{ 500 };
		size_t tapeLength{ 0 };
		uint32_t depth{ 0 };
	};

	enum class TapeType : uint8_t {
		Root = 'r',
		Start_Array = '[',
		Start_Object = '{',
		End_Array = ']',
		End_Object = '}',
		String = '"',
		Int64 = 'l',
		Uint64 = 'u',
		Double = 'd',
		True_Value = 't',
		False_Value = 'f',
		Null_Value = 'n'
	};

	class TapeBuilder;

	class JsonIterator {
	  public:
		uint64_t* nextStructural{ nullptr };
		uint32_t nextStructuralIndex{};
		const uint8_t* buf{ nullptr };
		SimdJsonValue* masterParser;
		
		inline ErrorCode walkDocument(TapeBuilder&& visitor) noexcept;

		inline JsonIterator(SimdJsonValue* masterParserNew, size_t start_structural_index);

		inline const uint8_t* peek() const noexcept;

		inline const uint8_t* advance() noexcept;

		inline size_t remainingLen() const noexcept;

		inline bool atEof() const noexcept;

		inline bool atBeginning() const noexcept;

		inline uint8_t lastStructural() const noexcept;

		inline ErrorCode visitRootPrimitive(TapeBuilder& visitor, const uint8_t* value) noexcept;
		inline ErrorCode visitPrimitive(TapeBuilder& visitor, const uint8_t* value) noexcept;
	};

	inline JsonIterator::JsonIterator(SimdJsonValue* masterParserNew, size_t start_structural_index)
		: nextStructural(masterParserNew->getStructuralIndexes()), buf{ reinterpret_cast<const uint8_t*>(masterParserNew->getStringView()) }, masterParser{
			  masterParserNew
		  } {};

	inline const uint8_t* JsonIterator::peek() const noexcept {
		return &buf[masterParser->getStructuralIndexes()[*this->nextStructural]];
	}

	inline const uint8_t* JsonIterator::advance() noexcept {
		return &buf[(*this->nextStructural++) & 0x00FFFFFFFFFFFFFF];
	}

	inline size_t JsonIterator::remainingLen() const noexcept {
		return masterParser->getTapeLength() - *this->nextStructural;
	}

	inline bool JsonIterator::atEof() const noexcept {
		return this->nextStructural == &masterParser->getStructuralIndexes()[masterParser->getTapeLength() - 1];
	}

	inline bool JsonIterator::atBeginning() const noexcept {
		return this->nextStructural == masterParser->getStructuralIndexes();
	}

	inline uint8_t JsonIterator::lastStructural() const noexcept {
		return buf[masterParser->getStructuralIndexes()[masterParser->getTapeLength() - 1]];
	}

	const bool structuralOrWhitespaceNegated[256]{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,

		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,

		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	inline uint32_t isNotStructuralOrWhitespace(uint8_t c) {
		return structuralOrWhitespaceNegated[c];
	}

	inline uint32_t string_to_uint32(const char* str) {
		uint32_t val{};
		std::memcpy(&val, str, sizeof(uint32_t));
		return val;
	}

	inline uint32_t isNotStructuralOrWhiteSpace(uint8_t c) {
		return structuralOrWhitespaceNegated[c];
	}

	 inline uint32_t str4ncmp(const uint8_t* src, const char* atom) {
		uint32_t srcval{};
		 static_assert(sizeof(uint32_t) <= 256, "SIMDJSON_PADDING must be larger than 4 bytes");
		std::memcpy(&srcval, src, sizeof(uint32_t));
		 return srcval ^ string_to_uint32(atom);
	}

	 inline bool isValidTrueAtom(const uint8_t* src) {
		return (str4ncmp(src, "true") | isNotStructuralOrWhitespace(src[4])) == 0;
	}

	 inline bool isValidTrueAtom(const uint8_t* src, size_t len) {
		if (len > 4) {
			 return isValidTrueAtom(src);
		} else if (len == 4) {
			return !str4ncmp(src, "true");
		} else {
			return false;
		}
	}

	 inline bool isValidFalseAtom(const uint8_t* src) {
		return (str4ncmp(src + 1, "alse") | isNotStructuralOrWhitespace(src[5])) == 0;
	}

	 inline bool isValidFalseAtom(const uint8_t* src, size_t len) {
		if (len > 5) {
			 return isValidFalseAtom(src);
		} else if (len == 5) {
			return !str4ncmp(src + 1, "alse");
		} else {
			return false;
		}
	}

	 inline bool isValidNullAtom(const uint8_t* src) {
		return (str4ncmp(src, "null") | isNotStructuralOrWhitespace(src[4])) == 0;
	}

	 inline bool isValidNullAtom(const uint8_t* src, size_t len) {
		if (len > 4) {
			 return isValidNullAtom(src);
		} else if (len == 4) {
			return !str4ncmp(src, "null");
		} else {
			return false;
		}
	}

	struct TapeWriter {
		TapeWriter(uint64_t* ptr) {
			this->nextTapeLocation = ptr;
		}
		uint64_t* nextTapeLocation;
		inline void appendS64(int64_t value) noexcept;
		inline void appendU64(uint64_t value) noexcept;
		inline void appendDouble(double value) noexcept;
		inline void append(uint64_t val, TapeType t) noexcept;
		inline void skip() noexcept;
		inline void skipLargeInteger() noexcept;
		inline void skipDouble() noexcept;
		inline static void write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept;

	  private:
		template<typename T> inline void append2(uint64_t val, T val2, TapeType t) noexcept;
	};

	inline void TapeWriter::appendS64(int64_t value) noexcept {
		append2(0, value, TapeType::Int64);
	}

	inline void TapeWriter::appendU64(uint64_t value) noexcept {
		append(0, TapeType::Uint64);
		*this->nextTapeLocation = value;
		this->nextTapeLocation++;
	}

	/** Write a double value to tape. */
	inline void TapeWriter::appendDouble(double value) noexcept {
		append2(0, value, TapeType::Double);
	}

	inline void TapeWriter::skip() noexcept {
		this->nextTapeLocation++;
	}

	inline void TapeWriter::skipLargeInteger() noexcept {
		this->nextTapeLocation += 2;
	}

	inline void TapeWriter::skipDouble() noexcept {
		this->nextTapeLocation += 2;
	}

	inline void TapeWriter::append(uint64_t val, TapeType t) noexcept {
		*this->nextTapeLocation = val | ((uint64_t(char(t))) << 56);
		this->nextTapeLocation++;
	}

	template<typename T> inline void TapeWriter::append2(uint64_t val, T val2, TapeType t) noexcept {
		append(val, t);
		static_assert(sizeof(val2) == sizeof(*this->nextTapeLocation), "Type is not 64 bits!");
		memcpy(this->nextTapeLocation, &val2, sizeof(val2));
		this->nextTapeLocation++;
	}

	inline void TapeWriter::write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept {
		tape_loc = val | ((uint64_t(char(t))) << 56);
	}

	struct TapeBuilder {
		static inline ErrorCode parseDocument(SimdJsonValue& masterParser) noexcept;

		inline ErrorCode visitDocumentStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitDocumentEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitArrayStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitArrayEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitEmptyArray(JsonIterator& iter) noexcept;

		inline ErrorCode visitObjectStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitKey(JsonIterator& iter, const uint8_t* key) noexcept;

		inline ErrorCode visitObjectEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitEmptyObject(JsonIterator& iter) noexcept;

		inline ErrorCode visitPrimitive(JsonIterator& iter, const uint8_t* value) noexcept;

		inline ErrorCode visitRootPrimitive(JsonIterator& iter, const uint8_t* value) noexcept;

		inline ErrorCode visitString(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitNumber(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitNullAtom(JsonIterator& iter, const uint8_t* value) noexcept;

		inline ErrorCode visitRootString(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootNumber(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootNullAtom(JsonIterator& iter, const uint8_t* value) noexcept;

		inline ErrorCode incrementCount(JsonIterator& iter) noexcept;

		TapeWriter tape;

	  private:
		uint8_t* currentStringBufferLocation{};

		inline TapeBuilder(SimdJsonValue& doc) noexcept;

		inline uint32_t nextTapeIndex(JsonIterator& iter) const noexcept;
		inline ErrorCode startContainer(JsonIterator& iter) noexcept;
		inline ErrorCode endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline ErrorCode emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline uint8_t* onStartString(JsonIterator& iter) noexcept;
		inline ErrorCode onEndString(uint8_t* dst) noexcept;
	};

	inline ErrorCode TapeBuilder::parseDocument(SimdJsonValue& masterParser) noexcept {
		JsonIterator iter(&masterParser, 0);
		TapeBuilder builder(masterParser);
		return iter.walkDocument(std::move(builder));
	}

	inline ErrorCode TapeBuilder::visitRootPrimitive(JsonIterator& iter, const uint8_t* value) noexcept {
		return iter.visitRootPrimitive(*this, value);
	}
	inline ErrorCode TapeBuilder::visitPrimitive(JsonIterator& iter, const uint8_t* value) noexcept {
		return iter.visitPrimitive(*this, value);
	}
	inline ErrorCode TapeBuilder::visitEmptyObject(JsonIterator& iter) noexcept {
		return emptyContainer(iter, TapeType::Start_Object, TapeType::End_Object);
	}
	inline ErrorCode TapeBuilder::visitEmptyArray(JsonIterator& iter) noexcept {
		return emptyContainer(iter, TapeType::Start_Array, TapeType::End_Array);
	}

	inline ErrorCode TapeBuilder::visitDocumentStart(JsonIterator& iter) noexcept {
		startContainer(iter);
		return ErrorCode::Success;
	}
	inline ErrorCode TapeBuilder::visitObjectStart(JsonIterator& iter) noexcept {
		startContainer(iter);
		return ErrorCode::Success;
	}
	inline ErrorCode TapeBuilder::visitArrayStart(JsonIterator& iter) noexcept {
		startContainer(iter);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitObjectEnd(JsonIterator& iter) noexcept {
		return endContainer(iter, TapeType::Start_Object, TapeType::End_Object);
	}

	inline ErrorCode TapeBuilder::visitArrayEnd(JsonIterator& iter) noexcept {
		return endContainer(iter, TapeType::Start_Array, TapeType::End_Array);
	}

	inline ErrorCode TapeBuilder::visitDocumentEnd(JsonIterator& iter) noexcept {
		constexpr uint32_t startTapeIndex = 0;
		tape.append(startTapeIndex, TapeType::Root);
		TapeWriter::write(iter.masterParser->getStructuralIndexes()[startTapeIndex], nextTapeIndex(iter), TapeType::Root);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitKey(JsonIterator& iter, const uint8_t* key) noexcept {
		return visitString(iter, key);
	}

	inline ErrorCode TapeBuilder::incrementCount(JsonIterator& iter) noexcept {
		iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].count++;
		return ErrorCode::Success;
	}

	inline TapeBuilder::TapeBuilder(SimdJsonValue& doc) noexcept
		: tape{ doc.getStructuralIndexes() }, currentStringBufferLocation{ reinterpret_cast<uint8_t*>(doc.getStringViewNew()) } {
	}

	inline size_t codepointToUtf8(uint32_t cp, uint8_t* c) {
		if (cp <= 0x7F) {
			c[0] = uint8_t(cp);
			return 1;
		}
		if (cp <= 0x7FF) {
			c[0] = uint8_t((cp >> 6) + 192);
			c[1] = uint8_t((cp & 63) + 128);
			return 2;
		} else if (cp <= 0xFFFF) {
			c[0] = uint8_t((cp >> 12) + 224);
			c[1] = uint8_t(((cp >> 6) & 63) + 128);
			c[2] = uint8_t((cp & 63) + 128);
			return 3;
		} else if (cp <= 0x10FFFF) {
			c[0] = uint8_t((cp >> 18) + 240);
			c[1] = uint8_t(((cp >> 12) & 63) + 128);
			c[2] = uint8_t(((cp >> 6) & 63) + 128);
			c[3] = uint8_t((cp & 63) + 128);
			return 4;
		}

		return 0;
	}

	inline bool handleUnicodeCodepoint(const uint8_t** srcPtr, uint8_t** dstPtr) {
		uint32_t codePoint = hexToU32Nocheck(*srcPtr + 2);
		*srcPtr += 6;
		if (codePoint >= 0xd800 && codePoint < 0xdc00) {
			const uint8_t* srcData = *srcPtr;
			if (((srcData[0] << 8) | srcData[1]) != ((static_cast<uint8_t>('\\') << 8) | static_cast<uint8_t>('u'))) {
				return false;
			}
			uint32_t codePoint2 = hexToU32Nocheck(srcData + 2);
			uint32_t lowBit = codePoint2 - 0xdc00;
			if (lowBit >> 10) {
				return false;
			}

			codePoint = (((codePoint - 0xd800) << 10) | lowBit) + 100;
			*srcPtr += 6;
		} else if (codePoint >= 0xdc00 && codePoint <= 0xdfff) {
			return false;
		}
		size_t offset = codepointToUtf8(codePoint, *dstPtr);
		*dstPtr += offset;
		return offset > 0;
	}

	static const uint8_t escapeMap[256]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0, 0, 0, 0, 0x08, 0, 0, 0, 0x0c, 0, 0, 0, 0, 0,
		0, 0, 0x0a, 0, 0, 0, 0x0d, 0, 0x09, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	inline int32_t trailingZeroCount(SimdBase256 toCount) {
		int32_t returnValue{};
		for (size_t x = 0; x < 4; ++x) {
			auto value = _tzcnt_u64(toCount.getUint64(x));
			if (value < 64) {
				return value;
			}
			returnValue += value;
		}
		return returnValue;
	}

	struct BackslashAndQuote {
	  public:
		static constexpr uint32_t BYTES_PROCESSED = 256;
		inline static bool copyAndFind(const uint8_t* src, uint8_t* dst);

		inline bool hasQuoteFirst() {
			return bool{ ((bsBits - 1) & quoteBits) == 0 };
		}
		inline bool hasBackslash() {
			return bool{ ((quoteBits - 1) & bsBits) == 0 };
		}
		inline int32_t quoteIndex() {
			return trailingZeroCount(quoteBits);
		}
		inline int32_t backslashIndex() {
			return trailingZeroCount(bsBits);
		}

		SimdBase256 bsBits{};
		SimdBase256 quoteBits{};
	};

	inline bool BackslashAndQuote::copyAndFind(const uint8_t* src, uint8_t* dst) {
		static_assert(256 >= (BYTES_PROCESSED - 1), "backslash and quote finder must process fewer than 256 bytes");
		SimdBase256 values{ reinterpret_cast<const char*>(src) };
		
		values.store(reinterpret_cast<char*>(dst));
		for (size_t x = 0; x < 32; ++x) {
			if (src[x] == '\"') {
				dst[x] = '\0';
				return true;
			}
		}

		//std::cout << "STRING: " << dst << std::endl;
		//auto result01 = convertSimdBytesToBits(returnValues01);
		//auto result02 = convertSimdBytesToBits(returnValues02);
		return false;
		//{ Sresult01, result02 };
	}

	inline uint8_t* parseString(const uint8_t* src, uint8_t* dst) {
		int32_t index{};
		while (1) {
			index += 32;
			if (BackslashAndQuote::copyAndFind(src, dst)) {

			}
			
			 /*
			if (bsQuote.hasBackslash()) {
				auto bsDist = bsQuote.backslashIndex();
				uint8_t escapeChar = src[bsDist + 1];
				if (escapeChar == 'u') {
					src += bsDist;
					dst += bsDist;
					if (!handleUnicodeCodepoint(&src, &dst)) {
						return nullptr;
					}
				} else {
					uint8_t escapeResult = escapeMap[escapeChar];
					if (escapeResult == 0u) {
						return nullptr;
					}
					dst[bsDist] = escapeResult;
					src += bsDist + 2ull;
					dst += bsDist + 1ull;
				}
			} else {
				src += BackslashAndQuote::BYTES_PROCESSED;
				dst += BackslashAndQuote::BYTES_PROCESSED;
			}
		}*/}
		return nullptr;
	}

	inline ErrorCode TapeBuilder::visitString(JsonIterator& iter, const uint8_t* value) noexcept {
		uint8_t* dst01 = onStartString(iter);
		auto dst02 = parseString(value + 1, dst01);
		if (dst02 == nullptr) {
			return ErrorCode::StringError;
		}
		onEndString(reinterpret_cast<uint8_t*>(dst02));
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootString(JsonIterator& iter, const uint8_t* value) noexcept {
		return visitString(iter, value);
	}

	inline ErrorCode TapeBuilder::visitNumber(JsonIterator& iter, const uint8_t* value) noexcept {
		return parseNumber<ErrorCode, TapeWriter>(value, this->tape);
	}

	inline ErrorCode TapeBuilder::visitRootNumber(JsonIterator& iter, const uint8_t* value) noexcept {
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[iter.remainingLen() + 256]);
		if (copy.get() == nullptr) {
			return ErrorCode::MemAlloc;
		}
		std::memcpy(copy.get(), value, iter.remainingLen());
		std::memset(copy.get() + iter.remainingLen(), ' ', 256);
		return visitNumber(iter, copy.get());
	}

	inline ErrorCode TapeBuilder::visitTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidTrueAtom(value, static_cast<size_t>(iter.masterParser->getStringView()[*iter.nextStructural]- (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return ErrorCode::TAtomError;
		}
		tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidTrueAtom(value,
				static_cast<size_t>(
					iter.masterParser->getStringView()[*iter.nextStructural] - (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return ErrorCode::NAtomError;
		}
		tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidFalseAtom(value,
				static_cast<size_t>(
					iter.masterParser->getStringView()[*iter.nextStructural] - (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return ErrorCode::FAtomError;
		}
		tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidFalseAtom(value,
				static_cast<size_t>(
					iter.masterParser->getStringView()[*iter.nextStructural] - (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return ErrorCode::FAtomError;
		}
		tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitNullAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidNullAtom(value,
				static_cast<size_t>(
					iter.masterParser->getStringView()[*iter.nextStructural] - (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return ErrorCode::NAtomError;
		}
		tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootNullAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidNullAtom(value,
				static_cast<size_t>(
					iter.masterParser->getStringView()[*iter.nextStructural] - (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return ErrorCode::NAtomError;
		}
		tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline uint32_t TapeBuilder::nextTapeIndex(JsonIterator& iter) const noexcept {
		return uint32_t(tape.nextTapeLocation - iter.masterParser->getStructuralIndexes());
	}

	inline ErrorCode TapeBuilder::emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept {
		auto startIndex = nextTapeIndex(iter);
		tape.append(startIndex + 2ull, start);
		tape.append(startIndex, end);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::startContainer(JsonIterator& iter) noexcept {
		iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].tapeIndex = nextTapeIndex(iter);
		iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].count = 0;
		tape.skip();
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept {
		const uint32_t startTapeIndex = iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].tapeIndex;
		tape.append(startTapeIndex, end);
		const uint32_t count = iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].count;
		const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
		TapeWriter::write(iter.masterParser->getStructuralIndexes()[startTapeIndex], nextTapeIndex(iter) | (uint64_t(cntsat) << 32), start);
		return ErrorCode::Success;
	}

	inline uint8_t* TapeBuilder::onStartString(JsonIterator& iter) noexcept {
		tape.append(currentStringBufferLocation - reinterpret_cast<uint8_t*>(iter.masterParser->getStringViewNew()), TapeType::String);
		return currentStringBufferLocation + sizeof(uint32_t);
	}

	inline ErrorCode TapeBuilder::onEndString(uint8_t* dst) noexcept {
		uint32_t strLength = uint32_t(dst - (this->currentStringBufferLocation + sizeof(uint32_t)));
		memcpy(this->currentStringBufferLocation, &strLength, sizeof(uint32_t));
		*dst = 0;
		this->currentStringBufferLocation = dst + 1;
		return ErrorCode::Success;
	}

	inline ErrorCode JsonIterator::walkDocument(TapeBuilder&& visitor) noexcept {
		
		if (atEof()) {
			return ErrorCode::Empty;
		}

		{
			auto value = this->advance();

			switch (*value) {
				case '{':
					if (*peek() == '}') {
						advance();
						visitor.visitEmptyObject(*this);
						break;
					}
					goto Object_Begin;
				case '[':
					if (*peek() == ']') {
						advance();
						visitor.visitEmptyArray(*this);
						break;
					}
					goto Array_Begin;
				default:
					visitor.visitRootPrimitive(*this, value);
					break;
			}
		}
		goto Document_End;

	Object_Begin:
		masterParser->getCurrentDepth()++;
		if (masterParser->getCurrentDepth() >= masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		masterParser->getIsArray()[masterParser->getCurrentDepth()] = false;
		visitor.visitObjectStart(*this);

		{
			auto key = this->advance();
			if (*key != '"') {
				return ErrorCode::TapeError;
			}
			visitor.visitKey(*this, key);
			visitor.incrementCount(*this);
		}

	Object_Field:
		if (*advance() != ':') {
			return ErrorCode::TapeError;
		}
		{
			auto value = this->advance();
			switch (*value) {
				case '{':
					if (*peek() == '}') {
						advance();
						visitor.visitEmptyObject(*this);
						break;
					}
					goto Object_Begin;
				case '[':
					if (*peek() == ']') {
						advance();
						visitor.visitEmptyArray(*this);
						break;
					}
					goto Array_Begin;
				default:
					visitor.visitPrimitive(*this, value);
					break;
			}
		}

	Object_Continue:
		switch (*advance()) {
			case ',':
				visitor.incrementCount(*this);
				{
					auto key = this->advance();
					if (*key != '"') {
						return ErrorCode::TapeError;
					}
				}
				goto Object_Field;
			case '}':
				visitor.visitObjectEnd(*this);
				goto Scope_End;
			default:
				return ErrorCode::TapeError;
		}

	Scope_End:
		masterParser->getCurrentDepth()--;
		if (masterParser->getCurrentDepth() == 0) {
			goto Document_End;
		}
		if (masterParser->getIsArray()[masterParser->getCurrentDepth()]) {
			goto Array_Continue;
		}
		goto Object_Continue;

	Array_Begin:
		masterParser->getCurrentDepth()++;
		if (masterParser->getCurrentDepth() >= masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		masterParser->getIsArray()[masterParser->getCurrentDepth()] = true;
		visitor.visitArrayStart(*this);
		visitor.incrementCount(*this);

	Array_Value : {
		auto value = this->advance();
		switch (*value) {
			case '{':
				if (*peek() == '}') {
					advance();
					visitor.visitEmptyObject(*this);
					break;
				}
				goto Object_Begin;
			case '[':
				if (*peek() == ']') {
					advance();
					visitor.visitEmptyArray(*this);
					break;
				}
				goto Array_Begin;
			default:
				break;
		}
	}

	Array_Continue:
		switch (*advance()) {
			case ',':
				visitor.incrementCount(*this);
				goto Array_Value;
			case ']':
				visitor.visitArrayEnd(*this);
				goto Scope_End;
			default:
				return ErrorCode::TapeError;
		}

	Document_End:
		visitor.visitDocumentEnd(*this);

		this->nextStructuralIndex = uint32_t(nextStructural - &masterParser->getStructuralIndexes()[0]);

		if (this->nextStructuralIndex != masterParser->getTapeLength()) {
			return ErrorCode::TapeError;
		}

		return ErrorCode::Success;
	}

	inline ErrorCode JsonIterator::visitRootPrimitive(TapeBuilder& visitor, const uint8_t* value) noexcept {
		switch (*value) {
			case '"':
				return visitor.visitRootString(*this, value);
			case 't':
				return visitor.visitRootTrueAtom(*this, value);
			case 'f':
				return visitor.visitRootFalseAtom(*this, value);
			case 'n':
				return visitor.visitRootNullAtom(*this, value);
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return visitor.visitRootNumber(*this, value);
			default:
				return ErrorCode::TapeError;
		}
	}

	inline ErrorCode JsonIterator::visitPrimitive(TapeBuilder& visitor, const uint8_t* value) noexcept {
		switch (*value) {
			case '"':
				return visitor.visitString(*this, value);
			case 't':
				return visitor.visitTrueAtom(*this, value);
			case 'f':
				return visitor.visitFalseAtom(*this, value);
			case 'n':
				return visitor.visitNullAtom(*this, value);
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return visitor.visitNumber(*this, value);
			default:
				return ErrorCode::TapeError;
		}
	}

	JsonParser SimdJsonValue::getJsonData(std::string& string) {
		this->generateJsonEvents(string.data(), string.size());
		TapeBuilder::parseDocument(*this);
		return std::move(this->tape);
	}

};
