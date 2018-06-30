/*
 * drumnode devices to the mesh (part2)
 *
 *   - a I2C Slave and a sounder device.
 *
 *   - soundmesh project @ 2018. 04.
 */

//problems:
//  enhancement on i2c protocol: start byte, CRC etc.

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

//set my group
String mygroup = "D"; //F : flag objects, C : circular objects, D : drumming objects, A : all objects

//servo
#include <Servo.h>
Servo left_drum;
Servo right_drum;

//task
#include <TaskScheduler.h>
Scheduler runner;
#define HIT_ANGLE 130
#define REST_ANGLE 40
#define HIT_DELAY 100
#define LR_DELAY 100
//callback prototypes
void left_drum_hit();
void left_drum_rest();
void right_drum_hit();
void right_drum_rest();
//tasks
Task left_drum_task(0, TASK_ONCE, left_drum_rest);
Task right_drum_task(0, TASK_ONCE, right_drum_rest);
//callbacks
void left_drum_hit() {
  left_drum.write(HIT_ANGLE);
  left_drum_task.setCallback(left_drum_rest);
  left_drum_task.restartDelayed(HIT_DELAY);
}
void left_drum_rest() {
  left_drum.write(REST_ANGLE);
}
void right_drum_hit() {
  right_drum.write(HIT_ANGLE);
  right_drum_task.setCallback(right_drum_rest);
  right_drum_task.restartDelayed(HIT_DELAY);
}
void right_drum_rest() {
  right_drum.write(REST_ANGLE);
}

//i2c
#include <Wire.h>
const int i2c_addr = 3;
void receiveEvent(int numBytes) {
  //numBytes : how many bytes received(==available)

  // Serial.println("[i2c] on receive!");
  int nb = Wire.readBytes(cmdstr, CMD_LENGTH);
  Serial.print("[i2c] cmdstr : ");
  Serial.println(cmdstr);

  if (CMD_LENGTH == nb) { // receive cmdstr.

    //convert to String
    String msg = String(cmdstr);

    //parse command string.
    String cmd = msg.substring(0,4); // 0123
    String song = msg.substring(5,8); // 567
    String group = msg.substring(9,10); // 9

    //process commands
    if (group == mygroup || group == "A") {

      if (cmd.equals("NONE")) {}
      else if (cmd.equals("PLAY")) {
        //play left drum once.
        left_drum_task.setCallback(left_drum_hit);
        left_drum_task.restart();
      }
      else if (cmd.equals("STOP")) {
        //play left drum once.
        left_drum_task.setCallback(left_drum_hit);
        left_drum_task.restart();
        //play right drum once.
        right_drum_task.setCallback(right_drum_hit);
        right_drum_task.restartDelayed(LR_DELAY);
      }
    }
  }
}

void setup() {
  //serial monitor
  Serial.begin(115200);
  delay(50);

  //i2c
  //NOTE: But, don't use pull-up registers for esp boards. -> esp i2c function will drive bus pull-up on the fly!
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH); // for i2c pullup
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH); // for i2c pullup
  Wire.begin(i2c_addr);
  Wire.onReceive(receiveEvent);

  //led
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // LOW: OFF

  //servo - drums
  left_drum.attach(9);
  right_drum.attach(10);

  //player task
  runner.addTask(left_drum_task);
  runner.addTask(right_drum_task);
  left_drum_task.enable();
  right_drum_task.enable();

  //
  Serial.println("[setup] done.");
}

void loop() {
  runner.execute();
}
