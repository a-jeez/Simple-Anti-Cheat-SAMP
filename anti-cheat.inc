#if defined _inc_anti_cheat
    #undef _inc_anti_cheat
#endif

#if defined _anticheat_included
    #endinput
#endif
#define _anticheat_included

#include <a_samp>

enum acEnum
{
	Float:	acPosX,
	Float:	acPosY,
	Float:	acPosZ,
	Float:	acSpawnX,
	Float:	acSpawnY,
	Float:	acSpawnZ,
 			acAirbreakTime,
			acImmunity
};

static ACInfo[MAX_PLAYERS][acEnum];

forward OnPlayerHackTeleport(playerid, Float:distance);
forward OnPlayerAirbreak(playerid);

static const Float:modShopCoords[][] =
{
    {10.0,  -1936.0861, 237.4443,   34.3125},
	{10.0,  -2714.6309, 217.3955,   4.2965},
	{10.0,  2386.7686,  1042.1649,  10.8203},
	{10.0,  2644.9480,  -2037.6088, 13.5500},
	{10.0,  1041.2783,  -1027.8124, 32.1016},
	{50.0,  616.0253,   -8.0157,    1000.9219},
	{50.0,  615.2108,   -75.3288,   997.9922},
	{50.0,  612.9303,   -124.1615,  997.9922}
};

static ACInfo_RangeCheck(Float:x1, Float:y1, Float:z1, Float:radius, Float:x2, Float:y2, Float:z2)
{
	x1 -= x2;
	y1 -= y2;
	z1 -= z2;

	return ((x1 * x1) + (y1 * y1) + (z1 * z1)) < (radius * radius);
}

static Float:ACInfo_GetSpeed(playerid)
{
	new
		Float:vx,
		Float:vy,
		Float:vz;

	if(IsPlayerInAnyVehicle(playerid))
	{
		GetVehicleVelocity(GetPlayerVehicleID(playerid), vx, vy, vz);
	}
	else
	{
	    GetPlayerVelocity(playerid, vx, vy, vz);
	}

	return floatsqroot((vx * vx) + (vy * vy) + (vz * vz));
}

static ACInfo_IsPlayerNearModShop(playerid)
{
	if(IsPlayerInAnyVehicle(playerid))
	{
		for(new i = 0; i < sizeof(modShopCoords); i ++)
		{
		    if(IsPlayerInRangeOfPoint(playerid, modShopCoords[i][0], modShopCoords[i][1], modShopCoords[i][2], modShopCoords[i][3]))
		    {
		        return 1;
			}
		}
	}
	
	return 0;
}

stock ACInfo_PutPlayerInVehicle(playerid, vehicleid, seatid)
{
	if(IsPlayerConnected(playerid))
	{
	    if(GetVehicleModel(vehicleid))
	    {
	        GetVehiclePos(vehicleid, ACInfo[playerid][acPosX], ACInfo[playerid][acPosY], ACInfo[playerid][acPosZ]);
		}

		ACInfo[playerid][acImmunity] = gettime() + 5;
	}

	return PutPlayerInVehicle(playerid, vehicleid, seatid);
}

#if defined _ALS_PutPlayerInVehicle
	#undef PutPlayerInVehicle
#else
	#define _ALS_PutPlayerInVehicle
#endif

#define PutPlayerInVehicle ACInfo_PutPlayerInVehicle

stock ACInfo_SetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
	new
	    ret = SetPlayerPos(playerid, x, y, z);

	if(ret)
	{
	    ACInfo[playerid][acPosX] = x;
	    ACInfo[playerid][acPosY] = y;
	    ACInfo[playerid][acPosZ] = z;
	    ACInfo[playerid][acImmunity] = gettime() + 5;
	}

	return ret;
}

#if defined _ALS_SetPlayerPos
	#undef SetPlayerPos
#else
	#define _ALS_SetPlayerPos
#endif

#define SetPlayerPos ACInfo_SetPlayerPos

stock ACInfo_SetVehiclePos(vehicleid, Float:x, Float:y, Float:z)
{
	new
	    ret = SetVehiclePos(vehicleid, x, y, z);

	if(ret)
	{
		for(new i = 0, l = GetPlayerPoolSize(); i <= l; i ++)
		{
		    if(GetPlayerState(i) == PLAYER_STATE_DRIVER && IsPlayerInVehicle(i, vehicleid))
		    {
		        ACInfo[i][acPosX] = x;
		        ACInfo[i][acPosY] = y;
		        ACInfo[i][acPosZ] = z;
		        ACInfo[i][acImmunity] = gettime() + 5;
		        break;
		    }
		}
	}

	return ret;
}

#if defined _ALS_SetVehiclePos
	#undef SetVehiclePos
#else
	#define _ALS_SetVehiclePos
#endif

#define SetVehiclePos ACInfo_SetVehiclePos

