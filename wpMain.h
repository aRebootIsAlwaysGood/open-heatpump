/**
 *  @file wpMain.h
 *  @brief Main Header Datei für grundlegende Strukturen und Pin-Definitionen.
 *
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

#ifndef WPMAIN_H
#define WPMAIN_H

// ***** PIN ASSIGNMENTS *****
// ----- DIGITAL INPUTS -----
#define PIN_ND 22 			/**< Niederdruck-Schalter an Digital-Input 22 (PA0) */
#define PIN_HD 23 			/**< Hochdruck-Schalter an Digital-Input 23 (PA1) */
#define PIN_MOTPROTECT 24	/**< Ueberstromsicherung Kompressormotor (PA2) */
#define PIN_ZUST_KSTART 25	/**< Schaltzustand des Kompressor-Anlaufsschütz (PA3) */
#define PIN_ZUST_KBETR 26	/**< Schaltzustand des Kompressor-Betriebsschütz (PA4) */
#define PIN_TARIFSPERRE 27	/**< Tarifsperren-Signal des EW (PA5) */
#define PIN_DI_RESERVE1 28	/**< Reserve Digital-Input (PA6) */
#define PIN_DI_RESERVE2 29	/**< Reserve Digital-Input (PA7) */
// ----- DIGITAL OUTPUTS -----
#define PIN_SUMPFHEIZUNG 30	/**< Ausgang für Kurbelwannenheizung des Kompressors (PC7) */
#define PIN_K_ANLAUF 31		/**< Schalten des Anlaufsschütz (PC6) */
#define PIN_K_BETRIEB 32	/**< Schalten des Betriebsschütz (PC5) */
#define PIN_BYPASS 33		/**< Schalten des Heissgas-Bypassventils (PC4) */
#define PIN_VENTILATOR 34	/**< Schalten des Ventilators (PC3) */
#define PIN_LADEPUMPE 35	/**< Schalten der Speicherladepumpe (PC2) */
#define PIN_HEIZPUMPE 36	/**< Schalten der Heizkreislaufpumpe (PC1) */
#define PIN_MISCHER_ZU 37	/**< Schliessen des Mischventils (Vorlauftemperatur senken) (PC0) */
#define PIN_MISCHER_AUF 38	/**< Öffnen des Mischventils (Vorlauftemperatur erhöhen) (PD7) */
#define PIN_SAMMELALARM 39	/**< Schaltkontakt zur Alarmweiterleitung (PG2) */
#define PIN_DO_RESERVE1 40	/**< Reserve Digital-Output (PG1) */
#define PIN_DO_RESERVE2 41	/**< Reserve Digital-Output (PG0) */
// ----- TEMPERATURES (AI) -----
#define PIN_T_AUSSEN A0		/**< Messeingang Aussentemperatur-Sensor (PF0, ADC0) */
#define PIN_T_KONDENSATOR A1/**< Messeingang Kondensatorrücklauf-Sensor (PF1, ADC1) */
#define PIN_T_SPEICHER A2	/**< Messeingang Speichertemperatur-Sensor (PF2, ADC2) */
#define PIN_T_VORLAUF A3	/**< Messeingang Vorlauftemperatur-Sensor (PF3, ADC3) */
#define PIN_AI_RESERVE1 A4	/**< Reserve AI (PF4, ADC4) */
#define PIN_AI_RESERVE2 A5	/**< Reserve AI (PF5, ADC5) */
// ----- USER IO (DI,DO,AI) -----
#define PIN_HEIZKURVE_NORM A10/**< Eingang Heizkurvenwahl Normalbetrieb (PK2, ADC10) */
#define PIN_HEIZKURVE_RED A11/**< Eingang Heizkurvenwahl reduzierter Betrieb (PK3, ADC11) */
#define PIN_PARALLELVS A12	/**< Eingang Parallelverschiebungsstufe (PK4, ADC12) */
#define PIN_BETRMODUS 2		/**< Eingang Betriebsmoduswahl (PE4) */
#define PIN_NAV_UP 3		/**< Eingang Menunavigation nach oben (PE5) */
#define PIN_NAV_DOWN 4		/**< Eingang Menunavigation nach unten (PG5) */
#define PIN_NAV_ENTER 5		/**< Eingang Menunavigation bestätige Auswahl (PE3) */
#define PIN_AUTOBETR_EN 6	/**< Anzeige Automatischer Betrieb aktiv (PH3) */
#define PIN_MANBETR_EN 7	/**< Anzeige Manueller Betrieb aktiv (PH4) */
#define PIN_ALARM 8			/**< Anzeige Störung (PH5) */


#define T_ANLASS 1000		/**< Anlassdauer Kompressormotor [ms] */
#define T_MISCHERSTELLZEIT 180000	/**< Fahrzeit Mischventil von 100% -> 0% [ms] */
#define T_NACHLAUF 100000 /**< Nachlaufzeit Ladepumpe nach Ende der Speicherladung [ms] */
#define TEMP_DEFROST_REQUIRED 10 /**< Grenze Aussentemperatur für periodische Enteisung */
#define T_MAX_LADEN 2400000 /**< maximale Ladedauer bis Enteisung erfolgen muss [ms] */
#define T_WIEDERANLAUFSPERRE 900000 /**< Anlaufsperre WP um Taktung gering zu halten [ms] */
#define T_DEFROSTSPERRE 210000 /**< Enteisungs-Sperrzeit nach Anlauf für Druckaufbau [ms] */
#define RX_BUFFERSIZE 21 /**< Anzahl empfangbarer ASCII Zeichen pro Befehl über UART (Befehlaufbau= HMIname:wert\n) */

void blinkFunction();


#endif
