/**
 *  @file wpAuto.ino
 *  @brief Automatikbetrieb der Wärmepumpe.
 *
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

 #include "wpAuto.h"


 void autoBetrieb(){
     Systemzustand.autobetrieb= 1;
     Systemzustand.manbetrieb= 0;
	 float vorlaufsoll= calcTvorlauf(getAussentemp(), getKurvenstufe(),getParallelvs(),modeReduziert());

	 speicherladung(vorlaufsoll);
 }
