#include "Utils/Logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace g3d {
void log(LogLevel level, const std::string& message) {
  static const char* names[] = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
  const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::tm local{};
#ifdef _WIN32
  localtime_s(&local, &now);
#else
  local = *std::localtime(&now);
#endif
  std::cerr << '[' << std::put_time(&local, "%H:%M:%S") << "] [" << names[static_cast<int>(level)] << "] " << message << '\n';
}
}
