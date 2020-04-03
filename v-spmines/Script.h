#pragma once
#include "Logger.h"

struct MineSetData
{
	Hash model;
	std::vector<Hash> weaponHashes;
};

const unsigned int INPUT_VEH_HORN = 86;
const unsigned int MineCooldown = 750; // In milliseconds

void ScriptInit();
void ScriptUpdate();
void LoadModel(Hash modelHash);

Vector3 func_8595(const Vector3& vParam0, const Vector3& vParam1, float fParam2, float fParam3, float fParam4);
float func_8596(float fParam0, float fParam1, float fParam2, float fParam3, float fParam4);
void func_8597(Vehicle vehicleEntity, Hash vehicleModel, Vector3& uParam2, Vector3& uParam3, Vector3& uParam4, Vector3& uParam5);

static Logger ScriptLog(fmt::format("{}.log", MOD_NAME), LogLevel::LOG_INFO, true);