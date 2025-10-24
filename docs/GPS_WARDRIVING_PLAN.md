# Plano de Implementação: GPS Wardriving

## Visão Geral
Adicionar funcionalidades completas de wardriving com GPS ao M5Gotchi Pro, baseado no ESP32Marauder.

---

## Funcionalidades

### 1. WiFi Wardriving
**Formato:** Wigle CSV (compatível com WigleWifi)

```csv
WigleWifi-1.4,appRelease=2.0.0,model=M5Gotchi Pro,...
MAC,SSID,AuthMode,FirstSeen,Channel,RSSI,CurrentLatitude,CurrentLongitude,AltitudeMeters,AccuracyMeters,Type
```

**Campos:**
- **MAC:** BSSID do AP
- **SSID:** Nome da rede
- **AuthMode:** [OPEN], [WEP], [WPA_PSK], [WPA2_PSK], etc.
- **FirstSeen:** 2024-01-01 12:00:00
- **Channel:** 1-13
- **RSSI:** -70
- **CurrentLatitude:** 40.7128
- **CurrentLongitude:** -74.0060
- **AltitudeMeters:** 10.5
- **AccuracyMeters:** 5.0
- **Type:** WIFI

### 2. Bluetooth Wardriving
**Mesmo formato CSV, com Type=BLE**

```csv
AA:BB:CC:DD:EE:FF,Device Name,[BLE],2024-01-01 12:00:00,0,-65,40.7128,-74.0060,10.5,5.0,BLE
```

### 3. Station Wardriving
**Detecta clientes WiFi (probe requests)**

```csv
AA:BB:CC:DD:EE:FF,SSID Requested,[WIFI_CLIENT],2024-01-01 12:00:00,6,-55,40.7128,-74.0060,10.5,5.0,WIFI
```

### 4. GPS Tracker
**Exporta trajeto em formato GPX**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<gpx version="1.1" creator="M5Gotchi Pro">
  <trk>
    <name>Wardriving Track</name>
    <trkseg>
      <trkpt lat="40.7128" lon="-74.0060">
        <ele>10.5</ele>
        <time>2024-01-01T12:00:00Z</time>
      </trkpt>
    </trkseg>
  </trk>
</gpx>
```

### 5. POI (Points of Interest)
**Marcar pontos específicos no trajeto**

```xml
<wpt lat="40.7128" lon="-74.0060">
  <ele>10.5</ele>
  <time>2024-01-01T12:00:00Z</time>
  <name>Target Network Found</name>
</wpt>
```

---

## Hardware Necessário

### GPS Module
**Modelos compatíveis:**
- NEO-6M (UART)
- NEO-7M (UART)
- NEO-8M (UART)
- Qualquer módulo NMEA

**Conexão M5Stack:**
```cpp
// UART2
#define GPS_TX 17  // M5Stack GPIO17 -> GPS RX
#define GPS_RX 16  // M5Stack GPIO16 -> GPS TX
```

**Conexão Cardputer:**
```cpp
// Grove Port (requer adaptador)
#define GPS_TX 2
#define GPS_RX 1
```

---

## Implementação

### Código Base do Marauder
**Arquivo:** `WiFiScan.cpp` linhas 3313-3385

```cpp
void executeWarDrive() {
    // Scan WiFi networks
    int n = WiFi.scanNetworks(false, true, false, 110, channel);
    
    for (int i = 0; i < n; i++) {
        String mac = WiFi.BSSIDstr(i);
        String ssid = WiFi.SSID(i);
        String auth = security_int_to_string(WiFi.encryptionType(i));
        int channel = WiFi.channel(i);
        int rssi = WiFi.RSSI(i);
        
        // Get GPS data
        if (gps.getFixStatus()) {
            String csv_line = mac + "," + ssid + "," + auth + "," +
                            gps.getDatetime() + "," + 
                            String(channel) + "," + String(rssi) + "," +
                            gps.getLat() + "," + gps.getLon() + "," +
                            String(gps.getAlt()) + "," + String(gps.getAccuracy()) + 
                            ",WIFI\n";
            
            saveToSD(csv_line);
        }
    }
}
```

### GPS Interface
**Código Base:** `GpsInterface.cpp`

```cpp
class GpsInterface {
public:
    void begin() {
        Serial2.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
        // Usar biblioteca MicroNMEA
    }
    
    void main() {
        while (Serial2.available()) {
            char c = Serial2.read();
            nmea.process(c);
        }
        
        if (nmea.isValid()) {
            setGPSInfo();
        }
    }
    
    String getLat() {
        return String((float)nmea.getLatitude()/1000000, 7);
    }
    
    String getLon() {
        return String((float)nmea.getLongitude()/1000000, 7);
    }
    
