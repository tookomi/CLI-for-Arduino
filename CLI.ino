#include <EEPROM.h>
#define success "operation successfull. "
#define failed "operation failed. "

String command;
int part;
int position;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

char* dec_to_hex(char* dec){
  char result[sizeof(dec)];
  for (int k=0;k<sizeof(dec);k++){
      if (dec[k]<10)
        dec[k]=dec[k]+48;
      else
        dec[k]=dec[k]+55;
  }
  return dec;
}

char* digit_to_chars(int value){
  char array[2]={0,0};
  int i=0;
  while (value!=0)
  {
    array[i]=value%16;
    value=value/16;
    ++i;
  }
  char buf=array[0];
  array[0]=array[1];
  array[1]=buf;
  return (dec_to_hex(array));
}

uint16_t getValue(byte* array, unsigned int com_length, uint8_t pos){
  uint16_t value=0;
  for (int i=pos;((array[i]>47) && (array[i]<58));i++)
    value=value*10+(array[i]-48);
  return value;
}

uint16_t getAdress(byte* array, unsigned int com_length){
  int adress;
  //14
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
      //EEPROM.write(111, 41);
      //Serial.write(EEPROM.read(111));
      byte buffer[command.length()];
      command.getBytes(buffer, command.length());
      if ((command.startsWith("eeprom -")) && ((command.charAt(10)=='-' && command.charAt(11)=='a') || command.charAt(8)=='d')){
           switch(command.charAt(8)) {        
              case 'w':
                for (int i=13;!(command.charAt(i)=='-' && command.charAt(i+1)=='v' && command.charAt(i+2)==' ');i++)
                  position=i+4;
                EEPROM.write(getAdress(buffer,command.length()), getValue(buffer,command.length(),position));

                Serial.write(getValue(buffer,command.length(),position));
                Serial.write(EEPROM.read(getAdress(buffer,command.length())));

                if (EEPROM.read(getAdress(buffer,command.length()))==getValue(buffer,command.length(),position))
                  Serial.write(success);
                else {
                    Serial.write(failed);
                    Serial.write("EEPROM cell erased. ");
                }
              break;
              case 'r':
                  part=EEPROM.read(getAdress(buffer,command.length()));
                  for (int i=0; i<2;i++)
                    Serial.write(digit_to_chars(part)[i]);
              break;
              case 'e':
                Report(EEPROM_erase(getAdress(buffer,command.length())));
              break;
              case 'd':
                EEPROM_dump();
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
