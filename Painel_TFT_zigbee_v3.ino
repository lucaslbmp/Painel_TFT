//#include <stdint.h>
#include <string.h>
#include <memorysaver.h>
#include <UTFT.h>
#include <avr/pgmspace.h>
//#include <SoftwareSerial.h>
#include <TinyGPS.h>

// Declarando quais fontes de texto usaremos
extern uint8_t BigFont[];
extern uint8_t franklingothic_normal[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t SevenSeg_XXXL_Num[];

//Declarando as imagens como arrays de bits
extern unsigned short logobaja[];
extern unsigned short temperatura_white[];
extern unsigned short temperatura_red[];
extern unsigned short fuel_icon_white[];
extern unsigned short fuel_icon_red[];
extern unsigned short bateria_0[];
extern unsigned short uart[];

//const unsigned short icone_bateria[] = {bateria_0 , bateria_25, bateria_50, bateria_75, bateria_100}; // Frames para cada percentual de bateria (0-100%)
const unsigned short icone_temperatura[] = {temperatura_white,temperatura_red}; // Frames para temperatura baixa / alta
const unsigned short icone_combustivel[] = {fuel_icon_white,fuel_icon_red}; // Frames para comusticel normal / baixo

// Variaveis referentes a comunicaçao serial
String auxD="v*,", auxT="r*,c*,", auxG="g*,"; // Strings que armazenam as mediads dos sensores (Diana (auxD), Tarsila(auxT), GPS(auxG))
unsigned long t_last_d = 0, t_last_t = 0; // Instantes da ultima vez que os buffers das portas seriais receberam algum dado (Diana, Tarsila)

// ================================== Constantes ===============================================

//Constantes tacometro
const int div_tacometro = 12; //Numero de divisoes do tacometro
const int deltayTacometro = 120; //Altura do conjunto de retangulos do tacometro
const int tac_mark = 3; // A cada quantos retangulos aparecerá um numero na escala

// Constantes combustivel/autonomia
const int div_comb = 6; //Numero de divisoes do indicador de combustivel
const float consumo = 5.0; // Consumo medio de combustivel [km/L] (verificar)
const float max_volume = 5.0; // Maximo volume de combustivel no tanque [L] (verificar)
const int deltayCombustivel = 80; //Altura do conjunto de retangulos do indicador de combustivel

// Constantes bateria
const int div_bateria = 4; //Numero de divisoes do indicador de bateria
const int xbat = 195; // Coordenada X do icone da bateria
const int ybat = 13;  // Coordenada Y do icone da bateria
const int len_barra_bat = 38; // Comprimento da barra de bateria em 100%
const int w_barra_bat = 14;

// Constantes rpm
const int max_rpm = 6000; // rpm maximo

//Constantes temperatura
const int max_temp = 70; // Maxima temperatura segura da CVT
const int x_temp_icone = 130; // Posiçao x do icone de temperatura
const int y_temp_icone = 5; // Posiçao x do icone de temperatura
const int len_temp_icone = 60; // Comprimento do icone de temperatura
const int w_temp_icone = 48; // Largura do icone de temperatura

// Constantes piloto
int c_help = 0; // Tempo em milissegundos em que a menagem de socorro aparece

// Constantes comunicaçao
const int x_icone_uart = 75;
const int y_icone_uart = 5;

// ================================================================================================

// Variaveis referentes as medições
int nv_bat = 3; //Nível da bateria
int nv_bat_ant = 3; //Nível anterior da bateria
int nv_comb  = 5; //Nível de combustivel
bool isFuelRed = 0; // Flag que indica se o icone do combustivel ficara vermelho
int rpm = 1200; // Rotacao do motor
int vel = 0; // Velocidade da roda
float temperatura = 0; //Temperatura da CVT
bool isTempRed = 0; // Flag que indica se o icone do combustivel ficara vermelho
int count_excel = 0; //numero de mensagens enviadas ao zigbee
int out_freio = 0; //Flag que indica se o freio esta pressionado
int help = 0; // Flag que indica se o botao do piloto foi pressionado

// Flags de erro
bool vel_err=0, rpm_err=0, temp_err=0, comb_err=0, bat_err=0, gps_err=0;

// Flags de sensores
bool gps_on = 0; // O sinal do GPS vai ser utilizado?
bool nv_analog_on = 0; // O sinal do sensor de nivel analogico vai ser utilizado?

bool rpm_up=1,vel_up=1,temp_up=1,comb_up=1,bat_up=1; //apagar (apenas para teste)

UTFT myGLCD(ITDB32S, 38, 39, 40, 41);
TinyGPS gps;
//SoftwareSerial mySerial(10, 11); // RX, TX

void setup() {
  Serial.begin(38400);  // Zigbee
  Serial1.begin(9600);  // GPS
  Serial2.begin(38400); // Tarsila
  Serial3.begin(38400); // Diana
  while(!Serial || !Serial1 || !Serial2 || !Serial3);
  //mySerial.begin(38400);
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.drawBitmap(5, 5, 54, 50, logobaja, 1);
  myGLCD.drawBitmap(x_icone_uart, y_icone_uart, 42, 35, uart, 1);
  tacometro_setup();
  velocidade_setup();
  combustivel_setup();
  temperatura_setup();
  bateria_setup();
}

void loop() {
  //teste(); // Deixar comentado! Descomentar apenas para teste
  getDados(); // Recebe os dados dos outrso modulos e atualiza as variaveis (comentar essa linha se "teste()" estiver descomentado)
  verifica_comunicacao(); // Sinaliza na tela se ha erros no recebimento de dados (D,T,G) --> (Diana,Tarsila,GPS)
  tacometro(rpm); // Atualiza o indicador de rotaçao na tela
  velocidade(vel); // Atualiza o indicador de velocidade na tela
  termometro(temperatura); // Atualiza o indicador de temperatura na tela
  bateria(nv_bat); // Atualiza o indicador de bateria na tela
  combustivel(nv_comb); // Atualiza o indicador de combustivel na tela
  zigbee(); // Envia os dados de todos os sensores para o zigbee
  //delay(30);
}
