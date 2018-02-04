/**
 *  @file wpSpeicherladung.ino
 *  @brief Aufladen des Pufferspeichers.
 *  
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

 #include "wpSpeicherladung.h"

// Stufenladung Speicher monovalent RAVEL Empfehlung: Siehe RAVEL Standardschaltungen S16
// Temperaturdiff über Verflüssiger = 0.5...0.7*Tempdiff VL/RL im Auslegepunkt(45-35=10°C)
// -> Ausschalten WP wenn Tspeicher - Tkondensatorrücklauf < 5...7°C
// 
// Einstellregeln:
// 1. Speichertemp bei WP-Start(Einschalttemp) <= Speichertemp bei WP-Stop(Ausschalttemp)
// 2. Ausschalttemp <= max. zul. Verflüssiger-Austrittstemp - max Tempdiff über Verfl.
// 3. Bedingung: Einschalttemp >= max. auftretende Rücklauftemp
// 
// -Festlegen max zul. Verflüssigeraustrittstemp auf <=50°C (R502)
// -Vor-&Nachlauf Ladepumpe normalerweise unnötig
// -Tempdiff über Verbraucher: 10K

 void speicherladung(float t_vorlaufsoll){
	 
	 static uint32t laufzeit;
	 
	 int16_t t_vorlauf= int(t_vorlaufsoll); // Umwandlung Float zu signed int	 
	 int16_t tmax_ruecklauf= t_vorlauf;
	 int16_t tspeicher= getSpeichertemp();
	 int16_t tkondens= getKondenstemp();
	 
	 // Einschaltbedingung (Tspeicher > Tmax_rücklauf) festlegen
	 if (t_vorlauf>40){
		 tmax_ruecklauf -= 5;
	 }
	 else if (t_vorlauf>36){
		 tmax_ruecklauf -= 4;
	 }	 	 
	 else if (t_vorlauf>32){
		 tmax_ruecklauf -= 3;
	 }	
	 else if (t_vorlauf>28){
		 tmax_ruecklauf -= 2;
	 }		
	 else if (t_vorlauf>22){
		 tmax_ruecklauf -= 1;
	 }	 
	else{
		;
	}
	
	// Einschaltbedingung prüfen
	if ((tspeicher- tmax_ruecklauf)< 1){
		
		wpStatemachine(WP_REQ_FUNC_LADEN);
	}
	 
	 // Ausschaltbedingung (Speicher bis unten warm)
	 // Bedingung 1: Speichertemp > 15°C ansonsten zu Beginn Heizperiode ständiges aus
	 if ((tspeicher> 15) && (tspeicher-tkondens)<= 6){
		 wpStatemachine(WP_REQ_FUNC_HALT);
	 }
	 
	 // max Verflüssigertemp - 0.5*Tempdiff über Verflüssiger Auslegepunkt zu hoch
	 if ((tspeicher>= 45) || (tkondens >= 45)){
		  wpStatemachine(WP_REQ_FUNC_HALT);
	 }
	 
	 
	
 }