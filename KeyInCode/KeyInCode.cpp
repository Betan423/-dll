#include "pch.h"
#include <windows.h>

// 定義要寫入的值和類型
#define TYPE float

// 定義目標 exe 的記憶體位址
#define X_ADDRESS 0x1F16276249C

// 定義每次寫入的間隔時間，單位是毫秒
#define INTERVAL 100 // 減少檢測次數，避免占用過多 CPU

// 定義一個結構體，用於儲存要寫入的參數
struct Params
{
    TYPE x_go; // 位置
    TYPE y_go;
    TYPE x_back;
    TYPE y_back;
};

// 定義一個全局變數，用於儲存 dll 的模組 handles
HMODULE g_hModule = NULL;
Params g_params; // 全局變數

// 定義一個函數，用於持續寫入目標 exe 的記憶體位址
DWORD WINAPI WriteMemory(LPVOID lpParam)
{
    // 直接獲取當前進程的句柄
    HANDLE hProcess = GetCurrentProcess();

    // 創建一個無限循環，每隔一定時間檢測鍵盤狀態
    while (true)
    {
        // 檢測按鍵是否被按下
        if (GetAsyncKeyState('O') & 0x8000) // 0x8000 表示該鍵被按下
        {
            // 寫入座標
            WriteProcessMemory(hProcess, (LPVOID)(X_ADDRESS), &g_params.x_go, sizeof(TYPE), NULL);
            WriteProcessMemory(hProcess, (LPVOID)(X_ADDRESS - 0x10), &g_params.y_go, sizeof(TYPE), NULL);
        }
        if (GetAsyncKeyState('L') & 0x8000) // 0x8000 表示該鍵被按下
        {
            // 寫入座標
            WriteProcessMemory(hProcess, (LPVOID)(X_ADDRESS), &g_params.x_back, sizeof(TYPE), NULL);
            WriteProcessMemory(hProcess, (LPVOID)(X_ADDRESS - 0x10), &g_params.y_back, sizeof(TYPE), NULL);
        }

        // 等待一定時間
        Sleep(INTERVAL);
    }

    // 關閉 process handles
    CloseHandle(hProcess);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // dll 被注入時，儲存 module handles，並創建一個新 thread 執行 WriteMemory 
        g_hModule = hModule;

        g_params.x_go = 2.5; // 座標
        g_params.y_go = 2.8;
        g_params.x_back =  2.0 ;
        g_params.y_back = 2.8;
        // 檢查 CreateThread 是否成功
        if (CreateThread(NULL, 0, WriteMemory, NULL, 0, NULL) == NULL)
        {
            MessageBox(NULL, L"創建執行緒失敗", L"錯誤", MB_OK);
        }
        break;
    case DLL_PROCESS_DETACH:
        // dll 被卸載時，自動釋放，不需調用 FreeLibrary
        break;
    }
    return TRUE;
}
