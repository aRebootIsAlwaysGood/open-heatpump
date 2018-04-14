/**
*  @file wpControl.ino
*  @brief Steuert die Wärmepumpe und verhindert unzulässige Betriebszustände.
*
*  @author Daniel Schmissrauter
*  @date	24.01.2018
*/

#include "wpControl.h"

/************************************************************************/
/**
*	@brief	Initialisieren der Steuerein- und Ausgänge sowie deren Zustandsvariablen.
*
*	Mithilfe der Funktion setupSteuerIO() werden die Digitalausgänge
*	in einen sicheren Betriebszustand gebracht. Ebenfalls werden die gespeicherten
*	Systemzustandsbits mit 0 überschrieben. Die globale Variable, welche die
*	Digitaleingangszustände enthält, wird weiter zurückgesetzt.
*/
/************************************************************************/
void setupSteuerIO(){

	digitalWrite(PIN_K_ANLAUF,LOW);
	digitalWrite(PIN_K_BETRIEB,LOW);
	digitalWrite(PIN_VENTILATOR, LOW);
	digitalWrite(PIN_BYPASS,LOW);
	digitalWrite(PIN_LADEPUMPE,LOW);
	digitalWrite(PIN_HEIZPUMPE,LOW);
	digitalWrite(PIN_MISCHER_AUF,LOW);
	digitalWrite(PIN_MISCHER_ZU,LOW);
	digitalWrite(PIN_SAMMELALARM,LOW);
	digitalWrite(PIN_SUMPFHEIZUNG,HIGH); // Sumpfheizung in Standby immer aktiv
	// only for debugging
	#ifdef DEBUG_PROGRAM_FLOW
		Serial.print(F("Executed: setupSteuerIO"));
		Serial.println(F(" ->Modul: Control"));
	#endif
}

