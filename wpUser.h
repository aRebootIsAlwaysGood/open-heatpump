/**
 *  @file wpUser.h
 *  @brief Header zum Modul Benutzersteuerung.
 *
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

#ifndef WPUSER_H
#define WPUSER_H

/** Struct Datentyp in welchem über die serielle Schnittstelle übertragene Werte
*		ein- und ausgelesen werden können. Die Optionen werden mit dem Parameter action
*		vom Empfänger identifiziert. */
struct SETTINGS
{
	const char * action; /**< C-String Konstante mit Optionsname */
	int16_t value;	/**< Wert der Option, positive oder negative Werte */
};

void receiveSerialData();
void transmitSerialData(uint8_t settingindex);
void combineZustandbits();
int8_t getKurvenstufe();
int8_t getParallelvs();
int8_t modeReduziert();

#endif
