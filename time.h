/* TIME METHODS AND VARIABLES, time.h
 * Ryan McGrail
 * 1/26/16
 * 
 * Provides methods and variables to keep track of time in milliseconds.
 */

#ifndef TIME_H
#define TIME_H

#include <Time.h> 
#include "Ethernet.h"
#include "EthernetUdp.h"
#include "SPI.h"



/****************************  VARIABLES  ****************************/

long 
  timer,
  timerPrevious, 
  lastPresetTime;


// NTP Servers:
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov

const int timeZone = -5;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)

EthernetUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets
String timeString = "_________________";
/*****************************  PROTOTYPES  *****************************/

void setupTime();
void loopTime();
time_t getNtpTime();
time_t getNtpTimeOriginal();
void sendNTPpacket(IPAddress&);


/****************************  TIME FUNCTIONS  *************************/

void setupTime() {
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(Ethernet.localIP());
  Udp.begin(localPort);
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);

  loopTime();
}

void loopTime() {
  timerPrevious = timer;
  timer = millis();
  
  char buffer[50];
  sprintf(buffer, "%4i/%02i/%02i-%02i:%02i:%02i", year(), month(), day(), hour(), minute(), second());

  timeString = buffer;
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets


time_t getNtpTime() {
  time_t time;
  do
    time = getNtpTimeOriginal();
  while(time == 0);
  return time;
}

time_t getNtpTimeOriginal() {
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmitting NTP Request...");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Received NTP Response!");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response.");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

#endif
