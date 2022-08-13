// **** Project code definition here ...
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <sstream>
#include <buttons.h>

// Variables used for multi-task
TaskHandle_t task_BLEScan;
int scanTime = 5;                               // In seconds
unsigned long Last_Scan_task = 0;               // Last time BLE Scan task was triggered
unsigned long TimeOUT = 25000;                           // [ms] time to consider not having the BT beacon present 
std::stringstream ss;
BLEScan* pBLEScan;

//char mac_template[] = "xx:xx:xx:xx:xx:xx,"
//char mac_list[] ="f8:06:4d:f7:2e:30,f8:06:4d:f7:2e:31,";

struct car_strut
 {
     String car_id;
     String BT;
     bool na_garagem;
     bool last_na_garagem;
     unsigned long last_time_na_gar;
     uint8_t batt;
     int bt_rssi;
 };
 
#define carro_max 2
car_strut carro[] = {
                        {"Golf",   "f8:06:4d:f7:2e:30", false, true, 0, 0, 0},
                        {"Megane", "c3:54:9d:96:3a:8b", false, true, 0, 0, 0}
                      };

static const String mqtt_pathCars[] = { "001001/" + String(ChipID) + "/" + carro[0].car_id + "/inform/",
                                        "001001/" + String(ChipID) + "/" + carro[1].car_id + "/inform/",
                                      };

void printarray(String data_array) {
uint8_t data;
int data_val;
  //for (int i = 0; i < 4; i++) { 
  for (int i = 0; i < sizeof(data_array); i++) { 
    data = (uint8_t)data_array[i];
    data_val = (int)data;
    Serial.printf("%X ", data_val);
  }
}

void display_info(BLEAdvertisedDevice advertisedDevice) {
      Serial.print("Addr: "); Serial.print(advertisedDevice.getAddress().toString().c_str());
      Serial.print("  Name: "); Serial.print(advertisedDevice.getName().c_str());
      if (advertisedDevice.haveManufacturerData()) { 
          Serial.print("  ManufacturerData: "); 
          char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t*)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
          Serial.print(pHex);
          free(pHex);
      }
      if (advertisedDevice.haveServiceUUID()) { Serial.print("  ServiceUUID: "); printarray(advertisedDevice.getServiceUUID().toString().c_str());}
      Serial.print("  ServiceDataUUID: "); printarray(advertisedDevice.getServiceDataUUID().toString().c_str());
      Serial.print("  ServiceData: "); printarray(advertisedDevice.getServiceData().c_str());

      if (advertisedDevice.haveRSSI()) {
          char valrssi[4];
		      snprintf(valrssi, sizeof(valrssi), "%d", advertisedDevice.getRSSI());
          Serial.print("  RSSI: "); Serial.print(valrssi);
      }

      if (advertisedDevice.haveTXPower()) {
          char valpwr[4];
		      snprintf(valpwr, sizeof(valpwr), "%d", advertisedDevice.getTXPower());
          Serial.print("  TXPower: "); Serial.print(valpwr);
      }
      Serial.println("");
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      for (size_t i = 0; i < carro_max; i++)
      {
          if (String(advertisedDevice.getAddress().toString().c_str()) == carro[i].BT) { 
              carro[i].na_garagem = true;
              carro[i].last_time_na_gar = millis();
              char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t*)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
              carro[i].batt = uint8_t((pHex[26] - 48) * 10 + (pHex[27] - 48));  // 48 => char "0"
              free(pHex);

              char valrssi[5];
		          snprintf(valrssi, sizeof(valrssi), "%d", advertisedDevice.getRSSI());
              carro[i].bt_rssi = (int)atoi(valrssi);

              telnet_println( String(carro[i].car_id) + " found!  -   RSSI: " + String(carro[i].bt_rssi) + "  -  Batt: " + String(carro[i].batt) + "%" );
              //Serial.println(carro[i].BT + " " + carro[i].car_id + " " + String(carro[i].batt) + " " + String(carro[i].bt_rssi) + " " + String(carro[i].na_garagem) + " " + String(carro[i].last_time_na_gar));

          }
      }
      //display_info(advertisedDevice);
      //Serial.println(advertisedDevice.toString().c_str());
    }
};


