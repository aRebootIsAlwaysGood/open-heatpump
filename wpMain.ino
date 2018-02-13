	
/**
* @file 	wpMain.ino
* @brief	Hauptmodul welches die oberste Programmebene darstellt
* @mainpage	Open-Source Wärmepumpensteuerung
* @section 	intro_sec Einleitung
* 
* Diese Dokumentation beschreibt den Quelltext der Open-Source Wärmepumpensteuerung
* für die Arduino Entwicklungsumgebung. Diese Software wurde speziell für die
* im selben Projekt entwickelte Steuerung geschrieben.
* 
* @section dependencies Abhängigkeiten
* 
* Dieses Programm benutzt die folgenden Libraries:
* PID_v1 <a href="https://github.com/adafruit/Adafruit_Sensor">Add Link</a>
* LiquidCrystal_I2C <a href="https://github.com/adafruit/Adafruit_Sensor">Add Link</a>
* Streaming <a href="https://github.com/adafruit/Adafruit_Sensor">Add Link</a>
* 
* ESPAsyncTCP		<a href="https://github.com/me-no-dev/ESPAsyncTCP"
*					>Async TCP Library for ESP8266</a>
* ESPAsyncWebServer	<a href="https://github.com/me-no-dev/ESPAsyncWebServer"
*					>Async Web Server for ESP8266 and ESP32</a>
* ESPUI				<a href="https://github.com/s00500/ESPUI"
*					>A simple web user interface library for ESP8266/ESP32</a>
* 
* 
* @section author Author
* 
* Entwickelt und geschrieben von Daniel Schmissrauter.
* 
* Jahr 2018
* 
* @section license Lizenz
*
* GNU GPLv2 or later
*/

/***************************************************************************************/
/** 
* @brief 	Enthält die Main- und Initialisierungsfunktion des gesamten Projektes.
* @details 	Hier sind die Top-Level Funktionen definiert, welche alle anderen aufrufen.
* Da die Setup-Funktion bei Programmstart als erste ausgeführt wird, müssen
* hier alle notwendigen Initialisierungsfunktion aufgerufen werden.
* Globale Variablen und Objekte sind desweiteren hier zu finden.
*/
/***************************************************************************************/

// ***** HEADERS *****
#include <stdint-gcc.h>
#include <PID_v1.h>	
#include <Streaming.h>
#include "wpMain.h"
#include "wpAuto.h"
#include "wpControl.h"
#include "wpEnteisung.h"
#include "wpManuell.h"
#include "wpRegler.h"
#include "wpSensorik.h"
#include "wpSpeicherladung.h"
#include "wpUser.h"


struct DI_STATES DiStates;	/** ausgelesene DI-Werte. */
struct SYSTEMZUSTAND Systemzustand; /** Systeminfos über Betriebszustand. */

struct USER_SETTINGS Usersettings[] =
{
	{0,0}, //No connection
	{1,1},	//SerialStatus(useraction, uservalue)
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
};

wpState_t wpState= WP_STATE_IDLE;	/** Switch-Case Variable für WP Betriebszustände. */
wpReqFunc_t wpReqFunc= WP_REQ_FUNC_IDLE;/** Betriebswahl-Anforderung an WP-Kontrollmodul. */
reglerState_t reglerState= REGLER_STATE_OFF;	/** Regler Betriebszustand Steuerung. */
ladenState_t ladenState= LADEN_STATE_IDLE; /** Speicherladung Zustandsautomaten-Variable. */


uint8_t blink1Hz;	/** Globale Blinkvariable gesteuert durch blinkFunction(). */


/************************************************************************/
/**
*	@brief Blinkfunktion, welche ein binäres Taktsignal von 1Hz liefert.
*	
*	Mittels der Blinkfunktion wird ein ständiges Taktsignal von 1Hz erzeugt, welches die
*	globale Variable blink1Hz aktualisiert. Somit kann diese eingelesen werden und somit
*	entfällt die ständige Notwendigkeit, auf die millis() Funktion zurückzugreifen.
*/
/************************************************************************/
void blinkFunction(){
	static uint32_t lastsec;
	if (millis()-lastsec >= 1000){
		blink1Hz = !blink1Hz;
		lastsec= millis();
		}
	return;
}

/***************************************************************************************/	
/**
* @brief Setup-Funktion welche zum Programmstart einmalig ausgeführt wird.
*
* Initialisiert Pins und externe Interruptquellen sowie weitere 
* Schnittstellen. Daneben werden spezifische Initialisierungsfunktion aufgerufen.
* 
* @return @c void
*/
/***************************************************************************************/
void setup(){
Serial.begin(9600); // PC
Serial1.begin(9600); // ESP8266
// WP Inputs active low
pinMode(PIN_ND,INPUT_PULLUP);
pinMode(PIN_HD,INPUT_PULLUP);
pinMode(PIN_MOTPROTECT,INPUT_PULLUP);
pinMode(PIN_ZUST_KSTART,INPUT_PULLUP);
pinMode(PIN_ZUST_KBETR,INPUT_PULLUP);
pinMode(PIN_TARIFSPERRE,INPUT_PULLUP);
pinMode(PIN_DI_RESERVE1,INPUT_PULLUP);
pinMode(PIN_DI_RESERVE2,INPUT_PULLUP);
// WP Outputs 
pinMode(PIN_SUMPFHEIZUNG,OUTPUT);
pinMode(PIN_K_ANLAUF,OUTPUT);
pinMode(PIN_K_BETRIEB,OUTPUT);
pinMode(PIN_BYPASS,OUTPUT);
pinMode(PIN_VENTILATOR,OUTPUT);
pinMode(PIN_LADEPUMPE,OUTPUT);
pinMode(PIN_HEIZPUMPE,OUTPUT);
pinMode(PIN_MISCHER_ZU,OUTPUT);
pinMode(PIN_MISCHER_AUF,OUTPUT);
pinMode(PIN_SAMMELALARM,OUTPUT);
pinMode(PIN_DO_RESERVE1,OUTPUT);
pinMode(PIN_DO_RESERVE2,OUTPUT);
// User IOs
pinMode(PIN_BETRMODUS,INPUT_PULLUP);
pinMode(PIN_NAV_UP,INPUT_PULLUP);
pinMode(PIN_NAV_DOWN,INPUT_PULLUP);
pinMode(PIN_NAV_ENTER,INPUT_PULLUP);
pinMode(PIN_AUTOBETR_EN,OUTPUT);
pinMode(PIN_MANBETR_EN,OUTPUT);
pinMode(PIN_ALARM,OUTPUT);

setupSteuerIO();
}

/***************************************************************************************/
/**
* @brief Main-Funktion welche ständig wiederholt wird.
*
* Enthält Funktionen welche vom Hauptprogramm direkt aufgerufen werden.
* 
* @return @c void
*/
/***************************************************************************************/
void loop(){
	;
}










