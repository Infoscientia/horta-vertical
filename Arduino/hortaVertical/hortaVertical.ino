#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif


#define WIFI_SSID "POLLY_2.4G"
#define WIFI_PASSWORD "g010203p" 

#define Data

#define Pino2 2 
#define Pino3 3
#define Pino4 4  

#define IrrigacaoManual 0
   
#define FIREBASE_HOST "horta-vertical-96557-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "y7RHHaVjB3XY9Cc8iQoKaanYGEqVCnYSk2PGo3op"  // your private key
FirebaseData firebaseData;

void setup ()
{
  pinMode(Pino2, OUTPUT);
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ") ;
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);     
}
void loop ()
{
  Data = DateTime.now();
  if(Firebase.getString(firebaseData, "/irrigarManual"))
  {
    String irrigarManual = firebaseData.stringData();
    if(irrigarManual.toInt() == 1){
      IrrigacaoManual = 1
      digitalWrite(Pino2, LOW);
      Serial.println("on");
    }
    else {
      IrrigacaoManual = 0
      digitalWrite(Pino2, HIGH);
      Serial.println("off");
    }
  }else{
    Firebase.setString("irrigarManual", IrrigacaoManual);
    Serial.print("Error in getInt, ");
    Serial.println(firebaseData.errorReason());
  } 
  Serial.println("Data: " + Data);
  
}
