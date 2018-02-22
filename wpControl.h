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
*       Dieser Strukturtyp kann als Variable zum Ablegen von Digitaleingangszuständen
*       genutzt werden. Es können Statusinfos für acht digitale Steuereingänge
*       abgelegt werden. Diese Strukturtyp Definition wurde für eine spezifische
*       Wärmepumpe geschrieben und enthält deshalb spezifische Membernamen.
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

/** Definiert einen 16-Bitfield Variablentyp für Informationen zum Systemzustand durch Ausgabe eines binären Wertes */
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

/** Switch-Case Variable Typedef mit WP Betriebszuständen */
typedef enum WP_STATE
	{
	WP_STATE_IDLE, /**< Standby. Corresponds to intvalue 0 */
	WP_STATE_START,    /**< Startsequenz. Corresponds to intvalue 1 */
	WP_STATE_RUN,  /**< Verdichterbetrieb. Corresponds to intvalue 2 */
	WP_STATE_STOP, /**< Stopsequenz. Corresponds to intvalue 3 */
	WP_STATE_DEFROST,  /**< Abtaubetrieb. Corresponds to intvalue 4 */
	WP_STATE_ERROR_P,  /**< Unter-/Überdruck Kältemittelkreislauf. Corresponds to intvalue 5 */
	WP_STATE_ERROR_M   /**< Motorschutzschalter ausgelöst. Corresponds to intvalue 6 */
	} wpState_t;

/** Betriebswahl-Anforderung Typedef an WP-Kontrollmodul */
typedef enum WP_REQ_FUNC
	{
	WP_REQ_FUNC_IDLE,  /**< Kein Betrieb angefordert, verbleibe in Standby. */
	WP_REQ_FUNC_HALT,  /**< Anforderung laufenden Betrieb zu beenden. */
	WP_REQ_FUNC_LADEN, /**< Anforderung Ladebetrieb. */
	WP_REQ_FUNC_DEFROST    /**< Anforderung Enteisung des Verdampfers durchführen. */
	} wpReqFunc_t;


void setupSteuerIO();
void getDIOstates();
void wpStatemachine(wpReqFunc_t wpReqFunc);


#endif
