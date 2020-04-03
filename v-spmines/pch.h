#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define FMT_HEADER_ONLY
#define _CRT_SECURE_NO_WARNINGS

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

#include "joaat.h"

// fmt
#include "fmt/format.h"
#include "fmt/chrono.h"

// pugixml
#include "pugixml.hpp"

// ScriptHookV
#include "main.h"
#include "types.h"
#include "nativeCaller.h"
#include "natives.h"

namespace fs = std::filesystem;