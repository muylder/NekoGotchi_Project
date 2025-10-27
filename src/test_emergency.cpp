// EMERGENCY TEST - Absolute minimum code
#include <Arduino.h>

void setup() {
    // Serial múltiplas vezes
    Serial.begin(115200);
    delay(1000);
    Serial.begin(115200);
    delay(1000);
    Serial.begin(115200);
    
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("================================");
    Serial.println("EMERGENCY TEST RUNNING!");
    Serial.println("If you see this, Serial works!");
    Serial.println("================================");
    Serial.flush();
    
    delay(5000);
}

void loop() {
    static unsigned long count = 0;
    Serial.print("Loop count: ");
    Serial.println(count++);
    delay(1000);
}
