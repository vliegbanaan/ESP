#include <WiFi.h>
#include "secret.h"

//data below is set as a define in the file secret.h
const char ssid[] = MIJN_SSID;
const char password[] = MIJN_PASSWORD;

void WiFi_Connect(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    printf(".");
  }
  printf(" CONNECTED\r\n");
}

void setup (){
  WiFi_Connect();
}

void loop(){
  
}
