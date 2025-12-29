#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Arduino.h>
//#include <math.h>

#define min(i, j) (((i) < (j)) ? (i) : (j))
#define abs(i)  (((i) >= 0) ? (i) : -(i))

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

std::string rxValue;
bool deviceConnected = false;
uint8_t scale = 0;
uint16_t angle = 0;
bool newData = false;

float tmp;
uint8_t M_out[8]={0}; 

//
#define ledPin 3 
const uint8_t M_M[8] ={D1,D10,D7,D0,D2,D5,D3,D4}; 
//

#define SERVICE_UUID        "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID "6e400012-b5a3-f393-e0a9-e50e24dcca9e"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    pServer->startAdvertising();
  }
};
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rx = pCharacteristic->getValue();
    if (rx.length() <= 3) {
      scale = rx[0];
      angle = (uint16_t(rx[1]) << 8) | uint8_t(rx[2]);
      newData = true;
    }
    //Serial.print(scale,angle);
    //Serial.println("");
  }
};
void setup() {
  //Serial.begin(9600);
  for(uint8_t i=0;i<8;i++){
    ledcAttachPin(M_M[i],i);
    //          235
    ledcSetup(i,10000,8);
  }
  //  ledcAttach(i, 10000, 8);
  BLEDevice::init("koko");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE_NR 
  );
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  // pCharacteristic->addDescriptor(new BLE2902());
  // descriptor_2901 = new BLE2901();
  // descriptor_2901->setDescription("My own description for this characteristic.");
  // descriptor_2901->setAccessPermissions(ESP_GATT_PERM_READ);
  // pCharacteristic->addDescriptor(descriptor_2901);

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  // pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection...");
   for(uint8_t i=0;i<8;i++){
      ledcWrite(i,0);
  }
}

void loop() {
  if (!deviceConnected) return;
    //angle /=45;
  for(uint8_t i=0;i<8;i++){
      tmp =abs(i-(angle/45.0));
      tmp =min(tmp,8-tmp);
      M_out[i] =exp(-tmp*tmp)*scale;
    }
    for(uint8_t i=0;i<8;i++){
      ledcWrite(i,M_out[i]);
    }
    // if (scale != 0) {
    //   Serial.print("received: ");
    //   for(int i=0;i<8;i++){
    //     Serial.print(M_out[i]);
    //     Serial.print(" ");
    //   }
    //   Serial.println("");
    //   // Serial.print(scale);
    //   // Serial.print(",");
    //   // Serial.println(angle);
    //   delay(100);
    // // }
    // }
  // }
  // else{
  //   Serial.println("a");
    //delay(50);
  
}
