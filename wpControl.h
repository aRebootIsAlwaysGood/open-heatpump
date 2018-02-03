/**
 *  @file wpControl.h
 *  @brief Header zum Steuer- und Sicherheitsmodul.
 *  
 *  @author Daniel Schmissrauter
 *  @date	24.01.2018
 */
 
 
 #ifndef WPCONTROL_H
#define WPCONTROL_H

/** Struct-Objekt welches für alle digitalen Steuereingänge ein Statusbit beinhaltet */
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

/** Struct-Objekt mit Informationen zum Systemzustand durch Ausgabe eines binären Wertes */
struct SYSTEMZUSTAND 
	{
	uint16_t sumpfheizung:1;	/**< Bit0 Kurbelwannenheizung: 0= AUS, 1= EIN */
	uint16_t kompressor:1;		/**< Bit1 Kompressor: 0= AUS, 1= EIN */
	uint16_t ventilator:1;		/**< Bit2 Ventilator: 0= AUS, 1= EIN */
	uint16_t bypass:1;			/**< Bit3 Heissgasventil: 0= OFFEN, 1= ZU */
	uint16_t ladepumpe:1;		/**< Bit4 Speicherladepumpe: 0= AUS, 1= EIN */
	uint16_t vorlaufregler:1;	/**< Bit5 Vorlauftemperaturregler: 0= AUS, 1= EIN */
	uint16_t heizpumpe:1;		/**< Bit6 Umwälzpumpe Heizkreis : 0= AUS, 1= EIN */
	uint16_t defrost:1;			/**< Bit7 Enteisung: 0= AUS, 1= EIN */
	uint16_t autobetrieb:1;		/**< Bit10 Automatikbetrieb : 0= AUS, 1= EIN */
	uint16_t reduziert:1;		/**< Bit11 reduzierter Heizbetrieb : 0= AUS, 1= EIN */
	uint16_t manbetrieb:1;		/**< Bit12 Manueller Betrieb : 0= AUS, 1= EIN */
	uint16_t reserve1:1;		/**< Bit13 Reserve */
	uint16_t druck:1;			/**< Bit14 Drucküberwachung: 0= Druck IO, 1= ALARM */
	uint16_t motorschutz:1;		/**< Bit15 Motorschutz: 0= IO, 1= AUSGELÖST */
	uint16_t alarm:1;			/**< Bit16 Alarm: 0= kein Alarm, 1= Anstehend */
	uint16_t reserve2:1;		/**< Bit17 Reserve */
	};

/** Switch-Case Variable Typedef mit WP Betriebszuständen */
typedef enum WP_STATE
	{
	WP_STATE_IDLE,
	WP_STATE_START,
	WP_STATE_RUN,
	WP_STATE_STOP,
	WP_STATE_DEFROST,
	WP_STATE_ERROR_P,
	WP_STATE_ERROR_M
	} wpState_t;

/** Betriebswahl-Anforderung Typedef an WP-Kontrollmodul */
typedef enum WP_REQ_FUNC
	{
	WP_REQ_FUNC_IDLE,
	WP_REQ_FUNC_HALT,
	WP_REQ_FUNC_LADEN,
	WP_REQ_FUNC_DEFROST
	} wpReqFunc_t;


void setupSteuerIO();
void getInputstates();
void wpStatemachine(wpReqFunc_t wpReqFunc);


#endif