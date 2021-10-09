#include <a_samp>
#include <anti-cheat>

public OnFilterScriptInit()
{
    return 1;
}

public OnFilterScriptExit()
{
    return 1;
}

public OnPlayerHackTeleport(playerid, Float:distance)
{
  SendClientMessage(playerid, -1, "You have detected hack teleport.");
  Kick(playerid);
  return 1;
}

public OnPlayerAirbreak(playerid)
{
  SendClientMessage(playerid, -1, "You have detected airbreak teleport.");
  Kick(playerid); 
  return 1;
}
