/**
 *  @file wpSpeicherladung.h
 *  @brief Header zum Modul Speicherladung.
 *  
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

#ifndef WPSPEICHERLADUNG_H
#define WPSPEICHERLADUNG_H


typedef enum LADEN_STATE
{
	LADEN_STATE_IDLE,
	LADEN_STATE_LADEN,
	LADEN_STATE_STOP,
	LADEN_STATE_DEFROST,
	LADEN_STATE_GESPERRT,
} ladenState_t;

void speicherladung(float t_vorlaufsoll);

#endif