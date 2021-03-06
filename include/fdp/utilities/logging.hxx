/*! **************************************************************************
 * @file fdp/utilities/logging.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File contains pointers for creating global logger during session
 *
 * This file should be included in all FDP header files as it allows all
 * debug/error information to be output to the same logger and hence the
 * same terminal session. It uses the SPDLog library.
 ****************************************************************************/
#ifndef __FDP_LOGGING_HXX__
#define __FDP_LOGGING_HXX__

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace FDP {
/*! **************************************************************************
 * @brief Create a the global logger pointer
 * @author K. Zarebski (UKAEA)
 *
 * @return a shared pointer to the global logger instance
 ****************************************************************************/
std::shared_ptr<spdlog::logger> create_logger_();

/**
 * @brief Global logger used across all FDP scripts
 * @author K. Zarebski (UKAEA)
 *
 */
const std::shared_ptr<spdlog::logger> APILogger = create_logger_();
} // namespace FDP

#endif