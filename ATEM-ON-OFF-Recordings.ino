/* ——————————————
  ATEM On/Off Recording Button
  ——————————————— */
// Off recording LED
#define PMD4_OUTPUT() (DDRD |= _BV(4))    // pinMode(4,OUTPUT)
#define DWD4_HIGH()   (PORTD |= _BV(4))   // digitalWrite(4,HIGH)
#define DWD4_LOW()    (PORTD &= ~_BV(4))  // digitalWrite(4,LOW)
// On Recording LED
#define PMD5_OUTPUT() (DDRD |= _BV(5))    // pinMode(5,OUTPUT)
#define DWD5_HIGH()   (PORTD |= _BV(5))   // digitalWrite(5,HIGH)
#define DWD5_LOW()    (PORTD &= ~_BV(5))  // digitalWrite(5,LOW)

#include <Ethernet.h>
#include <SPI.h>

#include <ATEMbase.h>
#include <ATEMstd.h>
ATEMstd AtemSwitcher;

#include <MemoryFree.h>
#define DEBUG // Debug Setting

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
  Serial.println(F("\nSerial Start"));

  // DHCPにてArduinoのIPアドレス取得
  if (Ethernet.begin(mac) == 0) {
#ifdef DEBUG
    Serial.println(F("\nDHCP Failed"));
#endif
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, clientIp);
  }
  // print your local IP address:
#ifdef DEBUG
  printIPAddress();
#endif

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();

  //  attachInterrupt(0, offRecording, RISING  ); // GPIO 2nd pin
  //  attachInterrupt(0, onRecording , FALLING ); // GPIO 2nd pin
  attachInterrupt(0, offRecording, HIGH ); // GPIO 2nd pin
  attachInterrupt(0, onRecording , LOW  ); // GPIO 2nd pin
}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
  while (!AtemSwitcher.hasInitialized())  {
    DWD5_LOW(); // digitalWrite(5,LOW)
    DWD4_LOW(); // digitalWrite(4,LOW)
    AtemSwitcher.runLoop();
  }
  if (AtemSwitcher.getDownstreamKeyerStatus(2) == true )  { // System OFF Recording
    DWD4_LOW(); // digitalWrite(4,LOW)
    DWD5_HIGH(); // digitalWrite(5,HIGH)
  } else {
    DWD4_HIGH(); // digitalWrite(4,HIGH)
    DWD5_LOW(); // digitalWrite(5,LOW)
  }
#ifdef DEBUG
  Serial.print(F("\nfreeMem="));
  Serial.println(freeMemory());
  delay(1000);
#endif
}

void offRecording() {
#ifdef DEBUG
  Serial.println(F("\nOffRecord"));
#endif
  AtemSwitcher.changeAudioMasterVolume(33); // Master Volume to -60
  AtemSwitcher.changeDownstreamKeyOn(2, true); // Downstream Keyer 2 OFF
}
void onRecording() {
#ifdef DEBUG
  Serial.println(F("\nOnRecord"));
#endif
  AtemSwitcher.changeAudioMasterVolume(32789.13); // Master Volume to 0
  AtemSwitcher.changeDownstreamKeyOn(2, false); // Downstream Keyer 2 ON
}



void printIPAddress()
{
#ifdef DEBUG
  Serial.print(F("\nClient IP:"));
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(F("."));
  }
#endif
}
