/* ——————————————
ATEM On/Off Recording Button
——————————————— */
#define PMD4_OUTPUT() (DDRD |= _BV(4))    // pinMode(4,OUTPUT)
#define DWD4_HIGH()   (PORTD |= _BV(4))   // digitalWrite(4,HIGH)
#define DWD4_LOW()    (PORTD &= ~_BV(4))  // digitalWrite(4,LOW)
#define PMD5_OUTPUT() (DDRD |= _BV(5))    // pinMode(5,OUTPUT)
#define DWD5_HIGH()   (PORTD |= _BV(5))   // digitalWrite(5,HIGH)
#define DWD5_LOW()    (PORTD &= ~_BV(5))  // digitalWrite(5,LOW)

// LED and Button Pin Mode
#define BtnPin    6

// Include UIPEthernet library
#include <UIPEthernet.h>
// #include <Ethernet.h>
// #include <SIP.h>

/*
 ttp://www.instructables.com/id/Add-Ethernet-to-any-Arduino-project-for-less-than-/
 Wire up as following (ENC = module side):
- ENC SO -> Arduino pin 12
- ENC SI -> Arduino pin 11
- ENC SCK -> Arduino pin 13
- ENC CS -> Arduino pin 8
- ENC VCC -> Arduino 3V3 pin
- ENC GND -> Arduino Gnd pin
*/

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0x09
};  // <= This Arduino MAC Address
IPAddress ip(192, 168, 1, 199);    // <= This Arduino IP Address

// Include ATEM library and make an instance:
#include <ATEM.h>

// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
ATEM AtemSwitcher(IPAddress(192, 168, 1, 10), 56417);  // IP address of the ATEM switcher

void setup() {
  pinMode(BtnPin,   INPUT_PULLUP);   // Button for off recordings
  PMD4_OUTPUT();
  PMD5_OUTPUT();

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac, ip);
  Serial.begin(9600);

  // Initialize a connection to the switcher:
  AtemSwitcher.serialOutput(true);
  AtemSwitcher.connect();
}
// For pushButtons:
int pushButton = 2;
void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();
  if (AtemSwitcher.isConnectionTimedOut())  {
    Serial.println(F("Connecting to ATEM:timedout - reconnect"));
    AtemSwitcher.connect();
  }

  // for debug
  /*
  Serial.print(F("  digitalRead(BtnPin):"));
  Serial.println(digitalRead(BtnPin));
  Serial.print(F("  pushButton:"));
  Serial.println(pushButton);
  Serial.println(AtemSwitcher.getDownstreamKeyerStatus(2));
  */

  if (AtemSwitcher.getDownstreamKeyerStatus(2) == true || (digitalRead(BtnPin) == LOW) ) { // System OFF Recording
    DWD4_LOW(); // digitalWrite(4,LOW)
    DWD5_HIGH(); // digitalWrite(5,HIGH)
  } else {
    DWD4_HIGH(); // digitalWrite(4,HIGH)
    DWD5_LOW(); // digitalWrite(5,LOW)
  }

  // */
  if (digitalRead(BtnPin) == LOW)  {
    if (pushButton != 2)  {
      pushButton = 2;
      OffRecording();
    }
  } else if (pushButton == 2) {
    pushButton = 0;
    OnRecording();
  }
  AtemSwitcher.delay(800);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             void OffRecording() {
  Serial.println(F("OffRecording"));
  AtemSwitcher.changeAudioChannelMode(2, 0); // Camera1 Audio Mix On
  AtemSwitcher.changeAudioChannelMode(3, 0); // Camera2 Audio Mix Off
  AtemSwitcher.changeDownstreamKeyOn(2, true); // Downstream Keyer 2 OFF
}
void OnRecording() {
  Serial.println(F("OnRecording"));
  AtemSwitcher.changeAudioChannelMode(2, 1); // Camera1 Audio Mix On
  AtemSwitcher.changeAudioChannelMode(3, 1); // Camera2 Audio Mix On
  AtemSwitcher.changeDownstreamKeyOn(2, false); // Downstream Keyer 2 ON
}

