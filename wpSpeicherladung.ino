/**
 *  @file wpSpeicherladung.ino
 *  @brief Aufladen des Pufferspeichers.
 *  
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

 #include "wpSpeicherladung.h"


 void speicherladung(){
	wpStatemachine(WP_REQ_FUNC_LADEN);
 }