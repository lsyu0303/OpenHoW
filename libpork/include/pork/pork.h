/* OpenHOW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

/* libpork - base library shared between launcher and editor */

#include <PL/platform_math.h>
#include <PL/platform_console.h>

#define PORK_TITLE              "OpenHoW"
#define PORK_EDITOR_TITLE       "OpenHoW Editor"
#define PORK_LOG                "debug"
#define PORK_EDITOR_LOG         "editor"

#define PORK_MAJOR_VERSION  0
#define PORK_MINOR_VERSION  0

#define PORK_BASE_DIR   "pork"

#define PORK_MAPS_DIR   PORK_BASE_DIR "/maps"
#define PORK_CHARS_DIR  PORK_BASE_DIR "/chars"
#define PORK_AUDIO_DIR  PORK_BASE_DIR "/audio"
#define PORK_SKYS_DIR   PORK_BASE_DIR "/skys"
#define PORK_SPEECH_DIR PORK_BASE_DIR "/speech"

// Frontend
#define PORK_FETEXT_DIR     PORK_BASE_DIR "/fetext"
#define PORK_FEBMPS_DIR     PORK_BASE_DIR "/febmps"
#define PORK_FESOUNDS_DIR   PORK_BASE_DIR "/fesounds"

///////////////////////////////////////////////////

enum {
    LOG_LEVEL_PORK,
    LOG_LEVEL_LAUNCHER,
};

#define print(...)          plLogMessage(LOG_LEVEL_PORK, __VA_ARGS__)
#define print_error(...)    plLogMessage(LOG_LEVEL_PORK, __VA_ARGS__); exit(-1)
#ifdef _DEBUG
#   define print_debug(...)      print(__VA_ARGS__)
#else
#   define DPRINT(...)      (__VA_ARGS__)
#endif

///////////////////////////////////////////////////

#define pork_fclose(FILE) if((FILE) != NULL) { fclose((FILE)); (FILE) = NULL; }
#define pork_free(DATA) if((DATA) != NULL) { free((DATA)); (DATA) = NULL; }

// todo, the below is a basic outline for API

PL_EXTERN_C

PL_EXTERN void InitPork(int argc, char **argv);

PL_EXTERN void ExtractGameData(const char *path);

PL_EXTERN_C_END

///////////////////////////////////////////////////

#define MAP_WIDTH   2048

enum {
    FT_MUD,
    FT_GRASS,
    FT_METAL,
    FT_WOOD,
    FT_WATER,
    FT_STONE,
    FT_ROCK,
    FT_SAND,
    FT_ICE,
    FT_SNOW,
    FT_QUAG,
    FT_LAVA,
};

#define TILE_FLAG_WATERY    32
#define TILE_FLAG_MINE      64
#define TILE_FLAG_WALL      128

///////////////////////////////////////////////////

#define MAX_PLAYERS 16

///////////////////////////////////////////////////

#define MAX_STATIC_OBJECTS  1024

enum { // Object flags
    OBJECT_FLAG_PLAYABLE    = 1,
    OBJECT_FLAG_SCRIPTED    = 16,
    OBJECT_FLAG_INSIDE      = 32,
    OBJECT_FLAG_DELAYED     = 64,
};

enum { // event types
    OBJECT_EVENT_NONE,                 // does nothing!
    OBJECT_EVENT_ITEM,                 // spawns an item on destruction
    OBJECT_EVENT_PROMOTION,            // spawns a promotion point on destruction
    OBJECT_EVENT_PROTECT = 4,          // spawns a promotion point if the object is not destroyed
    OBJECT_EVENT_AIRDROP_ITEM = 7,     // spawns item airdrop on destruction
    OBJECT_EVENT_AIRDROP_PROMOTION,    // spawns promotion point on destruction
    OBJECT_EVENT_GROUP_ITEM = 13,      // spawns item when group is destroyed
    OBJECT_EVENT_GROUP_PROMOTION,      // spawns promotion point when group is destroyed
    OBJECT_EVENT_REWARD = 19,          // returns specified item on destruction to destructor
    OBJECT_EVENT_GROUP_SPAWN,          // spawns group on destruction
    OBJECT_EVENT_VICTORY,              // triggers victory on destruction
    OBJECT_EVENT_BURST,                // spawns group upon destruction by TNT
    OBJECT_EVENT_GROUP_OBJECT,         // spawns group when object's group is destroyed
};

