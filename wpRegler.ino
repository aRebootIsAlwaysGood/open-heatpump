/**
* @file 	wpRegler.ino
*
* @brief 	Enthält die Heizkurvenfunktion und Vorlauftemperaturregelung.
*
* @details 	Für die Regelung der Bodenheizungs-Vorlauftemperatur muss das
* Verhältnis zwischen Warmwasser aus dem Speicher und kälterem
* Heizungsrücklauf durch einen 3-Wege Mischer eingestellt werden.
* Hierzu verwendet man einen stetigen Regler, welcher die Isttemperatur des
* Vorlaufs mit dem eingestellten Sollwert vergleicht und die Differenz
* ausregelt, ohne ins Schwingen zu geraten.
* Der Vorlauftemperatur-Sollwert wiederum ist witterungsgeführt, also
* abhängig von der Aussentemperatur. Weiter spielt die gewünschte Heizstufe
* eine Rolle. Diese Parameter werden in die Heizkurvenfunktion als Argumente
* eingegeben, welche daraus den spezifischen Vorlaufstemperatur-Sollwert
* errechnet.
* Die Ansteuerung des Mischers erfolgt durch 1Hz lange Pulse um ein
* Übersteuern zu vermeiden.
*/
/***************************************************************************************/

#include "wpRegler.h"

/***************************************************************************************/
/**
 *  @brief Array mit Steigung (m) und y-Verschiebung(n) der Heizkurvenstufen 1-11.
 *  Array des Typs HEIZKURVE. Enthält die Parameter m und n der
 *  Heizkurvenfunktion vergrössert um den Faktor 100 um Gleitpunktzahlen
 *  zu vermeiden. Die 11 Arrayelemente entsprechen den 11 Stufen
 *  der Heizkurve.
 *  Die Berechnung der Heizkurve resp. der Vorlauftemperatur erfolgt in
 *  der Funktion calcTvorlauf().
 */
 /***************************************************************************************/
struct HEIZKURVE heizkurve[11]=
{
	{-25,2550}, // Stufe 1
	{-44,2990}, // Stufe 2
	{-61,3420}, // Stufe 3
	{-83,3830}, // Stufe 4
	{-103,4280}, // Stufe 5
	{-118,4630}, // Stufe 6
	{-138,5010}, // Stufe 7
	{-156,5400}, // Stufe 8
	{-176,5820}, // Stufe 9
	{-192,6190}, // Stufe 10
	{-229,7020}, // Stufe 11
};

/***************************************************************************************/
/**
 *  @brief Berechnet Solltemperatur des Heizungsvorlauf mittels Heizkurve.
 *
 *  @param [in] tAussen     gemessene Aussenlufttemperatur als 8bit Ganzzahlwert.
 *  @param [in] kurvenstufe eingestellte Stufe der Heizkurve. 8bit positive Ganzzahl.
 *  @param [in] parallelver Eingestellte Parallelverschiebung der Vorlauftemperatur. 8bit Ganzzahl.
 *  @param [in] reduziert   Heizungsmodus mit 0= Normalbetrieb, 1= reduziert. 8bit Ganzzahl.
 *  @return Ausgabe der berechneten Solltemperatur des Vorlaufs als Gleitpunktzahl.
 *
 *  @details Berechnung des Vorlaufstemperatur-Sollwert abhängig von der aktuellen
 *  Aussenlufttemperatur, eingestellter Heizkurvenstufen, Parallelverschiebung und Betriebsmodus.
 *  Die Berechnung erfolgt mittels Ganzzahl-Arithmetik um den Speicherbedarf gering und
 *  die Rechenzeit kurz zu halten. Die Argumente werden deshalb um den Faktor 100 erhöht
 *  und erst nach erfolgter Berechnung wieder verkleinert.
 *  Steigung und Verschiebung in y-Richtung sind im Array #heizkurve definiert
 */
 /***************************************************************************************/
float calcTvorlauf(int16_t tAussen, int8_t kurvenstufe, int8_t parallelver, int8_t reduziert){
	// function of Tvorlauf, calculate with integer math instead of float to increase speed
	int16_t tvorlauf= heizkurve[kurvenstufe].steigung *tAussen +heizkurve[kurvenstufe].verschiebung +(parallelver *100);
	if(reduziert){tvorlauf -=1000;}
	tvorlauf += parallelver*100; // add parallelverschiebung (1K/ Stufe)
	tvorlauf= constrain(tvorlauf, 2200, 4500); // limits the output (Tvorlauf) between 22 to 45°C
	return (tvorlauf /100.0);					// divide trough 100 to return value in °C, treat 100 as floating constant
}


/************************************************************************/
/**
*	@brief	Zustandsautomat für den Heizungsvorlauf-Temperaturregler.
*			Der Regler kann verschiedene Zustände annehmen, welche die stetige Vorlauf-
*			Temperaturregelung beeinflussen beziehungsweise diese ausser Kraft setzen.
*			Im Zustand OFF wird der Regler ganz ausgeschalten, beispielsweise im Sommer.
*			Im Zustand AUTO wird die Vorlauftemperatur stetig geregelt.
*			Im Zustand MANUAL nimmt der Regler keinen Einfluss auf seine Steuerausgänge
*			(Mischer auf/zu und Heizkreislaufpumpe). Somit können diese von anderen
*			Modulen angesteuert werden ohne dass es zu Konflikten kommt.
*			Die Zustände LADEN und ENTEISEN initialisieren den Regler gegebenfalls mit
*			neuen Regelparametern, falls es durch die schnellen Temperaturwechsel zum
*			instabilen Regelverhalten kommt.
*
*	@param	[in] reglerState Variable für die Anwahl des Reglerzustandes.
*/
/************************************************************************/
void reglerStatemachine(reglerState_t reglerState){

	switch (reglerState)
	{
	case REGLER_STATE_OFF:

		break;

	case REGLER_STATE_AUTO:

		break;

	case REGLER_STATE_MANUAL:

		break;

	case REGLER_STATE_LADEN:

		break;

	case REGLER_STATE_DEFROST:

		break;

	}
}

void tristateRegler(){
	
}
