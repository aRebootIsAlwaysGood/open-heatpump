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
*			Mithilfe der Funktion setupSteuerIO() werden die Digitalausgänge
*			in einen sicheren Betriebszustand gebracht. Ebenfalls werden die gespeicherten
*			Systemzustandsbits mit 0 überschrieben. Die globale Variable, welche die
*			Digitaleingangszustände enthält, wird weiter zurückgesetzt.
*/
/************************************************************************/
void setupSteuerIO(){
	DiStates= {0,0,0,0,0,0,0,0}; // Setze alle Zustandsbits der DI auf 0
	Systemzustand= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
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
	Systemzustand.sumpfheizung= 1;
}

/************************************************************************/
/**
*	@brief Einlesen der Digitalen Eingänge.
*
*	Einlesen der digitalen Steuereingängen, die Eingänge der Nutzerbedienung jedoch nicht.
*	Alle Eingänge sind mittels Pullup-Widerstand ausgerüstet, sind also active-low
*	und werden demensprechend negiert bevor sie in der globalen Struktur DiStates
*	eingetragen werden . Debouncing ist hardwareseitig mittels RC-Tiefpass
*	implementiert.
*/
/************************************************************************/
void getInputstates(){
	DiStates.status_nd= digitalRead(PIN_ND); // bei Ansprechen DI=HIGH -> keine Negation
	DiStates.status_hd= digitalRead(PIN_HD); // bei Ansprechen DI=HIGH ->  keine Negation
	DiStates.status_motprotect= !digitalRead(PIN_MOTPROTECT);
	DiStates.status_k_start= !digitalRead(PIN_ZUST_KSTART);
	DiStates.status_k_run= !digitalRead(PIN_ZUST_KBETR);
	DiStates.status_tarif= !digitalRead(PIN_TARIFSPERRE);
	
}

