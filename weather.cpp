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

//
// Hardware configuration
//

// CE Pin, CSN Pin, SPI Speed

RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

// sets the role of this unit in hardware.  Connect to GND to be the 'pong' receiver
// Leave open to be the 'ping' transmitter
const int role_pin = 7;

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xf0f0f0f0d2LL, 0xf0f0f0f0e1LL };

int main(int argc, char** argv)
{
  // Setup and configure rf radio
  //
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //  radio.setPayloadSize(8);
  radio.setChannel(0x4c);
  radio.setPALevel(RF24_PA_LOW);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
 
  //
  // Start listening
  //
  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //
  radio.printDetails();
  //
  // Ping out role.  Repeatedly send the current time
  //

 // forever loop
 while (1)
 {
 // if there is data ready
  //printf("Check available...\n");
  if ( radio.available() )
  {
       // Dump the payloads until we've gotten everything
       unsigned long got_time;
       bool done = false;

       while (!done)
       {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_time, sizeof(unsigned long) );

        // Spew it
        printf("Got payload(%d) %lu...\n",sizeof(unsigned long), got_time);

        // Delay just a little bit to let the other unit
        // make the transition to receiver
        delay(20);
       }

       // First, stop listening so we can talk
       radio.stopListening();

       // Send the final one back.
       radio.write( &got_time, sizeof(unsigned long) );

       // Now, resume listening so we catch the next packets.
       radio.startListening();
  }

 return 0;
}
