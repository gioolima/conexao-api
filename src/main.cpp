#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ezTime.h>
//ex. 2
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd (0x27, 20, 4);
JsonDocument doc;

void conexaoWifi();
void reconexaoWifi();

Timezone fuso;

const char *SSID = "SENAI EDUCACIONAL";
const char *SENHA = "info@134";

float tempAnterior;
float sensAnterior;
int umiAnterior;
int presAnterior;
String condAnterior;
String nascAnterior;
String dataAnt;
String horaAnt;

const unsigned long tempoConexao = 10000;
const unsigned long tempoReconexao = 5000;
unsigned long intervalo = 10000;
unsigned long tempoAnterior = 0;

const String enderecoApi = "https://wttr.in/Sao+Caetano+do+Sul?lang=pt&format=";

//float temperatura, sensacaoTermica;
//int umidade, pressao;
//String condicao, nascerSol;

//função para coleta de dados da Api

String consultaApi(String codigo);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  conexaoWifi();

  waitForSync();

  fuso.setLocation("America/Sao_Paulo");



}

void loop() {

  reconexaoWifi();

  unsigned long tempoAtual = millis();


  //ex. utilização de api

  //temperatura = consultaApi("%t").toFloat();
  //sensacaoTermica = consultaApi("%f").toFloat();
  //umidade = consultaApi("%h").toInt();
  //pressao = consultaApi("%P").toInt();
  //condicao = consultaApi("%C");
  //nascerSol = consultaApi("%S");

  //Serial.printf("Temp: %f\n", temperatura);
  //Serial.printf("Sensacao: %f\n", sensacaoTermica);
  //Serial.printf("Umidade: %i\n", umidade);
  //Serial.printf("Pressao: %i\n", pressao);
  //Serial.println("Condicao: " + condicao);
  //Serial.println("Nascer Sol " + nascerSol);
  //Serial.println("--------------------------");

  //delay(10000);

  
  if(tempoAtual - tempoAnterior >= intervalo){
    doc["temp"] = consultaApi("%t").toFloat();
    doc["sensacao"] = consultaApi("%f").toFloat();
    doc["umidade"] = consultaApi("%h").toInt();
    doc["pressao"] = consultaApi("%P").toInt();
    doc["condicao"] = consultaApi("%C");
    doc["nascerSol"] = consultaApi("%S");
    doc["data"] = fuso.dateTime("d/m");
    doc["hora"] = fuso.dateTime("H:i");


    serializeJsonPretty(doc, Serial);
    Serial.println();
 
    float temperatura = doc["temp"];
    float sensacao = doc["sensacao"];
    int umidade = doc["umidade"];
    int pressao = doc["pressao"];
    String condicao = doc["condicao"];
    String nascerSol = doc["nascerSol"];
    String dataAtual = doc["data"];
    String horaAtual = doc["hora"];


    


    if (tempAnterior != temperatura || sensAnterior != sensacao || umiAnterior != umidade || presAnterior != pressao || condAnterior != condicao || nascAnterior != nascerSol || dataAnt != dataAnt || horaAnt != horaAtual){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("D:" + dataAtual + "     H:" + horaAtual);
      lcd.setCursor(0,1);
      lcd.print( "T:" + String(temperatura, 1) + " C    S:" + String(sensacao, 1) + " C");
      lcd.setCursor(0,2);
      lcd.print("U:" + String(umidade) + "%       P:" + String(pressao));
      lcd.setCursor(0,3);
      lcd.print("C:" + condicao.substring(0, 6) + "   N.S:" + nascerSol.substring(0, 5));

      tempAnterior = temperatura;
      sensAnterior = sensacao;
      umiAnterior = umidade;
      presAnterior = pressao;
      condAnterior = condicao;
      nascAnterior = nascerSol;
      dataAnt = dataAtual;
      horaAnt = horaAtual;

    }


    tempoAnterior = tempoAtual;
  }





}

void conexaoWifi() {
  
  // Conexao WiFi
  Serial.printf("Conectando ao WiFi: %s", SSID);

  WiFi.begin(SSID, SENHA);

  unsigned long tempoWifi = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - tempoWifi < tempoConexao)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi conectado com sucesso!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Falha ao se conectar");
  }
 
}

void reconexaoWifi() {
  unsigned long tempoAtual = millis();
  unsigned long tempoUltimaConexao = 0;

  if (tempoAtual - tempoUltimaConexao > tempoReconexao)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Conexao WiFi perdida");
      conexaoWifi();
    }

    tempoUltimaConexao = tempoAtual;
    
  }
  
}

String consultaApi(String codigo){
  HTTPClient http;

  http.begin(enderecoApi+codigo);
  int codigoHTTP = http.GET();
  String resposta = "";

  if (codigoHTTP == 200)
  {
    resposta = http.getString();
  }
  else{
    Serial.println("Deu erro. Código:" + String(codigoHTTP));
  }

  http.end();
  return resposta;
  
}



