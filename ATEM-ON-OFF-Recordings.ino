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
#define BtnPin    2

#include <Ethernet.h>
#include <SPI.h>

#include <ATEMbase.h>
#include <ATEMstd.h>
ATEMstd AtemSwitcher;

// Initialize the Ethernet client library
EthernetClient client;

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9
};      // <= SETUP!  MAC address of the Arduino
IPAddress clientIp(192, 168, 10, 99);        // <= SETUP!  IP address of the Arduino
IPAddress switcherIp(192, 168, 10, 240);     // <= SETUP!  IP address of the ATEM Switcher if DHCP if not working

void setup() {

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, clientIp);
  }
  // print your local IP address:
  printIPAddress();

  // put your setup code here, to run once:
  // Start the Ethernet, Serial (debugging) and UDP:
  //  Ethernet.begin(mac, clientIp);
  Serial.begin(115200);
  Serial.println(F("\n- - - - - - - -\nSerial Started"));

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();

  attachInterrupt(0, offRecording, RISING  ); // GPIO 2nd pin
  attachInterrupt(0, onRecording , FALLING ); // GPIO 2nd pin
}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
  while (!AtemSwitcher.hasInitialized())  {
    AtemSwitcher.runLoop();
  }
  AtemSwitcher.serialOutput(0);
  if (AtemSwitcher.getDownstreamKeyerStatus(2) == true )  { // System OFF Recording
    DWD4_LOW(); // digitalWrite(4,LOW)
    DWD5_HIGH(); // digitalWrite(5,HIGH)
  } else {
    DWD4_HIGH(); // digitalWrite(4,HIGH)
    DWD5_LOW(); // digitalWrite(5,LOW)
  }
}

void offRecording() {
  Serial.println(F("OffRecording"));
  AtemSwitcher.changeAudioMasterVolume(33); // Master Volume to -60
  AtemSwitcher.changeDownstreamKeyOn(2, true); // Downstream Keyer 2 OFF
}
void onRecording() {
  Serial.println(F("OnRecording"));
  AtemSwitcher.changeAudioMasterVolume(32789.13); // Master Volume to 0
  AtemSwitcher.changeDownstreamKeyOn(2, false); // Downstream Keyer 2 ON
}


void printIPAddress()
{
  Serial.println(F("My IP address: "));
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(F("."));
  }
}
