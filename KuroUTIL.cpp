//  ██╗  ██╗██╗   ██╗██████╗  ██████╗     ██╗██╗   ██╗████████╗██╗██╗     
//  ██║ ██╔╝██║   ██║██╔══██╗██╔═══██╗   ██╔╝██║   ██║╚══██╔══╝██║██║     
//  █████╔╝ ██║   ██║██████╔╝██║   ██║  ██╔╝ ██║   ██║   ██║   ██║██║     
//  ██╔═██╗ ██║   ██║██╔══██╗██║   ██║ ██╔╝  ██║   ██║   ██║   ██║██║     
//  ██║  ██╗╚██████╔╝██║  ██║╚██████╔╝██╔╝   ╚██████╔╝   ██║   ██║███████╗
//  ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═════╝    ╚═╝   ╚═╝╚══════╝
//  Made by Tomáš Křivan

#include "Arduino.h"
#include "KuroUTIL.h"

char* KuroUTIL::de_accent(char* text, uint16_t length){
  char* out = new char[length];
  for(uint16_t i = 0; i < length; i++){
    switch(text[i]){
      case 'á': {
        out[i] = 'a';
        break;
      }
      case 'é': {
        out[i] = 'e';
        break;
      }     
      case 'í': {
        out[i] = 'i';
        break;
      } 
      case 'ó': {
        out[i] = 'o';
        break;
      }     
      case 'ú': {
        out[i] = 'u';
        break;
      }     
      case 'Á': {
        out[i] = 'A';
        break;
      }     
      case 'É': {
        out[i] = 'E';
        break;
      }     
      case 'Í': {
        out[i] = 'I';
        break;
      }     
      case 'Ó': {
        out[i] = 'O';
        break;
      }     
      case 'Ú': {
        out[i] = 'U';
        break;
      }     
      case 'č': {
        out[i] = 'c';
        break;
      }     
      case 'Č': {
        out[i] = 'C';
        break;
      }     
      case 'ř': {
        out[i] = 'r';
        break;
      }     
      case 'Ř': {
        out[i] = 'R';
        break;
      }     
      case 'ě': {
        out[i] = 'e';
        break;
      }     
      case 'Ě': {
        out[i] = 'E';
        break;
      }     
      case 'š': {
        out[i] = 's';
        break;
      }     
      case 'Š': {
        out[i] = 'S';
        break;
      }     
      case 'ů': {
        out[i] = 'u';
        break;
      }     
      case 'Ů': {
        out[i] = 'U';
        break;
      }
      case 'ü': {
        out[i] = 'u';
        break;
      }     
      case 'Ü': {
        out[i] = 'U';
        break;
      }     
      case 'ý': {
        out[i] = 'y';
        break;
      }     
      case 'Ý': {
        out[i] = 'Y';
        break;
      }     
      default: {
        out[i] = text[i];
        break;
      }   
    }
  }
  return out;
}