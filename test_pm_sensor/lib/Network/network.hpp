#ifndef NET_WORK
#define NET_WORK

#include <Arduino.h>
#include <cstring>
#include <utility/wifi_drv.h>
#include <utility/wl_definitions.h>
#include <utility/wl_types.h>

class Network
{
    static char *ssid;
    static char *password;

public:
    // init network
    static void Setup(const char *SSID, const char *PASS);
    // set network ssid
    static void SetSSID(const char *SSID);
    // set network password
    static void SetPassword(const char *PASS);

    static uint8_t GetStatus();
    // connect with WPA
    static void Connect(int attempts = 3, unsigned long timeout = 10000);
    // disconnect from network
    static void Disconnect();

    static void PrintAvailableNetworks();
    
    static void PrintNetworkInfo();
};

#endif