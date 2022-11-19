#include <NTPClient.h>
#include <string>
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
int timeZone = -3;

struct Date
{
  int dayOfWeek;
  int day;
  int month;
  int year;
  int hours;
  int minutes;
  int seconds;
};

char *dayOfWeekNames[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define WIFI_SSID "POLLY_2.4G"
//#define WIFI_SSID "WI-FI"
#define WIFI_PASSWORD "g010203p"
//#define WIFI_PASSWORD "4dr3n4l1n4"

NTPClient ntpClient(ntpUDP, "0.br.pool.ntp.org", timeZone * 3600, 60000);

#define PinoBomba 14
#define PinoAtivaSensor 26
#define PinoTeste 2
#define PinoSensorSolo 34

int IrrigacaoManual = 0;
int IrrigacaoAutomatico = 0;
int UmidadeSensorSolo = -1;
int Irrigando = 0;
int Ciclos = 0;

int Minimo = 0;
int Maximo = 0;

int InicioHorario = 7;
int TerminoHorario = 20;

String dataIrrigacao;
String inicioIrrigacaoUmd;
String inicioIrrigacaoTempo;
String terminoIrrigacaoUmd;
String terminoIrrigacaoTempo;
String tempoIrrigacao;

String formattedDate;
String dayStamp;
String timeStamp;

int analogSoloSeco = 4000;
int analogSoloMolhado = 1500;
int percSoloSeco = 0;
int percSoloMolhado = 100;

#define FIREBASE_HOST "horta-vertical-96557-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "y7RHHaVjB3XY9Cc8iQoKaanYGEqVCnYSk2PGo3op" // your private key
FirebaseData firebaseData;
FirebaseJson json;
FirebaseJsonData result;

void setup()
{
  pinMode(PinoBomba, OUTPUT);
  pinMode(PinoTeste, OUTPUT);
  pinMode(PinoAtivaSensor, OUTPUT);
  pinMode(PinoSensorSolo, INPUT);
  Serial.begin(9600);
  connectWiFi();
  setupNTP();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  xTaskCreatePinnedToCore(wifiConnectionTask, "wifiConnectionTask", 10000, NULL, 2, NULL, 0);
}

void wifiConnectionTask(void *param)
{
  while (true)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      connectWiFi();
    }
    vTaskDelay(100);
  }
}

void connectWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void setupNTP()
{
  ntpClient.begin();
  while (!ntpClient.update())
  {
    ntpClient.forceUpdate();
    delay(500);
  }
}

Date getDate()
{
  char *strDate = (char *)ntpClient.getFormattedDate().c_str();
  Date date;
  sscanf(strDate, "%d-%d-%dT%d:%d:%dZ",
         &date.year,
         &date.month,
         &date.day,
         &date.hours,
         &date.minutes,
         &date.seconds);

  // Dia da semana de 0 a 6, sendo 0 o domingo
  date.dayOfWeek = ntpClient.getDay();
  return date;
}

void getIrrigacaoManual()
{
  if (Firebase.getString(firebaseData, "/irrigarManual"))
  {
    String irrigarManual = firebaseData.stringData();
    if (irrigarManual.toInt() == 1)
    {
      IrrigacaoManual = 1;
      digitalWrite(PinoBomba, HIGH);
      digitalWrite(PinoTeste, HIGH);
      // gravaHistoricoIrrigacao();
    }
    else
    {
      IrrigacaoManual = 0;
      digitalWrite(PinoBomba, LOW);
      digitalWrite(PinoTeste, LOW);
    }
  }
  else
  {
    Firebase.setString(firebaseData, "irrigarManual", IrrigacaoManual);
    Serial.print("Error in getIrrigacaoManual, ");
    Serial.println(firebaseData.errorReason());
  }
}

void getIrrigacaoAtumatica()
{
  if (Firebase.getString(firebaseData, "/irrigarAutomatico"))
  {
    String irrigarAutomatico = firebaseData.stringData();
    IrrigacaoAutomatico = irrigarAutomatico.toInt();
    if (irrigarAutomatico.toInt() == 1)
    {
      int umidade = UmidadeSensorSolo;
      int min = getRangeMinIrrigacao();
      int max = getRangeMaxIrrigacao();
      consultaSensorUmdSolo(min, max, umidade);
    }
  }
  else
  {
    Firebase.setString(firebaseData, "irrigarAutomatico", IrrigacaoAutomatico);
    Serial.print("Error in getIrrigacaoAtumatica, ");
    Serial.println(firebaseData.errorReason());
  }
}

