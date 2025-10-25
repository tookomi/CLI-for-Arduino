#include <EEPROM.h>
#define success "Оperation successfull. "
#define failed "Operation failed. "

String command=" ";
int part;
int position=6;
int value=0xFFFF;
int adress=0xFFFF;
char operation;
bool type_defined;
char array[2]={0,0};
void setup() {
  Serial.begin(9600);
}

//eeprom -w -a 33 -v 45
//eeprom -v 55 -a 44 -r

unsigned int find_smth(unsigned int starting_pos ){
  for (unsigned int i=starting_pos;((i+1)<(command.length()-1) && command.charAt(i)!=' ');i++){
    position=i+1;
  }
  for (unsigned int i=position;i<(command.length()-1);i++){
    if (command.charAt(i)!=' '){
      if (command.charAt(i)=='-'){
        position=i+1;
      }
      else{
        position=i;
      }
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

int getValue(byte* array, unsigned int com_length, uint8_t pos,char type){
  uint16_t val=0;
  for (int i=pos;((array[i]>47) && (array[i]<58) && (i<(com_length-1)));i++){
    val=val*10+(array[i]-48);
  }
  if (type=='a'){
    if(val>0x3FF){
      Serial.write(failed);
      Serial.write("Adress is too high");
      return false;
    }
    else
        return val;
  }
  if (type=='v'){
      if(val>255){
      Serial.write(failed);
      Serial.write("Value is too high");
      return 0xFFFF;
    }
    else
      return val;
  }
}

void Check_char(byte* buffer){
  switch(command.charAt(position)) {        
    case 'w':
        type_defined=true;
        for(int i=0;(adress==0xFFFF || value==0xFFFF) && i!=5;i++){
          find_smth(position);
          if (command.charAt(position-1)=='-')
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
          type_defined=true;
          for(int i=0;(adress==0xFFFF && i!=5);i++){
            //Serial.write(adress);
            //Serial.write(10);
            find_smth(position);
            if (command.charAt(position-1)=='-')
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
          //Serial.write(adress);
          //Serial.write(10);
        break;
        default:
          Serial.write("invalid command - no such key ");
          //Serial.write(command.charAt(position));
          Serial.write(0x0A);
        break;
    }

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
      if (command.startsWith("eeprom ")){
        while(!type_defined){
          find_smth(position);
          Check_char(buffer);
        }
      }
      else
        Serial.print("invalid command");
      adress=0xFFFF;
      value=0xFFFF;
      position=6;
      type_defined=false;
  }
}
