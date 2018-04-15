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
/*****************************************************************************/

#include "wpRegler.h"

/*****************************************************************************/
/**
 *  @brief Array mit Steigung (m) und y-Verschiebung(n) der Heizkurvenstufen 1-11.
 *
 *  Array des Typs HEIZKURVE. Enthält die Parameter m und n der
 *  Heizkurvenfunktion vergrössert um den Faktor 100 um Gleitpunktzahlen
 *  zu vermeiden. Die 11 Arrayelemente entsprechen den 11 Stufen
 *  der Heizkurve.
 *  Die Berechnung der Heizkurve resp. der Vorlauftemperatur erfolgt in
 *  der Funktion calcTvorlauf().
 */
 /****************************************************************************/
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

/*****************************************************************************/
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
 *  Steigung und Verschiebung in y-Richtung sind im Array #heizkurve definiert.
 */
 /****************************************************************************/
float calcTvorlauf(int16_t tAussen, int8_t kurvenstufe, int8_t parallelver, int8_t reduziert){
	// function of Tvorlauf, calculate with integer math instead of float to increase speed
	int16_t tvorlauf= heizkurve[kurvenstufe].steigung *tAussen +heizkurve[kurvenstufe].verschiebung +(parallelver *100);
	if(reduziert){tvorlauf -=1000;}
	tvorlauf += parallelver*100; // add parallelverschiebung (1K/ Stufe)
	tvorlauf= constrain(tvorlauf, 2200, 4500); // limits the output (Tvorlauf) between 22 to 45°C
	// only for debugging
	#ifdef DEBUG_PROGRAM_FLOW
		Serial.print(F("Executed: calcTvorlauf"));
		Serial.println(F(" ->Modul: Regler"));
	#endif
	#ifdef DEBUG_REGLER
		Serial.print(F("REGLER: ber. Vorlauftemp: "));
		Serial.println(tvorlauf/100.0,DEC);
	#endif
	return (tvorlauf /100.0);					// divide trough 100 to return value in °C, treat 100 as floating constant
}


/************************************************************************/
/**
*	@brief	Zustandsautomat für den Heizungsvorlauf-Temperaturregler.
*
*	Der Regler kann verschiedene Zustände annehmen, welche die stetige Vorlauf-
*	Temperaturregelung beeinflussen beziehungsweise diese ausser Kraft setzen.\n
*	Im Zustand OFF wird der Regler ganz ausgeschalten, beispielsweise im Sommer.\n
*	Im Zustand AUTO wird die Vorlauftemperatur stetig geregelt.\n
*	Im Zustand MANUAL nimmt der Regler keinen Einfluss auf seine Steuerausgänge
*	(Mischer auf/zu und Heizkreislaufpumpe). Somit können diese von anderen
*	Modulen angesteuert werden ohne dass es zu Konflikten kommt.\n
*	Die Zustände LADEN und ENTEISEN initialisieren den Regler gegebenfalls mit
*	neuen Regelparametern, falls es durch die schnellen Temperaturwechsel zum
*	instabilen Regelverhalten kommt.
*
*	@param	[in] reglerState Variable für die Anwahl des Reglerzustandes.
*/
/************************************************************************/
void reglerStatemachine(reglerState_t reglerState){
  float sollwert= 0.0;
	switch (reglerState)
	{
	case REGLER_STATE_OFF:
		digitalWrite(PIN_HEIZPUMPE,LOW);
		digitalWrite(PIN_MISCHER_ZU,LOW);
		digitalWrite(PIN_MISCHER_AUF,LOW);
		Systemzustand.vorlaufregler= 0; // update Regler Systemzustandsbit
		break;

	case REGLER_STATE_AUTO:
		sollwert= calcTvorlauf(getAussentemp(), getKurvenstufe(), getParallelvs(), modeReduziert());
		tristateRegler(sollwert, getVorlauftemp());
		digitalWrite(PIN_HEIZPUMPE,HIGH);
		Systemzustand.vorlaufregler= 1; // update Regler Systemzustandsbit
		break;

	case REGLER_STATE_MANUAL:
		// do nothing, OUTPUTs overrriden in wpStatemachine
		Systemzustand.vorlaufregler= 0; // update Regler Systemzustandsbit
		break;

	case REGLER_STATE_LADEN:
		// unused case
		digitalWrite(PIN_HEIZPUMPE,HIGH);
		Systemzustand.vorlaufregler= 1; // update Regler Systemzustandsbit
		break;

	case REGLER_STATE_DEFROST:
		// unused case
		Systemzustand.vorlaufregler= 1; // update Regler Systemzustandsbit
		break;
	}
	// only for debugging
	#ifdef DEBUG_PROGRAM_FLOW
		Serial.print(F("Executed: reglerStatemachine"));
		Serial.println(F(" ->Modul: Regler"));
	#endif
	#ifdef DEBUG_REGLER
		Serial.print(F("REGLER: Sollwert: "));
		Serial.println(sollwert);
		Serial.print(F("REGLER: Statemachine: "));
		Serial.println(reglerState);
	#endif
}

