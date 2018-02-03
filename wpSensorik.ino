/**
 *  @file wpSensorik.ino
 *  @brief Auswertung der Analogsignale der Temperatursensoren.
 *   
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

 #include "wpSensorik.h"

 /************************************************************************/
 /** 
 *	@brief	Messung der aktuellen Aussentemperatur in Grad Celsius.
 *			Die Auswertung des Analogeinganges erfolgt bei Aufruf dieser Funktion.
 *			Der Analog-Digital Wandler erzeugt aus dem Messbereich von 0 bis 5V eine
 *			Zahlenmenge von 0 bis 1023 (10bit ADC). Somit entspricht die Auflösung des ADC
 *			0.0049V = 4.9mV. 
 *			Der KTY81/210 Messfühler ist als Spannungsteiler-Widerstand zwischen GND und
 *			dem ADC-Eingang angeschlossen, zwischen 5V und ADC ein Festwiderstand von 2k7.
 *			Über den KTY81 fällt somit bei -50°C eine Spannung von 1.38V, bei +100°C
 *			eine Spannung von 2.78V ab. Linearisiert man die Temperatur-Spannungs-
 *			Kennlinie in diesem Bereich mittels Regressionsgeraden, ergeben sich Werte
 *			von 1.41V bei -50°C und 2.82V bei +100°C.
 *			Mittels bekannter ADC-Auflösung entspricht -50°C einem linearisierten Wert
 *			von 287 und +100°C einem Rückgabewert von 576. 
 *			Durch Einsatz der Funktion map() wird der ADC-Ausgangswert in Grad Celsius
 *			ausgegeben und mittels char() in eine vorzeichenbehaftete 8bit Ganzzahl
 *			übersetzt.
 *
 *	@return	gemessene Aussentemperatur in Grad Celsius (ganzzahlig).
 */
 /************************************************************************/
 int8_t getAussentemp(){
	int16_t sensorval= analogRead(PIN_T_AUSSEN);
	sensorval= map(sensorval,287,576,-50,100);
	return char(sensorval);
 }

  /************************************************************************/
 /** 
 *	@brief	Messung der aktuellen Wassertemperatur nach dem Wärmetauscher.
 *			Die Auswertung des Analogeinganges erfolgt bei Aufruf dieser Funktion.
 *			Der Analog-Digital Wandler erzeugt aus dem Messbereich von 0 bis 5V eine
 *			Zahlenmenge von 0 bis 1023 (10bit ADC). Somit entspricht die Auflösung des ADC
 *			0.0049V = 4.9mV. 
 *			Der KTY81/210 Messfühler ist als Spannungsteiler-Widerstand zwischen GND und
 *			dem ADC-Eingang angeschlossen, zwischen 5V und ADC ein Festwiderstand von 2k7.
 *			Über den KTY81 fällt somit bei -50°C eine Spannung von 1.38V, bei +100°C
 *			eine Spannung von 2.78V ab. Linearisiert man die Temperatur-Spannungs-
 *			Kennlinie in diesem Bereich mittels Regressionsgeraden, ergeben sich Werte
 *			von 1.41V bei -50°C und 2.82V bei +100°C.
 *			Mittels bekannter ADC-Auflösung entspricht -50°C einem linearisierten Wert
 *			von 287 und +100°C einem Rückgabewert von 576. 
 *			Durch Einsatz der Funktion map() wird der ADC-Ausgangswert in Grad Celsius
 *			ausgegeben und mittels char() in eine vorzeichenbehaftete 8bit Ganzzahl
 *			übersetzt.
 *
 *	@return	gemessene Kondensatorrücklauf-Temperatur in Grad Celsius (ganzzahlig).
 */
 /************************************************************************/
 int8_t getKondenstemp(){
	int16_t sensorval= analogRead(PIN_T_KONDENSATOR);
	sensorval= map(sensorval,287,576,-50,100);
	return char(sensorval);
 }

  /************************************************************************/
 /** 
 *	@brief	Messung der aktuellen Pufferspeichertemperatur in Grad Celsius.
 *			Die Auswertung des Analogeinganges erfolgt bei Aufruf dieser Funktion.
 *			Der Analog-Digital Wandler erzeugt aus dem Messbereich von 0 bis 5V eine
 *			Zahlenmenge von 0 bis 1023 (10bit ADC). Somit entspricht die Auflösung des ADC
 *			0.0049V = 4.9mV. 
 *			Der KTY81/210 Messfühler ist als Spannungsteiler-Widerstand zwischen GND und
 *			dem ADC-Eingang angeschlossen, zwischen 5V und ADC ein Festwiderstand von 2k7.
 *			Über den KTY81 fällt somit bei -50°C eine Spannung von 1.38V, bei +100°C
 *			eine Spannung von 2.78V ab. Linearisiert man die Temperatur-Spannungs-
 *			Kennlinie in diesem Bereich mittels Regressionsgeraden, ergeben sich Werte
 *			von 1.41V bei -50°C und 2.82V bei +100°C.
 *			Mittels bekannter ADC-Auflösung entspricht -50°C einem linearisierten Wert
 *			von 287 und +100°C einem Rückgabewert von 576. 
 *			Durch Einsatz der Funktion map() wird der ADC-Ausgangswert in Grad Celsius
 *			ausgegeben und mittels char() in eine vorzeichenbehaftete 8bit Ganzzahl
 *			übersetzt.
 *
 *	@return	gemessene Speichertemperatur in Grad Celsius (ganzzahlig).
 */
 /************************************************************************/
 int8_t getSpeichertemp(){
	int16_t sensorval= analogRead(PIN_T_SPEICHER);
	sensorval= map(sensorval,287,576,-50,100);
	return char(sensorval);
 }

  /************************************************************************/
 /** 
 *	@brief	Messung der  aktuellen Heizungs-Vorlauftemperatur in Grad Celsius.
 *			Die Auswertung des Analogeinganges erfolgt bei Aufruf dieser Funktion.
 *			Der Analog-Digital Wandler erzeugt aus dem Messbereich von 0 bis 5V eine
 *			Zahlenmenge von 0 bis 1023 (10bit ADC). Somit entspricht die Auflösung des ADC
 *			0.0049V = 4.9mV. 
 *			Der KTY81/210 Messfühler ist als Spannungsteiler-Widerstand zwischen GND und
 *			dem ADC-Eingang angeschlossen, zwischen 5V und ADC ein Festwiderstand von 2k7.
 *			Über den KTY81 fällt somit bei -50°C eine Spannung von 1.38V, bei +100°C
 *			eine Spannung von 2.78V ab. Linearisiert man die Temperatur-Spannungs-
 *			Kennlinie in diesem Bereich mittels Regressionsgeraden, ergeben sich Werte
 *			von 1.41V bei -50°C und 2.82V bei +100°C.
 *			Mittels bekannter ADC-Auflösung entspricht -50°C einem linearisierten Wert
 *			von 287 und +100°C einem Rückgabewert von 576. 
 *			Durch Einsatz der Funktion map() wird der ADC-Ausgangswert in Grad Celsius
 *			ausgegeben und mittels char() in eine vorzeichenbehaftete 8bit Ganzzahl
 *			übersetzt.
 *
 *	@return	gemessene Vorlauftemperatur nach dem Mischer in Grad Celsius (ganzzahlig).
 */
 /************************************************************************/
 int8_t getVorlauftemp(){
	int16_t sensorval= analogRead(PIN_T_VORLAUF);
	sensorval= map(sensorval,287,576,-50,100);
	return char(sensorval);
 }



