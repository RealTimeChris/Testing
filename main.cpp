#ifndef _TESTING
	#define _TESTING
#endif

#include "DataParsingFunctions.hpp"
#include <simdjson.h>
#include <fstream>

Jsonifier::StopWatch stopWatchNew{ std::chrono::nanoseconds{ 1 } };
int64_t timeValueDouble{};
int64_t timeValueBool{};
int64_t timeValueString{};
int64_t timeValueInt64{};
int64_t iterationCountNew{};

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::Value value) {
		//iterationCountNew++;
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_00 = Jsonifier::getFloat(value, "TEST_VALUE_00");
		//timeValueDouble += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_01 = Jsonifier::getBool(value, "TEST_VALUE_01");
		//timeValueBool += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_02 = Jsonifier::getString(value, "TEST_VALUE_02");
		//timeValueString += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_03 = Jsonifier::getUint64(value, "TEST_VALUE_03");
		//timeValueInt64 += stopWatchNew.totalTimePassed().count();
		this->TEST_VALUE_04 = Jsonifier::getUint64(value, "TEST_VALUE_04");
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_05 = Jsonifier::getFloat(value, "TEST_VALUE_05");
		//timeValueDouble += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_06 = Jsonifier::getBool(value, "TEST_VALUE_06");
		//timeValueBool += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_07 = Jsonifier::getString(value, "TEST_VALUE_07");
		//timeValueString += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_08 = Jsonifier::getUint64(value, "TEST_VALUE_08");
		//timeValueInt64 += stopWatchNew.totalTimePassed().count();
		this->TEST_VALUE_09 = Jsonifier::getUint64(value, "TEST_VALUE_09");
	};
	double TEST_VALUE_00{};
	bool TEST_VALUE_01{};
	std::string TEST_VALUE_02{};
	int64_t TEST_VALUE_03{};
	uint64_t TEST_VALUE_04{};
	double TEST_VALUE_05{};
	bool TEST_VALUE_06{};
	std::string TEST_VALUE_07{};
	int64_t TEST_VALUE_08{};
	uint64_t TEST_VALUE_09{};
};

struct TheDJson {
	TheDJson() noexcept = default;
	TheDJson(Jsonifier::Document value) {
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatchNew{ std::chrono::nanoseconds{ 1 } };
		int64_t totalTime{};
		int64_t totalIterations{};
		Jsonifier::Array valueNew{};
		value["TEST_VALUE_11"]["d"]["TEST_VALUES"].get(valueNew);
		timeValueDouble = 0;
		timeValueBool = 0;
		timeValueString = 0;
		timeValueInt64 = 0;
		iterationCountNew = 0;
		for (auto valueIterator: valueNew) {
			//std::cout << "FIELD COUNT: " << +valueNewer.countFields().error() << std::endl;
			//std::cout << "THE TYPE: " << ( int32_t )valueNewer.type().value_unsafe() << std::endl;
			strings.emplace_back(std::move(valueIterator.valueUnsafe()));
		}
		//std::cout << "TOTAL TIME PASSED:(JSON) (double) " << timeValueDouble / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED:(JSON) (Bool) " << timeValueBool / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED:(JSON) (String) " << timeValueString / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED:(JSON) (int64_t) " << timeValueInt64 / iterationCountNew << std::endl;

		//std::cout << "TOTAL TIME PASSED: (JSON) " << totalTime << ", THE COUNT: " << this->strings.size() << std::endl;
	}
	std::vector<ActivitiesJson> strings{};
};

struct TheValueJson {
	TheValueJson(Jsonifier::Document value) {
		//std::cout << "THE DATA: " << value.rawJson().value_unsafe() << std::endl;
		this->theD = TheDJson{ std::forward<Jsonifier::Document>(value) };
	}
	TheDJson theD{};
};