/************************************************************************/
/**
*   @brief Reinitialisieren des Dreipunktreglers.
*
*   Diese Funktion initialisiert den Regler und setzt die internen Parameter
*	zurück. Nach starken Temperaturschwankungen des Vorlaufs, beispielsweise
*	nach Aufladen des Speichers, sollte eine Reinitialisierung vorgenommen
*	werden.
*/
/************************************************************************/
void initRegler(){
	tristateRegler(0,0);
	// only for debugging
	#ifdef DEBUG_PROGRAM_FLOW
		Serial.print(F("Executed: initRegler"));
		Serial.println(F(" ->Modul: Regler"));
	#endif
}

/************************************************************************/
/**
*   @brief Quasi-stegiger Dreipunktregler für Dreiweg-Mischventil.
*
*	Diese Funktion beinhaltet einen Dreipunktregler mit quasi-stetigem
*	Regelverhalten. Dieser Reglertyp ist zur Ansteuerung von elektrischen
*	Stellantrieben geeignet, welche mit konstanter Stellgeschwindigkeit
*	in beide Richtungen fahren können.\n
*	Hauptmerkmal dieses Reglers ist die
*	Rückführung einer prognostizierten Regelabweichung (ep) welche noch gar
*	nicht an der Regelstrecke auftritt. Dadurch kann ein vielfach schnelleres
*	Ausregeln bei langsam reagierenden Systemen erreicht werden.\n
*	Die Funktion arbeitet mit Gleitpunktarithmetik was sie genau jedoch langsam
*	macht. Reinitialisieren erfolgt durch Aufruf mit den Argumenten @c w = 0
*	 und @c x = 0 oder einfacher durch Aufruf der Funktion initRegler().
*
*   @param w Sollwert auf welchen geregelt wird in °C.
*
*   @param x Regelgrösse, also die gemessene Vorlauftemperatur in °C.
*
*   @param kp Übertragungsbeiwert Rückführung optionaler Parameter
*
*   @param e_min Ansprechempfindlichkeit (entspricht Totzone/2) in °C, optional.
*
*   @param hyst Hysterese an den Totzonengrenzen in °C, optional.
*
*   @param tn Rückführzeitkonstante in Sekunden, optional.
*
*	@note	Falls die Funktion ohne die optionalen Parameter aufgerufen wird,
*			werden folgende Standardwerte eingesetzt:
*			- kp: 	0.35
*			- e_min: 0,4°C
*			- hyst: 0,2°C
*			- tn:	1,0s
*/
/************************************************************************/
void tristateRegler(float w,float x,float kp,float e_min, float hyst, float tn){
	float e= 0.0;	/**< Regelabweichung */
	float ep= 0.0; /**< prognostizierte Regelabweichung */
	float yr= 0.0; /**< Rückführwert */
	static float ty; /**< aktuelle Laufzeit Motor in s */
	static float ep_old; /**< letzte berechnete Abweichung */
	static uint32_t lastcalctime; // Zeitpunkt letzte Berechnung

	// set w & x to 0 will reinitialize the controller to zero
	if(w==0 && x==0){
		ty=0.0;
		ep_old=0.0;
		lastcalctime=0;
		digitalWrite(PIN_MISCHER_ZU, LOW);
		digitalWrite(PIN_MISCHER_AUF, LOW);
	}

	// normal controller loop
	else if((millis()-lastcalctime)>(tn*1000)){
		e= w-x; // berechne Regelabweichung
		yr= (kp*ty)/(ty+tn); // berechne Rückführwert. kp und tn konstant!
		ep= e-yr; // berechne Prognosewert Regelabweichung mittels Rückführwert

		// Hysterese Totzonengrenze (Ansprechempfindlichkeit e_min verändern)
		if(abs(ep)>abs(ep_old)){e_min+=hyst;} //Regelabw steigt->e_min vergr
		else if (abs(ep)<abs(ep_old)) {e_min-=hyst;} //Abwsinkt->e_min verkl.

		// Regelabweichung ausserhalb Totzone/2 (e_min) und Hysterese
		if(abs(ep)>e_min){
			ty += tn; // zähle Laufzeit hoch
			// öffne oder schliesse Mischventil
			if(ep< 0.0){ // wenn Abw negativ, ist Vorlauf > Soll: schliessen
				digitalWrite(PIN_MISCHER_ZU, HIGH);
				digitalWrite(PIN_MISCHER_AUF, LOW);
			}
			else{ // falls Abw positivm ist Vorlauf < Soll: Ventil öffnen
				digitalWrite(PIN_MISCHER_AUF, HIGH);
				digitalWrite(PIN_MISCHER_ZU, LOW);
			}
		}
		// Regelabw innerhalb Totzone, Ausgang 0
		else{
			digitalWrite(PIN_MISCHER_ZU, LOW);
			digitalWrite(PIN_MISCHER_AUF, LOW);
			if(ty > 0.0){ty -=tn;} // Ziehe Laufzeit ab
		}
		ep_old=ep; // speichere berechnete Abweichung vor erneuter Berechnung
		lastcalctime=millis();
	}
	// only for debugging
	#ifdef DEBUG_PROGRAM_FLOW
		Serial.print(F("Executed: tristateRegler"));
		Serial.println(F(" ->Modul: Regler"));
	#endif
	#ifdef DEBUG_REGLER
		Serial.print(F("REGLER: Regelabweichung: "));
		Serial.println(ep);
		Serial.print(F("REGLER: Motorlaufzeit: "));
		Serial.println(ty/1000);
	#endif
}
