/**
 *  @file wpEnteisung.ino
 *  @brief Steuert den Enteisungsvorgang der Wärmepumpe.
 *   
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */
 #include "wpEnteisung.h"




 void enteisung(){
	wpStatemachine(WP_REQ_FUNC_DEFROST);
 }
