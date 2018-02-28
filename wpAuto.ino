/**
 *  @file wpAuto.ino
 *  @brief Automatikbetrieb der Wärmepumpe.
 *
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

 #include "wpAuto.h"

/************************************************************************/
/**
*   @brief Ruft die für den automatischen Betrieb nötigen Funktionen auf.
*
*   Mittels der Funktion autoBetrieb() kann der automatische Betrieb
*   gestartet werden. Die Funktion ruft die entsprechend nötigen, weiteren
*   Funktionen und Abläufe auf. Davon ausgenommen sind Funktionen zur
*   Benutzersteuerung welche auf globaler Ebene gesteuert werden.
*/
/************************************************************************/
 void autoBetrieb(){

	 float vorlaufsoll= calcTvorlauf(getAussentemp(), getKurvenstufe(),getParallelvs(),modeReduziert());
	 speicherladung(vorlaufsoll);
 }

 
