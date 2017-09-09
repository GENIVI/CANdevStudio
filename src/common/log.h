#include <cstring>
#include <iostream>
#include <memory>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

extern std::shared_ptr<spdlog::logger> kDefaultLogger;

#define __FILENAME__ (std::strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define cds_debug(fmt, ...)                                                                                            \
    do {                                                                                                               \
        kDefaultLogger->debug("[{}():{}@{}] " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__);               \
    } while (0)
#define cds_warn(fmt, ...)                                                                                             \
    do {                                                                                                               \
        kDefaultLogger->warn("[{}():{}@{}] " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__);                \
    } while (0)
#define cds_error(fmt, ...)                                                                                            \
    do {                                                                                                               \
        kDefaultLogger->error("[{}():{}@{}] " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__);               \
    } while (0)
#define cds_info(fmt, ...)                                                                                             \
    do {                                                                                                               \
        kDefaultLogger->info("[{}():{}@{}] " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__);                \
    } while (0)
