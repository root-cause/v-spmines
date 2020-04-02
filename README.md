# v-spmines

Vehicle proximity mine mod for GTA V.

See [GTA5-Mods](https://www.gta5-mods.com/scripts/sp-proximity-mines) for a detailed look.

## default_data.xml

This file is copied to the output folder after building the mod and without it, vehicles can't drop proximity mines.

Three types of data is loaded from here:

* Which particle effect assets/libraries are loaded for mine explosion effects. (ParticleFxAssets node)

* What kind of mines can be dropped. (MineSets node)

* Which vehicle models can drop mines. (VehicleModels node)

for future-proofing of the mod.

### ParticleFxAssets

Each `Item` node will make a `REQUEST_NAMED_PTFX_ASSET` call with fxName being the node's value.

### MineSets

Each `Set` node must have `name` (must be unique, used to link vehicle models to the set) and `model` (name of the dropped mine model) attributes.

Each `Item` node of a set equals to a mod index of vehicle mod type 9. (Mod type 9 is hardcoded at the moment because all vehicles that can drop proximity mines use it)

Value of the `Item` nodes are used to spawn the mines.

### VehicleModels

Each `Model` node must have a `name` (spawn name of the vehicle) and `mineSet` (mine set name) attributes.

### Example

```xml
<SPMineData>
    <ParticleFxAssets>
        <!-- This ptfx asset contains mine explosion effects -->
        <Item>veh_xs_vehicle_mods</Item>
    </ParticleFxAssets>

    <MineSets>
        <Set name="COOL_BOND_CAR" model="w_ex_vehiclemine">
            <Item>VEHICLE_WEAPON_MINE_SPIKE</Item> <!-- SET_VEHICLE_MOD(handle, 9, 0) -->
            <Item>VEHICLE_WEAPON_MINE_SLICK</Item> <!-- SET_VEHICLE_MOD(handle, 9, 1) -->
        </Set>
    </MineSets>

    <VehicleModels>
        <!--
            As you can see here, we make a set named "COOL_BOND_CAR" first, then use it here to tell the mod "jb7002" model can drop:
            VEHICLE_WEAPON_MINE_SPIKE
            VEHICLE_WEAPON_MINE_SLICK
        -->
        <Model name="jb7002" mineSet="COOL_BOND_CAR" />

        <!-- Now if we want to add the same mines to another vehicle (RC Bandito), we can do this: -->
        <Model name="rcbandito" mineSet="COOL_BOND_CAR" />

        <!--
            Note that mod type 9 is not "Proximity Mine" for a lot of vehicle models.
            So while the mod accepts all vehicle models, it plays nicely with the models that support proximity mines only.
        -->
    </VehicleModels>
</SPMineData>
```