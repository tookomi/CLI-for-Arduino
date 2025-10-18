#include <EEPROM.h>
String command;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

int getAdress(char pos14, char pos15){
  int adress;
  //14 15
  if ((pos15>47) && (pos15<58))
    adress=(pos14-48)*10+(pos15-48);
    else
    adress=(pos14-48);
  return adress;
}

bool EEPROM_erase(){
  bool status=true;
  for (int i=0; i<EEPROM.length();i++){
      EEPROM.write(i, 0);
      if (EEPROM.read(i)!=0)
      status=false;
  }
  return status;
}

uint8_t EEPROM_dump(){

}

void Report(bool function){
  if(function==true)
    Serial.write("operation succsesfull ");
  else
    Serial.write("operation failed ");
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
                if ((command.charAt(11)=="-")&&(command.charAt(12)=="a"))
                EEPROM.read(getAdress(command.charAt(14),command.charAt(15)));
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
  }
}
