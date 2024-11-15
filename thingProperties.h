#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char DEVICE_LOGIN_NAME[]  = "";

const char SSID[]               = SECRET_SSID;    // Network SSID (name)
const char PASS[]               = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)
const char DEVICE_KEY[]  = SECRET_DEVICE_KEY;    // Secret device password

void onLed1Change();
void onLed2Change();
void onLed3Change();
void onTemperatureChange();

CloudLight led1;
CloudLight led2;
CloudLight led3;
CloudTemperatureSensor temperature;
  
void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(led1, READWRITE, ON_CHANGE, onLed1Change);
  ArduinoCloud.addProperty(led2, READWRITE, ON_CHANGE, onLed2Change);
  ArduinoCloud.addProperty(led3, READWRITE, ON_CHANGE, onLed3Change);
  ArduinoCloud.addProperty(temperature, READWRITE, ON_CHANGE, onTemperatureChange);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);