/************************************************************************/
/**
*	@brief Einlesen der Zustände von Digital-Eingänge und Ausgänge.
*
*	Einlesen der digitalen SteuerIOs, die IOs der Nutzerbedienung jedoch nicht.
*	Alle Eingänge sind mittels Pullup-Widerstand ausgerüstet, sind also
*	active-low und werden demensprechend negiert bevor sie in der globalen
*	Strukturvariable #DiStates sowie #Systemzustand eingetragen werden.
*	Debouncing ist hardwareseitig mittels RC-Tiefpass implementiert.
*	Die Digitalausgänge werden ebenfalls eingelesen, um deren Zustand in
*	die Bitfelder der Struktur #Systemzustand zu schreiben. \n
*	Die abgelegten Bits in den Strukturen sind wert-homogen. Das heisst, ein
*	Bitwert von 1=HIGH=true entspricht einem geschlossenen Kontakt. Dadurch
*	entsprechen sie dem elektrischen Signal und ein Schliesser- resp. ein
*	Öffner-Kontakt verhält sich programmintern gleich wie im Stromkreis.
*
* @note	Die Bitfelder in #Systemzustand dürfen keinesfalls für Freigabeprüfungen
*		genutzt werden sondern lediglich als Informationsquelle, da es
*		ansonsten zu gegenseitiger Blockierung kommen kann.
*/
/************************************************************************/
void getDIOstates(){
	DiStates.status_nd= !digitalRead(PIN_ND); // NC: DI LOW = Pressure OK
	DiStates.status_hd= !digitalRead(PIN_HD); // NC: DI LOW = Pressure OK
	DiStates.status_motprotect= !digitalRead(PIN_MOTPROTECT); // NC: DI LOW = Motor OK
	DiStates.status_k_start= !digitalRead(PIN_ZUST_KSTART); // DI LOW = ON
	DiStates.status_k_run= !digitalRead(PIN_ZUST_KBETR); // DI LOW = ON
	DiStates.status_tarif= !digitalRead(PIN_TARIFSPERRE); // DI LOW = ON

	Systemzustand.sumpfheizung= digitalRead(PIN_SUMPFHEIZUNG); // DO: HIGH= ON
	Systemzustand.kompressor= DiStates.status_k_run;
	Systemzustand.ventilator= digitalRead(PIN_VENTILATOR); // DO: HIGH= ON
	Systemzustand.bypass= digitalRead(PIN_BYPASS); // DO: HIGH= ON
	Systemzustand.ladepumpe= digitalRead(PIN_LADEPUMPE); // DO: HIGH= ON
	Systemzustand.heizpumpe= digitalRead(PIN_HEIZPUMPE); // DO: HIGH= ON
	Systemzustand.drucktief= DiStates.status_nd;
	Systemzustand.druckhoch= DiStates.status_hd;
	Systemzustand.motorschutz= DiStates.status_motprotect;
	Systemzustand.tarifsperre= DiStates.status_tarif;
	// only for debugging
	#ifdef DEBUG_PROGRAM_FLOW
		Serial.print(F("Executed: getDIOstates"));
		Serial.println(F(" ->Modul: Control"));
	#endif
	// only for debugging inputvalues
    #ifdef DEBUG_INPUTVALUES
        Serial.print(F("DI: ND, VALUE: "));
        Serial.println(!digitalRead(PIN_ND));
		Serial.print(F("DI: HD, VALUE: "));
		Serial.println(!digitalRead(PIN_HD));
		Serial.print(F("DI: Motprotect, VALUE: "));
		Serial.println(!digitalRead(PIN_MOTPROTECT));
		Serial.print(F("DI: Zustand Kstart, VALUE: "));
		Serial.println(!digitalRead(PIN_ZUST_KSTART));
		Serial.print(F("DI: Zustand Kbetrieb, VALUE: "));
		Serial.println(!digitalRead(PIN_ZUST_KBETR));
		Serial.print(F("DI: Tarifsperre, VALUE: "));
		Serial.println(!digitalRead(PIN_TARIFSPERRE));
		Serial.println();
    #endif
	// only for debugging Outputstates
	#ifdef DEBUG_OUTPUTVALUES
		Serial.print(F("DO: Sumpfheizung, VALUE: "));
		Serial.println(digitalRead(PIN_SUMPFHEIZUNG));
		Serial.print(F("DO: K Anlauf, VALUE: "));
		Serial.println(digitalRead(PIN_K_ANLAUF));
		Serial.print(F("DO: K Betrieb, VALUE: "));
		Serial.println(digitalRead(PIN_K_BETRIEB));
		Serial.print(F("DO: Bypass, VALUE: "));
		Serial.println(digitalRead(PIN_BYPASS));
		Serial.print(F("DO: Ventilator, VALUE: "));
		Serial.println(digitalRead(PIN_VENTILATOR));
		Serial.print(F("DO: Ladepumpe, VALUE: "));
		Serial.println(digitalRead(PIN_LADEPUMPE));
		Serial.print(F("DO: Heizpumpe, VALUE: "));
		Serial.println(digitalRead(PIN_HEIZPUMPE));
		Serial.print(F("DO: Mischer auf, VALUE: "));
		Serial.println(digitalRead(PIN_MISCHER_AUF));
		Serial.print(F("DO: Mischer zu, VALUE: "));
		Serial.println(digitalRead(PIN_MISCHER_ZU));
		Serial.print(F("DO: Sammelalarm, VALUE: "));
		Serial.println(digitalRead(PIN_SAMMELALARM));
		Serial.println();
	#endif
}

