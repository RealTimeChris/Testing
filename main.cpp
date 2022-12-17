#include "Jsonifier.hpp"
#include "DataParsingFunctionc.hpp"
#include <simdjson.h>

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::Jsonifier&& value) {
		this->createdAt = value["created_at"].getValue<std::string>();
		this->id = value["id"].getValue<std::string>();
		this->testDouble = value["test_double"].getValue<double>();
	};
	std::string createdAt{};
	double testDouble{};
	std::string id{};
	operator Jsonifier::Jsonifier() {
		Jsonifier::Jsonifier data{};
		data["test_double"] = this->testDouble;
		data["created_at"] = this->createdAt;
		data["id"] = this->id;
		return data;
	}
};

struct TheDJson {
	TheDJson() noexcept = default;
	TheDJson(Jsonifier::Jsonifier& value) {
		auto theArray = value["d"]["activitiess"].getValue<std::vector<Jsonifier::Jsonifier>>();
		for (auto& value: theArray) {
			activities.emplace_back(std::move(value));
		}
	}
	operator Jsonifier::Jsonifier() {
		Jsonifier::Jsonifier serializer{};
		for (auto& value: this->activities) {
			serializer["d"]["activitiess"].emplaceBack(value);
		}
		return serializer;
	}
	std::vector<ActivitiesJson> activities{};
};

namespace Jsonifier {
	template<> TheDJson Jsonifier::getValue() {
		return TheDJson{ *this };
	}

}

struct TheValueJson {
	TheValueJson(Jsonifier::Jsonifier& value) {
		this->theD = value.getValue<TheDJson>();
	}
	TheDJson theD{};
};

struct Activities {
	Activities(simdjson::ondemand::value value) {
		this->createdAt = DiscordCoreAPI::getString(value, "created_at");
		this->id = DiscordCoreAPI::getString(value, "id");
		this->testDouble = DiscordCoreAPI::getFloat(value, "test_double");
	};
	std::string createdAt{};
	double testDouble{};
	std::string id{};
};

struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::value value) {
		simdjson::ondemand::value valueNew{};
		value["d"]["activitiess"].get(valueNew);
		auto theArray = DiscordCoreAPI::getArray(value["d"], "activitiess");
		if (theArray.didItSucceed) {
			for (auto value: theArray.arrayValue) {
				activities.push_back(value.value());
			}
		}
	}
	std::vector<Activities> activities{};
};

struct TheValue {
	TheValue(simdjson::ondemand::value value) {
		this->theD = TheD{ value };
	}
	TheD theD{};
};

int32_t main() noexcept {
	try {
		std::string stringNew{
			"{\"d\":{\"activitiess\":[{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|\",\"test_double\":334.4545,"
			"\"type\":3,\"ANOTHER_VALUE\":3434,\"ANOTHER_TEST_VALUE\":\"TESTING-TESTING\",\"ANOTHER_VALUE_02\":3434,\"ANOTHER_TEST_"
			"VALUE_03\":\"TESTING-TESTING_031\",\"ANOTHER_VALUE_02w\":3434,\"ANOTHER_TEST_VALUE_03d\":\"TESTING-TESTING_031d\"}]}}"
		};

		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		size_t oldSize = stringNew.size();
		stopWatch.resetTimer();
		Jsonifier::prepStringForParsing(stringNew);
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
			Jsonifier::Jsonifier jsonData{};
			jsonData.parseString(stringNew);
			TheValueJson value{ jsonData };
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();

		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		std::string stringNewer = stringNew;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		stringNewer.reserve(stringNewer.size() + simdjson::SIMDJSON_PADDING);
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
			simdjson::ondemand::parser parser{};
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheValue value{ newDocument };
			totalSize += oldSize;
		}

		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;


	} catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}


	return 0;
};