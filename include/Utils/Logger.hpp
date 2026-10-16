#pragma once
#include <string>

namespace g3d {
enum class LogLevel { Debug, Info, Warning, Error, Critical };
void log(LogLevel level, const std::string& message);
}
