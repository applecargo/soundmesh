/*
 * a gateway device to the mesh (part1)
 *
 *   - a I2C Slave with LCD screen & Actions controls.
 *
 *   - soundmesh project @ 2018. 04.
 */

// shared global (protocol) : begin
//command (i2c)
#define CMD_LENGTH 10
#define CMD_BUFF_LEN (CMD_LENGTH + 1)
char cmdstr[CMD_BUFF_LEN] = "NONE#SSS@P";
//info (mesh)
char memberCount = -1;       //before any update from the mesh
//flags (events)
bool cmdsent = false;
bool newcmd = false;
// shared global (protocol) : end

//libraries
#include <Wire.h>
#include <LiquidCrystal.h>

//lcd
LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

//i2c
const int i2c_addr = 8;
void receiveEvent(int numBytes) {
  //numBytes : how many bytes received(==available)
  memberCount = Wire.read();
}
void requestEvent() {
  Wire.write(cmdstr);
  cmdsent = true;
  strcpy(cmdstr, "NONE#SSS@P"); //reset cmd str.
}

//button
const int button_pin = 10;

void setup() {
  //lcd
  lcd.begin(16, 2);

  //i2c
  Wire.begin(i2c_addr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //button interrupt
  pinMode(button_pin, INPUT_PULLUP);

  //serial monitor
  Serial.begin(115200);
}

void periodic() {

  //read button
  static int button = HIGH;
  static int lastButton = HIGH;
  button = digitalRead(button_pin);
  if (button == LOW && button != lastButton) { // falling edge
    strcpy(cmdstr, "PLAY#010@B"); //a new cmd str.
    cmdsent = false;
  }
  lastButton = button;

  // lcd
  const char runner[] = {'-', '|'};
  static char linebuffer[16 + 1];
  // first line
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("!soundmesh! "); //12
  lcd.print(runner[millis()%2]); // indicator //13
  lcd.print(" "); // 14
  lcd.print(memberCount, DEC); // members up to 99 // 16
  // second line
  lcd.setCursor(0, 1);
  if (cmdsent == true) {
    lcd.print(linebuffer); //16
    lcd.setCursor(14, 1); //14
    lcd.print(":O"); // +2
  }
  else {
    lcd.print("CMD:"); // 4
    strcpy(linebuffer, "CMD:");
    lcd.print(cmdstr); // 10
    strcat(linebuffer, cmdstr);
    lcd.print(":X"); // 2
    strcat(linebuffer, ":X");
  }

  //TEST
  Serial.println("HI?");
}

void loop() {
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 100) { // every 100ms
    //
    periodic();
    //
    lastMillis = millis();
  }
}
