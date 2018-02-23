/**
 *  @file wpSpeicherladung.h
 *  @brief Header zum Modul Speicherladung.
 *
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

#ifndef WPSPEICHERLADUNG_H
#define WPSPEICHERLADUNG_H

/**
*	@brief	Definition des Enumtyps für die Speicherladungs-Statemachine.
*
*			Mittels dieses Enumerator-Variablentyps, können die Zustände
*			der Speicherladungs-Statemachine speicherladung()
*			als Klarnamen statt numerischer Werte aufgerufen werden.
*/
typedef enum LADEN_STATE
{
	LADEN_STATE_IDLE, /**< Bereitschaftsmodus zum den Speicher laden */
	LADEN_STATE_LADEN, /**< Speicherladung wird ausgeführt */
	LADEN_STATE_STOP, /**< Speicher geladen, Wärmepumpe anhalten */
	LADEN_STATE_DEFROST, /**< Verdampferenteisung ausführen */
	LADEN_STATE_GESPERRT, /**< Wiederanlaufsperre */
} ladenState_t;

void speicherladung(float t_vorlaufsoll);

#endif