stock ACInfo_SetSpawnInfo(playerid, team, skin, Float:x, Float:y, Float:z, Float:rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo)
{
	if(IsPlayerConnected(playerid))
	{
	    ACInfo[playerid][acSpawnX] = x;
	    ACInfo[playerid][acSpawnY] = y;
	    ACInfo[playerid][acSpawnZ] = z;
	}

	return SetSpawnInfo(playerid, team, skin, x, y, z, rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
}

#if defined _ALS_SetSpawnInfo
	#undef SetSpawnInfo
#else
	#define _ALS_SetSpawnInfo
#endif

#define SetSpawnInfo ACInfo_SetSpawnInfo

public OnEnterExitModShop(playerid, enterexit, interiorid)
{
	GetPlayerPos(playerid, ACInfo[playerid][acPosX], ACInfo[playerid][acPosY], ACInfo[playerid][acPosZ]);
	ACInfo[playerid][acImmunity] = gettime() + 5;

	#if defined ACInfo_OnEnterExitModShop
	    return ACInfo_OnEnterExitModShop(playerid, enterexit, interiorid);
	#else
		return 1;
	#endif
}

#if defined _ALS_OnEnterExitModShop
	#undef OnEnterExitModShop
#else
	#define _ALS_OnEnterExitModShop
#endif

#define OnEnterExitModShop ACInfo_OnEnterExitModShop

#if defined ACInfo_OnEnterExitModShop
	forward ACInfo_OnEnterExitModShop(playerid, enterexit, interiorid);
#endif

public OnPlayerSpawn(playerid)
{
    ACInfo[playerid][acImmunity] = gettime() + 5;

    #if defined ACInfo_OnPlayerSpawn
        return ACInfo_OnPlayerSpawn(playerid);
	#else
	    return 1;
	#endif
}

#if defined _ALS_OnPlayerSpawn
	#undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif

#define OnPlayerSpawn ACInfo_OnPlayerSpawn

#if defined ACInfo_OnPlayerSpawn
	forward ACInfo_OnPlayerSpawn(playerid);
#endif

public OnPlayerUpdate(playerid)
{
	if(GetPlayerState(playerid) != PLAYER_STATE_SPECTATING && GetPlayerState(playerid) != PLAYER_STATE_NONE)
	{
		if(gettime() > ACInfo[playerid][acImmunity])
		{
			if(!IsPlayerInRangeOfPoint(playerid, 100.0, ACInfo[playerid][acPosX], ACInfo[playerid][acPosY], ACInfo[playerid][acPosZ]) && !IsPlayerInRangeOfPoint(playerid, 5.0, ACInfo[playerid][acSpawnX], ACInfo[playerid][acSpawnY], ACInfo[playerid][acSpawnZ]))
			{
			    new
					Float:x,
					Float:y,
					Float:z;

				GetPlayerPos(playerid, x, y, z);

				if(!ACInfo_RangeCheck(ACInfo[playerid][acPosX], ACInfo[playerid][acPosY], ACInfo[playerid][acPosZ], 3.0, ACInfo[playerid][acSpawnX], ACInfo[playerid][acSpawnY], ACInfo[playerid][acSpawnZ]) && x != 0.0 && y != 0.0 && z != 0.0 && GetPlayerState(playerid) != PLAYER_STATE_PASSENGER && !ACInfo_IsPlayerNearModShop(playerid))
				{
   					CallLocalFunction("OnPlayerHackTeleport", "if", playerid, GetPlayerDistanceFromPoint(playerid, ACInfo[playerid][acPosX], ACInfo[playerid][acPosY], ACInfo[playerid][acPosZ]));
				}
			}
			else if(gettime() > ACInfo[playerid][acAirbreakTime] && !IsPlayerInRangeOfPoint(playerid, 10.0, ACInfo[playerid][acPosX], ACInfo[playerid][acPosY], ACInfo[playerid][acPosZ]))
			{
				if((GetPlayerState(playerid) == PLAYER_STATE_ONFOOT || GetPlayerState(playerid) == PLAYER_STATE_DRIVER) && GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID && GetPlayerSurfingObjectID(playerid) == INVALID_OBJECT_ID)
				{
				    new
				        Float:px,
				        Float:py,
				        Float:pz;

					GetPlayerPos(playerid, px, py, pz);

					px = floatabs(ACInfo[playerid][acPosX] - px);
					py = floatabs(ACInfo[playerid][acPosY] - py);
					pz = floatabs(ACInfo[playerid][acPosZ] - pz);

		            if(((0.5 <= px < 13.9) && (0.5 <= py <= 13.9)) || (4.2 <= pz <= 19.2))
					{
						new
						    Float:speed = ACInfo_GetSpeed(playerid);

						if((0.082 <= speed <= 0.215 && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT) || (0.0009 <= speed <= 0.0013 && GetPlayerState(playerid) == PLAYER_STATE_DRIVER))
						{
							CallLocalFunction("OnPlayerAirbreak", "i", playerid);
	     					ACInfo[playerid][acAirbreakTime] = gettime() + 1;
						}
					}
			    }
			}
		}

		GetPlayerPos(playerid, ACInfo[playerid][acPosX], ACInfo[playerid][acPosY], ACInfo[playerid][acPosZ]);
	}

	#if defined ACInfo_OnPlayerUpdate
	    return ACInfo_OnPlayerUpdate(playerid);
	#else
	    return 1;
	#endif
}

#if defined _ALS_OnPlayerUpdate
	#undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate ACInfo_OnPlayerUpdate

#if defined ACInfo_OnPlayerUpdate
	forward ACInfo_OnPlayerUpdate(playerid);
#endif
