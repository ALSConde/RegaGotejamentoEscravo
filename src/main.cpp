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
#define U0_TXD 1 // Pino TX da serial
#define U0_RXD 3 // Pino de RX da serial
#define DIFJ 2   // Pino de saida da solenoide

/*
 *Definicao do ID do escravo
 *Por questoes de conveniencia, foi decidido que independente da quantidade de mestres, os ids dos escravos serao sequenciais
 */
#define ID 1

// Definicao do intervalo de tempo entre as leituras
#define INTLEITURA 500

void serialEvent();

// Definicao das variaveis
float PUmidade;
float media;
float leitura;
int dif; // Dispositivo interruptor de fluxo d'agua
float umidade;
string msg;
int t1, t2; // Variaveis de temporizacao

void setup()
{
  // Iniciar a serial e pinos usados na transmissao serial
  Serial.begin(9600);
  pinMode(U0_TXD, INPUT_PULLUP);
  pinMode(U0_RXD, OUTPUT);

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

  // Intervalo Entre as leituras
  if (t2 - t1 > INTLEITURA)
  {
    // Variavel umidade para ler a umidade registrada pelo sensor
    umidade = 0;

    // TODO: Levantar a curva do sensor
    // Leitura do sensor e ajuste da curva de umidade
    leitura = analogRead(A0);
    umidade += (-0.0002 * leitura * leitura + 10.25 * leitura - 0.000000000001) / 100;
    // Serial.println("Umidade: " + (String)umidade);

    // Calcular a media
    media = (double)(umidade / (1));
    // Serial.println("Parametro: " + (String)PUmidade);

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
void serialEvent()
{
  String aux = "";
  String id = "";
  String umidade = "";
  char c;

  while (Serial.available() > 0)
  {
    c = (char)Serial.read();
    aux.concat(c);

    if (c == ';')
    {
      break;
    }
  }

  // Obter o Id enviado na serial
  id = aux.substring(aux.indexOf(":"), aux.indexOf(","));

  // Verificar o id recebido
  if (id.toInt() == ID)
  {
    // Enviar os dados atuais de umidade e o parametro autual
    msg = "Umidade: " + to_string(media) + "\nParametro:" + to_string(PUmidade);
    Serial.println(msg.c_str());

    // Atualizar aux removendo o id e obter a umidade que sera definida como parametro
    aux = aux.substring(aux.indexOf(",") + 1, aux.indexOf(";"));
    umidade = aux.substring(0, aux.indexOf(";"));

    // Atualizar o parametro umidade
    PUmidade = umidade.toFloat();
  }
}