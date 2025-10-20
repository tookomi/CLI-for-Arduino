#include <EEPROM.h>
#define success "Оperation successfull. "
#define failed "Operation failed. "

String command;
int part;
int position;
char array[2]={0,0};
void setup() {
  Serial.begin(9600);
}

void digit_to_chars(int value){
  array[1]=value%16;
  array[0]=value/16;
  for (int k=0;k<2;k++){
      if (array[k]<10)
        array[k]=array[k]+48;
      else
        array[k]=array[k]+55;
      Serial.write(array[k]);
  }
  Serial.write(" ");
}

uint16_t getValue(byte* array, unsigned int com_length, uint8_t pos){
  uint16_t value=0;
  for (int i=pos;((array[i]>47) && (array[i]<58));i++)
    value=value*10+(array[i]-48);
  return value;
}

uint16_t getAdress(byte* array, unsigned int com_length){
  int adress;
  uint8_t below_digits=47, above_digits=58,zero=48;
  if (com_length>14 && ((array[14]>below_digits) && (array[14]<above_digits))){
    adress=(array[13]-zero)*10+(array[14]-zero);
    if (com_length>15 && ((array[15]>below_digits) && (array[15]<above_digits))){
      adress=adress*10+(array[15]-zero);
      if (com_length>16 && ((array[16]>below_digits) && (array[16]<above_digits)))
        adress=adress*10+(array[16]-zero);
    }
  }
    else
      adress=(array[13]-zero);
  if(adress>0x3FF){
    Serial.write(failed);
    Serial.write("Adress is too high");
  }
  return adress;
}

bool EEPROM_erase(unsigned int adress){
  bool status=true;
      EEPROM.write(adress, 0);
      if (EEPROM.read(adress)!=0)
      status=false;
  return status;
}

uint8_t EEPROM_dump(){
  int adress_digits[4];
  for (int i=0; i<EEPROM.length();i=i+8){
    adress_digits[0]=i/1000;
    adress_digits[1]=(i/100)%10;
    adress_digits[2]=(i%100)/10;
    adress_digits[3]=i%10;
    for (int k=0;k!=4;k++){
      Serial.write((adress_digits[k]+48));
    }
    Serial.write(" : ");
    for (int k=i;k<(i+8);k++){
    digit_to_chars(EEPROM.read(k));
    }
    Serial.write(0x0A);
  }
}

void Report(bool function){
  if(function==true)
    Serial.write(success);
  else
    Serial.write(failed);
}

void loop() {
  if(Serial.available()){
      command=Serial.readString();
      byte buffer[command.length()];
      command.getBytes(buffer, command.length());
      if ((command.startsWith("eeprom -")) && ((command.charAt(10)=='-' && command.charAt(11)=='a') || command.charAt(8)=='d')){
           switch(command.charAt(8)) {        
              case 'w':
                for (int i=13;!(command.charAt(i)=='-' && command.charAt(i+1)=='v' && command.charAt(i+2)==' ');i++)
                  position=i+4;
                EEPROM.write(getAdress(buffer,command.length()), getValue(buffer,command.length(),position));
                if (EEPROM.read(getAdress(buffer,command.length()))==getValue(buffer,command.length(),position))
                  Serial.write(success);
                else {
                    Serial.write(failed);
                    Serial.write("EEPROM cell erased. ");
                }
              break;
              case 'r':
                  part=EEPROM.read(getAdress(buffer,command.length()));
                  digit_to_chars(part);
                  Serial.write(success);
              break;
              case 'e':
                Report(EEPROM_erase(getAdress(buffer,command.length())));
              break;
              case 'd':
                EEPROM_dump();
                Serial.write(success);
              break;
              default:
                Serial.write("invalid command");
              break;
           }
      }
      else
        Serial.print("invalid command");
  }
}
