/**
 *  @file wpSpeicherladung.ino
 *  @brief Aufladen des Pufferspeichers.
 *  
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

 #include "wpSpeicherladung.h"

// Stufenladung Speicher monovalent RAVEL Empfehlung: Siehe RAVEL Standardschaltungen S16
// Temperaturdiff �ber Verfl�ssiger = 0.5...0.7*Tempdiff VL/RL im Auslegepunkt(45-35=10�C)
// -> Ausschalten WP wenn Tspeicher - Tkondensatorr�cklauf < 5...7�C
// 
// Einstellregeln:
// 1. Speichertemp bei WP-Start(Einschalttemp) <= Speichertemp bei WP-Stop(Ausschalttemp)
// 2. Ausschalttemp <= max. zul. Verfl�ssiger-Austrittstemp - max Tempdiff �ber Verfl.
// 3. Einschalttemp >= max. auftretende R�cklauftemp
// 
// -Festlegen max zul. Verfl�ssigeraustrittstemp auf <=50�C (R502)
// -Vor-&Nachlauf Ladepumpe normalerweise unn�tig
// -Tempdiff �ber Verbraucher: 10K

 void speicherladung(){
	wpStatemachine(WP_REQ_FUNC_LADEN);
 }