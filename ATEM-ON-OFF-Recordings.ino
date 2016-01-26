/* ——————————————
  ATEM On/Off Recording Button
  ——————————————— */

#include <Ethernet.h>
#include <SPI.h>

#include <ATEMbase.h>
#include <ATEMstd.h>
ATEMstd AtemSwitcher;

#include <MemoryFree.h>

// DEBUG Switch.
#define DEBUG // Debug Setting
//#define MEM_DEBUG // Debug Setting

int onLED  = 5;
int offLED = 4;
int button = 2;

volatile bool SW_status = false;

// Initialize the Ethernet client library
EthernetClient client;

// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9
};      // <= SETUP!  MAC address of the Arduino
IPAddress clientIp(192, 168, 10, 99);        // <= SETUP!  IP address of the Arduino, if DHCP if not working
IPAddress switcherIp(192, 168, 10, 240);     // <= SETUP!  IP address of the ATEM Switcher

void setup() {
  // Start the Ethernet, Serial (debugging) and UDP:
  //  Ethernet.begin(mac, clientIp);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println(F("Serial Start"));

  // DHCPにてArduinoのIPアドレス取得
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("DHCP Failed"));
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, clientIp);
  }
  // print your local IP address:
#ifdef DEBUG
  printIPAddress();
#endif

  // Off recording LED
  pinMode(onLED, OUTPUT);
  // On Recording LED
  pinMode(offLED, OUTPUT);
  // Button
  pinMode(2, INPUT_PULLUP);

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();
}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
  while (!AtemSwitcher.hasInitialized())  {
    digitalWrite(onLED, LOW);
    digitalWrite(offLED, LOW);
    AtemSwitcher.runLoop();
  }

  if (digitalRead(button) == HIGH) {
    offRecording();
  } else {
    onRecording();
  }

#ifdef MEM_DEBUG
  Serial.print(F("freeMem="));
  Serial.println(freeMemory());
  delay(200);
#endif
}

void offRecording() {
#ifdef DEBUG
  Serial.println(F("OffRecord"));
#endif
  if (!SW_status) {
    AtemSwitcher.changeAudioMasterVolume(33); // Master Volume to -60
    AtemSwitcher.changeDownstreamKeyOn(2, true); // Downstream Keyer 2 OFF
    SW_status = true;
    checkLED();
  }
}

void onRecording() {
#ifdef DEBUG
  Serial.println(F("OnRecord"));
#endif
  if (SW_status) {
    AtemSwitcher.changeAudioMasterVolume(32789.13); // Master Volume to 0
    AtemSwitcher.changeDownstreamKeyOn(2, false); // Downstream Keyer 2 ON
    SW_status = false;
    checkLED();
  }
}

void printIPAddress()
{
#ifdef DEBUG
  Serial.print(F("Client IP:"));
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(F("."));
  }
#endif
}

void checkLED() {
  if (AtemSwitcher.getDownstreamKeyerStatus(2) == true )  { // System OFF Recording
    digitalWrite(onLED, LOW);
    digitalWrite(offLED, HIGH);
  } else {
    digitalWrite(onLED, HIGH);
    digitalWrite(offLED, LOW);
  }
}
