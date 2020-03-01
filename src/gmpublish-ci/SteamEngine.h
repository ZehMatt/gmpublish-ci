#pragma once

#include "Steamtypes.h"

class ISteamUtils;
class IClientUser;
class ISteamRemoteStorage;

#define CLIENTENGINE_INTERFACE_VERSION005 "CLIENTENGINE_INTERFACE_VERSION005"

class IClientEngine
{
public:
    virtual int CreateSteamPipe(int*) = 0;
    virtual int BReleaseSteamPipe(int) = 0;
    virtual int CreateGlobalUser(int *) = 0;
    virtual int ConnectToGlobalUser(int) = 0;
    virtual int CreateLocalUser(int *, int) = 0;
    virtual int CreatePipeToLocalUser(int, int *) = 0;
    virtual int ReleaseUser(int, int) = 0;
    virtual int IsValidHSteamUserPipe(int, int) = 0;
    virtual IClientUser* GetIClientUser(int, int) = 0;
    virtual int GetIClientGameServer(int, int, char const*) = 0;
    virtual int SetLocalIPBinding(unsigned int, unsigned short) = 0;
    virtual int GetUniverseName(int) = 0;
    virtual int GetIClientFriends(int, int, char const*) = 0;
    virtual ISteamUtils* GetIClientUtils(int) = 0;
    virtual int GetIClientBilling(int, int, char const*) = 0;
    virtual int GetIClientMatchmaking(int, int, char const*) = 0;
    virtual int GetIClientApps(int, int, char const*) = 0;
    virtual int GetIClientMatchmakingServers(int, int, char const*) = 0;
    virtual int GetIClientGameSearch(int, int, char const*) = 0;
    virtual int RunFrame(void) = 0;
    virtual int GetIPCCallCount(void) = 0;
    virtual int GetIClientUserStats(int, int, char const*) = 0;
    virtual int GetIClientGameServerStats(int, int, char const*) = 0;
    virtual int GetIClientNetworking(int, int, char const*) = 0;
    virtual ISteamRemoteStorage* GetIClientRemoteStorage(int, int, char const*) = 0;
};

#define STEAM_CALL __cdecl

extern "C" __declspec(dllimport) void* STEAM_CALL CreateInterface(const char*, int);
extern "C" __declspec(dllimport) bool STEAM_CALL Steam_BGetCallback(HSteamPipe hSteamPipe, CallbackMsg_t *pCallbackMsg);
extern "C" void STEAM_CALL Steam_FreeLastCallback(HSteamPipe hSteamPipe);