/*
 * freenode devices to the mesh (part1)
 *
 *   - a I2C Master that logs in to the mesh.
 *
 *   - soundmesh project @ 2018. 04.
 */

// shared global (protocol) : begin
//command (i2c)
#define CMD_LENGTH 10
#define CMD_BUFF_LEN (CMD_LENGTH + 1)
char cmdstr[CMD_BUFF_LEN] = "NONE#SSS@P";
//info (mesh)
char memberCount = -1;      //before any update from the mesh
//flags (events)
bool cmdsent = false;
bool newcmd = false;
// shared global (protocol) : end

//libraries
#include <Wire.h>
#include <painlessMesh.h>

//defines
#define MESH_SSID "soundmesh"
#define MESH_PASSWORD "timemachine999"
#define MESH_PORT 5555
#define MESH_CHANNEL 5

//i2c
const int i2c_addr = 3;

//mesh
painlessMesh mesh;
void receivedCallback(uint32_t from, String & msg) { // needed to be exist
  //i2c
  msg.toCharArray(cmdstr, CMD_BUFF_LEN);

  Wire.beginTransmission(i2c_addr);
  int nb = Wire.write(cmdstr);
  Wire.endTransmission();
}

//tasks
Scheduler runner;
//
// LED status indication
//
// operation modes
//
// 0 - booted. and running. no connection. scanning.
// 1 - + connected.
// 2 - + got a message.
//
// notifying patterns
//
// 0 - steady on
// 1 - slow blinking (syncronized)
// 2 - fast blinking for N times
//
#define LED_PIN 2 // built-in LED
#define LED_PERIOD 1000
#define LED_ONTIME 100
bool onFlag = false;
Task statusblinks(0, 1, &taskStatusBlink_steadyOn); // at start, steady on. default == disabled. ==> setup() will enable.
// when disconnected, steadyon.
void taskStatusBlink_steadyOn() {
  onFlag = true;
}
// blink per 1s. sync-ed.
void taskStatusBlink_slowblink_insync() {
  // toggler
  if (onFlag) {
    onFlag = false;
  }
  else {
    onFlag = true;
  }
  // on-time
  statusblinks.delay(LED_ONTIME);
  // re-enable & sync.
  if (statusblinks.isLastIteration()) {
    statusblinks.setIterations(2); //refill iteration counts
    statusblinks.enableDelayed(LED_PERIOD - (mesh.getNodeTime() % (LED_PERIOD*1000))/1000); //re-enable with sync-ed delay
  }
}
void taskStatusBlink_fastblink() {
}

void changedConnectionCallback() {
  // Serial.println("changed connection");
  // Serial.print("mesh.getNodeList().size():");
  // Serial.println(mesh.getNodeList().size());

  // check status -> modify status LED
  if (mesh.getNodeList().size() > 0) {
    // (still) connected.
    onFlag = false; //reset flag stat.
    statusblinks.set(LED_PERIOD, 2, &taskStatusBlink_slowblink_insync);
    statusblinks.enable();
    // Serial.println("connected!");
  }
  else {
    // disconnected!!
    statusblinks.set(0, 1, &taskStatusBlink_steadyOn);
    statusblinks.enable();
    // Serial.println("disconnected!");
  }
}

void setup() {
  //i2c master
  Wire.begin();

  //led
  pinMode(LED_PIN, OUTPUT);

  //mesh
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);
  mesh.init(MESH_SSID, MESH_PASSWORD, &runner, MESH_PORT, WIFI_AP_STA, MESH_CHANNEL);
  mesh.setContainsRoot(true);
  //callbacks
  mesh.onReceive(&receivedCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  //tasks
  runner.addTask(statusblinks);
  statusblinks.enable();

  //serial
  Serial.begin(9600);
}

void loop() {
  runner.execute();
  mesh.update();
  digitalWrite(LED_PIN, !onFlag); // value == false is ON. so onFlag == true is ON. (pull-up)
}