/************************************************************************/
/**
*	@brief	Statemachine für die Low-Level Steuerung der Wärmepumpe.
*
*	In dieser Funktion werden die Steuerausgänge geschalten und die Eingänge
*	überwacht.
*	Durch das Zwischenschalten dieser Low-Level Funktion, können unsichere
*	Betriebszustände vermieden werden.
*	Das Einschalten des Kompressors muss zwingend über zuschaltbare
*	Anlasswiderstände erfolgen, was diese Funktion automatisch ausführt
*	wenn die Wärmepumpe angefordert wird. \n
*	Übergeordnete Funktionen können entweder eine Speicherladung, Enteisung
*	oder das Beenden dieser beiden anfordern. Das direkte Umschalten von Laden
*	zu Enteisen und umgekehrt ist möglich und erwünscht. \n
*
*	@note	Die Anforderungsbedingungen wie Enteisungszeit und -Dauer,
*	Speichersolltemperatur, etc. müssen in den höheren Schichten implementiert
*	werden.
*
*	@param	wpReqFunc
*			wpReqFunc_t Instanz.Enum welche die angeforderbaren Zustände enthält.
*/
/************************************************************************/
void wpStatemachine(wpReqFunc_t wpReqFunc)
{
	static uint32_t starttime;	// Vergangene Zeit ab Anforderung um Anlaufphase zu bestimmen

	switch(wpState)
	{
	case WP_STATE_IDLE:		// Default case
		getDIOstates();
		reglerStatemachine(REGLER_STATE_AUTO);	// rufe Regler auf
		Systemzustand.vorlaufregler= 1; // update Regler Systemzustandsbit
		digitalWrite(PIN_SUMPFHEIZUNG,HIGH);
		if (wpReqFunc== (WP_REQ_FUNC_LADEN || WP_REQ_FUNC_DEFROST)){
			starttime=millis();		// Startzeitpunkt
			wpState= WP_STATE_START;
		}
		break;

	case WP_STATE_START:	// Startsequenz einleiten, Eingänge prüfen
		getDIOstates();
		if ((DiStates.status_nd) || (DiStates.status_hd)){// Druckalarm wenn Unter-/Überdruck
			wpState = WP_STATE_ERROR_P;
		}

		else if (DiStates.status_motprotect){	// Wicklungsschutz Motor ausgelöst
			wpState = WP_STATE_ERROR_M;
		}

		// Mischventil öffnen
		else if ((millis()-starttime) <= T_MISCHERSTELLZEIT){
			reglerStatemachine(REGLER_STATE_MANUAL); // Regler überbrücken
			digitalWrite(PIN_MISCHER_AUF, HIGH);
			digitalWrite(PIN_MISCHER_AUF, LOW);
			digitalWrite(PIN_HEIZPUMPE, HIGH);
		}

		// Anlassen
		else if ((millis()-starttime) <= (T_ANLASS+ T_MISCHERSTELLZEIT)){
			reglerStatemachine(REGLER_STATE_MANUAL); // Regler überbrücken
			digitalWrite(PIN_BYPASS, HIGH); // evt auch LOW (Lastanlauf)
			digitalWrite(PIN_K_ANLAUF, HIGH);
			digitalWrite(PIN_K_BETRIEB, LOW);
			digitalWrite(PIN_HEIZPUMPE, HIGH);
			digitalWrite(PIN_SUMPFHEIZUNG, LOW);
		}

		// Anlassen komplett
		else if((millis()-starttime) > (T_ANLASS+ T_MISCHERSTELLZEIT)){
			reglerStatemachine(REGLER_STATE_MANUAL); // Regler überbrücken
			digitalWrite(PIN_K_ANLAUF, HIGH);
			digitalWrite(PIN_K_BETRIEB, HIGH);
			digitalWrite(PIN_BYPASS, LOW);
			digitalWrite(PIN_MISCHER_AUF,LOW);
			digitalWrite(PIN_HEIZPUMPE, HIGH);
			digitalWrite(PIN_SUMPFHEIZUNG,LOW);
			starttime=millis();		//Reset Laufzeittimer auf RUN
			// Enteisung angefordert
			if (wpReqFunc== WP_REQ_FUNC_DEFROST){
				initRegler();
				wpState= WP_STATE_DEFROST;
			}
			// Laden, gehe zu RUN
			else{
				wpState= WP_STATE_RUN;
				initRegler(); // Regler neu initialisieren
			}
		}

		break;

	case WP_STATE_RUN:		// Speicher laden bis Anforderung kommt aufzuhören
		getDIOstates();
		reglerStatemachine(REGLER_STATE_AUTO); // Regler autonom
		digitalWrite(PIN_SUMPFHEIZUNG,LOW);
		if ((DiStates.status_nd) || (DiStates.status_hd)){ // Druckcheck
			wpState = WP_STATE_ERROR_P;
		}

		else if (DiStates.status_motprotect){		// Überstromcheck
			wpState = WP_STATE_ERROR_M;
		}

		else if (wpReqFunc== WP_REQ_FUNC_HALT){		// Stop-Anforderung
			starttime= millis(); // update starttime auf Stopanford. Zeit
			wpState= WP_STATE_STOP;
		}
		else if (wpReqFunc== WP_REQ_FUNC_DEFROST){	// Enteisungs-Anforderung
			initRegler(); // Regler neu initialisieren
			starttime= (millis()+T_DEFROSTSPERRE); // update starttime auf Abtauanford. Zeit + überspringe Druckaufbau-Sperrzeit
			wpState= WP_STATE_DEFROST;
		}

		else{
			digitalWrite(PIN_K_ANLAUF,HIGH);
			digitalWrite(PIN_K_BETRIEB,HIGH);
			digitalWrite(PIN_VENTILATOR,HIGH);
			digitalWrite(PIN_LADEPUMPE,HIGH);
			digitalWrite(PIN_BYPASS, LOW);
		}

		break;

	case WP_STATE_STOP:	// Beende WP-Betrieb, Sumpfheizung ein, gehe zu IDLE
		reglerStatemachine(REGLER_STATE_AUTO);	// Regler wieder autonom
		digitalWrite(PIN_K_BETRIEB,LOW);
		digitalWrite(PIN_VENTILATOR, LOW);
		digitalWrite(PIN_BYPASS, LOW);		// Bypass zu
		digitalWrite(PIN_SUMPFHEIZUNG, HIGH);

		if ((millis()-starttime) >= 500){	// Softstop durch verzögertes AUS Anlaufschütz
			digitalWrite(PIN_K_ANLAUF,LOW);
		}
		if ((millis()-starttime) <= T_NACHLAUF){	// Ladepumpe Nachlauf nur nach Laden
			digitalWrite(PIN_LADEPUMPE, HIGH);
		}
		else{
			digitalWrite(PIN_LADEPUMPE,LOW);
			starttime= millis(); // update starttime auf WP-Stopzeit
			wpState= WP_STATE_IDLE;
		}

		break;

	case WP_STATE_DEFROST:		// Enteisen, Ventilator + Ladepumpe aus
		getDIOstates();
		reglerStatemachine(REGLER_STATE_AUTO);	// Regler wieder autonom
		digitalWrite(PIN_SUMPFHEIZUNG,LOW);
		if ((DiStates.status_nd) || (DiStates.status_hd)){ // Druckcheck
			wpState = WP_STATE_ERROR_P;
		}
		else if (DiStates.status_motprotect){		// Überstromcheck
			wpState = WP_STATE_ERROR_M;
		}
		else if(wpReqFunc== WP_REQ_FUNC_HALT){	// Stop-Anforderung
			starttime= millis(); // update starttime auf Stopanford. Zeit
			wpState= WP_STATE_STOP;
		}

		else if(wpReqFunc== WP_REQ_FUNC_LADEN){ // Speicher laden Anforderung
			starttime= millis(); // update starttime auf Ladeanford. Zeit
			wpState= WP_STATE_RUN;
		}

		else{
			if ((millis()-starttime) <= T_DEFROSTSPERRE){ // verhindere sofortige Enteisung
				// Baue erst Ladedruck auf
				digitalWrite(PIN_K_BETRIEB, HIGH);
				digitalWrite(PIN_K_ANLAUF, HIGH);
				digitalWrite(PIN_VENTILATOR, HIGH);
				digitalWrite(PIN_LADEPUMPE, HIGH);
				digitalWrite(PIN_BYPASS, LOW);
			}
			else{
			digitalWrite(PIN_K_BETRIEB, HIGH);
			digitalWrite(PIN_K_ANLAUF, HIGH);
			digitalWrite(PIN_VENTILATOR, LOW);
			digitalWrite(PIN_LADEPUMPE, LOW);
			digitalWrite(PIN_BYPASS, HIGH);
			}
		}

		break;

	case WP_STATE_ERROR_P:	// Druckfehler, Sammelalarm aktiv
		digitalWrite(PIN_K_ANLAUF,LOW);
		digitalWrite(PIN_K_BETRIEB,LOW);
		digitalWrite(PIN_VENTILATOR, LOW);
		digitalWrite(PIN_LADEPUMPE, LOW);
		digitalWrite(PIN_BYPASS, LOW);
		digitalWrite(PIN_SUMPFHEIZUNG, HIGH);
		digitalWrite(PIN_SAMMELALARM,HIGH);
		reglerStatemachine(REGLER_STATE_AUTO);

		getDIOstates();
		if ((!DiStates.status_nd) && (!DiStates.status_hd)){	// gehe zu IDLE wenn Alarme nicht mehr anstehend
			digitalWrite(PIN_SAMMELALARM,LOW);
			wpState= WP_STATE_IDLE;
		}

		break;

	case WP_STATE_ERROR_M:	// Motorfehler, Sammelalarm aktiv, Blinklicht aktiv
		digitalWrite(PIN_K_ANLAUF,LOW);
		digitalWrite(PIN_K_BETRIEB,LOW);
		digitalWrite(PIN_VENTILATOR, LOW);
		digitalWrite(PIN_LADEPUMPE, LOW);
		digitalWrite(PIN_BYPASS, LOW);
		digitalWrite(PIN_SUMPFHEIZUNG, HIGH);
		digitalWrite(PIN_SAMMELALARM,HIGH);
		reglerStatemachine(REGLER_STATE_AUTO);

		getDIOstates();
		if (!DiStates.status_motprotect){	// gehe zu IDLE wenn quittiert
			digitalWrite(PIN_SAMMELALARM,LOW);
			//digitalWrite(PIN_LED_ALARM, LOW);
			wpState= WP_STATE_IDLE;
		}
		break;
	}
	// only for debugging
	#ifdef DEBUG_PROGRAM_FLOW
		Serial.print(F("Executed: wpStatemachine"));
		Serial.println(F(" ->Modul: Control"));
	#endif
}
