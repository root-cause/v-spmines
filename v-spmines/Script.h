#pragma once
#include "Logger.h"

struct MineSetData
{
	Hash model;
	std::vector<Hash> weaponHashes;
};

enum VehicleModel : Hash
{
	SPEEDO4 = 0x0D17099D,
	ZR380 = 0x20314B42,
	ZR3802 = 0xBE11EFC6,
	ZR3803 = 0xA7DCC35C,
	DEATHBIKE = 0xFE5F0722,
	DEATHBIKE2 = 0x93F09558,
	DEATHBIKE3 = 0xAE12C99C,
	IMPERATOR = 0x1A861243,
	IMPERATOR2 = 0x619C1B82,
	IMPERATOR3 = 0xD2F77E37,
	SLAMVAN4 = 0x8526E2F5,
	SLAMVAN5 = 0x163F8520,
	SLAMVAN6 = 0x67D52852
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