typedef struct Actor { // Generic Object Properties
    PLVector3 position, angles;
    PLVector3 bounds[2];

    unsigned int spawn_delay;
    unsigned int team;

    int16_t health;

    unsigned int type;
} Actor;

enum {
    ITEM_TROTTER = 1,
    ITEM_KNIFE,
    ITEM_BAYONET,
    ITEM_SWORD,
    ITEM_CATTLEPROD,
    ITEM_PISTOL,
    ITEM_RIFLE,
    ITEM_RIFLE_BURST,
    ITEM_MACHINEGUN,
    ITEM_HMG,
    ITEM_SNIPER_RIFLE,
    ITEM_SHOTGUN,
    ITEM_FLAMETHROWER,
    ITEM_ROCKET_LAUNCHER = 0x0E,
    ITEM_GUIDED_MISSILE = 0x0F,
    ITEM_MEDICINE_DART = 0x10,
    ITEM_TRANQ = 0x11,
    ITEM_GRENADE = 0x12,
    ITEM_CLUSTER_GRENADE = 0x13,
    ITEM_HX_GRENADE = 0x14,
    ITEM_ROLLER_GRENADE = 0x15,
    ITEM_CONFUSION_GAS = 0x16,
    ITEM_FREEZE_GAS = 0x17,
    ITEM_MADNESS_GAS = 0x18,
    ITEM_POISON_GAS = 0x19,
    ITEM_MORTAR = 0x1A,
    ITEM_BAZOOKA = 0x1B,
    ITEM_AIRBURST = 0x1C,
    ITEM_SUPER_AIRBURST = 0x1D,
    ITEM_MEDICINE_BALL = 0x1E,
    ITEM_HOMING_MISSILE = 0x1F,
    ITEM_MINE = 0x20,
    AntiPMine = 0x21,
    ITEM_TNT = 0x22,

    //Vehicle weapons
    VehLongRangeShell = 35,
    VehShockShell,
    Veh1000LBSShell,
    VehFireShell = 0x26,
    VehGasShell = 0x27,
    VehMineShell = 0x28,
    VehHeavyMachineGun = 0x29,
    VehFlamethrower = 0x2A,
    ITEM_VEHICLE_AIRBURST = 0x2B,
    ITEM_VEHICLE_BAZOOKA = 0x2C,
    ITEM_VEHICLE_MORTAR = 0x2D,

    //Missing 0x2E
    //Missing 0x2F
    //Missing 0x30
    //Missing 0x31
    //Missing 0x32

    ITEM_JETPACK = 0x33,
    ITEM_SUICIDE = 0x34,
    ITEM_HEALINGHANDS = 0x35,
    ITEM_SELFHEAL = 0x36,
    ITEM_PICKPOCKET = 0x37,
    ITEM_SHOCKWAVE = 0x38,
    ITEM_SPECIALOPS = 0x39,
    ITEM_AIRSTRIKE = 0x3A,
    FireRainAirstrike = 0x3B,

    //Tools
    ITEM_ENTERVEHICLE = 0x3C,
    ITEM_ENTERBUILDING = 0x3D,
    ITEM_EJECTPIG = 0x3E,
    ITEM_MAPVIEW = 0x3F,
    ITEM_BINOCULARS = 64,
    ITEM_SKIPTURN,
    ITEM_SURRENDER,

    ITEM_SUPERTNT,
    ITEM_HIDE,
    ITEM_SUPERSHOTGUN,
    ITEM_SHRAPNELGRENADE,
    ITEM_GRENADELAUNCHER,

    //Missing 0x48

    ITEM_RANDOM = 73,
    ITEM_HEALTH = 255,

    // Any custom types go here!!!
};

enum {
    
