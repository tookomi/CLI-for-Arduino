#include <EEPROM.h>
#define success "operation successfull. "
#define failed "operation failed. "

String command;
int part;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

char digit_to_char(int value){
  
  return (value+48);
}

uint16_t getAdress(byte* array, unsigned int com_length){
  int adress;
  //14
  uint8_t below_digits=47, above_digits=58,zero=48;
  if (com_length>14 && ((array[14]>below_digits) && (array[14]<above_digits))){
    adress=(array[14]-zero)*10+(array[14]-zero);
    if (com_length>15 && ((array[15]>below_digits) && (array[15]<above_digits))){
      adress=adress*10+(array[15]-zero);
      if (com_length>16 && ((array[16]>below_digits) && (array[16]<above_digits)))
        adress=adress*10+(array[16]-zero);
    }
  }
    else
      adress=(array[13]-zero);
  if(adress>0x3FF)
  Serial.write(failed);
  Serial.write("Adress is too high");
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
    Serial.write(success);
  else
    Serial.write(failed);
}

void loop() {
   //put your main code here, to run repeatedly:
  if(Serial.available()){
      command=Serial.readString();

      //for (int i=0;i<command.length();i++)
        //Serial.write(command.charAt(i));
      //Serial.write(command.charAt(8));
      byte buffer[command.length()];
      command.getBytes(buffer, command.length());
      if (command.startsWith("eeprom -")){
           switch(command.charAt(8)) {        
              case 'w':
                
                EEPROM.write(getAdress(buffer,command.length()), 255);
              break;
              case 'r':
                if ((command.charAt(10)=="-")&&(command.charAt(11)=="a"))
                part=EEPROM.read(getAdress(buffer,command.length()));
                Serial.write(digit_to_char(part));
              break;
              case 'e':
                Report(EEPROM_erase());
              break;
              case 'd':
                EEPROM_dump();
              break;
              deafult:
                Serial.write("invalid command");
              break;
           }
      }
      else
        Serial.print("invalid command");
  }
}
