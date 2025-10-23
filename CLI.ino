#include <EEPROM.h>
#define success "Оperation successfull. "
#define failed "Operation failed. "

String command=" ";
int part;
int position=6;
int value=0xFFFF;
int adress=0xFFFF;
char operation;
char array[2]={0,0};
void setup() {
  Serial.begin(9600);
}

//eeprom -w -a 33 -v 45
//eeprom -v 55 -a 44 -r

unsigned int find_smth(unsigned int starting_pos ){
  for (unsigned int i=starting_pos;i<command.length();i++){
    if (command.charAt(i)!=' '){
      if (command.charAt(i)=='-')
        position=i+1;
      else
        position=i;
      return i;
    }
  }
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

bool getValue(byte* array, unsigned int com_length, uint8_t pos,char type){
  uint16_t val=0;
  for (int i=pos;((array[i]>47) && (array[i]<58) && (i<(com_length)));i++)
    value=value*10+(array[i]-48);
  if (type=='a'){
    if(value>0x3FF){
      Serial.write(failed);
      Serial.write("Adress is too high");
      return false;
    }
    else
      adress=val;
  }
  if (type=='v'){
      if(value>255){
      Serial.write(failed);
      Serial.write("Value is too high");
      return false;
    }
    else
      value=val;
  }
  return true;
}

void Check_char(byte* buffer){
  switch(command.charAt(position)) {        
    case 'w':
        for(int i=0;(adress=0xFFFF || value==0xFFFF) || i!=10;i++){
          find_smth(position);
          Check_char(buffer);
        }
        if((adress<EEPROM.length()) && value)
          EEPROM.write(adress,value);
        if (EEPROM.read(adress)==value)
          Serial.write(success);
        else {
          Serial.write(failed);
          Serial.write("EEPROM cell erased. ");
        }
        break;
        case 'r':
          find_smth(position+1);
          for(int i=0;adress=0xFFFF || i!=10;i++){
            find_smth(position);
            Check_char(buffer);
          }
          part=EEPROM.read(adress);
          digit_to_chars(part);
          Serial.write(success);
        break;
        case 'e':
          Report(EEPROM_erase(adress));
        break;
        case 'd':
          EEPROM_dump();
          Serial.write(success);
        break;
        case 'v':
          find_smth(position);
          value=getValue(buffer,command.length(),position,'v');
        break;
        case 'a':
          find_smth(position);
          adress=getValue(buffer,command.length(),position,'a');
        break;
        default:
          Serial.write("invalid command - no such key ");
          Serial.write(0x0A);
        break;
    }

}

/*uint16_t getAdress(byte* array, unsigned int com_length){
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
}*/

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
      if (command.startsWith("eeprom ")){
        find_smth(position);
        Check_char(buffer);
      }
      else
        Serial.print("invalid command");
      adress=0xFFFF;
      value=0xFFFF;
      position=6;
  }
}
