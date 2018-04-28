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

  // //TEST
  // Serial.print("message! : ");
  // Serial.println(msg);
  // //
  // Serial.print("cmdstr! : ");
  // Serial.println(cmdstr);

  Wire.beginTransmission(i2c_addr);
  int nb = Wire.write(cmdstr);
  Wire.endTransmission();

  // // TEST
  // Serial.print("bytes sent : ");
  // Serial.println(nb);
}

void setup() {
  //i2c master
  Wire.begin();

  //mesh
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT, STA_AP, WIFI_AUTH_WPA2_PSK, MESH_CHANNEL);
  //callbacks
  mesh.onReceive(&receivedCallback);

  //serial
  Serial.begin(9600);
}

void loop() {
  mesh.update();
}