void fn_task_BLEScan(void *parameters){
    uint8_t buf_idx = 0;
    //telnet_println("fn_task_BLEScan running on core " + String(xPortGetCoreID()) + "  Last_Scan_task: " + String(Last_Scan_task));
    BLEScanResults foundDevices = pBLEScan->start(scanTime);
      //int count = foundDevices.getCount();
      //for (int i = 0; i < count; i++) {
      //    BLEAdvertisedDevice d = foundDevices.getDevice(i);
      //    String gaddr = d.getAddress().toString().c_str();
      //    Serial.print("Address:"); Serial.print(gaddr); Serial.print("  Rssi:"); Serial.print(d.getRSSI());
      //    if (d.haveName())             Serial.print("  Name:"); Serial.print(d.getName().c_str());
          //if (d.haveAppearance())       Serial.print(" Appearance:" + d.getAppearance());
          //if (d.haveManufacturerData()) {
          //    std::string md = d.getManufacturerData();
          //    uint8_t *mdp = (uint8_t *)d.getManufacturerData().data();
          //    char *pHex = BLEUtils::buildHexData(nullptr, mdp, md.length());
          //    Serial.print("  ManufacturerData:" + pHex);
          //    free(pHex);
          //}
          //if (d.haveServiceUUID())      Serial.print("  ServiceUUID:" + d.getServiceUUID().toString());
          //if (d.haveTXPower())          Serial.print("  TxPower:" + (int)d.getTXPower().str());
      //    Serial.println("");
      //}
      //Serial.println(ss.str().c_str());

    telnet_println("Scan done --");
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
    vTaskDelete(NULL);
}


// **** Project code functions here ...
void project_hw() {
 // Output GPIOs
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);  // initialize LCD off


 // Input GPIOs


}

void button_loop() {
    if (A_COUNT == 1 && (millis() - Last_A > Butt_Interval)) {
        mqtt_publish(mqtt_pathtele, "Button_1", String(A_COUNT));
        //telnet_println("Button A pressed ", true);
        flash_LED(A_COUNT);
        A_COUNT = 0;
    }
    if (A_COUNT ==2 && (millis() - Last_A > Butt_Interval)) {
        mqtt_publish(mqtt_pathtele, "Button_1", String(A_COUNT));
        //telnet_println("Button A pressed " + String(A_COUNT) + "times!", true);
        flash_LED(A_COUNT);
        A_COUNT = 0;
    }
    if (A_COUNT == 5 && (millis() - Last_A > Butt_Interval)) {
        mqtt_publish(mqtt_pathtele, "Button_1", String(A_COUNT));
        //telnet_println("Button A pressed " + String(A_COUNT) + "times!", true);
        flash_LED(A_COUNT);
        A_COUNT = 0;
        //ESPRestart();
    }
}

void publish_carro_presence() {
    for (size_t i = 0; i < carro_max; i++) {
        mqtt_publish(mqtt_pathCars[i], "Presence", String(carro[i].na_garagem));
        if (carro[i].na_garagem == true) {
            mqtt_publish(mqtt_pathCars[i], "Battery", String(carro[i].batt));
            mqtt_publish(mqtt_pathCars[i], "RSSI", String(carro[i].bt_rssi));
        }
    }
}

void project_setup() {
  // Start Ambient devices
      buttons_setup();
      BLEDevice::init("");
      pBLEScan = BLEDevice::getScan(); //create new scan
      pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
      pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
      pBLEScan->setInterval(0x50);
      pBLEScan->setWindow(0x30);  // less or equal setInterval value
}

void project_loop() {
  // Ambient handing
    button_loop();
    if (Last_Scan_task > millis()) Last_Scan_task = 0;
    if( (millis() - (Last_Scan_task))  >  ulong(scanTime * 1100) ) {       // Convert sec to millis and add 10%
        for (size_t i = 0; i < carro_max; i++) {
            //Serial.println(carro[i].BT + " " + carro[i].car_id + " " + String(carro[i].batt) + " " + String(carro[i].bt_rssi) + " " + String(carro[i].na_garagem) + " " + String(carro[i].last_time_na_gar));
            if ((millis() - carro[i].last_time_na_gar) > TimeOUT) carro[i].na_garagem = false;
            if(carro[i].last_na_garagem != carro[i].na_garagem) {
                mqtt_publish(mqtt_pathCars[i], "Presence", String(carro[i].na_garagem));
                if (carro[i].na_garagem == true) {
                    mqtt_publish(mqtt_pathCars[i], "Battery", String(carro[i].batt));
                    mqtt_publish(mqtt_pathCars[i], "RSSI", String(carro[i].bt_rssi));
                };
                carro[i].last_na_garagem = carro[i].na_garagem;
            }
        };
        Last_Scan_task = millis();
        xTaskCreate(fn_task_BLEScan, "task_BLEScan", 8192, (void *)NULL, 0, &task_BLEScan);
    }      
}
