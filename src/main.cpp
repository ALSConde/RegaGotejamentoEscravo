#include <Arduino.h>
#include <iostream>

using namespace std;

/*
TODO LIST:
Levantar a curva do sensor de umidade
Funcao de leitura que descarte de valores irreais
*/

#define DIFJ 2 // Pino de saida da solenoide
#define ID 1   // Identificador unico do microcontrolador

// Definicao das variaveis
float PUmidade;
float media;
float leitura;
int dif;
float umidade;
string msg;

void setup()
{
  // Iniciar serial
  Serial.begin(9600);

  // Definir parametros iniciais
  PUmidade = 50;
  dif = 1;
  media = 50;

  // Definir pino de saida
  pinMode(DIFJ, OUTPUT);
}

void loop()
{
  delay(750);

  // Variavel umidade para ler a umidade registrada pelo sensor
  umidade = 0;

  // TODO: Levantar a curva do sensor
  // Leitura do sensor e ajuste da curva de umidade
  leitura = analogRead(A0);
  umidade += (-0.0002 * leitura * leitura + 10.25 * leitura - 0.000000000001) / 100;

  // Calcular a media
  media = (double)(umidade / (1));
  Serial.println(media);

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
  digitalWrite(DIFJ, dif);
}

// Funcao de tratamento dos eventos seriais
void serialEvent()
{
  // Verificar a disponibilidade da Serial
  if (Serial.available())
  {
    // ID e definido fixamente no codigo
    // Ler o numero do ID enviado
    if (Serial.read() == ID)
    {
      // Criacao da mensagem com as informacoes solicitadas
      PUmidade = Serial.read();
      msg = "\nUmidade: " + (to_string(PUmidade));
      msg += "\nMedia Selecionada: " + to_string(media);
      // Enviar as informacoes
      if (Serial.availableForWrite())
      {
        Serial.println(msg.c_str());
      }
    }
  }
}