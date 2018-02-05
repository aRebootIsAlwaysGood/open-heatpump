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
//	ND: 2bar= -19°C, HD: 26bar= 60°C
// -Vor-&Nachlauf Ladepumpe normalerweise unnötig
// -Tempdiff über Verbraucher: 10K

 void speicherladung(float t_vorlaufsoll){
	 
	 static uint32_t laufzeit;
	 uint32_t defrostzeit= 180000; // Enteisungszeitdauer abhängig von Aussentemp
	 int16_t t_vorlauf= int(t_vorlaufsoll); // Umwandlung Float zu signed int	 
	 int16_t tmax_ruecklauf= t_vorlauf;
	 int16_t tspeicher= getSpeichertemp();
	 int16_t tkondens= getKondenstemp();
	 int16_t taussen= getAussentemp();
	 
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
	else{
		 tmax_ruecklauf -= 1;
	 }
 
	// setze Enteisungsdauer fest
	if (taussen < T_DEFROST_REQUIRED){
		if (taussen < 7){
			defrostzeit= 180000; // 3min
		}
		else if (taussen < 2){
			defrostzeit= 360000; // 6min
		}
		else{
			defrostzeit= 720000;	// 12min
		}
	}	
	
	switch (ladenState)
		{
		case LADEN_STATE_IDLE:
			// Einschaltbedingung prüfen(Speichertemp höher als Rücklaufmax + über 20°C)
			 if (((tspeicher- tmax_ruecklauf)<= 1) || (tspeicher< 20)){
				 laufzeit= millis(); // Startzeit festhalten
				 ladenState= LADEN_STATE_LADEN;
			 }
			break;
			
		case LADEN_STATE_LADEN:			
			// max Verflüssigertemp - 0.5*Tempdiff über Verflüssiger Auslegepunkt zu hoch
			if ((tspeicher>= 45) || (tkondens >= 50)){
				ladenState= LADEN_STATE_STOP;
			}
			
			 // Ausschaltbedingung (Speicher bis unten warm)
			 // Bedingung 1: Speichertemp > 15°C ansonsten zu Beginn Heizperiode ständiges aus
			 else if ((tspeicher> 15) && ((tspeicher-tkondens)<= 6)){				 
				 // Anschliessende Enteisung falls Aussentemp kritisch
				 if (taussen<= T_DEFROST_REQUIRED){
					 ladenState= LADEN_STATE_DEFROST;
					 laufzeit= millis();
				 }
				 else{
					 ladenState= LADEN_STATE_STOP;
				 }
			 }
			
			// falls Vereisungsgefahr Laden pausieren um zu Enteisen, ansonsten Timerreset
			else if ((millis()-laufzeit)> MAX_LADEZEIT){
				 if (taussen<= T_DEFROST_REQUIRED){
					 ladenState= LADEN_STATE_DEFROST;
				 }
				 else{
					 laufzeit= millis();
				 }				
			}
			else{				
				wpStatemachine(WP_REQ_FUNC_LADEN);
			}
			break;
			
		case LADEN_STATE_STOP:	
			// Laden beenden, Sperrzeit initiieren
			wpStatemachine(WP_REQ_FUNC_HALT);
			laufzeit= millis();
			ladenState= LADEN_STATE_GESPERRT;
			break;
			
		case LADEN_STATE_DEFROST:
			// Enteisen bis Defrostzeit abgelaufen
			if ((millis()-laufzeit) > defrostzeit){
				ladenState= LADEN_STATE_STOP;
			}
			else{
				wpStatemachine(WP_REQ_FUNC_DEFROST);
			}
			break;
			
		case LADEN_STATE_GESPERRT:
			// Warten bis Sperrzeit abgelaufen
			if((millis()-laufzeit)> WIEDERANLAUF_SPERRZEIT){
				ladenState= LADEN_STATE_IDLE;
			}			
			break;
			
	}
}