    //MCAP (in-game)
    ANI_RUN_NORMAL = 0,         // Run cycle (normal)
    ANI_RUN_WOUNDED1 = 1,       // Run cycle (wounded)
    ANI_RUN_WOUNDED2 = 2,       // Run cycle (more wounded)
    ANI_WALK_BACKWARD = 3,      // Walk cycle (backwards)
    ANI_TURN = 4,               // Turning on Spot
    ANI_SWIM1 = 5,              // Swimming
    ANI_SWIM2 = 6,              // Swimming like Rick
    ANI_ENTER = 7,              // Getting into Vehicles
    ANI_JUMP_START = 8,         // Jumping - Start
    ANI_JUMP_LOOP = 9,          // Jumping - Middle
    ANI_JUMP_END = 10,          // Jumping - End
    ANI_CLIMB = 11,             // Scramble
    ANI_EQUIP_HANDGUN = 12,     // Getting out Handgun
    ANI_EQUIP_RIFLE = 13,       // Getting out Rifle
    ANI_EQUIP_MG = 14,          // Getting out Machine gun
    ANI_EQUIP_HEAVY = 15,       // Getting out Heavy weapon
    ANI_EQUIP_PUNCH = 16,       // Getting out Punch
    ANI_EQUIP_THROW = 17,       // Getting out Grenade
    ANI_EQUIP_MEELE = 18,       // Getting out Sword / Knife
    ANI_USE_THROW = 19,         // Using Grenade
    ANI_USE_PUNCH = 20,         // Using Punch
    ANI_USE_MEELE = 21,         // Sword / Knife
    ANI_USE_BAYONET = 22,       // Bayonet
    ANI_USE_HANDGUN = 23,       // Aiming Handgun
    ANI_USE_RIFLE = 24,         // Aiming Rifle
    ANI_USE_MG = 25,            // Aiming Machine gun
    ANI_USE_HEAVY = 26,         // Aiming Heavy weapon
    ANI_IDLE1 = 27,             // Standing around cycle 1
    ANI_IDLE2 = 28,             // Standing around cycle 2
    ANI_HURT = 29,              // Very Wounded
    ANI_PROUD = 30,             // Lord Flash-Heart Pose
    ANI_LOOK = 31,              // Looking around
    ANI_STUPID = 32,            // Looking gormless
    ANI_FEAR = 33,              // Cowering
    ANI_CLEAN1 = 34,            // Brushoff 1
    ANI_CLEAN2 = 35,            // Brushoff 2
    ANI_CLEAN3 = 36,            // Brushoff 3
    ANI_SNEEZE = 37,            // Sneeze
    ANI_FALL = 38,              // Flying through air/falling
    ANI_BOUNCE = 39,            // Bouncing on B-Hind
    ANI_STANDUP = 40,           // Getting to feet
    ANI_CELEBRATE1 = 41,        // Celebration #1
    ANI_CELEBRATE2 = 42,        // Celebration #2
    ANI_CELEBRATE3 = 43,        // Celebration #3
    ANI_SALUTE = 44,            // Salute
    ANI_LOOK_BACK = 45,         // Look back
    ANI_THINK = 46,             // Thinking
    ANI_DEATH1 = 47,            // Dying #1
    ANI_DEATH2 = 48,            // Dying #2
    ANI_DEATH3 = 49,            // Dying #3
    ANI_DROWN = 50,             // Drowning
    ANI_IDLE_COLD = 51,         // Idle Cold
    ANI_IDLE_HOT = 52,          // Idle Hot
    ANI_USE_MINE = 53,          // Lay Mine
    ANI_USE_HEAL = 54,          // Heal
    ANI_USE_STEAL = 55,         // Pick pocket
    ANI_USE_AIRSHIP = 56,       // Air strike
    ANI_USE_SUICIDE = 57,       // Hari Kiri
    ANI_PARACHUTE = 58,         // Parachuting
    
    //FEMCAP (front-end)
    ANI_59 = 59,                //
    ANI_60 = 60,                //
    ANI_61 = 61,                //
    ANI_62 = 62,                //
    ANI_63 = 63,                //
    ANI_64 = 64,                //
    ANI_65 = 65,                //
    ANI_66 = 66,                //
    ANI_67 = 67,                //
    ANI_68 = 68,                //
    ANI_69 = 69,                //
    ANI_70 = 70,                //
    ANI_71 = 71,                //
    ANI_72 = 72,                //
    ANI_73 = 73,                //
    ANI_74 = 74,                //
    ANI_75 = 75,                //
    ANI_76 = 76,                //
    ANI_77 = 77,                //
    ANI_78 = 78,                //
    ANI_79 = 79,                //
    ANI_80 = 80,                //
    ANI_81 = 81,                //
    ANI_82 = 82,                //
    ANI_83 = 83,                //
    ANI_84 = 84,                //
    ANI_85 = 85,                //
    ANI_86 = 86,                //
    ANI_87 = 87,                //
    ANI_88 = 88,                //
    ANI_89 = 89,                //
    ANI_90 = 90,                //
    ANI_91 = 91,                //
    ANI_92 = 92,                //
};