    float getAlt() {
        long alt = 0;
        nmea.getAltitude(alt);
        return (float)alt / 1000;
    }
    
    String getDatetime() {
        // Formato: 2024-01-01 12:00:00
        char datetime[20];
        sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d",
                nmea.getYear(), nmea.getMonth(), nmea.getDay(),
                nmea.getHour(), nmea.getMinute(), nmea.getSecond());
        return String(datetime);
    }
    
    bool getFixStatus() {
        return nmea.isValid() && nmea.getNumSatellites() > 0;
    }
    
    float getAccuracy() {
        // HDOP to meters
        return 2.5 * ((float)nmea.getHDOP()/10);
    }
};
```

---

## Estrutura de Menus

### M5Stack (Botões)
```
[GPS Menu]
Button A: Wardrive WiFi
Button B: Wardrive BLE
Button C: GPS Tracker

[Durante Wardrive]
Button A: Mark POI
Button B: Stop
Button C: View Stats
```

### Cardputer (Teclado)
```
[GPS Menu]
↑/↓  : Select mode
ENTER: Start
ESC  : Back

[Durante Wardrive]
SPACE: Mark POI
ESC  : Stop
```

---

## Display de Informações

### Tela Durante Wardriving
```
┌─────────────────────────┐
│ WARDRIVING WiFi         │
├─────────────────────────┤
│ Sats: 8  Fix: 3D        │
│ Lat: 40.712800          │
│ Lon: -74.006000         │
│ Alt: 10.5m  Acc: 5.0m   │
├─────────────────────────┤
│ Networks: 42            │
│ Saved: 38               │
│ Time: 00:15:32          │
├─────────────────────────┤
│ Last: MyNetwork         │
│ -65 dBm | Ch 6          │
└─────────────────────────┘
```

---

## Formato de Arquivos

### Wigle CSV
**Nome:** `wardrive_YYYYMMDD_HHMMSS.csv`
```
/sd/wardrives/wardrive_20240101_120000.csv
```

### GPX Track
**Nome:** `tracker_YYYYMMDD_HHMMSS.gpx`
```
/sd/gps/tracker_20240101_120000.gpx
```

### POI File
**Nome:** `poi_YYYYMMDD_HHMMSS.gpx`
```
/sd/gps/poi_20240101_120000.gpx
```

---

## Dependências

### Bibliotecas
```ini
lib_deps = 
    stevemarple/MicroNMEA@^2.0.6
    bblanchon/ArduinoJson@^6.21.3
```

### Configuração platformio.ini
```ini
[env:m5stack-gps]
platform = espressif32
board = m5stack-core-esp32
framework = arduino

build_flags = 
    -DHAS_GPS
    -DGPS_TX=17
    -DGPS_RX=16
    -DGPS_SERIAL_INDEX=2
```

---

## Pós-Processamento

### Upload para WigleWifi
1. Acessar https://wigle.net
2. Upload CSV file
3. Visualizar no mapa

### Visualizar GPX
**Software recomendado:**
- Google Earth
- GPX Viewer
- QGIS (profissional)

### Converter para KML
```bash
gpsbabel -i gpx -f track.gpx -o kml -F track.kml
```

---

## Testes

### Teste 1: GPS Fix
```
1. Conectar GPS module
2. Iniciar GPS Data
3. Aguardar fix (LED pisca quando tem fix)
4. Verificar coordenadas no display
```

### Teste 2: Wardrive Estático
```
1. Obter GPS fix
2. Iniciar wardrive
3. Aguardar detecção de 5+ networks
4. Verificar arquivo CSV no SD
```

### Teste 3: Wardrive em Movimento
```
1. Wardrive em carro/bicicleta por 5 minutos
2. Marcar 2-3 POIs
3. Verificar:
   - CSV com coordenadas diferentes
   - GPX com trajeto
   - POIs no arquivo GPX
```

---

## Melhorias Futuras

### 1. Live Mapping
- Display de mapa em tempo real (bitmap)
- Indicação de direção de movimento

### 2. Statistics
- Networks/minute
- Coverage area (km²)
- Unique SSIDs vs Total

### 3. Filtering
- Salvar apenas WPA2/WPA3
- Ignorar redes conhecidas
- Distance threshold (não salvar se <10m do último)

### 4. Auto-Upload
- WiFi upload para servidor
- FTP/HTTP POST
- WigleWifi API integration

---

## Referências
- ESP32Marauder GPS: https://github.com/justcallmekoko/ESP32Marauder
- WigleWifi CSV Format: https://api.wigle.net/
- GPX Format: https://www.topografix.com/gpx.asp
- MicroNMEA: https://github.com/stevemarple/MicroNMEA
