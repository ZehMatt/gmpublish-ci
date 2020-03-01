#include <iostream>

#include "SteamEngine.h"
#include "SteamRemote.h"
#include "SteamUser.h"
#include "SteamUtils.h"

#include <windows.h>

static int LaunchGMPublish(int argc, const char* argv[])
{
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    char commandLineBuf[4096]{};
    int p = 0;

    p += snprintf(commandLineBuf + p, sizeof(commandLineBuf) - p, "gmpublish.exe");
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-login") == 0 || strcmp(argv[i], "-pass") == 0)
        {
            i += 1;
            continue;
        }
        p += snprintf(commandLineBuf + p, sizeof(commandLineBuf) - p, " ");
        p += snprintf(commandLineBuf + p, sizeof(commandLineBuf) - p, "\"%s\"", argv[i]);
    }

    if (!CreateProcessA(nullptr,
        commandLineBuf,
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi))
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return -1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return exitCode;
}

int main(int argc, const char* argv[])
{
    const char *login = nullptr;
    const char *pass = nullptr;

    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(argv[i], "-login") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Missing argument: -login\n");
                return -1;
            }
            login = argv[++i];
        }
        else if (strcmp(argv[i], "-pass") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Missing argument: -pass\n");
                return -1;
            }
            pass = argv[++i];
        }
    }
    if (login == nullptr || pass == nullptr)
    {
        printf("No login/password was supplied, aborting.\n");
        return -1;
    }

    IClientEngine* clientEngine = static_cast<IClientEngine*>(CreateInterface(CLIENTENGINE_INTERFACE_VERSION005, 0));
    if (!clientEngine)
    {
        printf("Unable to create CLIENTENGINE_INTERFACE_VERSION_005\n");
        return -1;
    }

    int hPipe;
    int hUser;
    hUser = clientEngine->CreateGlobalUser(&hPipe);

    IClientUser* clientUser = clientEngine->GetIClientUser(hPipe, hUser);
    if (!clientUser)
    {
        printf("Unable to create ISteamUser\n");
        return -1;
    }

    clientEngine->RunFrame();

    printf("Attempting logon...\n");
    clientUser->LogOnWithPassword(login, pass);

    CallbackMsg_t callBack;

    bool exitLoop = false;
    while (!exitLoop)
    {
        clientEngine->RunFrame();

        while (Steam_BGetCallback(hPipe, &callBack))
        {
            switch (callBack.m_iCallback)
            {
            case SteamServersConnected_t::k_iCallback:
            {
                printf("Login success, connected!\n");
                exitLoop = true;
                break;
            }
            case SteamServerConnectFailure_t::k_iCallback:
            {
                SteamServerConnectFailure_t data;
                std::memcpy(&data, callBack.m_pubParam, callBack.m_cubParam);
                switch (data.m_eResult)
                {
                case k_EResultBanned:
                    printf("Login failure: Account was banned\n");
                    break;
                case k_EResultInvalidName:
                    printf("Login failure: Invalid name\n");
                    break;
                case k_EResultInvalidPassword:
                    printf("Login failure: Invalid password\n");
                    break;
                case k_EResultAccountNotFound:
                    printf("Login failure: Account not found\n");
                    break;
                }
                exitLoop = true;
                break;
            }
            }
            Steam_FreeLastCallback(hPipe);
        }
    }

    int res = -1;

    if (clientUser->BLoggedOn())
    {
        printf("Launching GMPublish...\n");
        res = LaunchGMPublish(argc, argv);
    }

    clientEngine->ReleaseUser(hPipe, hUser);
    clientEngine->BReleaseSteamPipe(hPipe);

    return res;
}
