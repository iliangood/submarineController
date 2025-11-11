#if !defined UTILS_H
#define UTILS_H

#include <inttypes.h>
#include <chrono>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

inline uint64_t millis()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void debug_init()
{
	
	std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/submarine.log", 10*1024*1024, 5);
    file_sink->set_level(spdlog::level::trace); 

    spdlog::logger logger("submarine", {console_sink, file_sink});
    logger.set_level(spdlog::level::trace);

    logger.set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%l] %v%$");

    spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));
}
#endif