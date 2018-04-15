
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

* -Streaming: <a href="http://arduiniana.org/libraries/streaming/">Streaming V5.0</a>
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

/******************************************************************************/
/**
* @brief 	Enthält die Main- und Initialisierungsfunktion des gesamten Projektes.
* @details 	Hier sind die Top-Level Funktionen definiert, welche alle anderen aufrufen.
* Da die Setup-Funktion bei Programmstart als erste ausgeführt wird, müssen
* hier alle notwendigen Initialisierungsfunktion aufgerufen werden.
* Globale Variablen und Objekte sind desweiteren hier zu finden.
*/
/******************************************************************************/
// ***** HEADERS *****

#include <Streaming.h>
#include "wpMain.h"
#include "wpAuto.h"
#include "wpControl.h"
#include "wpManuell.h"
#include "wpRegler.h"
#include "wpSensorik.h"
#include "wpSpeicherladung.h"
#include "wpUser.h"

struct DI_STATES DiStates= {0,0,0,0,0,0,0,0}; /** ausgelesene DI-Werte als Bitfields mit 0 initialisieren. */

struct SYSTEMZUSTAND Systemzustand= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; /** Systemzustand als Bitfields.  */

/** Structarray mit Einstellungen welche vom User am HMI vorgenommen werden */
struct SETTINGS Usersettings[15] =
{
	//123456789012345678| max command length
	{"HMIready",1},	 /**< Determines if HMI is ready and serial link up. */
	{"HMIbmode",1},	/**< Betriebsmodus 0=Stdby, 1=AutoN, 2=AutoR, 3=Man, 4=Error */
	{"HMIversN",0},	/**< Parallelverschiebungsstufe Normalbetrieb -5...+5. */
	{"HMIversR",0},	/**< Parallelverschiebungsstufe reduzierter Betrieb -5...+5. */
	{"HMIstufe",4},	/**< Heizkurvenstufe  1...11 */
	{"HMItimeNow",0},	/**< Aktuelle Uhrzeit in Minuten und Stunden [hhmm] */
	{"HMIdateNow",0},	/**< Aktueller Tag und Monat als [ddmm] */
	{"HMIyearNow",0},	/**< Aktuelles Jahr als [yyyy] */
	{"HMItimerON",0},	/**< Zeitgesteuerter Betrieb Einschaltzeit [hhmm] */
	{"HMItimerOFF",0},	/**< Zeitgesteuerter Betrieb Ausschaltzeit [hhmm] */
	{"HMItimerDay",0},	/**< Zeitgesteuerter Betrieb Wochentage [0b0SSFDMDM] */
	{"HMIdevToggle",0},	/**< Manuelles Ein-/Aus der Komponenten  */
	{"HMIuser1",0},	/**< Unbelegte Option */
	{"HMIuser2",0},	/**< Unbelegte Option */
	{"HMIuser3",0}	/**< Unbelegte Option */
};

/** Structarray mit Infos, welche an das HMI gesendet werden */
struct SETTINGS Systemsettings[8] =
{
	{"SYSready",1},		/**< Serialport on System ready, send Val 1 */
	{"SYStaussen",0},	/**< Aussentemperatur ganzzahlig [°C] */
	{"SYStvorl",0},		/**< Heizungsvorlauftemperatur ganzzahlig [°C] */
	{"SYStkond",0},		/**< Kondensatorrücklauftemperatur ganzzahlig [°C] */
	{"SYStspeich",0},	/**< Pufferspeichertemperatur ganzzahlig [°C] */
	{"SYSsysZust",0},	/**< Systemkomponentenzustand als Bitfelder */
	{"SYSuser1",0},		/**< Unbelegte Option */
	{"SYSuser2",0}		/**< Unbelegte Option */
};

wpState_t wpState= WP_STATE_IDLE;	/** Switch-Case Variable für WP Betriebszustände. */
//wpReqFunc_t wpReqFunc= WP_REQ_FUNC_IDLE;/** Betriebswahl-Anforderung an WP-Kontrollmodul. */
//reglerState_t reglerState= REGLER_STATE_OFF;	/** Regler Betriebszustand Steuerung. */
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
	// only for debugging
	#ifdef DEBUG_PROGRAM_FLOW
		Serial.print(F("Executed: blinkFunction"));
		Serial.println(F(" ->Modul: Main"));
	#endif
	return;
}

/*****************************************************************************/
/**
* @brief Setup-Funktion welche zum Programmstart einmalig ausgeführt wird.
*
* Initialisiert Pins und externe Interruptquellen sowie weitere
* Schnittstellen. Daneben werden spezifische Initialisierungsfunktion aufgerufen.
*
* @return @c void
*/
/***************************************************************************/
void setup(){
	Serial.begin(115200); // PC
	Serial1.begin(115200); // ESP8266
	Serial1.setTimeout(20); // Serial1 keep connection waiting time
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
pinMode(PIN_FORCE_LOCAL,INPUT_PULLUP);
pinMode(PIN_LED_STBY,OUTPUT);
pinMode(PIN_LED_AUTONORM,OUTPUT);
pinMode(PIN_LED_AUTORED,OUTPUT);
pinMode(PIN_LED_MAN,OUTPUT);
pinMode(PIN_LED_HDND,OUTPUT);
pinMode(PIN_LED_ALARM,OUTPUT);
pinMode(PIN_LED_KEY5,OUTPUT);

setupSteuerIO();
}

/*****************************************************************************/
/**
* @brief Main-Funktion welche ständig wiederholt wird.
*
* Enthält Funktionen welche vom Hauptprogramm direkt aufgerufen werden.
*
* @return @c void
*/
/****************************************************************************/
void loop(){
	userMain();
	// AutoBetrieb normal/reduziert
	//if((Usersettings[1].value==2)||(Usersettings[1].value==3)){
		autoBetrieb();
	//}
	// manueller Betrieb
	//else if(Usersettings[1].value==1){;} // todo manueller Betrieb
	// Standby
//	else if(Usersettings[1].value==3){;}
  blinkFunction();
  delay(2000); //only for debugging needed. (slows down Serialprint data )
}

/*****************************************************************************/
/**
* @brief Funktion, welche den Serial Empfangsbuffer auf neue Daten prüft
*
*	Die Funktion wird jeweils nach Abarbeiten der Funktion loop() aufgerufen.
*	Falls Daten im Empfangsbuffer anstehen, ruft sie receiveSerialData() auf,
*	welche den Buffer ausliest.
*
* @return @c void
*/
/****************************************************************************/
void serialEvent1() {
  if (Serial1.available()>0){
    receiveSerialData();
  }
  // only for debugging
  #ifdef DEBUG_PROGRAM_FLOW
	  Serial.print(F("Executed: serialEvent1"));
	  Serial.println(F(" ->Modul: Main"));
  #endif
}
