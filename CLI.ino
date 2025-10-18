#include <EEPROM.h>
//#include <string.h>

String command;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

uint8_t EEPROM_erase(){


}

uint8_t EEPROM_dump(){

}

void loop() {
   //put your main code here, to run repeatedly:
  if(Serial.available()){
      command=Serial.readString();
      if (command.startsWith("eeprom -")){
           switch(command.charAt(9)) {        
              case 'w':

                EEPROM.write(0, 0);
              break;
              case 'r':

                EEPROM.read(0);
              break;
              case 'e':

                EEPROM_erase();
              break;
              case 'd':
                EEPROM_dump();
              break;
           }
      }
      else
        Serial.print("invalid command");
      delay(50);
      

  }
  
  
}
