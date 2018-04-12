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
/***** DIGITAL INPUTS *****/
#define PIN_ND 22 			/**< Niederdruck-Schalter an Digital-Input 22 (PA0) */
#define PIN_HD 23 			/**< Hochdruck-Schalter an Digital-Input 23 (PA1) */
#define PIN_MOTPROTECT 24	/**< Ueberstromsicherung Kompressormotor (PA2) */
#define PIN_ZUST_KSTART 25	/**< Schaltzustand des Kompressor-Anlaufsschütz (PA3) */
#define PIN_ZUST_KBETR 26	/**< Schaltzustand des Kompressor-Betriebsschütz (PA4) */
#define PIN_TARIFSPERRE 27	/**< Tarifsperren-Signal des EW (PA5) */
#define PIN_DI_RESERVE1 28	/**< Reserve Digital-Input (PA6) */
#define PIN_DI_RESERVE2 29	/**< Reserve Digital-Input (PA7) */

/***** DIGITAL OUTPUTS *****/
#define PIN_SUMPFHEIZUNG 30	/**< Ausgang für Kurbelwannenheizung des Kompressors (PC7) */
#define PIN_K_ANLAUF 31		/**< Schalten des Anlaufsschütz (PC6) */
#define PIN_K_BETRIEB 32	/**< Schalten des Betriebsschütz (PC5) */
#define PIN_BYPASS 33		/**< Schalten des Heissgas-Bypassventils (PC4) */
#define PIN_VENTILATOR 34	/**< Schalten des Ventilators (PC3) */
#define PIN_LADEPUMPE 35	/**< Schalten der Speicherladepumpe (PC2) */
#define PIN_HEIZPUMPE 36	/**< Schalten der Heizkreislaufpumpe (PC1) */
#define PIN_MISCHER_AUF 37	/**< Öffnen des Mischventils (Vorlauftemperatur erhöhen) (PC0) */
#define PIN_MISCHER_ZU 38	/**< Schliessen des Mischventils (Vorlauftemperatur senken) (PD7) */
#define PIN_SAMMELALARM 39	/**< Schaltkontakt zur Alarmweiterleitung (PG2) */
#define PIN_DO_RESERVE1 40	/**< Reserve Digital-Output (PG1) */
#define PIN_DO_RESERVE2 41	/**< Reserve Digital-Output (PG0) */

/***** TEMPERATUR INPUTS (AI) *****/
#define PIN_T_AUSSEN A0		/**< Messeingang Aussentemperatur-Sensor (PF0, ADC0) */
#define PIN_T_KONDENSATOR A1/**< Messeingang Kondensatorrücklauf-Sensor (PF1, ADC1) */
#define PIN_T_SPEICHER A2	/**< Messeingang Speichertemperatur-Sensor (PF2, ADC2) */
#define PIN_T_VORLAUF A3	/**< Messeingang Vorlauftemperatur-Sensor (PF3, ADC3) */
#define PIN_AI_RESERVE1 A4	/**< Reserve AI (PF4, ADC4) */
#define PIN_AI_RESERVE2 A5	/**< Reserve AI (PF5, ADC5) */

/***** USER IO (DI,DO,AI) *****/
#define PIN_PARALLELVS_NORM A10/**< Eingang Parallelvs Normalbetrieb (PK2, ADC10) */
#define PIN_PARALLELVS_RED A11/**< Eingang Parallelvs reduzierter Betrieb (PK3, ADC11) */
#define PIN_HEIZKURVENSTUFE A12	/**< Eingang Heizkurvenwahl (PK4, ADC12) */
#define PIN_LOCALBUTTONS A13    /**< Eingang lokale Bedienung (PK5, ADC13) */
#define PIN_FORCE_LOCAL 2		/**< Eingang Erzwinge lokale Bedienung (PE4) */
#define PIN_LED_STBY 3		/**< Anzeige Standby (PE5) */
#define PIN_LED_MAN 4	/**< Anzeige manueller Betrieb aktiv (PG5) */
#define PIN_LED_AUTORED 5		/**< Anzeige Autobetrieb reduziert. (PE3) */
#define PIN_LED_AUTONORM 6		/**< Anzeige Autobetrieb normal. (PH3) */
#define PIN_LED_HDND 7	/**< Anzeige Druck (PH4) */
#define PIN_LED_ALARM 8			/**< Anzeige Störung (PH5) */
#define PIN_LED_KEY5 9   /**< Anzeige zu unbelegtem lokalem Taster 5 (PH6) */

/***** MAKROS ZUR BETRIEBSANPASSUNG *****/
#define T_ANLASS 1000		/**< Anlassdauer Kompressormotor [ms] */
#define T_MISCHERSTELLZEIT 155000	/**< Stellzeit Mischventil von 100% -> 0% [ms] + 5s Reserve. */
#define T_NACHLAUF 100000 /**< Nachlaufzeit Ladepumpe nach Ende der Speicherladung [ms] */
#define TEMP_DEFROST_REQUIRED 10 /**< Grenze Aussentemperatur für periodische Enteisung */
#define T_MAX_LADEN 2400000 /**< maximale Ladedauer bis Enteisung erfolgen muss [ms] */
#define T_WIEDERANLAUFSPERRE 900000 /**< Anlaufsperre WP um Taktung gering zu halten [ms] */
#define T_DEFROSTSPERRE 210000 /**< Enteisungs-Sperrzeit nach Anlauf für Druckaufbau [ms] */
#define RX_BUFFERSIZE 21 /**< Anzahl empfangbarer ASCII Zeichen pro Befehl über UART (Befehlaufbau= HMIname:wert\n) */

/***** DEBUGGING MACROS *****/
//#define DEBUG_PROGRAM_FLOW /**< Uncomment to print function calls over USB-Port (Serial0) */
#define DEBUG_INPUTVALUES /**< Uncomment to print values read by Analog-& Digital Inputs */
#define DEBUG_OUTPUTVALUES /**< Uncomment to print states of Digital Outputs */
#define DEBUG_SYSTEMZUSTAND /**< Uncomment to print values stored in struct Systemzustand */
#define DEBUG_SETTINGS /**< Uncomment to print values stored in User-& Systemsettings */
#define DEBUG_COMMUNICATION /**< Uncomment to print values stored in User-& Systemsettings */

void blinkFunction();


#endif
