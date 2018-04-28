/*
 * freenode devices to the mesh (part2)
 *
 *   - a I2C Slave and a sounder device.
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

//teensy SD

//TO DO - try SDIO??

#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

//teensy audio
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlaySdWav playSdWav1;               //xy=376,303
AudioMixer4 mixer2;                      //xy=649,366
AudioMixer4 mixer1;                      //xy=651,283
AudioOutputAnalogStereo dacs1;           //xy=931,300
AudioConnection patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection patchCord2(playSdWav1, 1, mixer2, 0);
AudioConnection patchCord3(mixer2, 0, dacs1, 1);
AudioConnection patchCord4(mixer1, 0, dacs1, 0);
// GUItool: end automatically generated code

//teensy threads
#include <TeensyThreads.h>
volatile bool is_play_start_req = false;
volatile bool is_play_stop_req = false;
volatile int song_now = 0;
Threads::Mutex x_snd_player;
void sound_player_thread()
{
  //thread loop
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // LOW: OFF
  while(1) {
    //scoped mutex
    {
      Threads::Scope m(x_snd_player);

      if (is_play_start_req == true) {
        is_play_start_req = false;
        //TEST
        Serial.println("play acknowledged.");

        //
        char filename[13]; //8.3 naming convension! 8+1+3+1 = 13
        sprintf(filename, "%2d.WAV", song_now); // NN.WAV format!
        //TEST
        Serial.print("play start! : ");
        Serial.println(filename);
        //
        playSdWav1.play(filename);
        digitalWrite(13, HIGH); // HIGH: ON
        //
        threads.delay(10); // to update isPlaying()...
      }

      if (is_play_stop_req == true) {
        is_play_stop_req = false;
        //TEST
        Serial.println("stop acknowledged.");
        //
        if (playSdWav1.isPlaying() == true) {
          playSdWav1.stop();
        }
      }

      if (playSdWav1.isPlaying() == false) {
        digitalWrite(13, LOW); // LOW: OFF
        // Serial.println("not playing...");
      }

      //problems: delay / wireless network dropping / buzz & temporal stop
    }

    //
    threads.yield();
  }
}

//i2c
#include <Wire.h>
const int i2c_addr = 3;
void receiveEvent(int numBytes) {
  //numBytes : how many bytes received(==available)

  // Serial.println("on receive!");
  int nb = Wire.readBytes(cmdstr, CMD_LENGTH);
  // Serial.print("cmdstr : ");
  // Serial.println(cmdstr);
  // Serial.print("nb : ");
  // Serial.println(nb);

  if (CMD_LENGTH == nb) { // receive cmdstr.

    //convert to String
    String msg = String(cmdstr);

    //parse command string.
    String cmd = msg.substring(0,4); // 0123
    String song = msg.substring(5,8); // 567
    String channel = msg.substring(9,10); // 9

    //process commands
    if (cmd.equals("NONE")) {
      // //TEST
      // Serial.println("none recognized.");
      // Serial.println("song: " + song);
      // Serial.println("channel: " + channel);

      //do action
      //nothing to do.
    }
    else if (cmd.equals("PLAY")) {
      // //TEST
      // Serial.println("play recognized.");
      // Serial.println("song: " + song);
      // Serial.println("channel: " + channel);

      //do action
      Threads::Scope m(x_snd_player);
      song_now = song.toInt();
      if (song_now > 0) { // 0 is a parsing error
        is_play_start_req = true;
      }
      else {
        //error!
        Serial.println("parsing error of 'song' parameter!");
      }
    }
    else if (cmd.equals("STOP")) {
      // //TEST
      // Serial.println("stop recognized.");
      // Serial.println("song: " + song);
      // Serial.println("channel: " + channel);

      //do action
      Threads::Scope m(x_snd_player);
      is_play_stop_req = true;
      //TO DO : maybe fade-out needed.
    }
  }
}

// // SD TEST
// void printDirectory(File dir, int numTabs) {
//   while(true) {
//
//     File entry =  dir.openNextFile();
//     if (!entry) {
//       // no more files
//       //Serial.println("**nomorefiles**");
//       break;
//     }
//     for (uint8_t i=0; i<numTabs; i++) {
//       Serial.print('\t');
//     }
//     Serial.print(entry.name());
//     if (entry.isDirectory()) {
//       Serial.println("/");
//       printDirectory(entry, numTabs+1);
//     } else {
//       // files have sizes, directories do not
//       Serial.print("\t\t");
//       Serial.println(entry.size(), DEC);
//     }
//     entry.close();
//   }
// }

File root;
void setup() {
  //i2c
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH); // for i2c pullup
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH); // for i2c pullup
  Wire.begin(i2c_addr);
  Wire.onReceive(receiveEvent);
  //Wire.onRequest(requestEvent);

  delay(1000);

  // SD TEST
  // if (!SD.begin(BUILTIN_SDCARD)) {
  //   Serial.println("initialization failed!");
  //   return;
  // }
  // Serial.println("initialization done.");
  // root = SD.open("/");
  // printDirectory(root, 0);

  //serial monitor
  Serial.begin(9600);

  //audio
  AudioMemory(20);
  mixer1.gain(0,1);
  mixer1.gain(1,0);
  mixer1.gain(2,0);
  mixer1.gain(3,0);
  mixer2.gain(0,1);
  mixer2.gain(1,0);
  mixer2.gain(2,0);
  mixer2.gain(3,0);

  //player thread
  threads.addThread(sound_player_thread);

  //
  Serial.println("setup done.");
}

void loop() {
  delay(5);
}
