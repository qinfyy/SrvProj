// SrvProj.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "AccountServer.h"
#include <iostream>
#include "Handlers\GatewayController.h"
#include "Logger.h"
#include "DbMgr.h"
#include "AeadTool.h"
#include "Util.h"
#include "Config.h"
#include "GameServices.h"
#include "Command\CommandMgr.h"
#include ".\Resources\ResourceLoader.h"

HANDLE g_hExitEvent = NULL;
HANDLE g_hCleanupDone = NULL;    // 清理完成标志

BOOL WINAPI ConsoleHandler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        LOG_INFO("收到关闭信号，正在关闭服务器...");
        if (g_hExitEvent) {
            SetEvent(g_hExitEvent);
        }

        WaitForSingleObject(g_hCleanupDone, INFINITE);
        return TRUE;
    }
    return FALSE;
}

int main() {
    Logger::Instance().ShowTimeStamp(true)
        .ShowFileName(true)
        .ShowLineNumber(true)
        .EnableColors(true).LogToFile();

    g_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_hCleanupDone = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (!g_hExitEvent || !g_hCleanupDone) {
        LOG_ERROR("创建事件失败");
        if (g_hExitEvent) {
            CloseHandle(g_hExitEvent);
        }

        if (g_hCleanupDone) {
            CloseHandle(g_hCleanupDone);
        }
        return 1;
    }

    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        LOG_ERROR("无法设置控制台中断事件处理器");
        return 1;
    }

    // AeadTool Test
    //[20:18:22] [INFO] Client Public: BIqTyNHu7VmQ+vIpLigrWuBR6EhLDnU0XWzNgMNJjm4TILtf0ROb/6LgZaJKWzWQSgR19sYYIpaomgmcOk+XXIY=
    //[20:18:22] [INFO] Server Public: BF0w+kxH+++1s0rlYOs+u1zkfVgQeVT38rPciC7CDV9UWdFnUpZgsNDMJEsM/FrB6lhSpHg9I4ttJyuFXdRp8y8=
    //[20:18:22] [INFO] Server Private: NVgvKHgQWh+w1oE2uRP/iOgpMb5ScbHouh26Gzhz0ak=
    //[20:18:22] [INFO] sharedKey: Myv/0jlvSs2J/WBRYdQlUuBzLoq0GKNDPl/eaIPj3mE=
    //[20:18:22] [INFO] Key: hxnOJJIjFEGsopvEMYKnBOqILgowF4h/HMJO8xo+Zg8=
 //   std::string clientPunlicKey = Base64Decode("BIqTyNHu7VmQ+vIpLigrWuBR6EhLDnU0XWzNgMNJjm4TILtf0ROb/6LgZaJKWzWQSgR19sYYIpaomgmcOk+XXIY=");
 //   std::string serverPunlicKey = Base64Decode("BF0w+kxH+++1s0rlYOs+u1zkfVgQeVT38rPciC7CDV9UWdFnUpZgsNDMJEsM/FrB6lhSpHg9I4ttJyuFXdRp8y8=");
 //   std::string serverPriviteKey = Base64Decode("NVgvKHgQWh+w1oE2uRP/iOgpMb5ScbHouh26Gzhz0ak=");


 //   if (serverPriviteKey.size() == 33 && serverPriviteKey[0] == 0)
 //   {
 //       serverPriviteKey.erase(0, 1);
 //   }

 //   LOG_DEBUG("clientPunlicKey: {}", Base64Encode(clientPunlicKey));

    //auto sharedKey = AeadTool::CalECDHSharedKey(serverPriviteKey, clientPunlicKey);
    ////auto info = AeadTool::CalInfo(serverPunlicKey, clientPunlicKey);
 //   auto info = AeadTool::CalInfo(clientPunlicKey, serverPunlicKey);
    //auto secretX = AeadTool::CalSecretX(serverPunlicKey, info, sharedKey);
 //   LOG_DEBUG("sharedKey: {}", Base64Encode(sharedKey));
 //   LOG_DEBUG("secretX: {}", Base64Encode(secretX));

    //Sleep(-1);

    LOG_INFO_CFMT("Srv Proj");
    //Logger::InfoCFmt("no file line");
    Config::Get().LoadFromFile();
    LoadResources();
    if (!DbMgr::Instance().Init(Config::Get().DatabasePath)) {
        return 1;
    }

    //GetServerList();
    //GetNoticeList();
    AccountServer server(Config::Get().httpServerConfig.ip, Config::Get().httpServerConfig.port);
    if (!server.Start()) {
        return 1;
    }

    CommandMgr::StartConsoleThread();

    WaitForSingleObject(g_hExitEvent, INFINITE);

    server.Stop();
    GameServices::Instance().Shutdown();
    DbMgr::Instance().UnInit();

    SetEvent(g_hCleanupDone);

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
