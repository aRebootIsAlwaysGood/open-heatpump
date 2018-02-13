
/**
 *  @file wpUser.ino
 *  @brief Benutzersteuerung und Kommunikation mit Frontendcontroller.
 *			Der Mikrocontroller, welcher die Wärmepumpen- und Heizungssteuerung übernimmt,
 *			besitzt keine grafische Benutzerschnittstelle um eine Trennung zwischen den
 *			sicherheitsrelevanten und kritischen Funktionen und den Benutzerfunktionen,
 *			welche weder sicherheitstechnisch relevant noch zeitkrisch sind, zu erreichen.
 *			
 *			
 *  
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

 #include "wpUser.h"
 
 
 // called when Serial data available
void serialEvent1(){
	receiveSerialData();
	if (Serial1.available() > 0){
		receiveSerialData(); // if Buffer still contains unread data, call receivefunc. again
	}
	return;
}

// Read one Line of Serial Data received 
void receiveSerialData(){
	uint8_t indexval= Serial1.parseInt();		//Serial.parseInt() für lesen uint
	uint8_t actionval= Serial1.parseInt();
	uint8_t settingval= Serial1.parseInt();
		
	// if Data was received properly, store it or set readflag
	if(Serial1.read()=='\n'){
		Usersettings[indexval].useraction= actionval;
			
		// if read request flag set, respond with requested data
		if (actionval==1){
			transmitSerialData(indexval);	
		}
		// if write flag was set, overwrite stored value
		else if (actionval==2){
			Usersettings[indexval].uservalue= settingval;
		}
		// if write negative flag was set, overwrite stored value with negative val
		else if (actionval==4){
			Usersettings[indexval].uservalue= settingval *(-1);
		}					
	}
}

void transmitSerialData(uint8_t settingindex){
		
}
 
 int8_t getKurvenstufe(){
	return 5; 
 }
 
 int8_t getParallelvs(){
	return 0;
 }
 
 int8_t modeReduziert(){
	 return 0;
 }
 
