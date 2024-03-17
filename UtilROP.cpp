//  ██╗  ██╗██╗   ██╗██████╗  ██████╗     ██╗██╗   ██╗████████╗██╗██╗     
//  ██║ ██╔╝██║   ██║██╔══██╗██╔═══██╗   ██╔╝██║   ██║╚══██╔══╝██║██║     
//  █████╔╝ ██║   ██║██████╔╝██║   ██║  ██╔╝ ██║   ██║   ██║   ██║██║     
//  ██╔═██╗ ██║   ██║██╔══██╗██║   ██║ ██╔╝  ██║   ██║   ██║   ██║██║     
//  ██║  ██╗╚██████╔╝██║  ██║╚██████╔╝██╔╝   ╚██████╔╝   ██║   ██║███████╗
//  ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═════╝    ╚═╝   ╚═╝╚══════╝
//  Made by Tomáš Křivan

#include "Arduino.h"
#include "UtilROP.h"

char* UtilROP::de_accent_utf8(char* text, uint8_t length){
  char* out = new char[length];
  char control = 0;
  uint8_t offset = 0;
  char de_accented;
  for(uint8_t i = 0; i < length; i++){
    if(control != 0){
      switch(control) {
        case 195: {
          if(text[i] >= 128 && 134 >= text[i])       de_accented = 'A';
          else if(text[i] == 135)                    de_accented = 'C';
          else if(text[i] >= 136 && 139 >= text[i])  de_accented = 'E';
          else if(text[i] >= 140 && 143 >= text[i])  de_accented = 'I';
          else if(text[i] == 144)                    de_accented = 'D';
          else if(text[i] == 145)                    de_accented = 'N';
          else if(text[i] >= 146 && 150 >= text[i])  de_accented = 'O';
          else if(text[i] >= 153 && 156 >= text[i])  de_accented = 'U';
          else if(text[i] == 157)                    de_accented = 'Y';
          else if(text[i] == 159)                    de_accented = 's';
          else if(text[i] >= 160 && 166 >= text[i])  de_accented = 'a';
          else if(text[i] == 167)                    de_accented = 'c';
          else if(text[i] >= 168 && 171 >= text[i])  de_accented = 'e';
          else if(text[i] >= 172 && 175 >= text[i])  de_accented = 'i';
          else if(text[i] == 177)                    de_accented = 'n';
          else if(text[i] >= 178 && 182 >= text[i])  de_accented = 'o';
          else if(text[i] >= 185 && 188 >= text[i])  de_accented = 'u';
          else if(text[i] == 189 && 192 == text[i])  de_accented = 'y';
          break;
        }
        case 196: {
          const uint8_t arr_size = 4;
          char* le_leterre = new char[arr_size] {'A','C','D','E'}; 
          uint8_t* le_counte = new uint8_t[arr_size] {6,8,4,10}; 

          uint8_t pos = 131;
          for(uint8_t area = 0; area < arr_size; area++){
            if(pos + le_counte[area] - 1 < text[i]) {
              pos += le_counte[area];
              continue;
            }
            for(uint8_t offset = 0; offset < le_counte[area]; offset++){
              if(pos + offset == text[i]){
                de_accented = le_leterre[area] + (offset % 2 == 0 ? 0 : 32);
                area = arr_size; //break loop
                break;
              }
            }
          }

          delete[] le_leterre;
          delete[] le_counte;
          break; 
        }
        case 197: {
          const uint8_t arr_size = 11;
          char* le_leterre = new char[arr_size] {'N',' ','O',' ','R','S','T','U',' ','Y','Z'}; 
          uint8_t* le_counte = new uint8_t[arr_size] {6,3,6,2,6,8,6,12,2,3,6}; 

          uint8_t pos = 131;
          for(uint8_t area = 0; area < arr_size; area++){
            if(pos + le_counte[area] - 1 < text[i]) {
              pos += le_counte[area];
              continue;
            }
            for(uint8_t offset = 0; offset < le_counte[area]; offset++){
              if(pos + offset == text[i]){
                de_accented = le_leterre[area] + (offset % 2 == 0 ? 0 : 32);
                area = arr_size; //break loop
                break;
              }
            }
          }

          delete[] le_leterre;
          delete[] le_counte;
          break; 
        }
      }
      out[i - offset] = de_accented;
      de_accented = ' ';
      control = 0;
      continue;
    }
    if(text[i] == 195 || text[i] == 196 || text[i] == 197) {
      control = text[i];
      offset++;
      continue;
    }
    out[i - offset] = text[i];
  }
  return out;
}

char* UtilROP::fill_space(char* text, uint8_t length){
  char* output = new char[length];
  for(int i = 0; i < length; i++){
    if(text[i] == 0x0){ // could be janky
      while(i < length){
        output[i] = ' ';
        i++;
      }
      break;
    }
    output[i] = text[i];
  }
}