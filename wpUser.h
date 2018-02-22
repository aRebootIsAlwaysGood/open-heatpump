/**
 *  @file wpUser.h
 *  @brief Header zum Modul Benutzersteuerung.
 *
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

#ifndef WPUSER_H
#define WPUSER_H

/**
*		@brief Definition des Strukturtyps welcher Benutzer-Werte oder -Infos enthält.
*		Definiert den Strukturtyp SETTINGS in welchem über die serielle
*		Schnittstelle übertragene Befehle als Namens- und Wertfeld
*		ein- und ausgelesen werden können. Die Nachrichten werden mit dem Namensfeld
*		@c action vom Empfänger identifiziert und können dadurch den gesendeten
*		Wert im Feld @c value entsprechend zuordnen. Strukturvariablen dieses Typs
*		übernehmen eine doppelte Funktion. Sie speichern einersets Einstellungwerte
*		global im Feld value und dienen anderseits in Kombination mit dem Feld action
*		auch gleich dazu, Einstellungen über eine Kommunikationsschnittstelle
*		auszulesen resp. zu verändern.
*/
struct SETTINGS
{
	const char * action; /**< C-String Konstante mit Befehlsname zur Identifizierung */
	int16_t value;	/**< Wertfeld des Datensatzes, positive oder negative Werte */
};

void receiveSerialData();
void transmitSerialData(uint8_t settingindex);
void combineZustandbits();
int8_t getKurvenstufe();
int8_t getParallelvs();
int8_t modeReduziert();
int8_t readLocalParallelvsRed();
int8_t readLocalParallelvsNorm();
int8_t readLocalKurvenstufe();
int8_t forceLocalBedienung(int8_t reqForce= 0);
int8_t getBetriebsmodus();
void userMain();
#endif
