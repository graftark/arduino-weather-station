/*
 Copyright (C) 2014 C. Cross 

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 Original Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 03/17/2013 : Charles-Henri Hallard (http://hallard.me)
              Modified to use with Arduipi board http://hallard.me/arduipi
                 Changed to use modified bcm2835 and RF24 library

 */

/**
 * For use on Raspberry Pi as a receiver for data from 
 * the Arduino powered weather station.
 * 
 */

#include <cstdlib>
#include <iostream>
#include "./RF24.h"

using namespace std;

// Radio pipe addresses for the 2 nodes to communicate.
// First pipe is for writing, 2nd, 3rd, 4th, 5th & 6th is for reading...
/*
const uint64_t pipes[6] = 
					{ 0xF0F0F0F0D2LL, 0xF0F0F0F0E1LL, 
						0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 
						0xF0F0F0F0F1, 0xF0F0F0F0F2 
					};
*/
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0x7365727631LL };
const uint64_t pipes[2] = { 0xf0f0f0f0d2LL, 0xf0f0f0f0e1LL };
//const uint64_t pipes[2] = { 0xf0f0f0f0e1LL, 0xf0f0f0f0d2LL };

// CE Pin, CSN Pin, SPI Speed

// Setup for GPIO 22 CE and GPIO 25 CSN with SPI Speed @ 1Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_18, BCM2835_SPI_SPEED_1MHZ);

// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 4Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ); 

// Setup for GPIO 22 CE and CE1 CSN with SPI Speed @ 8Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ); 
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_26, BCM2835_SPI_SPEED_8MHZ);  
RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

int main(int argc, char** argv) 
{
	uint8_t len;

	// Refer to RF24.h or nRF24L01 DS for settings
	radio.begin();
	radio.enableDynamicPayloads();
	radio.setAutoAck(0);
	radio.setRetries(15,15);
	radio.setDataRate(RF24_1MBPS);
//	radio.setPALevel(RF24_PA_LOW);
	radio.setPALevel(RF24_PA_MAX);
	radio.setChannel(0x4c);
//	radio.setChannel(76);
//	radio.setCRCLength(RF24_CRC_16);

	// Open 6 pipes for readings ( 5 plus pipe0, also can be used for reading )
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1,pipes[1]);
//	radio.openReadingPipe(2,pipes[2]);
//	radio.openReadingPipe(3,pipes[3]);
//	radio.openReadingPipe(4,pipes[4]);
//	radio.openReadingPipe(5,pipes[5]);

	//
	// Start listening
	//
	radio.startListening();

	//
	// Dump the configuration of the rf unit for debugging
	//
	radio.printDetails();
	
	printf("Output below : \n");
	delay(1);
	
	while(1)
	{
		radio.powerUp();
		delayMicroseconds(200);
		//printf("radio on\n");
		char receivePayload[32];
		uint8_t pipe = 1;
		
		// Start listening
		radio.startListening();
		//printf("startListening\n");			
		//while ( radio.available(&pipe) ) 
		//{
			len = radio.getDynamicPayloadSize();
			radio.read( receivePayload, len );

			// Display it on screen
			printf("Recv: size=%i payload=%s pipe=%i",len,receivePayload,pipe);

			// Send back payload to sender
			radio.stopListening();

			pipe++;
			
			// reset pipe to 0
			if ( pipe > 6 ) 
				pipe = 0;
		//}
		
		delayMicroseconds(200);
		radio.powerDown();
		delayMicroseconds(2000000);
	}
	
	return 0;
}