struct Activities {
	Activities() noexcept = default;
	Activities(simdjson::ondemand::value value) {
		//iterationCountNew++;
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_00 = Jsonifier::getFloat(value, "TEST_VALUE_00");
		//timeValueDouble += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_01 = Jsonifier::getBool(value, "TEST_VALUE_01");
		//timeValueBool += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_02 = Jsonifier::getString(value, "TEST_VALUE_02");
		//timeValueString += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_03 = Jsonifier::getUint64(value, "TEST_VALUE_03");
		//timeValueInt64 += stopWatchNew.totalTimePassed().count();
		this->TEST_VALUE_04 = Jsonifier::getUint64(value, "TEST_VALUE_04");
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_05 = Jsonifier::getFloat(value, "TEST_VALUE_05");
		//timeValueDouble += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_06 = Jsonifier::getBool(value, "TEST_VALUE_06");
		//timeValueBool += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_07 = Jsonifier::getString(value, "TEST_VALUE_07");
		//timeValueString += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->TEST_VALUE_08 = Jsonifier::getUint64(value, "TEST_VALUE_08");
		//timeValueInt64 += stopWatchNew.totalTimePassed().count();
		this->TEST_VALUE_09 = Jsonifier::getUint64(value, "TEST_VALUE_09");
	};
	double TEST_VALUE_00{};
	bool TEST_VALUE_01{};
	std::string TEST_VALUE_02{};
	int64_t TEST_VALUE_03{};
	uint64_t TEST_VALUE_04{};
	double TEST_VALUE_05{};
	bool TEST_VALUE_06{};
	std::string TEST_VALUE_07{};
	int64_t TEST_VALUE_08{};
	uint64_t TEST_VALUE_09{};
};



struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::document value) {
		Jsonifier::StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
		int64_t totalTime{};
		int64_t totalIterations{};
		simdjson::ondemand::array valueNew{};
		value["TEST_VALUE_11"]["d"]["TEST_VALUES"].get(valueNew);
		stopWatch.resetTimer();
		timeValueDouble = 0;
		timeValueBool = 0;
		timeValueString = 0;
		timeValueInt64 = 0;
		iterationCountNew = 0;
		for (auto valueIterator: valueNew) {
			stopWatch.resetTimer();//
			//std::cout << "FIELD COUNT: " << +valueNewer.countFields().error() << std::endl;
			//std::cout << "THE TYPE: " << ( int32_t )valueNewer.type().value_unsafe() << std::endl;
			strings.emplace_back(std::move(valueIterator.value_unsafe()));
			totalTime += stopWatch.totalTimePassed().count();
			totalIterations++;
		}
		//std::cout << "TOTAL TIME PASSED: (double) " << timeValueDouble / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED: (Bool) " << timeValueBool / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED: (String) " << timeValueString / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED: (int64_t) " << timeValueInt64 / iterationCountNew << std::endl;

		//std::cout << "TOTAL TIME PASSED: " << totalTime << ", THE COUNT: " << this->strings.size() << std::endl;
	}
	std::vector<Activities> strings{};
};

struct TheValue {
	TheValue(simdjson::ondemand::document value) {
		this->theD = TheD{ std::move(value) };
	}
	TheD theD{};
};

class FileLoader {
  public:
	FileLoader(const char* filePath) {
		std::ofstream theStream{ filePath, std::ios::out | std::ios::in };
		std::stringstream inputStream{};
		inputStream << theStream.rdbuf();
		this->fileContents = inputStream.str();
	}
	std::string& getFileContents() {
		return this->fileContents;
	}
	operator std::string() {
		return this->getFileContents();
	}
  protected:
	std::string fileContents{};
};

