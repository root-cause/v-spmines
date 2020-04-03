#include "pch.h"
#include "Script.h"

std::unordered_map<Hash, MineSetData> g_mineSets;
std::unordered_map<Hash, Hash> g_vehicleModels; /* VehicleModelHash, MineSetNameHash */
int g_lastMineDrop;

void LoadModel(Hash modelHash)
{
	if (!STREAMING::IS_MODEL_VALID(modelHash))
	{
		return;
	}

	STREAMING::REQUEST_MODEL(modelHash);
	while (!STREAMING::HAS_MODEL_LOADED(modelHash)) WAIT(0);
}

// From 1868 decompiled scripts
Vector3 func_8595(const Vector3& vParam0, const Vector3& vParam1, float fParam2, float fParam3, float fParam4)
{
	return Vector3{
		// x
		func_8596(vParam0.x, vParam1.x, fParam2, fParam3, fParam4),
		0,

		// y
		func_8596(vParam0.y, vParam1.y, fParam2, fParam3, fParam4),
		0,

		// z
		func_8596(vParam0.z, vParam1.z, fParam2, fParam3, fParam4),
		0
	};
}

float func_8596(float fParam0, float fParam1, float fParam2, float fParam3, float fParam4)
{
	return (fParam1 - fParam0) / (fParam3 - fParam2) * (fParam4 - fParam2) + fParam0;
}

void func_8597(Vehicle vehicleEntity, Hash vehicleModel, Vector3& uParam2, Vector3& uParam3, Vector3& uParam4, Vector3& uParam5)
{
	Vector3 min, max;
	MISC::GET_MODEL_DIMENSIONS(vehicleModel, &min, &max);

	uParam2 = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(vehicleEntity, min.x, max.y, min.z);
	uParam3 = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(vehicleEntity, max.x, max.y, min.z);
	uParam4 = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(vehicleEntity, min.x, min.y, min.z);
	uParam5 = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(vehicleEntity, max.x, min.y, min.z);
}

// Blades?
bool func_8651(Vehicle vehicleEntity, Hash vehicleModel)
{
	if (!ENTITY::DOES_ENTITY_EXIST(vehicleEntity))
	{
		return false;
	}

	bool isSpecial = false;
	switch (vehicleModel)
	{
		case joaat::generate("zr380"):
		case joaat::generate("zr3802"):
		case joaat::generate("zr3803"):
		case joaat::generate("deathbike"):
		case joaat::generate("deathbike2"):
		case joaat::generate("deathbike3"):
		case joaat::generate("imperator"):
		case joaat::generate("imperator2"):
		case joaat::generate("imperator3"):
		case joaat::generate("slamvan4"):
		case joaat::generate("slamvan5"):
		case joaat::generate("slamvan6"):
			isSpecial = true;
			break;
	}

	return isSpecial && VEHICLE::GET_VEHICLE_MOD(vehicleEntity, 44) > -1;
}

void ScriptInit()
{
	#ifdef MOD_DEBUG
	ScriptLog.SetLogLevel(LogLevel::LOG_DEBUG);
	#endif

	// Reset globals
	g_mineSets.clear();
	g_vehicleModels.clear();
	g_lastMineDrop = 0;

	fs::path dataFilePath = fs::current_path().append(fmt::format("{}.xml", MOD_NAME));
	ScriptLog.Write(LogLevel::LOG_DEBUG, fmt::format("Data path: {}", dataFilePath.generic_string()));

	// Load data
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(dataFilePath.c_str());
	if (result)
	{
		// Load ptfx assets
		ScriptLog.Write(LogLevel::LOG_DEBUG, "========== LOADING PTFX ASSETS ==========");

		for (pugi::xpath_node ptfx : doc.select_nodes("//ParticleFxAssets/Item"))
		{
			pugi::xml_node node = ptfx.node();
			STREAMING::REQUEST_NAMED_PTFX_ASSET(node.text().as_string());

			ScriptLog.Write(LogLevel::LOG_DEBUG, fmt::format("Requested {}.", node.text().as_string()));
		}

		// Load mine sets
		ScriptLog.Write(LogLevel::LOG_DEBUG, "========== LOADING MINESETS ==========");

		for (pugi::xpath_node set : doc.select_nodes("//MineSets/Set"))
		{
			pugi::xml_node node = set.node();
			pugi::xml_attribute name = node.attribute("name");
			pugi::xml_attribute model = node.attribute("model");

			if (name.empty() || model.empty())
			{
				ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("A MineSets node did not have a name or model (at {}), skipping it.", node.offset_debug()));
				continue;
			}

			Hash setHash = joaat::generate(name.value());
			if (g_mineSets.find(setHash) != g_mineSets.end())
			{
				ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("A MineSet node ({}) is defined already, skipping it.", name.value()));
				continue;
			}

			Hash modelHash = joaat::generate(model.value());
			if (!STREAMING::IS_MODEL_VALID(modelHash))
			{
				ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("A MineSet node ({}) has an invalid model ({}), skipping it.", name.value(), model.value()));
				continue;
			}

			MineSetData& data = g_mineSets[setHash];
			data.model = modelHash;

			for (pugi::xml_node setItem : node.children("Item"))
			{
				data.weaponHashes.push_back(joaat::generate(setItem.text().as_string()));
				ScriptLog.Write(LogLevel::LOG_DEBUG, fmt::format("Added {} to {}.", setItem.text().as_string(), name.value()));
			}

			ScriptLog.Write(LogLevel::LOG_DEBUG, fmt::format("Loaded set: {}, model: {}, items: {}.", name.value(), model.value(), data.weaponHashes.size()));
		}

		ScriptLog.Write(LogLevel::LOG_INFO, fmt::format("Loaded {} mine sets.", g_mineSets.size()));

		// Load vehicle models
		ScriptLog.Write(LogLevel::LOG_DEBUG, "========== LOADING VEHICLEMODELS ==========");

		for (pugi::xpath_node model : doc.select_nodes("//VehicleModels/Model"))
		{
			pugi::xml_node node = model.node();
			pugi::xml_attribute name = node.attribute("name");
			pugi::xml_attribute mineSet = node.attribute("mineSet");

			if (name.empty() || mineSet.empty())
			{
				ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("A VehicleModels node did not have a name or mineSet (at {}), skipping it.", node.offset_debug()));
				continue;
			}

			Hash setHash = joaat::generate(mineSet.value());
			if (g_mineSets.find(setHash) == g_mineSets.end())
			{
				ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("A VehicleModels node ({}) contains an invalid mineSet ({}), skipping it.", name.value(), mineSet.value()));
				continue;
			}

			g_vehicleModels[ joaat::generate(name.value()) ] = setHash;
			ScriptLog.Write(LogLevel::LOG_DEBUG, fmt::format("Loaded vehicle: {}, mineSet: {}.", name.value(), mineSet.value()));
		}

		ScriptLog.Write(LogLevel::LOG_INFO, fmt::format("Loaded {} vehicle models.", g_vehicleModels.size()));
	}
	else
	{
		ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("Failed to load data: {}", result.description()));
		return;
	}

	// Start script loop
	while (true)
	{
		ScriptUpdate();
		WAIT(0);
	}
}

