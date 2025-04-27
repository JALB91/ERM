#pragma once

#include "erm/log/Logger.h"
#include "erm/log/LogLevel.h"

#include <erm/utils/Macros.h>

#ifndef ERM_LOG_LEVEL
#	define ERM_LOG_LEVEL "INFO"
#endif

#define ERM_LOG_INDENT()	  erm::internal::gLogger.indent()
#define ERM_LOG_UNINDENT()	  erm::internal::gLogger.unindent()
#define ERM_LOG(...)		  erm::internal::gLogger.log(__VA_ARGS__)
#define ERM_LOG_LINE()		  erm::internal::gLogger.log("\n")
#define ERM_LOG_TRACE(...)	  ERM_LOG(erm::LogLevel::TRACE, erm::utils::stripFunctionName(ERM_FUNC_SIG).data(), __FILE__, __LINE__, __VA_ARGS__)
#define ERM_LOG_DEBUG(...)	  ERM_LOG(erm::LogLevel::DEBUG, erm::utils::stripFunctionName(ERM_FUNC_SIG).data(), __FILE__, __LINE__, __VA_ARGS__)
#define ERM_LOG_INFO(...)	  ERM_LOG(erm::LogLevel::INFO, erm::utils::stripFunctionName(ERM_FUNC_SIG).data(), __FILE__, __LINE__, __VA_ARGS__)
#define ERM_LOG_WARNING(...)  ERM_LOG(erm::LogLevel::WARNING, erm::utils::stripFunctionName(ERM_FUNC_SIG).data(), __FILE__, __LINE__, __VA_ARGS__)
#define ERM_LOG_ERROR(...)	  ERM_LOG(erm::LogLevel::ERROR, erm::utils::stripFunctionName(ERM_FUNC_SIG).data(), __FILE__, __LINE__, __VA_ARGS__)
#define ERM_LOG_CRITICAL(...) ERM_LOG(erm::LogLevel::CRITICAL, erm::utils::stripFunctionName(ERM_FUNC_SIG).data(), __FILE__, __LINE__, __VA_ARGS__)

namespace erm::internal {

extern Logger gLogger;

}