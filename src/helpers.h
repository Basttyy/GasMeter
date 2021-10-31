#include <config.h>

void mqttCallback(char* topic, byte* message, unsigned int len) {
  DEBUG_PRINT("Message arrived on topic: ");
  DEBUG_PRINT(topic);
  DEBUG_PRINT(". Message: ");
  String messageTemp;
  
  for (unsigned int i = 0; i < len; i++) {
    DEBUG_PRINT((char)message[i]);
    messageTemp += (char)message[i];
  }
  DEBUG_PRINTLN();

  if (String(topic) == "esp/output1") {
    DEBUG_PRINT("Changing output to ");
    if(messageTemp == "true"){
      DEBUG_PRINTLN("true");
      //digitalWrite(OUTPUT_1, HIGH);
    }
    else if(messageTemp == "false"){
      DEBUG_PRINTLN("false");
      //digitalWrite(OUTPUT_1, LOW);
    }
  }
  else if (String(topic) == "esp/output2") {
    DEBUG_PRINT("Changing output to ");
    if(messageTemp == "true"){
      DEBUG_PRINTLN("true");
      //digitalWrite(OUTPUT_2, HIGH);
    }
    else if(messageTemp == "false"){
      DEBUG_PRINTLN("false");
      //digitalWrite(OUTPUT_2, LOW);
    }
  }
}
//Initiate Connection the the MQTT Server
boolean mqttConnect() {
  DEBUG_PRINT("Connecting to ");
  DEBUG_PRINT(broker);

  boolean status = mqtt.connect("GsmClientN", mqttUsername, mqttPassword);

  if (status == false) {
    DEBUG_PRINTLN(" fail");
    //ESP.restart();
    return false;
  }
  DEBUG_PRINTLN(" success");
  mqtt.subscribe(subBalance.c_str());
  mqtt.subscribe(subOutput.c_str());

  return mqtt.connected();
}

void loopMqtt() {
  //Handle Mqtt Hearbeat and processing
  if (!mqtt.loop()) {
    DEBUG_PRINTLN("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > RECONNECT) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
        mqtt.loop();
      }
    }
    delay(10);
    return;
  }
}

void initMqtt()
{
      // Set GSM module baud rate and UART pins
  #ifdef ARDUINO_ESP8266_WEMOS_D1MINIPRO
  SerialAT.begin(9600, SWSERIAL_8N1, D6, D5, false);
  #endif
  #ifdef ARDUINO_AVR_PRO
  SerialAT.begin(9600);
  #endif
  delay(2000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DEBUG_PRINTLN("Initializing modem...");
  while (!modem.restart());;
  //modem.addCertificate("B8F409971E276E2593A8EAB1BA45B003B88ECA4F87C8205ED1FD97554C6F9D8F");   //TODO: Add certificate for SSL
  // modem.init();

  String modemInfo = modem.getModemInfo();
  DEBUG_PRINT("Modem Info: ");
  DEBUG_PRINTLN(modemInfo);

  // Unlock your SIM card with a PIN if needed
  #ifdef GSM_PIN
    if (modem.getSimStatus() != 3 ) {
      modem.simUnlock(GSM_PIN);
    }
  #endif

  DEBUG_PRINT("Connecting to APN: ");
  DEBUG_PRINT(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    DEBUG_PRINTLN(" fail");
    //ESP.restart();
  }
  else {
    DEBUG_PRINTLN(" OK");
  }
  
  if (modem.isGprsConnected()) {
    DEBUG_PRINTLN("GPRS connected");
  }

  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}

//Control all Peripherals accoringly
void handleInpOut() {
  // Keep power when running from battery
  //bool isOk = setPowerBoostKeepOn(1);
  //DEBUG_PRINTLN(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  // Set modem reset, enable, power pins
  // pinMode(MODEM_PWKEY, OUTPUT);
  // pinMode(MODEM_RST, OUTPUT);
  // pinMode(MODEM_POWER_ON, OUTPUT);
  // digitalWrite(MODEM_PWKEY, LOW);
  // digitalWrite(MODEM_RST, HIGH);
  // digitalWrite(MODEM_POWER_ON, HIGH);
  
  // pinMode(OUTPUT_1, OUTPUT);
  // pinMode(OUTPUT_2, OUTPUT);
}

//Update LCD Information
void updateDisplay() {

}