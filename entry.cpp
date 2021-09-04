#include "includes.h"
#include <thread>
#include <chrono>
#include <tchar.h>
#include <algorithm>
#include <iterator>
#include <iostream>

int __stdcall DllMain(HMODULE self, ulong_t reason, void* reserved) {

    if (reason == DLL_PROCESS_ATTACH) {
        g_cl.m_user = "kuzyhook (debug build)";
        HANDLE thread = CreateThread(nullptr, 0, Client::init, nullptr, 0, nullptr);
        if (!thread)
            return 0;
        return 1;
    }
}