/************************************************************************/
/**
*	@brief	Statemachine für die Low-Level Steuerung der Wärmepumpe.
*			In dieser Funktion werden die Steuerausgänge geschalten und die Eingänge
*			überwacht.
*			Durch das Zwischenschalten dieser Low-Level Funktion, können unsichere
*			Betriebszustände vermieden werden.
*			Das Einschalten des Kompressors muss zwingend über zuschaltbare
*			Anlasswiderstände erfolgen, was diese Funktion automatisch ausführt
*			wenn die Wärmepumpe angefordert wird.
*			Übergeordnete Funktionen können entweder eine Speicherladung, Enteisung
*			oder das Beenden dieser beiden anfordern. Das direkte Umschalten von Laden
*			zu Enteisen und umgekehrt ist möglich und erwünscht.
*			Die Anforderungsbedingungen wie Enteisungszeit und -Dauer,
*			Speichersolltemperatur, etc. müssen in den höheren Schichten implementiert
*			werden.
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
		getInputstates();

		if (wpReqFunc== (WP_REQ_FUNC_LADEN || WP_REQ_FUNC_DEFROST)){
			starttime=millis();		// Startzeitpunkt
			wpState= WP_STATE_START;
		}
		reglerStatemachine(REGLER_STATE_AUTO);	// rufe Regler auf, evt unnötig
		break;

	case WP_STATE_START:	// Startsequenz einleiten, Eingänge prüfen

		getInputstates();
		if (DiStates.status_nd || DiStates.status_hd){// Druckalarm wenn Unter-/Überdruck
			wpState = WP_STATE_ERROR_P;
		}

		else if (DiStates.status_motprotect){	// Wicklungsschutz Motor ausgelöst
			wpState = WP_STATE_ERROR_M;
		}

		else if (millis()-starttime <= T_MISCHERSTELLZEIT){	// Mischventil öffnen
			reglerStatemachine(REGLER_STATE_MANUAL);	// Regler IO-Steuerung übernehmen
			digitalWrite(PIN_MISCHER_AUF, blink1Hz);
			digitalWrite(PIN_HEIZPUMPE, HIGH);

		}

		else if (millis()-starttime <= (T_ANLASS+ T_MISCHERSTELLZEIT)){	// Anlassen
			digitalWrite(PIN_BYPASS, HIGH);
			digitalWrite(PIN_K_ANLAUF, HIGH);
			digitalWrite(PIN_K_BETRIEB, LOW);
			digitalWrite(PIN_SUMPFHEIZUNG, LOW);
		}

		else{										// Anlassen komplett
			digitalWrite(PIN_K_ANLAUF, HIGH);
			digitalWrite(PIN_K_BETRIEB, HIGH);
			digitalWrite(PIN_BYPASS, LOW);
			digitalWrite(PIN_MISCHER_AUF,LOW);
			starttime=millis();						//Reset Laufzeittimer auf RUN
			if (wpReqFunc== WP_REQ_FUNC_DEFROST){	// Enteisung angefordert
				wpState= WP_STATE_DEFROST;
			}
			else{									// Laden, gehe zu RUN
				wpState= WP_STATE_RUN;
			}
		}

		break;

	case WP_STATE_RUN:		// Speicher laden bis Anforderung kommt aufzuhören
		getInputstates();
		if (DiStates.status_nd || DiStates.status_hd){ // Druckcheck
			wpState = WP_STATE_ERROR_P;
		}

		else if (DiStates.status_motprotect){		// Überstromcheck
			wpState = WP_STATE_ERROR_M;
		}

		else if (wpReqFunc== WP_REQ_FUNC_HALT){		// Stop-Anforderung
			starttime= millis();
			wpState= WP_STATE_STOP;
		}
		else if (wpReqFunc== WP_REQ_FUNC_DEFROST){	// Enteisungs-Anforderung
			wpState= WP_STATE_DEFROST;
		}

		else{
			digitalWrite(PIN_K_ANLAUF,HIGH);
			digitalWrite(PIN_K_BETRIEB,HIGH);
			digitalWrite(PIN_VENTILATOR,HIGH);
			digitalWrite(PIN_LADEPUMPE,HIGH);
			digitalWrite(PIN_BYPASS, LOW);
			reglerStatemachine(REGLER_STATE_LADEN);	// Regler hat wieder IO Kontrolle
		}

		break;

	case WP_STATE_STOP:		// Beende Betrieb, schalte Sumpfheizung ein, gehe zu IDLE
		digitalWrite(PIN_K_BETRIEB,LOW);
		digitalWrite(PIN_VENTILATOR, LOW);
		digitalWrite(PIN_BYPASS, HIGH);		// Bypass auf für Druckausgleich
		digitalWrite(PIN_SUMPFHEIZUNG, HIGH);
		reglerStatemachine(REGLER_STATE_AUTO);	// Regler wieder autonom

		if (millis()-starttime >= 500){	// Softstop durch verzögertes AUS Anlaufschütz
			digitalWrite(PIN_K_ANLAUF,LOW);
		}
		if (millis()-starttime <= T_NACHLAUF){	// Ladepumpe Nachlauf nur nach Laden
			digitalWrite(PIN_LADEPUMPE, HIGH);
		}
		else{
			digitalWrite(PIN_LADEPUMPE,LOW);
			wpState= WP_STATE_IDLE;
		}

		break;

	case WP_STATE_DEFROST:		// Enteisen, Ventilator + Ladepumpe aus
		getInputstates();
		if (DiStates.status_nd || DiStates.status_hd){ // Druckcheck
			wpState = WP_STATE_ERROR_P;
		}

		else if (DiStates.status_motprotect){		// Überstromcheck
			wpState = WP_STATE_ERROR_M;
		}

		else if(wpReqFunc== WP_REQ_FUNC_HALT){	// Stop-Anforderung
			wpState= WP_STATE_STOP;
		}

		else if(wpReqFunc== WP_REQ_FUNC_LADEN){ // Speicher laden Anforderung
			wpState= WP_STATE_RUN;
		}

		else{
			if (millis()-starttime <= T_DEFROSTSPERRE){ // verhindere sofortige Enteisung
				// Baue erst Ladedruck auf
				digitalWrite(PIN_K_BETRIEB, HIGH);
				digitalWrite(PIN_K_ANLAUF, HIGH);
				digitalWrite(PIN_VENTILATOR, HIGH);
				digitalWrite(PIN_LADEPUMPE, HIGH);
				digitalWrite(PIN_BYPASS, LOW);
			}
			else{
			reglerStatemachine(REGLER_STATE_DEFROST);
			digitalWrite(PIN_K_BETRIEB, HIGH);
			digitalWrite(PIN_K_ANLAUF, HIGH);
			digitalWrite(PIN_VENTILATOR, LOW);
			digitalWrite(PIN_LADEPUMPE, LOW);
			digitalWrite(PIN_BYPASS, HIGH);
			}
		}

		break;

	case WP_STATE_ERROR_P:	// Druckfehler, Sammelalarm aktiv, Blinklicht aktiv
		digitalWrite(PIN_K_ANLAUF,LOW);
		digitalWrite(PIN_K_BETRIEB,LOW);
		digitalWrite(PIN_VENTILATOR, LOW);
		digitalWrite(PIN_LADEPUMPE, LOW);
		digitalWrite(PIN_BYPASS, HIGH);
		digitalWrite(PIN_SUMPFHEIZUNG, HIGH);
		digitalWrite(PIN_SAMMELALARM,HIGH);
		digitalWrite(PIN_ALARM, blink1Hz);
		reglerStatemachine(REGLER_STATE_AUTO);

		getInputstates();
		if (!DiStates.status_nd || !DiStates.status_hd){	// gehe zu IDLE wenn quittiert
			digitalWrite(PIN_SAMMELALARM,LOW);
			digitalWrite(PIN_ALARM, LOW);
			wpState= WP_STATE_IDLE;
		}

		break;

	case WP_STATE_ERROR_M:	// Motorfehler, Sammelalarm aktiv, Blinklicht aktiv
		digitalWrite(PIN_K_ANLAUF,LOW);
		digitalWrite(PIN_K_BETRIEB,LOW);
		digitalWrite(PIN_VENTILATOR, LOW);
		digitalWrite(PIN_LADEPUMPE, LOW);
		digitalWrite(PIN_BYPASS, HIGH);
		digitalWrite(PIN_SUMPFHEIZUNG, HIGH);
		digitalWrite(PIN_SAMMELALARM,HIGH);
		digitalWrite(PIN_ALARM, blink1Hz);
		reglerStatemachine(REGLER_STATE_AUTO);

		getInputstates();
		if (!DiStates.status_motprotect){	// gehe zu IDLE wenn quittiert
			digitalWrite(PIN_SAMMELALARM,LOW);
			digitalWrite(PIN_ALARM, LOW);
			wpState= WP_STATE_IDLE;
		}

		break;

	}
}