void gravaUmidadeAtual()
{
  digitalWrite(PinoAtivaSensor, HIGH);
  delay(500);
  if (Firebase.getString(firebaseData, "/umidadeAtual"))
  {
    int valorSensor = analogRead(PinoSensorSolo);
    UmidadeSensorSolo = constrain(valorSensor, analogSoloMolhado, analogSoloSeco);
    UmidadeSensorSolo = map(UmidadeSensorSolo, analogSoloMolhado, analogSoloSeco, percSoloMolhado, percSoloSeco);
    Firebase.setString(firebaseData, "umidadeAtual", UmidadeSensorSolo);
  }
  else
  {
    Firebase.setString(firebaseData, "umidadeAtual", 0);
    Serial.print("Error in gravaUmidadeAtual, ");
    Serial.println(firebaseData.errorReason());
  }
  digitalWrite(PinoAtivaSensor, LOW);
}

int getRangeMinIrrigacao()
{
  if (Firebase.getString(firebaseData, "/range/min"))
  {
    String min = firebaseData.stringData();
    Minimo = min.toInt();
    return Minimo;
  }
  else
  {
    // Firebase.setString(firebaseData, "irrigarAutomatico", IrrigacaoAutomatico);
    Serial.print("Error in getRangeMinIrrigacao, ");
    Serial.println(firebaseData.errorReason());
    return 0;
  }
}

int getRangeMaxIrrigacao()
{
  if (Firebase.getString(firebaseData, "/range/max"))
  {
    String max = firebaseData.stringData();
    Maximo = max.toInt();
    return Maximo;
  }
  else
  {
    // Firebase.setString(firebaseData, "irrigarAutomatico", IrrigacaoAutomatico);
    Serial.print("Error in getRangeMaxIrrigacao, ");
    Serial.println(firebaseData.errorReason());
    return 0;
  }
}

void gravaCiclos()
{
  if (Firebase.getInt(firebaseData, "/ciclos"))
  {
    int ciclos = firebaseData.intData();
    Ciclos = ciclos + 1;
    Firebase.setInt(firebaseData, "ciclos", Ciclos);
    gravaHistoricoIrrigacao();
  }
  else
  {
    Firebase.setInt(firebaseData, "ciclos", 1);
    Serial.print("Error in Ciclos, ");
    Serial.println(firebaseData.errorReason());
  }
}

void consultaSensorUmdSolo(int min, int max, int umidade)
{
  if (IrrigacaoManual == 0 && umidade > 0 && max > 0)
  {
    if ((umidade < min) && Irrigando == 0)
    {
      Irrigando = 1;
      digitalWrite(PinoBomba, HIGH);
      inicioIrrigacaoUmd = umidade;
      inicioIrrigacaoTempo = ntpClient.getFormattedTime();
      return;
    }
    if ((umidade > max) && Irrigando == 1)
    {
      Irrigando = 0;
      digitalWrite(PinoBomba, LOW);
      terminoIrrigacaoUmd = umidade;
      terminoIrrigacaoTempo = ntpClient.getFormattedTime();
      gravaCiclos();
      return;
    }
  }
}

void gravaHistoricoIrrigacao()
{
  if (Firebase.getJSON(firebaseData, "/historico"))
  {
    FirebaseJson jsonUp;
    jsonUp.set("data", ntpClient.getFormattedDate());
    jsonUp.set("hora", ntpClient.getFormattedTime());
    jsonUp.set("inicio", inicioIrrigacaoUmd);
    jsonUp.set("termino", terminoIrrigacaoUmd);
    Firebase.pushJSON(firebaseData, "historico", jsonUp);
  }
  else
  {
    FirebaseJson jsonUp;
    jsonUp.set("data", ntpClient.getFormattedDate());
    jsonUp.set("hora", ntpClient.getFormattedTime());
    jsonUp.set("inicio", inicioIrrigacaoUmd);
    jsonUp.set("termino", terminoIrrigacaoUmd);
    Firebase.setJSON(firebaseData, "historico", jsonUp);
    Serial.print("Error in HistoricoIrrigacao, ");
    Serial.println(firebaseData.errorReason());
  }
}

void loop()
{
  ntpClient.update();
  int horaAtual = ntpClient.getHours();
  if (horaAtual > InicioHorario && horaAtual < TerminoHorario)
  {
    if (Irrigando == 0)
    {
      getIrrigacaoManual();
    }
    getIrrigacaoAtumatica();
  }
  gravaUmidadeAtual();
  delay(1000);
}
