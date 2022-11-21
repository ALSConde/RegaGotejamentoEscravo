#include <Arduino.h>
#include <iostream>
#include <SoftwareSerial.h>

using namespace std;

/*
TODO LIST:
Levantar a curva do sensor de umidade
Funcao de leitura que descarte de valores irreais
*/

// Definicao dos pinos usados
#define U0_TXD 15 // Pino TX da serial
#define U0_RXD 13 // Pino de RX da serial
#define DIFJ 2    // Pino de saida da solenoide

/*
 *Definicao do ID do escravo
 *Por questoes de conveniencia, foi decidido que independente da quantidade de mestres, os ids dos escravos serao sequenciais
 */
#define ID "1"

// Definicao do intervalo de tempo entre as leituras
#define INTLEITURA 1500

void masterCOMEnv();

// Definicao das variaveis
float PUmidade;
float media;
float leitura;
int dif; // Dispositivo interruptor de fluxo d'agua
float umidade;
String msg;
int t1, t2; // Variaveis de temporizacao
SoftwareSerial masterCOM(U0_RXD, U0_TXD);

void setup()
{
  // Iniciar a serial e pinos usados na transmissao serial
  Serial.begin(9600);
  masterCOM.begin(9600, SWSERIAL_8N1);
  masterCOM.enableRx(true);
  masterCOM.enableTx(true);

  // Definir parametros iniciais
  PUmidade = 50;
  dif = 1;
  media = 50;

  // Definir pino de saida
  pinMode(DIFJ, OUTPUT);

  // Iniciar cronometro
  t1 = millis();
}

void loop()
{
  // Atualizar o contador do cronometro
  t2 = millis();

  if (masterCOM.available() > 0)
  {
    masterCOMEnv();
  }

  // Intervalo Entre as leituras
  if (t2 - t1 > INTLEITURA)
  {
    // Variavel umidade para ler a umidade registrada pelo sensor
    umidade = 0;

    // TODO: Levantar a curva do sensor
    // Leitura do sensor e ajuste da curva de umidade
    leitura = analogRead(A0);
    umidade += (-0.0002 * leitura * leitura + 10.25 * leitura - 0.000000000001) / 100;

    // Calcular a media
    media = (umidade / (1));
    Serial.println("Umidade: " + (String)umidade);
    Serial.println("PUmidade: " + (String)PUmidade);

    // Comparacao da media para liberar o fluxo d'agua
    if (media < PUmidade)
    {
      // Libera o fluxo d'agua
      dif = 0;
    }
    else
    {
      // Bloqueia o fluxo d'agua
      dif = 1;
    }

    // Alterar o estado do DIFJ
    digitalWrite(DIFJ, dif);

    // Zerar o cronometro
    t1 = millis();
  }
}

// Funcao de tratamento dos eventos seriais
void masterCOMEnv()
{
  String aux = "";
  String id = "";
  String strUmidade = "";
  char c;

  while (masterCOM.available() > 0)
  {
    c = (char)masterCOM.read();
    aux.concat(c);

    if (c == ';')
    {
      break;
    }
  }

  // Obter o Id enviado na serial
  id = aux.substring(aux.indexOf(":") + 1, aux.indexOf(","));

  // Verificar o id recebido
  if (id.equalsIgnoreCase(ID))
  {
    // Formatar a mensagem a ser enviada
    msg = "Umidade: " + String(media) + " Parametro: " + PUmidade + ";";

    // Atualizar aux removendo o id e obter a umidade que sera definida como parametro
    aux = aux.substring(aux.indexOf(",") + 1, aux.indexOf(";"));
    strUmidade = aux.substring(aux.indexOf(":") + 1, aux.indexOf(";"));

    // Atualizar o parametro umidade
    PUmidade = (float)(strUmidade.toFloat());

    // Enviar os dados atuais de umidade e o parametro autual
    masterCOM.println(msg.c_str());
  }
}