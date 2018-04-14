/**
 *  @file wpControl.h
 *  @brief Header zum Steuer- und Sicherheitsmodul.
 *
 *  @author Daniel Schmissrauter
 *  @date	24.01.2018
 */


 #ifndef WPCONTROL_H
#define WPCONTROL_H

/**
* @brief Definition Strukturtyps zum Speichern von 8 binären Werten in Bitfeldern.
*
*  Dieser Strukturtyp kann als Variable zum Ablegen von Digitaleingangszuständen
*  genutzt werden. Es können Statusinfos für acht digitale Steuereingänge
*  abgelegt werden. Diese Strukturtyp Definition wurde für eine spezifische
*   Wärmepumpe geschrieben und enthält deshalb spezifische Membernamen.
*/
struct DI_STATES
  {
    uint8_t status_nd:1;		/**< Statusbit0 ND-Schalter */
    uint8_t status_hd:1;		/**< Statusbit1 HD-Schalter */
    uint8_t status_motprotect:1;/**< Statusbit2 Motorschutz */
    uint8_t status_k_start:1;	/**< Statusbit3 Anlaufschütz */
    uint8_t status_k_run:1;		/**< Statusbit4 Betriebsschütz */
    uint8_t status_tarif:1;		/**< Statusbit5 Tarifsperre EW */
    uint8_t status_reserve1:1;	/**< Statusbit6 Reserve */
    uint8_t status_reserve2:1;	/**< Statusbit7 Reserve */
    };

/**
* @brief Definiert einen 16-Bitfield Variablentyp für Informationen.
*
*   Innerhalb solchen Strukturen können Variablen mit nur zwei Werten
*   speichersparend abgelegt werden, beispielsweise zum Systemzustand.
*   Der Aufruf eines Members kann wie bei Klassenfunktionen erfolgen.
*/
struct SYSTEMZUSTAND
	{
	int16_t sumpfheizung:1;	   /**< Bit0 Kurbelwannenheizung: 0= AUS, 1= EIN */
	int16_t kompressor:1;		/**< Bit1 Kompressor: 0= AUS, 1= EIN */
	int16_t ventilator:1;		/**< Bit2 Ventilator: 0= AUS, 1= EIN */
	int16_t bypass:1;			/**< Bit3 Heissgasventil: 1= OFFEN, 0= ZU */
	int16_t ladepumpe:1;		/**< Bit4 Speicherladepumpe: 0= AUS, 1= EIN */
	int16_t vorlaufregler:1;	/**< Bit5 Vorlauftemperaturregler: 0= AUS, 1= EIN */
	int16_t heizpumpe:1;		/**< Bit6 Umwälzpumpe Heizkreis : 0= AUS, 1= EIN */
	int16_t defrost:1;			/**< Bit7 Enteisung: 0= AUS, 1= EIN */
	int16_t autobetrieb:1;		/**< Bit10 Automatikbetrieb : 0= AUS, 1= EIN */
	int16_t reduziert:1;		/**< Bit11 reduzierter Heizbetrieb : 0= AUS, 1= EIN */
	int16_t manbetrieb:1;		/**< Bit12 Manueller Betrieb : 0= AUS, 1= EIN */
	int16_t drucktief:1;		/**< Bit13 Drucküberwachung: 0= Druck IO, 1= ALARM */
	int16_t druckhoch:1;			/**< Bit14 Drucküberwachung: 0= Druck IO, 1= ALARM */
	int16_t motorschutz:1;		/**< Bit15 Motorschutz: 0= IO, 1= AUSGELÖST */
	int16_t tarifsperre:1;			/**< Bit16 Tarifsperre: 0= AUS, 1= EIN */
	int16_t reserved_msb:1;		/**< Bit17 MSB Do not use unless you know its meaning */
	};

/**
* @brief Enumerator Typendefinition für die WP-Betriebs Statemachine.
*
*    Dies ist die Definition der Switch-Case Variable welche die WP
*    Betriebszustände enthält.
*/
typedef enum WP_STATE
	{
	WP_STATE_IDLE, /**< Standby. Numeric value 0 */
	WP_STATE_START, /**< Startsequence. Numeric value 1 */
	WP_STATE_RUN, /**< Verdichterbetrieb. Numeric value 2 */
	WP_STATE_STOP, /**< Stopsequenz. Numeric value 3 */
	WP_STATE_DEFROST, /**< Abtaubetrieb. Numeric value 4 */
	WP_STATE_ERROR_P,/**< Unter-/Überdruckstörung. Numeric value 5 */
	WP_STATE_ERROR_M   /**< Motorschutzschalter Numeric value 6 */
	} wpState_t;

/**
*   @brief Enumerator Typdefinition zur WP-Betriebswahl-Anforderung.
*
*   Dieser Variablentyp wird genutzt um Betriebswahl Anforderungen
*   an das WP-Kontrollmodul zu übergeben.
*/
typedef enum WP_REQ_FUNC
	{
	WP_REQ_FUNC_IDLE, /**< Keine Betriebsanforderung, WP-Standby. */
	WP_REQ_FUNC_HALT,  /**< Anforderung WP-Betrieb beenden. */
	WP_REQ_FUNC_LADEN, /**< Anforderung Speicherladebetrieb. */
	WP_REQ_FUNC_DEFROST /**< Anforderung Abtaubetrieb einleiten. */
	} wpReqFunc_t;

void setupSteuerIO();
void getDIOstates();
void wpStatemachine(wpReqFunc_t wpReqFunc);


#endif
