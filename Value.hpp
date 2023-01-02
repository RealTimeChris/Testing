#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class EnumStringConverter {
	  public:
		inline EnumStringConverter(ErrorCode error) {
			this->code = error;
		}
		inline operator std::string() {
			switch (this->code) {
				case ErrorCode::Empty: {
					return "Empty";
				}
				case ErrorCode::Tape_Error: {
					return "Tape Error";
				}
				case ErrorCode::Depth_Error: {
					return "Depth Error";
				}
				case ErrorCode::Success: {
					return "Success";
				}
				case ErrorCode::Parse_Error: {
					return "Parse Error";
				}
				case ErrorCode::String_Error: {
					return "String Error";
				}
				case ErrorCode::TAtom_Error: {
					return "TAtom Error";
				}
				case ErrorCode::FAtom_Error: {
					return "FAtom Error";
				}
				case ErrorCode::NAtom_Error: {
					return "NAtom Error";
				}
				case ErrorCode::Mem_Alloc_Error: {
					return "Mem Alloc";
				}
				case ErrorCode::Invalid_Number: {
					return "Invalid Number";
				}
				default: {
					return "Unknown Error";
				}
			}
		}

	  protected:
		ErrorCode code{};
	};

	struct JsonifierException : public std::runtime_error, std::string {
		JsonifierException(const std::string&, std::source_location = std::source_location::current()) noexcept;
	};

	template<typename OTy> class JsonifierResult : protected std::pair<OTy, ErrorCode> {
	  public:
		template<typename OTy> inline JsonifierResult() noexcept : std::pair<OTy, ErrorCode>{ OTy{}, ErrorCode::Success } {};
		template<typename OTy>
		inline JsonifierResult(OTy&& other, ErrorCode&& error) noexcept : std::pair<OTy, ErrorCode>{ std::move(other), std::move(error) } {};

		template<typename OTy> inline ErrorCode get(OTy& value) noexcept {
			value = std::move(this->first);
			return std::move(this->second);
		}

		inline OTy getValue() {
			if (this->second != ErrorCode::Success) {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(this->second) } };
			}
			return std::move(this->first);
		}
	};

	class RawJsonString {
	  public:
		inline RawJsonString() noexcept = default;
		inline RawJsonString(uint8_t* _buf) noexcept;
		inline char* raw() const noexcept;
		inline bool unsafeIsEqual(size_t length, std::string_view target) const noexcept;
		inline bool unsafeIsEqual(std::string_view target) const noexcept;
		inline bool unsafeIsEqual(const char* target) const noexcept;
		inline bool is_equal(std::string_view target) const noexcept;
		inline bool is_equal(const char* target) const noexcept;
		static inline bool is_free_from_unescaped_quote(std::string_view target) noexcept;
		static inline bool is_free_from_unescaped_quote(const char* target) noexcept;

	  protected:
		inline void consume() noexcept {
			stringView = nullptr;
		}
		inline bool alive() const noexcept {
			return stringView != nullptr;
		}
		template<typename OTy> inline std::string_view unescape(IteratorBaseBase& iter) noexcept;

		uint8_t* stringView{};
		friend class JsonIterator;
	};

	
	
}