void ScriptUpdate()
{
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	if (HUD::IS_PAUSE_MENU_ACTIVE() || !PED::IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		return;
	}

	Vehicle currentVehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);
	if (!VEHICLE::IS_VEHICLE_DRIVEABLE(currentVehicle, false) || VEHICLE::GET_PED_IN_VEHICLE_SEAT(currentVehicle, -1, 0) != playerPed)
	{
		return;
	}

	Hash vehicleModel = ENTITY::GET_ENTITY_MODEL(currentVehicle);
	if (g_vehicleModels.find(vehicleModel) == g_vehicleModels.end())
	{
		return;
	}

	int currentProxMine = VEHICLE::GET_VEHICLE_MOD(currentVehicle, 9);
	const MineSetData& currentMineSet = g_mineSets[ g_vehicleModels[vehicleModel] ];
	if (currentProxMine == -1 || currentProxMine >= currentMineSet.weaponHashes.size())
	{
		return;
	}

	if (!ENTITY::IS_ENTITY_UPRIGHT(currentVehicle, 90.0f) || !VEHICLE::IS_VEHICLE_ON_ALL_WHEELS(currentVehicle) || ENTITY::IS_ENTITY_UPSIDEDOWN(currentVehicle))
	{
		return;
	}

	PAD::DISABLE_CONTROL_ACTION(0, INPUT_VEH_HORN, true);

	int gameTime = MISC::GET_GAME_TIMER();
	if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_VEH_HORN) && (gameTime - g_lastMineDrop) >= MineCooldown)
	{
		g_lastMineDrop = gameTime;
		LoadModel(currentMineSet.model);

		Vector3 vVar3, vVar4, vVar5, vVar6;
		func_8597(currentVehicle, vehicleModel, vVar3, vVar4, vVar5, vVar6);

		Vector3 vVar7 = func_8595(vVar3, vVar4, 0.0f, 1.0f, 0.5f);
		Vector3 vVar8 = func_8595(vVar5, vVar6, 0.0f, 1.0f, 0.5f);
		vVar7.z += 0.2f;
		vVar8.z += 0.2f;

		float fVar9 = (vehicleModel == joaat::generate("speedo4") || func_8651(currentVehicle, vehicleModel)) ? 1.0f : 0.7f;
		Vector3 vVar10 = func_8595(vVar7, vVar8, 0.0f, 1.0f, fVar9);
		vVar7.z -= 0.2f;
		vVar8.z -= 0.2f;

		Vector3 vVar11 = func_8595(vVar7, vVar8, 0.0f, 1.0f, fVar9 + 0.1f);
		MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS_IGNORE_ENTITY_NEW(
			vVar10.x, vVar10.y, vVar10.z,
			vVar11.x, vVar11.y, vVar11.z,
			0,
			1,
			currentMineSet.weaponHashes[currentProxMine],
			playerPed,
			1,
			1,
			-1.0f,
			0,
			0,
			0,
			0,
			1,
			1,
			false
		);

		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(currentMineSet.model);
	}
}