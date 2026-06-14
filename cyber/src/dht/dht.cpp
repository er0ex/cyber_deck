#include "../general.h"
#include <DHT.h>

DHT dht(DHT_PIN, DHT_TYPE);

void initDHT(){
    dht.begin();
}

String getDHTData(){
    float t = dht.readTemperature();
    if (isnan(t)) {
        Serial.println("[DHT] Temp read failed");
        return "-";
    }
    String res = String(t);
    return res;
}

