// HARWARE & SOFTWARE Version
#define BRANDName "AlBros_Team"                         // Hardware brand name
#define MODELName "CarroemCasa"                         // Hardware model name
#define SWVer "02.01"                                   // Major.Minor Software version (use String 01.00 - 99.99 format !)

// Power Source & Battery Level
bool BattPowered = false;                               // Is the device battery powered?
#define Batt_L_Thrs 15                                  // Battery level threshold [0%-100%] (before slepping forever).

// GPIO to Function Assignment
#define Using_ADC true                                  // will this device use the ADC? (if not it will measure the internal voltage)
#define LED_ESP 22                                      // ESP Led is connected to GPIO 2. -1 means NOT used!
#define DHTTYPE 2                                       // use 1 for "DHT11", 2 for "DHT22", or 3 for "AM2320" to select the DHT Model
#define DHTPIN -1                                       // GPIO connected to DHT Data PIN. -1 means NO DHT used!
#define SDAPIN -1                                       // GPIO connected to (AM) I2C SDA PIN. -1 means NO SDA used!
#define SCKPIN -1                                       // GPIO connected to (AM) I2C SCK PIN. -1 means NO SCK used!
#define BUZZER -1                                       // (Active) Buzzer pin. Suggest to use pin 0.  -1 means NOT used!


void config_defaults() {
    Serial.println("Setting config Default values");

    strcpy(config.DeviceName, "CarroemCasa");             // Device Name
    strcpy(config.Location, "Garagem");                  // Device Location
    strcpy(config.ClientID, "001001");                    // Client ID (used on MQTT)
    config.ONTime = 10;                                   // 0-255 seconds (Byte range)
    config.SLEEPTime = 0;                                 // 0-255 minutes (Byte range)
    config.DEEPSLEEP = false;                             // 0 - Disabled, 1 - Enabled
    config.LED = true;                                    // 0 - OFF, 1 - ON
    config.TELNET = false;                                // 0 - Disabled, 1 - Enabled
    config.OTA = true;                                    // 0 - Disabled, 1 - Enabled
    config.WEB = false;                                   // 0 - Disabled, 1 - Enabled
    config.Remote_Allow = true;                           // 0 - Not Allow, 1 - Allow remote operation
    config.STAMode = true;                                // 0 - AP or AP+STA Mode, 1 - Station only Mode
    config.APMode = false;                                // 0 - AP Mode Disabled, 1 - AP Mode Enabled
    strcpy(config.SSID, "WiFiCasaN");                     // Wireless LAN SSID (STA mode)
    strcpy(config.WiFiKey, "12345678");                   // Wireless LAN Key (STA mode)
    config.DHCP = true;                                   // 0 - Static IP, 1 - DHCP
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 10;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 1;
    config.DNS_IP[0] = 192; config.DNS_IP[1] = 168; config.DNS_IP[2] = 1; config.DNS_IP[3] = 93;
    strcpy(config.NTPServerName, "pt.pool.ntp.org");      // NTP Server
    config.Update_Time_Via_NTP_Every = 1200;              // Time in minutes to re-sync the clock
    config.TimeZone = 0;                                  // -12 to 13. See Page_NTPSettings.h why using -120 to 130 on the code.
    config.isDayLightSaving = 1;                          // 0 - Disabled, 1 - Enabled
    strcpy(config.MQTT_Server, "iothubna.hopto.org");     // MQTT Broker Server (URL or IP)
    config.MQTT_Port = 1883;                              // MQTT Broker TCP port
    config.MQTT_Secure = false;                           // 0 - Unsecure, 1 - TLS v1.2 Secured!!
    strcpy(config.MQTT_User, "admin");                    // MQTT Broker username
    strcpy(config.MQTT_Password, "admin");                // MQTT Broker password
    strcpy(config.UPDATE_Server, "iothubna.hopto.org");   // UPDATE Server (URL or IP)
    config.UPDATE_Port = 1880;                            // UPDATE Server TCP port
    strcpy(config.UPDATE_User, "user");                   // UPDATE Server username
    strcpy(config.UPDATE_Password, "1q2w3e4r");           // UPDATE Server password
    strcpy(config.WEB_User, "admin");                     // WEB Server username
    strcpy(config.WEB_Password, "admin");                 // WEB Server password
    config.SWITCH_Default = false;                        // 0 - OFF, 1 - ON - Default SWITCH Status 
    config.Temp_Corr = 0.0;                               // Sensor Temperature Correction Factor, typically due to electronic self heat.
    config.LDO_Corr = 0.6;                                // Battery Voltage [volt] corrective Factor due to LDO/Diode voltage drop
    config.HW_Module = true;                              // Is HW module plugged (ex.: GPS hardware)used / connected?
    config.HASSIO_CFG = false;                            // Is HASSIO configured? If not, it should perform the discovery.
    config.SW_Upgraded = false;                           // Is SW Upgrade completed? If not, clean the house and Update status.
    config.Voltage_Multiplier = 326341.66;                // Unit: us/V
    config.Current_Multiplier = 21182.61;                 // Unit: us/A
    config.Power_Multiplier = 792008.54;                  // Unit: us/W
}


