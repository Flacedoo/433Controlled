/*
  RFSniffer

  Usage: ./RFSniffer [<pulseLength>]
  [] = optional

  Hacked from http://code.google.com/p/rc-switch/
  by @justy to provide a handy RF code sniffer
*/

#include "../rc-switch/RCSwitch.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../lib/libconfig.h++"  

using namespace std;
using namespace libconfig;

     
RCSwitch mySwitch;
 


int main(int argc, char *argv[]) {
  
     // This pin is not the first pin on the RPi GPIO header!
     // Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
     // for more information.

     Config cfg;

     try {
     	cfg.readFile("../433Controlled.conf");
     } catch (const FileIOException &fioex) {
     	std::cerr << "I/O error while reading file." << std::endl;
     	return(EXIT_FAILURE);
     } catch (const ParseException &pex) {
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;     	return (EXIT_FAILURE);
     }

     try {
     	string name = cfg.lookup("execAOn");
     	cout << "Store name: " << name << endl << endl;
     } catch (const SettingNotFoundException &nfex) {
     	cerr << "No 'name' setting in configuration file." << endl;
     }

     int PIN = 2;
     
     if(wiringPiSetup() == -1) {
       printf("wiringPiSetup failed, exiting...");
       return 0;
     }

     int pulseLength = 0;
     if (argv[1] != NULL) pulseLength = atoi(argv[1]);

     mySwitch = RCSwitch();
     if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
     mySwitch.enableReceive(PIN);  // Receiver on interrupt 0 => that is pin #2
     bool state = true;
    
     while(1) {
  
      if (mySwitch.available()) {
    
        int value = mySwitch.getReceivedValue();

        if (value == 0) {
          printf("Unknown encoding\n");
        } else {    
   
          printf("Received %i\n", mySwitch.getReceivedValue() );
          if ((value == 83281) && (state == true)) {
            state = false;
            system("/home/pi/./testscript.sh");
            // printf("Switched B On\n");
            sleep(2);
            state = true;
          } else if ((value == 83284) && (state == true)) {
            state = false;
            system("/home/pi/./testscript.sh");
            // printf("Switched B Off\n");
            sleep(2);
            state = true;
          }
        }
    
        mySwitch.resetAvailable();
    
      }
      
  
  }

  exit(0);


}

