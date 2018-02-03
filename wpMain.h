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
#define PIN_ND 22 			/**< Niederdruck-Schalter an Digital-Input 22 */
#define PIN_HD 23 			/**< Hochdruck-Schalter an Digital-Input 23 */
#define PIN_MOTPROTECT 24	/**< Ueberstromsicherung Kompressormotor */
#define PIN_ZUST_KSTART 25	/**< Schaltzustand des Kompressor-Anlaufsschütz */
#define PIN_ZUST_KBETR 26	/**< Schaltzustand des Kompressor-Betriebsschütz */
#define PIN_TARIFSPERRE 27	/**< Tarifsperren-Signal des EW */
#define PIN_DI_RESERVE1 28	/**< Reserve Digital-Input */
#define PIN_DI_RESERVE2 29	/**< Reserve Digital-Input */
// ----- DIGITAL OUTPUTS -----
#define PIN_SUMPFHEIZUNG 30	/**< Ausgang für Kurbelwannenheizung des Kompressors */
#define PIN_K_ANLAUF 31		/**< Schalten des Anlaufsschütz */
#define PIN_K_BETRIEB 32	/**< Schalten des Betriebsschütz */
#define PIN_BYPASS 33		/**< Schalten des Heissgas-Bypassventils */
#define PIN_VENTILATOR 34	/**< Schalten des Ventilators */
#define PIN_LADEPUMPE 35	/**< Schalten der Speicherladepumpe */
#define PIN_HEIZPUMPE 36	/**< Schalten der Heizkreislaufpumpe */
#define PIN_MISCHER_ZU 37	/**< Schliessen des Mischventils (Vorlauftemperatur senken) */
#define PIN_MISCHER_AUF 38	/**< Öffnen des Mischventils (Vorlauftemperatur erhöhen) */
#define PIN_SAMMELALARM 39	/**< Schaltkontakt zur Alarmweiterleitung */
#define PIN_DO_RESERVE1 40	/**< Reserve Digital-Output */
#define PIN_DO_RESERVE2 41	/**< Reserve Digital-Output */
// ----- TEMPERATURES (AI) -----
#define PIN_T_AUSSEN A0		/**< Messeingang Aussentemperatur-Sensor */
#define PIN_T_KONDENSATOR A1/**< Messeingang Kondensatorrücklauf-Sensor */
#define PIN_T_SPEICHER A2	/**< Messeingang Speichertemperatur-Sensor */
#define PIN_T_VORLAUF A3	/**< Messeingang Vorlauftemperatur-Sensor */
#define PIN_AI_RESERVE1 A4	/**< Reserve AI */
#define PIN_AI_RESERVE2 A5	/**< Reserve AI */
// ----- USER IO (DI,DO,AI) -----
#define PIN_HEIZKURVE_NORM A10/**< Eingang Heizkurvenwahl Normalbetrieb */
#define PIN_HEIZKURVE_RED A11/**< Eingang Heizkurvenwahl reduzierter Betrieb */
#define PIN_PARALLELVS A12	/**< Eingang Parallelverschiebungsstufe */
#define PIN_BETRMODUS 2		/**< Eingang Betriebsmoduswahl */
#define PIN_NAV_UP 3		/**< Eingang Menunavigation nach oben */
#define PIN_NAV_DOWN 4		/**< Eingang Menunavigation nach unten */
#define PIN_NAV_ENTER 5		/**< Eingang Menunavigation bestätige Auswahl */
#define PIN_AUTOBETR_EN 6	/**< Anzeige Automatischer Betrieb aktiv */
#define PIN_MANBETR_EN 7	/**< Anzeige Manueller Betrieb aktiv */
#define PIN_ALARM 8			/**< Anzeige Störung */


#define T_ANLASS 3000		/**< Anlassdauer Kompressormotor in ms */
#define T_MISCHERSTELLZEIT 180000	/**< Fahrzeit Mischventil von 100% -> 0% in ms */
#define T_NACHLAUF 100000 /**< Nachlaufzeit Ladepumpe nach Ende der Speicherladung */

void blinkFunction();


#endif