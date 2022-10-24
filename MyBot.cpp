#include "MyBot.h"
#include <dpp/dpp.h>
#include <thread>
#include <iostream>
/* Be sure to place your token in the line below.
 * Follow steps here to get a token:
 * https://dpp.dev/creating-a-bot-application.html
 * When you invite the bot, be sure to invite it with the
 * scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064
 */
const std::string    BOT_TOKEN = "";

template<typename TimeType> class StopWatch {
public:
	using DoubleTimeDuration = std::chrono::duration<double, std::nano>;

	using DoubleTimePoint = std::chrono::time_point<std::chrono::system_clock, DoubleTimeDuration>;

	StopWatch<TimeType>& operator=(StopWatch<TimeType>&& other) noexcept {
		if (this != &other) {
			this->maxNumberOfMs = std::move(other.maxNumberOfMs);
			this->startTime = std::move(other.startTime);
		}
		return *this;
	}

	StopWatch(StopWatch<TimeType>&& other) noexcept {
		*this = std::move(other);
	}

	StopWatch<TimeType>& operator=(const StopWatch<TimeType>& other) noexcept {
		if (this != &other) {
			this->maxNumberOfMs = other.maxNumberOfMs;
			this->startTime = other.startTime;
		}
		return *this;
	}

	StopWatch(const StopWatch<TimeType>& other) noexcept {
		*this = other;
	}

	StopWatch<TimeType>& operator=(StopWatch<TimeType>& other) noexcept {
		if (this != &other) {
			this->maxNumberOfMs = other.maxNumberOfMs;
			this->startTime = other.startTime;
		}
		return *this;
	}

	StopWatch(StopWatch<TimeType>& other) noexcept {
		*this = other;
	}

	StopWatch() = default;

	StopWatch<TimeType>& operator=(TimeType maxNumberOfMsNew) {
		std::unique_lock theLock{ this->theMutex };
		this->maxNumberOfMs = DoubleTimePoint{ maxNumberOfMsNew };
		this->startTime = std::chrono::system_clock::now();
		return *this;
	}

	explicit StopWatch(TimeType maxNumberOfMsNew) {
		*this = maxNumberOfMsNew;
	}

	uint64_t totalTimePassed() {
		std::unique_lock theLock{ this->theMutex };
		auto elapsedTime = std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()) -
			std::chrono::duration_cast<TimeType>(this->startTime.time_since_epoch());
		return elapsedTime.count();
	}

	bool hasTimePassed() {
		std::unique_lock theLock{ this->theMutex };
		DoubleTimeDuration elapsedTime = std::chrono::system_clock::now() - this->startTime;
		if (elapsedTime >= this->maxNumberOfMs.time_since_epoch()) {
			return true;
		}
		else {
			return false;
		}
	}

	void resetTimer(uint64_t theNewTime = 0) {
		std::unique_lock theLock{ this->theMutex };
		if (theNewTime != 0) {
			this->maxNumberOfMs = DoubleTimePoint{ TimeType{ theNewTime } };
		}
		this->startTime = std::chrono::system_clock::now();
	}

protected:
	DoubleTimePoint maxNumberOfMs{};
	DoubleTimePoint startTime{};
	std::mutex theMutex{};
};

int32_t main() {
	std::string theToken{ "OTMxMzEyODA0NjM4NjIxNzU3.Gibyoz.kFDz6jlfNo6rTRESly3otRjLynHnqHamAn-wxs" };
	/* Create bot cluster */
	dpp::cache_policy_t cachePolicy{};
	cachePolicy.emoji_policy = dpp::cache_policy_setting_t::cp_aggressive;
	cachePolicy.role_policy = dpp::cache_policy_setting_t::cp_aggressive;
	cachePolicy.user_policy = dpp::cache_policy_setting_t::cp_aggressive;
	dpp::cluster bot{ theToken, dpp::intents::i_all_intents, 12,0,1,false,cachePolicy };
	std::string defaultGateWay{ "192.168.0.28" };
	bot.set_default_gateway(defaultGateWay);
	bot.set_websocket_protocol(dpp::ws_etf);

	/* Output simple log messages to stdout */
	bot.on_log(dpp::utility::cout_logger());

	std::thread theThread{ [&]() {
		StopWatch theStopWatch{std::chrono::seconds{1}};
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			if (dpp::get_guild_cache()->count() < 179000&&dpp::get_guild_cache()->count()%100==0) {
				std::cout << "THE GUILD COUNT: " << dpp::get_guild_cache()->count() << ", THE TIME: " << theStopWatch.totalTimePassed() << std::endl;
				std::cout << "THE ROLE COUNT: " << dpp::get_role_cache()->count() << ", THE TIME: " << theStopWatch.totalTimePassed() << std::endl;
				std::cout << "THE CHANNEL COUNT: " << dpp::get_channel_cache()->count() << ", THE TIME: " << theStopWatch.totalTimePassed() << std::endl;
			}
		}
	} };
	theThread.detach();

	/* Start the bot */
	bot.start(false);

	return 0;
}
