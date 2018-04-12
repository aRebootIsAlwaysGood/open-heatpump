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
 *
 *	Die Auswertung des Analogeinganges erfolgt bei Aufruf dieser Funktion.
 *	Der Analog-Digital Wandler erzeugt aus dem Messbereich von 0 bis 5V eine
 *	Zahlenmenge von 0 bis 1023 (10bit ADC). Somit entspricht die Auflösung des ADC
 *	0.0049V = 4.9mV.\n
 *	Der KTY81/210 Messfühler ist als Spannungsteiler-Widerstand zwischen GND und
 *	dem ADC-Eingang angeschlossen, zwischen 5V und ADC ein Festwiderstand von 2k7.
 *	Über den KTY81 fällt somit bei -50°C eine Spannung von 1.38V, bei +100°C
 *	eine Spannung von 2.78V ab. Linearisiert man die Temperatur-Spannungs-
 *	Kennlinie in diesem Bereich mittels Regressionsgeraden, ergeben sich Werte
 *	von 1.41V bei -50°C und 2.82V bei +100°C.\n
 *	Mittels bekannter ADC-Auflösung entspricht -50°C einem linearisierten Wert
 *	von 287 und +100°C einem Rückgabewert von 576.
 *	Durch Einsatz der Funktion map() wird der ADC-Ausgangswert in Grad Celsius
 *	ausgegeben. Die Ausgabe erfolgt als 16bit Integer, demzufolge nur
 *  ganzzahlige Werte.\n
 *	Um genauere Messwerte zu erhalten, erfolgen drei Messungen deren Mittelwert
 *	der neue Messwert wird.
 *
 *	@return	gemessene Aussentemperatur in Grad Celsius (ganzzahlig).
 */
 /************************************************************************/
 int16_t getAussentemp(){
	int16_t adc_val;
	for (uint8_t i = 0; i < 3; i++){   // 3 fache Messung und Mittelung
		adc_val += analogRead(PIN_T_AUSSEN);
	}
	adc_val = adc_val / 3;  // Mittelwert der drei Messungen
	adc_val= map(adc_val,287,576,-50,100);

    // only for debugging Inputvalues
    #ifdef DEBUG_INPUTVALUES
        Serial.print(F("AI: Aussentemp [°C], VALUE: "));
        Serial.println(adc_val);
        Serial.println();
    #endif
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: getAussentemp"));
        Serial.println(F(" ->Modul: Sensorik"));
    #endif
	return adc_val;
 }

  /************************************************************************/
 /**
 *	@brief	Messung der aktuellen Wassertemperatur vor dem Wärmetauscher.
 *
 *	Die Auswertung des Analogeinganges erfolgt bei Aufruf dieser Funktion.
 *	Der Analog-Digital Wandler erzeugt aus dem Messbereich von 0 bis 5V eine
 *	Zahlenmenge von 0 bis 1023 (10bit ADC). Somit entspricht die Auflösung des ADC
 *	0.0049V = 4.9mV. \n
 *	Der KTY81/210 Messfühler ist als Spannungsteiler-Widerstand zwischen GND und
 *	dem ADC-Eingang angeschlossen, zwischen 5V und ADC ein Festwiderstand von 2k7.
 *	Über den KTY81 fällt somit bei -50°C eine Spannung von 1.38V, bei +100°C
 *	eine Spannung von 2.78V ab. Linearisiert man die Temperatur-Spannungs-
 *	Kennlinie in diesem Bereich mittels Regressionsgeraden, ergeben sich Werte
 *	von 1.41V bei -50°C und 2.82V bei +100°C.\n
 *	Mittels bekannter ADC-Auflösung entspricht -50°C einem linearisierten Wert
 *	von 287 und +100°C einem Rückgabewert von 576.
 *	Durch Einsatz der Funktion map() wird der ADC-Ausgangswert in Grad Celsius
 *	ausgegeben. Die Ausgabe erfolgt als 16bit Integer, demzufolge nur ganzzahlige
 *	Werte.\n
 *	Um genauere Messwerte zu erhalten, erfolgen drei Messungen deren Mittelwert
 *	der neue Messwert wird.
 *
 *	@return	gemessene Temperatur des unteren Speicherausgangs in Grad Celsius (ganzzahlig).
 */
 /************************************************************************/
 int16_t getKondenstemp(){
	int16_t adc_val;
	for (uint8_t i = 0; i < 3; i++){   // 3 fache Messung und Mittelung
		adc_val += analogRead(PIN_T_KONDENSATOR);
	}
	adc_val = adc_val / 3;  // Mittelwert der drei Messungen
	adc_val= map(adc_val,287,576,-50,100);

    // only for debugging Inputvalues
    #ifdef DEBUG_INPUTVALUES
        Serial.print(F("AI: Kondensatortemp [°C], VALUE: "));
        Serial.println(adc_val);
        Serial.println();
    #endif
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: getKondenstemp"));
        Serial.println(F(" ->Modul: Sensorik"));
    #endif
	return adc_val;
 }

  /************************************************************************/
 /**
 *	@brief	Messung der aktuellen Pufferspeichertemperatur in Grad Celsius.
 *
 *	Die Auswertung des Analogeinganges erfolgt bei Aufruf dieser Funktion.
 *	Der Analog-Digital Wandler erzeugt aus dem Messbereich von 0 bis 5V eine
 *	Zahlenmenge von 0 bis 1023 (10bit ADC). Somit entspricht die Auflösung des ADC
 *	0.0049V = 4.9mV. \n
 *	Der KTY81/210 Messfühler ist als Spannungsteiler-Widerstand zwischen GND und
 *	dem ADC-Eingang angeschlossen, zwischen 5V und ADC ein Festwiderstand von 2k7.
 *	Über den KTY81 fällt somit bei -50°C eine Spannung von 1.38V, bei +100°C
 *	eine Spannung von 2.78V ab. Linearisiert man die Temperatur-Spannungs-
 *	Kennlinie in diesem Bereich mittels Regressionsgeraden, ergeben sich Werte
 *	von 1.41V bei -50°C und 2.82V bei +100°C.\n
 *	Mittels bekannter ADC-Auflösung entspricht -50°C einem linearisierten Wert
 *	von 287 und +100°C einem Rückgabewert von 576.
 *	Durch Einsatz der Funktion map() wird der ADC-Ausgangswert in Grad Celsius
 *	ausgegeben. Die Ausgabe erfolgt als 16bit Integer, demzufolge nur ganzzahlige
 *	Werte.\n
 *	Um genauere Messwerte zu erhalten, erfolgen drei Messungen deren Mittelwert
 *	der neue Messwert wird.
 *
 *	@return	gemessene Speichertemperatur in Grad Celsius (ganzzahlig).
 */
 /************************************************************************/
 int16_t getSpeichertemp(){
	int16_t adc_val;
	for (uint8_t i = 0; i < 3; i++){   // 3 fache Messung und Mittelung
		adc_val += analogRead(PIN_T_SPEICHER);
	}
	adc_val = adc_val / 3;  // Mittelwert der drei Messungen
	adc_val= map(adc_val,287,576,-50,100);

    // only for debugging Inputvalues
    #ifdef DEBUG_INPUTVALUES
        Serial.print(F("AI: Speichertemp [°C], VALUE: "));
        Serial.println(adc_val);
        Serial.println();
    #endif
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: getSpeichertemp"));
        Serial.println(F(" ->Modul: Sensorik"));
    #endif
	return adc_val;
 }

  /************************************************************************/
 /**
 *	@brief	Messung der  aktuellen Heizungs-Vorlauftemperatur in Grad Celsius.
 *
 *	Die Auswertung des Analogeinganges erfolgt bei Aufruf dieser Funktion.
 *	Der Analog-Digital Wandler erzeugt aus dem Messbereich von 0 bis 5V eine
 *	Zahlenmenge von 0 bis 1023 (10bit ADC). Somit entspricht die Auflösung des ADC
 *	0.0049V = 4.9mV. \n
 *	Der KTY81/210 Messfühler ist als Spannungsteiler-Widerstand zwischen GND und
 *	dem ADC-Eingang angeschlossen, zwischen 5V und ADC ein Festwiderstand von 2k7.
 *	Über den KTY81 fällt somit bei -50°C eine Spannung von 1.38V, bei +100°C
 *	eine Spannung von 2.78V ab. Linearisiert man die Temperatur-Spannungs-
 *	Kennlinie in diesem Bereich mittels Regressionsgeraden, ergeben sich Werte
 *	von 1.41V bei -50°C und 2.82V bei +100°C.\n
 *	Mittels bekannter ADC-Auflösung entspricht -50°C einem linearisierten Wert
 *	von 287 und +100°C einem Rückgabewert von 576.
 *	Durch Einsatz der Funktion map() wird der ADC-Ausgangswert in Grad Celsius
 *	ausgegeben. Die Ausgabe erfolgt als 16bit Integer, demzufolge nur ganzzahlige
 *	Werte.\n
 *	Um genauere Messwerte zu erhalten, erfolgen drei Messungen deren Mittelwert
 *	der neue Messwert wird.
 *
 *	@return	gemessene Vorlauftemperatur nach dem Mischer in Grad Celsius (ganzzahlig).
 */
 /************************************************************************/
 int16_t getVorlauftemp(){
	int16_t adc_val;
	for (uint8_t i = 0; i < 3; i++){   // 3 fache Messung und Mittelung
		adc_val += analogRead(PIN_T_VORLAUF);
	}
	adc_val = adc_val / 3;  // Mittelwert der drei Messungen
	adc_val= map(adc_val,287,576,-50,100);

    // only for debugging Inputvalues
    #ifdef DEBUG_INPUTVALUES
        Serial.print(F("AI: Vorlauftemp [°C], VALUE: "));
        Serial.println(adc_val);
        Serial.println();
    #endif
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: getVorlauftemp"));
        Serial.println(F(" ->Modul: Sensorik"));
    #endif
	return adc_val;
 }
