#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ezTime.h>

void conexaoWifi();
void reconexaoWifi();

const char *SSID = "SENAI EDUCACIONAL";
const char *SENHA = "info@134";


const unsigned long tempoConexao = 10000;
const unsigned long tempoReconexao = 5000;

const String enderecoApi = "https://wttr.in/Sao+Caetano+do+Sul?lang=pt&format=";

float temperatura, sensacaoTermica;
int umidade, pressao;
String condicao, nascerSol;

String consultaApi(String codigo);

void setup() {
  Serial.begin(115200);

  conexaoWifi();


}

void loop() {

  reconexaoWifi();

  temperatura = consultaApi("%t").toFloat();
  sensacaoTermica = consultaApi("%f").toFloat();
  umidade = consultaApi("%h").toInt();
  pressao = consultaApi("%P").toInt();
  condicao = consultaApi("%C");
  nascerSol = consultaApi("%S");

  Serial.printf("Temp: %f\n", temperatura);
  Serial.printf("Sensacao: %f\n", sensacaoTermica);
  Serial.printf("Umidade: %i\n", umidade);
  Serial.printf("Pressao: %i\n", pressao);
  Serial.println("Condicao: " + condicao);
  Serial.println("Nascer Sol " + nascerSol);
  Serial.println("--------------------------");

  delay(10000);

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