int32_t main() {
	try {
		Jsonifier::Serializer serializer{};
		Jsonifier::Serializer arrayValueNew{};
		arrayValueNew["TEST_VALUE_00"] = 0.00333423;
		arrayValueNew["TEST_VALUE_01"] = true;
		arrayValueNew["TEST_VALUE_02"] = "TESTING_VALUE112323";
		arrayValueNew["TEST_VALUE_03"] = 4325454ll;
		arrayValueNew["TEST_VALUE_04"] = 4325454ull;
		arrayValueNew["TEST_VALUE_05"] = 0.00333423;
		arrayValueNew["TEST_VALUE_06"] = true;
		arrayValueNew["TEST_VALUE_07"] = "TESTING_VALUE";
		arrayValueNew["TEST_VALUE_08"] = 4325454ll;
		arrayValueNew["TEST_VALUE_09"] = 23423423ull;
		auto& arrayValue = arrayValueNew;
		for (size_t x = 0; x < 90; ++x) { 
			serializer["TEST_VALUE_11"]["d"]["TEST_VALUES"].emplaceBack(arrayValueNew);
		}
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::string stringNew{ FileLoader{ "C:/users/chris/source/repos/jsonifier/benchmarking/test_data.json" } };
		//std::string stringNew{ serializer.operator std::string&&() };
		/*
		 std::string stringNew{
			"{\"d\":{\"_trace\":[\"[\"gateway-prd-us-east1-c-hxpp\",{\"micros\":69465,\"calls\":[\"id_created\",{\"micros\":818,\"calls\":[]},"
			"\"session_lookup_time\",{\"micros\":1732,\"calls\":[]},\"session_lookup_finished\",{\"micros\":16,\"calls\":[]},\"discord-sessions-blue-"
			"prd-2-110\",{\"micros\":65609,\"calls\":[\"start_session\",{\"micros\":56176,\"calls\":[\"discord-api-bb96f7fd9-r2n7w\",{\"micros\":"
			"50339,\"calls\":[\"get_user\",{\"micros\":11747},\"get_guilds\",{\"micros\":4833},\"send_scheduled_deletion_message\",{\"micros\":13},"
			"\"guild_join_requests\",{\"micros\":2},\"authorized_ip_coro\",{\"micros\":11}]}]},\"starting_guild_connect\",{\"micros\":196,\"calls\":["
			"]},\"presence_started\",{\"micros\":368,\"calls\":[]},\"guilds_started\",{\"micros\":155,\"calls\":[]},\"guilds_connect\",{\"micros\":2,"
			"\"calls\":[]},\"presence_connect\",{\"micros\":8659,\"calls\":[]},\"connect_finished\",{\"micros\":8665,\"calls\":[]},\"build_ready\",{"
			"\"micros\":47,\"calls\":[]},\"optimize_ready\",{\"micros\":0,\"calls\":[]},\"split_ready\",{\"micros\":1,\"calls\":[]},\"clean_ready\",{"
			"\"micros\":0,\"calls\":[]}]}]}]\"],\"application\":{\"flags\":27828224,\"id\":\"1008143759944450190\"},\"geo_ordered_rtc_regions\":["
			"\"montreal\",\"newark\",\"us-east\",\"us-central\",\"atlanta\"],\"guild_join_requests\":null,\"guilds\":[{\"id\":\"815087249556373516\","
			"\"unavailable\":true},{\"id\":\"931640556814237706\",\"unavailable\":true},{\"id\":\"991025447875784714\",\"unavailable\":true},{\"id\":"
			"\"995048955215872071\",\"unavailable\":true},{\"id\":\"1049467122486612088\",\"unavailable\":true}],\"presences\":null,\"private_"
			"channels\":null,\"relationships\":null,\"resume_gateway_url\":\"wss://"
			"gateway-us-east1-c.discord.gg\",\"session_id\":\"19cbf5b63ad502f2821ef711b694718e\",\"session_type\":\"normal\",\"shard\":\"01\","
			"\"user\":{\"avatar\":\"46abb15c4bf57cd5dfc6db3fd661a491\",\"bot\":true,\"discriminator\":\"9071\",\"email\":null,\"flags\":0,\"id\":"
			"\"1008143759944450190\",\"mfa_enabled\":false,\"username\":\"MBot-MusicHouse-2\",\"verified\":true},\"user_settings\":{},\"v\":10},"
			"\"op\":0,\"s\":1,\"t\":\"READY\"}\\"
		};*/
		size_t totalTime{};
		size_t totalSize{};
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t oldSize = stringNew.size();
 		std::cout << "THE STRING: " << stringNew << std::endl;
		std::cout << "THE STRING LENGTH: " << stringNew.size() << std::endl;
		std::string stringNewer = stringNew;
				
		int32_t iterationCount{};
		

		iterationCount = 0;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();

		

		//std::cout << "IT TOOK: " << stopWatch.totalTimePassed() << ", TO RESERVE THE STRING!" << std::endl;

		
		
		stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
		simdjson::ondemand::parser parser{};
		stopWatch.resetTimer();
		for (size_t x = 0ull; x < 10; ++x) {
			auto jsonData = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheD theValue{ std::move(jsonData) };
			//std::cout << "VALUE00 (TESTING): " << value.theD.strings.begin().operator*().TEST_VALUE_00 << std::endl;
			//std::cout << "VALUE01: " << value.theD.strings.begin().operator*().TEST_VALUE_01 << std::endl;
			//std::cout << "VALUE02: " << value.theD.strings.begin().operator*().TEST_VALUE_02 << std::endl;
			//std::cout << "VALUE03: " << theValue.strings.begin().operator*().TEST_VALUE_03 << std::endl;
			//std::cout << "VALUE04: " << value.theD.strings.begin().operator*().TEST_VALUE_04 << std::endl;
			//std::cout << "VALUE06: " << value.theD.strings.begin().operator*().TEST_VALUE_06 << std::endl;
			//std::cout << "VALUE07: " << value.theD.strings.begin().operator*().TEST_VALUE_07 << std::endl;
			//std::cout << "THE NAME: " << value.name << std::endl;
			//std::cout << "MEMBER COUNT: " << value.memberCount << std::endl;
			iterationCount++;
			totalSize += oldSize;
		}
		std::cout << "IT TOOK: " << stopWatch.totalTimePassed().count() / iterationCount << "ns TO PARSE THROUGH IT: " << totalSize / iterationCount
				  << " BYTES!" << std::endl;
		iterationCount = 0;
		totalSize = 0;
		totalTime = 0;

		
		
		Jsonifier::Parser parserOld{};
		stopWatch.resetTimer();
		for (size_t x = 0ull; x < 10; ++x) {
			auto jsonData = parserOld.parseJson(stringNew.data(), stringNew.size());
			TheDJson value{ std::move(jsonData) };
			//GuildData value{ std::move(jsonData["d"]) };
			//WebSocketMessage value{ std::move(jsonData) };
			//std::cout << "VALUE00: " << value.strings.begin().operator*().TEST_VALUE_00 << std::endl;
			//std::cout << "VALUE01: " << value.strings.begin().operator*().TEST_VALUE_01 << std::endl;
			//std::cout << "VALUE02: " << value.strings.begin().operator*().TEST_VALUE_02 << std::endl;
			std::cout << "VALUE03: " << value.strings.begin().operator*().TEST_VALUE_03 << std::endl;
			std::cout << "VALUE04: " << value.strings.begin().operator*().TEST_VALUE_04 << std::endl;
			std::cout << "VALUE05: " << value.strings.begin().operator*().TEST_VALUE_05 << std::endl;
			std::cout << "VALUE06: " << value.strings.begin().operator*().TEST_VALUE_06 << std::endl;
			std::cout << "VALUE07: " << value.strings.begin().operator*().TEST_VALUE_07 << std::endl;
			//std::cout << "VALUE08: " << value.strings.begin().operator*().TEST_VALUE_08 << std::endl;
			iterationCount++;
			totalSize += oldSize;
		}
		totalTime = stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << stopWatch.totalTimePassed().count() / iterationCount << "ns TO PARSE THROUGH IT: " << totalSize / iterationCount
				  << " BYTES!" << std::endl;
		
		
		
	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	return 0;
};

