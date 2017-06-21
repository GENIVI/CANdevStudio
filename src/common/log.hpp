#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <cstring>
#include <memory>
#include <iostream>

extern std::shared_ptr<spdlog::logger> kDefaultLogger;

#define __FILENAME__ \
    (std::strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define cds_debug(fmt, ...)                                           \
    do {                                                          \
        kDefaultLogger->debug("[{}@{}] " fmt, __FILENAME__, __LINE__, \
                              ##__VA_ARGS__);                     \
    } while (0)
#define cds_warn(fmt, ...)                                            \
    do {                                                          \
        kDefaultLogger->warn("[{}@{}] " fmt, __FILENAME__, ___LINE__, \
                             ##__VA_ARGS__);                      \
    } while (0)
#define cds_error(fmt, ...)                                           \
    do {                                                          \
        kDefaultLogger->error("[{}@{}] " fmt, __FILENAME__, __LINE__, \
                              ##__VA_ARGS__);                     \
    } while (0)
#define cds_info(fmt, ...)                                               \
    do {                                                             \
        kDefaultLogger->info("[{}@{}] " fmt, __FILENAME__, __LINE__, \
                             ##__VA_ARGS__);                         \
    } while (0)
