/**
 *  @file 	wpRegler.h
 *  @brief 	Header-Datei zum Modul Regler mit Datenstrukturen.
 *  
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

#ifndef WPREGLER_H
#define WPREGLER_H
/***************************************************************************************/
/**
 *  @brief Datenstruktur zur Erzeugung des Arrays mit Heizkurven-Steigung und Verschiebung.
 *  
 *  Mittels dieser Datenstruktur können die Steigung und y-Verschiebung der Heizkurvenfunktion
 *  für jede Stufe einzeln als Element abgelegt werden. Die Werte sind zuvor mit dem Faktor 100
 *  zu multiplizieren, damit keine Gleitpunktzahlen nötig sind. 
 */
/***************************************************************************************/
struct HEIZKURVE{
	int16_t steigung;		/**< Steigung m um Faktor 100 vergrössert. */
	int16_t verschiebung;	/**< Verschiebung in y-Richtung um Faktor 100 vergrössert. */
};

/** Typedef für Regler Betriebszustand Steuerung */
typedef enum REGLER_STATE{
	REGLER_STATE_OFF,
	REGLER_STATE_AUTO,
	REGLER_STATE_MANUAL, // nichts machen, Ausgänge extern ansteuerbar
	REGLER_STATE_LADEN,
	REGLER_STATE_DEFROST,
}reglerState_t;





float calcTvorlauf(int8_t tAussen, uint8_t kurvenstufe, int8_t parallelver, int8_t reduziert);

void reglerStatemachine(reglerState_t reglerState);







#endif