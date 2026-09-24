#include "BLEDevice.h"


// The remote service we wish to connect to.
static BLEUUID serviceUUID("0000ffe0-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("0000ffe1-0000-1000-8000-00805f9b34fb");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = true;
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;
static BLEClient* pClient;
static long int val = 0; // Declare the long int variable
int dig1,dig2,dig3,dig4,dig5,dig6,dig7,dig8,dig9,weapon,shoot=0;
static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
  // Convert the received data to a string
  String dataStr = String((char*)pData);
  
  // Convert the string to a long int
  val = dataStr.toInt();
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {}

  void onDisconnect(BLEClient *pclient) {
    connected = false;
    doScan = true; // Start scanning again when disconnected
  }
};

bool connectToServer() {
  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remote BLE Server
  pClient->connect(myDevice);

  // Obtain a reference to the service we are after in the remote BLE server
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    pClient->disconnect();
    return false;
  }

  // Obtain a reference to the characteristic in the service of the remote BLE server
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    pClient->disconnect();
    return false;
  }

  // Register for notifications
  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  }

  connected = true;
  doScan = false;
  return true;
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

void setup() 
{
  pinMode(2,OUTPUT);//motor1
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);//motor2
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);//weapon
  
  Serial.begin(115200);
  BLEDevice::init("");

  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  
}

void loop() {
  if (doConnect) {
    if (connectToServer()) {
      Serial.println("Connected to the BLE Server.");
    } else {
      Serial.println("Failed to connect to the server.");
    }
    doConnect = false;
  }

  if (connected) {
    // Print the value of "val" continuously
    Serial.print("val: ");
    Serial.println(val);

if(val>99999999)
{

dig9=val%10;val/=10;
dig8=val%10;val/=10;
dig7=val%10;val/=10;
dig6=val%10;val/=10;
dig5=val%10;val/=10;
dig4=val%10;val/=10;
dig3=val%10;val/=10;
dig2=val%10;val/=10;
dig1=val%10;
if(dig1!=weapon)
{
  shoot=(shoot+1)%2;
  digitalWrite(6,shoot);
}
if((dig9>6||dig9<4)||(dig8>6||dig8<4))
{
  shoot=(shoot+1)%2;
  digitalWrite(6,shoot);
}

if(dig4>6)
{
  digitalWrite(2,HIGH);
  digitalWrite(3,LOW);
}
else if(dig4<4)
{
  digitalWrite(3,HIGH);
  digitalWrite(2,LOW);
}
else
{
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
}


if(dig3>6)
{
  digitalWrite(4,HIGH);
  digitalWrite(5,LOW);
}
else if(dig3<4)
{
  digitalWrite(5,HIGH);
  digitalWrite(4,LOW);
}
else
{
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
}

weapon=dig1;
}

  } else if (doScan) {
    BLEDevice::getScan()->start(600);
  }

  // Check connection status and attempt reconnect if not connected
  if (!connected && !doConnect) {
    Serial.println("Attempting to reconnect...");
    BLEDevice::getScan()->start(600);
    doConnect = true;
  }
}
