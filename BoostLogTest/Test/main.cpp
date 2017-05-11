#include <iostream>

#include "logger.h"

using namespace std;

int main()
{
    LogInit();
    LogStart();

    LogInfo("ABSdg");
    LogError("Error");
    LogWarning("Warning");
    LogDebug("Test");
    LogData("Fuction", "Test");

    LogEnableDebug();
    LogDebug("Debug");
    LogData("Fuction", "Data");

    LogStop();
    LogDeInit();
}
