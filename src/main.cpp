#include <RF24.h>
#include <SPI.h>
#include "printf.h"

/*
	Uncomment the following line if analog audio output is necessary. 
	Note that serial output will not work in that case and only audio
	reception would be possible.  

	if commented, both button press and audio signal will be written
	to serial.
*/

// #define AnalogAudioMode

/*
	Uncomment the following line if Radio Meta Data is necessary.
	Note that, meta does not comply with the bit count during operation.
	This can cause asymetrical reading in the client program if not
	explicitly handled.
*/

#define NoMeta

RF24 radio(7, 8); // Set radio up using pins 7 (CE) 8 (CS)

// Necessary include for analog Audio out mode
#ifdef AnalogAudioMode
#include <RF24Audio.h>
RF24Audio rfAudio(radio, 1); // Set up the audio using the radio, and set to radio number 1

#else
	// These are the default multicast addresses used by the library. By simply using the
// RF24 radio library, we can interact with the RF24 Audio library directly
const uint64_t addresses[14] = {0xABCDABCD71LL, 0x544d52687CLL,
								0x544d526832LL, 0x544d52683CLL,
								0x544d526846LL, 0x544d526850LL,
								0x544d52685ALL, 0x544d526820LL,
								0x544d52686ELL, 0x544d52684BLL,
								0x544d526841LL, 0x544d526855LL,
								0x544d52685FLL, 0x544d526869LL};
// Note: This will only receive data broadcast to the entire radio group. Capturing private
// channel communication as well would require listening to more addresses on more pipes.

#endif

void setup()
{

	Serial.begin(115200); // Set up the serial port
	printf_begin();

	radio.begin(); // Initialize the radio

#ifdef AnalogAudioMode

#ifndef NoMeta
	radio.printDetails(); // Print the info
	Serial.println("Radio Starting in Analog Audio Mode");
#endif

	rfAudio.begin(); // Start up the radio and audio libraries

#else
	/* Set our radio options to the audio library defaults */
	radio.setChannel(1);			// Set RF channel to 1
	radio.setAutoAck(0);			// Disable ACKnowledgement packets to allow multicast reception
	radio.setCRCLength(RF24_CRC_8); // Only use 8bit CRC for audio
	//radio.setDataRate(RF24_1MBPS);      	// Library default is RF24_1MBPS for RF24 and RF24Audio
	//radio.openWritingPipe(addresses[0]);  // Set up reading and writing pipes.
	radio.openReadingPipe(1, addresses[1]); // All of the radios listen by default to the same multicast pipe
	radio.openReadingPipe(2, addresses[2]);
	radio.openReadingPipe(3, addresses[3]);

#ifndef NoMeta
	radio.printDetails();
	Serial.println("Radio starting in Remote Mode");
#endif
	radio.startListening(); // Need to start the radio listening

#endif
}

byte audioData[32];			// Set up a buffer for the received data
byte samplesToDisplay = 32; // Change this to 32 to send the entire payload over USB/Serial

void loop()
{
#ifndef AnalogAudioMode
	while (radio.available())
	{
		radio.read(&audioData, 32);

		for (int i = 0; i < samplesToDisplay; i++)
		{
			Serial.write(audioData[i]);
		}
		
		// default is 24kHZ sample rate which results in 750 payloads/second with 8bit audio
		// Decrease the audio sample rate via userConfig.h if the data stream is too fast, or increase the Serial baud rate
	}
#endif
}