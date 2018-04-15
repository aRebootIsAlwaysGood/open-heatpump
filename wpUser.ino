
/**
 *  @file wpUser.ino
 *  @brief Benutzersteuerung und Kommunikation mit Frontendcontroller.
 *
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

 /****************************************************************************/
 /**
 *  @brief Liest über UART empfangene Daten ein und legt sie als Einstellung ab.
 *
 *  Wenn am Serialport 1 vom HMI gesendete Daten anstehen, wird die Funktion
 *  receiveSerialData() von der Funktion serialEvent1() aufgerufen.\n
 *  Die im Schnittstellen-Buffer enthaltene Daten werden eingelesen bis
 *  ein LINEFEED als Terminator erkannt wird, das Empfangsarray @c rxbuffer
 *  voll ist oder 20ms keine weiteren Daten empfangen werden.\n
 *  Mittels #RX_BUFFERSIZE kann die Anzahl empfangbarer ASCII-Zeichen/Bytes
 *  pro Empfangseinheit definiert werden, wobei gilt:\n
 *  Buffersize= empfangbare Zeichen + 3\n
 *  Durch die Begrenzung der Befehlslänge kann ein Buffer-Overflow resp.
 *  eine Out of bounds Exception effektiv verhindert werden.\n
 *  Die Funktion teilt empfangene Zeichenkette in Namen- und Wertteil auf
 *  wobei sie folgendes Befehlsformat erwartet:\n
 *  Datapaket <=RX_BUFFERSIZE= PARAMNAME:PARAMVALUE'NL'  (PARAMVALUE < +/-32767)\n
 *  Der Namenteil wird anschliessend mit den Namensfelder der Einträge im
 *  Strukturarray #Usersettings verglichen. Bei Übereinstimmung wird der
 *  empfangene Wertteil beim gefundenen Eintrag im Feld @c value abgelegt.
 *  Wird nur ein Befehlsname ohne Wert gesendet, wird der Defaultwert
 *  -99 zugewiesen und der Wert nicht gespeichert.
 *
 * @remark Das alleinige Senden des Namenteils hat aktuell keine Anwendung.
 *        Eine denkbare und möglicherweise folgende Anwendung ist, das alleinige
 *        Senden des Namens als Request zur Rücksendung des Aktualwertes, welcher
 *       beim Empfänger gespeichert ist.
 *
 * @return @c void
 */
 /***************************************************************************/
void receiveSerialData() {
  char rxterminator = '\n'; // explicit initialisation for type char needed!
  char rxbuffer[RX_BUFFERSIZE] = ""; // init Receivingbuffer
  char * nameptr;
  char * splitptr;
  int16_t intvalue= -99;
  Serial1.readBytesUntil(rxterminator, rxbuffer, RX_BUFFERSIZE);
  splitptr = strtok(rxbuffer, ":");
    if (splitptr != NULL) {
      nameptr = splitptr;
      splitptr = strtok(NULL, ":");
      intvalue= atoi(splitptr);
    }
// check if received name is known, initvalue for i= (Usersettings.length -1)
    for(uint8_t i=14;i>=0;i--){
      if(strcmp(nameptr,Usersettings[i].action)==0){
        if(intvalue!= -99){   // update only if valid value received
          Usersettings[i].value= intvalue;
        }
        // only for debugging
        #ifdef DEBUG_COMMUNICATION
          Serial.print(F("Updated "));
          Serial.print(Usersettings[i].action);
          Serial.print(F(" mit Wert: "));
          Serial.println(Usersettings[i].value);
        #endif
        break;
      }
    }
  #ifdef DEBUG_COMMUNICATION
    Serial.print(F("Splitted Namenteil: "));
    Serial.println(nameptr);
    Serial.print(F("Splitted Wertteil: "));
    Serial.println(splitptr);
    Serial.print(F("Wertteil als Intwert: "));
    Serial.println(intvalue,DEC);
    Serial.println();
  #endif
}


/************************************************************************/
/**
*	@brief  Sendet Daten aus der Variable Systemsettings über die UART Schnittstelle.
*
*         Die Funktion transmitSerialData() ist das Gegenstück zur Funktion
*         receiveSerialData(). Sie liest die abgelegten Werte des Datenarrays
*         #Systemsettings beim Index, welcher als Funktionsargument (uint8_t)
*         übergeben werden muss.
*
* @param  settingindex
*         Wählt den Index des Array Systemsettings dessen Inhalte,
*         sprich die Datenfelder Systemsettings#action und Systemsettings#value,
*         gesendet werden sollen.
*
* @return @c void
*
* @note   Die Befehlslänge darf die Anzahl Zeichen definiert in #RX_BUFFERSIZE
*         nicht überschreiten! Der Overhead beträgt jeweils drei Zeichen, weshalb
*         die die nutzbare Zeichenlänge jeweils RX_BUFFERSIZE -3 beträgt.
*/
/************************************************************************/
void transmitSerialData(uint8_t settingindex){
  if(Usersettings[0].value== 1 && settingindex< 8){
      if(settingindex==5){
          combineZustandbits(); // Auslesen der Zustände aus Systemzustand.
      }
    Serial1<<Systemsettings[settingindex].action<<":"<<Systemsettings[settingindex].value<<"\n";
    Serial1.flush();
  }
  // only for debugging
  #ifdef DEBUG_PROGRAM_FLOW
      Serial.print(F("Executed: transmitSerialData"));
      Serial.println(F(" ->Modul: User"));
  #endif
}

/************************************************************************/
/**
*   @brief Periodisches Update der Werte ans HMI senden.
*
*   Aktualisiert die Werte für das HMI periodisch. Alle Einträge des Arrays
*   #Systemsettings werden dabei nacheinander an die Funktion
*   transmitSerialData() übergeben und von dieser an das HMI gesendet.
*   Die Aktualisierung erfolgt jede Sekunde.
*/
/************************************************************************/
void updateHMI(){
    static int8_t lastupdate;
    if(lastupdate!=blink1Hz){
        for(int8_t j=7;j>=0;j--){
            transmitSerialData(j);
        }
        lastupdate=blink1Hz;
    }
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: updateHMI"));
        Serial.println(F(" ->Modul: User"));
    #endif
}

  /************************************************************************/
  /**
  *	@brief Fasst die Zustände aller Bitfelder der Variable #Systemzustand zusammen.
  *
  *    Mithilfe der Funktion combineZustandbits() können alle Werte (der
  *    Bitfelder in der Strukturvariable #Systemzustand) in einer 16bit Variable
  *    abgebildet werden. Dadurch können alle Felder in Einer statt 16
  *    Nachrichten über die serielle Verbindung geschickt werden und damit
  *	   erheblich Ressourcen eingespart werden. \n
  *    Die 16 verschiedenen Meldungsfelder werden den 16bits der Variable
  *    mittels Bitmanipulation zugewiesen. Durch die bitweise OR-Verknüpfung
  *    können die Bits der Variable @c zustaende nicht von 1 nach 0 geändert
  *    werden, weshalb es wichtig ist, dieser bei Initialisieung explizit
  *    den Wert 0 zuzuweisen.\n
  *    Diese Bitcodierung muss bei einer Übertragung bekannt sein damit im
  *    Empfänger das Datenpaketes auch entsprechend wieder decodiert werden
  *    kann.
  *
  * @return @c void
  *
  * @see SYSTEMZUSTAND
  */
  /************************************************************************/
void combineZustandbits(){
  int16_t zustaende=0;
  if(Systemzustand.tarifsperre){zustaende |= 1 << 14;}  //01xxxxxx...
  if(Systemzustand.motorschutz){zustaende |= 1 << 13;}  //0x1xxxxx...
  if(Systemzustand.druckhoch){zustaende |= 1 << 12;}    //0xx1xxxx...
  if(Systemzustand.drucktief){zustaende |= 1 << 11;}    //0xxx1xxx...
  if(Systemzustand.manbetrieb){zustaende |= 1 << 10;}   //0xxxx1xx...
  if(Systemzustand.reduziert){zustaende |= 1 << 9;}     //0xxxxx1x...
  if(Systemzustand.autobetrieb){zustaende |= 1 << 8;}   //0xxxxxx1...

  if(Systemzustand.defrost){zustaende |= 1 << 7;}       //...1xxxxxxx
  if(Systemzustand.heizpumpe){zustaende |= 1 << 6;}     //...x1xxxxxx
  if(Systemzustand.vorlaufregler){zustaende |= 1 << 5;} //...xx1xxxxx
  if(Systemzustand.ladepumpe){zustaende |= 1 << 4;}     //...xxx1xxxx
  if(Systemzustand.bypass){zustaende |= 1 << 3;}        //...xxxx1xxx
  if(Systemzustand.ventilator){zustaende |= 1 << 2;}    //...xxxxx1xx
  if(Systemzustand.kompressor){zustaende |= 1 << 1;}    //...xxxxxx1x
  if(Systemzustand.sumpfheizung);{zustaende |= 1 << 0;} //...xxxxxxx1

  Systemsettings[5].value= zustaende;

  // only for debugging Inputvalues
  #ifdef DEBUG_SYSTEMZUSTAND
      Serial.print(F("Systemzustand binär, VALUE: "));
      Serial.println(zustaende,BIN);
      Serial.println();
  #endif
  // only for debugging
  #ifdef DEBUG_PROGRAM_FLOW
      Serial.print(F("Executed: combineZustandbits"));
      Serial.println(F(" ->Modul: User"));
  #endif
}


/****************************************************************/
/**
*	@brief Gibt die vom User eingestellte Heizkurven-Parallelverschiebung zurück.
*
*     Je nach aktivem Betriebsmodus wird entweder die Parallelverschiebung für
*     den reduzierten Betrieb oder den Normalbetrieb zurückgegeben.
*     Die wählbaren Verschiebungsstufen gehen jeweils von -5...+5
*
* @return Eingestellte Heizkurvenverschiebung des aktiven Betriebsmodus als @c int8_t.
*/
/************************************************************************/
 int8_t getParallelvs(){
     // only for debugging
     #ifdef DEBUG_PROGRAM_FLOW
         Serial.print(F("Executed: getParallelvs"));
         Serial.println(F(" ->Modul: User"));
     #endif
     if(Usersettings[1].value==2){     // falls reduzierter Betrieb gewählt
         return Usersettings[3].value; // Rückgabe von Parallelvs reduziert
     }
     else{
         return Usersettings[2].value; // sonst Rückgabe Parallelvs Normalbetr
	}
 }

 /************************************************************************/
 /**
 *	@brief Bereitstellen der eingestellten Heizkurvenstufe.
 *
 *         Diese Funktion liest die vom Nutzer eingestellte Stufe
 *         der Heizkurve (1...11) ein und gibt diese zurück.
 *        Für die Berechnung der Heizkurvenfunktion ist die Funktion
 *        calcTvorlauf() zuständig.
 *
 * @return eingestellte Heizkurvenstufe als @c int8_t.
 */
 /************************************************************************/
 int8_t getKurvenstufe(){
     // only for debugging
     #ifdef DEBUG_PROGRAM_FLOW
         Serial.print(F("Executed: getKurvenstufe"));
         Serial.println(F(" ->Modul: User"));
     #endif
	return Usersettings[4].value;
 }

 /************************************************************************/
 /**
 *	@brief Prüft ob eine reduzierte Vorlauftemperatur gewünscht wird.
 *
 *          Die Funktion modeReduziert() gibt den Wert 1 zurück, falls
 *          mit reduzierter Vorlauftemperatur gefahren werden soll.
 *          Die entprechende Moduswahl wird vom Benutzer gewählt.
 *
 * @return TRUE falls der reduzierte Modus gewählt ist, ansonsten FALSE.
 */
 /************************************************************************/
 int8_t modeReduziert(){
     // only for debugging
     #ifdef DEBUG_PROGRAM_FLOW
         Serial.print(F("Executed: modeReduziert"));
         Serial.println(F(" ->Modul: User"));
     #endif
     if(Usersettings[1].value==2){
         return 1;
     }
	 else{return 0;}
 }

 /************************************************************************/
 /**
 *	@brief  Auslesen der lokalen, reduzierten Parallelverschiebungsstufe.
 *
 *          Liest den Wert des Analogeingangs #PIN_PARALLELVS_RED
 *          an welchem die Verschiebungsstufe der Heizkurve im reduzierten
 *          Betrieb per Drehregler, für die lokale respektive die
 *          Notbedienung, eingestellt werden kann. Die Funktion rechnet
 *          den Eingangswert in die Stufen -5...+5 um.
 *
 * @return Eingestellte Stufe im Bereich -5...+5 als @c int8_t.
 */
 /************************************************************************/
int8_t readLocalParallelvsRed(){
    int16_t inputval= analogRead(PIN_PARALLELVS_RED);
    // only for debugging Inputvalues
    #ifdef DEBUG_INPUTVALUES
        Serial.print(F("AI: Local ParaVs Red, VALUE: "));
        Serial.println(map(inputval,0,1023,-5,5));
    #endif
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: readLocalParallelvsRed"));
        Serial.println(F(" ->Modul: User"));
    #endif
    return map(inputval,0,1023,-5,5); // remap ADC-Valuerange to -5...5
}

/************************************************************************/
/**
*	@brief  Auslesen der lokalen, normalen Parallelverschiebungsstufe.
*
*          Liest den Wert des Analogeingangs #PIN_PARALLELVS_NORM
*          an welchem die Verschiebungsstufe der Heizkurve im normalen
*          Betrieb per Drehregler, für die lokale respektive die
*          Notbedienung, eingestellt werden kann. Die Funktion rechnet
*          den Eingangswert in die Stufen -5...+5 um.
*
* @return Eingestellte Stufe im Bereich -5...+5 als @c int8_t.
*/
/************************************************************************/
int8_t readLocalParallelvsNorm(){
    int16_t inputval= analogRead(PIN_PARALLELVS_NORM);
    // only for debugging Inputvalues
    #ifdef DEBUG_INPUTVALUES
        Serial.print(F("AI: Local ParaVs Normal, VALUE: "));
        Serial.println(map(inputval,0,1023,-5,5));
    #endif
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: readLocalParallelvsNorm"));
        Serial.println(F(" ->Modul: User"));
    #endif
    return map(inputval,0,1023,-5,5); // remap ADC-Valuerange to -5...5
}

/************************************************************************/
/**
*	@brief  Auslesen der lokalen, Stufenwahl der Heizkurve.
*
*          Liest den Wert des Analogeingangs #PIN_HEIZKURVENSTUFE
*          an welchem die Steigung der Heizkurvenfunktion
*          per Drehregler, für die lokale respektive die
*          Notbedienung, eingestellt werden kann. Die Funktion rechnet
*          den Eingangswert in die Stufen 1...11 um.
*
* @return Eingestellte Stufe im Bereich 1...11 als @c int8_t.
*/
/************************************************************************/
int8_t readLocalKurvenstufe(){
    int16_t inputval= analogRead(PIN_HEIZKURVENSTUFE);
    // only for debugging Inputvalues
    #ifdef DEBUG_INPUTVALUES
        Serial.print(F("AI: Local Heizkurve, VALUE: "));
        Serial.println(map(inputval,0,1023,1,11));
    #endif
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: readLocalKurvenstufe"));
        Serial.println(F(" ->Modul: User"));
    #endif
    return map(inputval,0,1023,1,11); // remap ADC-Valuerange to 1...11
}

/************************************************************************/
/**
*	@brief  Erzwingt die Verwendung der lokal eingestellten Parametern.
*
*   Die Funktion forceLocalBedienung() forciert die Verwendung
*   der lokalen Bedienungswerte (Notbedienung) wenn diese aktiviert
*   ist. Dadurch werden die gespeicherten Einstellungen überschrieben
*   und betreffende Eingaben des HMI ignoriert.\n
*   Die lokale Bedienung kann einerseits mittels Hardwareschalter
*   aktiviert werden anderseits mittels optionalem Funktionsparameter.
*   Danach werden die lokalen Taster ausgewertet und der entsprechende
*   Betriebsmodus ausgewählt.
*
* @todo Die Taster 5 und 6 sind noch unbelegt und somit Funktionslos.
*       Die Einbindung dieser kann beliebig erfolgen.
*
* @param    reqForce
*           Funktionsaufruf mit Argumentwert ungleich 0 als @c int8_t.
*
* @return TRUE falls lokale Bedienung aktiv, ansonsten FALSE.
*/
/************************************************************************/
int8_t forceLocalBedienung(int8_t reqForce){
    int8_t enabled= !digitalRead(PIN_FORCE_LOCAL);
    // only for debugging inputvalues
    #ifdef DEBUG_INPUTVALUES
        Serial.print(F("DI: Force Local, VALUE: "));
        Serial.println(enabled);
    #endif
    if(enabled || reqForce){
        int16_t buttonread= analogRead(PIN_LOCALBUTTONS);
        // reserve Button (read: 0...10)
        if(buttonread<50){;}
        // reserve Button (read: 127...137)
        else if(buttonread>100 && buttonread< 200){;}
        // set to Auto Normal (read: 241...253)
        else if(buttonread>200 && buttonread< 300){Usersettings[1].value=1;}
        // set to Auto reduziert (read: 352...366)
        else if(buttonread>300 && buttonread< 400){Usersettings[1].value=2;}
        // set to Manual Mode (read: 445...460)
        else if(buttonread>400 && buttonread< 500){Usersettings[1].value=3;}
        // set to Standby (read: 523...531)
        else if(buttonread>500 && buttonread< 600){Usersettings[1].value=0;}
        // only for debugging inputvalues
        #ifdef DEBUG_INPUTVALUES
            Serial.print(F("AI: Local Buttons, VALUE: "));
            Serial.println(buttonread);
        #endif

        Usersettings[4].value= readLocalKurvenstufe(); // overwrite Heizkurvenstufe
        Usersettings[3].value= readLocalParallelvsRed(); // overwrite Verschiebung reduzierter Betrieb
        Usersettings[2].value= readLocalParallelvsNorm(); // overwrite Verschiebung Normalbetrieb
    }
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: forceLocalBedienung"));
        Serial.println(F(" ->Modul: User"));
    #endif
    getBetriebsmodus(); // Update Mode to Systemsettings and show LEDs
    return enabled;
}

/************************************************************************/
/**
*   @brief  Aktualisiert den Betriebsmodus und zeigt diesen lokal an.
*
*           Die Funktion getBetriebsmodus() liest den Wert der Moduswahl
*           aus der Strukturvariable #Usersettings und aktualisiert damit
*           die entsprechenden Bitfields der Variable #Systemzustand.
*           Gleichzeitig aktualisiert sie die Digitalausgänge mit den
*           optischen Zustandsanzeigen.
*
* @return Vom User gewählter Betriebsmodus: @<tt@> 0 Standby, 1 Auto Normal, 2 Auto reduziert, 3 Manuell, 4 Störung.
*/
/************************************************************************/
int8_t getBetriebsmodus(){
    // if pressure alarm overwrite user-selected mode with error mode
    if(DiStates.status_hd || DiStates.status_nd){Usersettings[1].value=4;}
    // if motor protection alarm overwrite user-selected mode with error mode
    else if(DiStates.status_motprotect){Usersettings[1].value=4;}
    else{;}

    int8_t mode= Usersettings[1].value; // 0 Stdby, 1 AutoN, 2 AutoR, 3 Man, 4 Error
    static uint8_t savebetriebsmodus; // save current mode to restore after alarm ack.

    // Standby mode
    if(mode==0){
        Systemzustand.autobetrieb=0; // update Statusbit
        Systemzustand.manbetrieb=0; // update Statusbit
        Systemzustand.reduziert=0; // update Statusbit
        digitalWrite(PIN_LED_STBY,HIGH); // Stby mode Indicator ON
        digitalWrite(PIN_LED_AUTONORM,LOW);
        digitalWrite(PIN_LED_AUTORED,LOW);
        digitalWrite(PIN_LED_MAN,LOW);
        digitalWrite(PIN_LED_ALARM,LOW);
        digitalWrite(PIN_LED_HDND,HIGH); // pressure ok, Indicator ON
        savebetriebsmodus=0;
    }
    // auto mode normal
    else if(mode==1){
        Systemzustand.autobetrieb=1; // update Statusbit
        Systemzustand.reduziert=0; // update Statusbit
        Systemzustand.manbetrieb=0; // update Statusbit
        digitalWrite(PIN_LED_STBY,LOW);
        digitalWrite(PIN_LED_AUTONORM,HIGH); // automode normal ON
        digitalWrite(PIN_LED_AUTORED,LOW);
        digitalWrite(PIN_LED_MAN,LOW);
        digitalWrite(PIN_LED_ALARM,LOW);
        digitalWrite(PIN_LED_HDND,HIGH); // pressure ok, Indicator ON
        savebetriebsmodus=1;
    }
    // auto red. mode
    else if(mode==2){
        Systemzustand.autobetrieb=1; // update Statusbit
        Systemzustand.reduziert=1; // update Statusbit
        Systemzustand.manbetrieb=0; // update Statusbit
        digitalWrite(PIN_LED_STBY,LOW);
        digitalWrite(PIN_LED_AUTONORM,LOW);
        digitalWrite(PIN_LED_AUTORED,HIGH); // automode reduced ON
        digitalWrite(PIN_LED_MAN,LOW);
        digitalWrite(PIN_LED_ALARM,LOW);
        digitalWrite(PIN_LED_HDND,HIGH); // pressure ok, Indicator ON
        savebetriebsmodus=2;
    }
    // manual mode, turn on LED
    else if(mode==3){
        Systemzustand.autobetrieb=0; // update Statusbit
        Systemzustand.reduziert=0; // update Statusbit
        Systemzustand.manbetrieb=1; // update Statusbit
        digitalWrite(PIN_LED_STBY,LOW);
        digitalWrite(PIN_LED_AUTONORM,LOW);
        digitalWrite(PIN_LED_AUTORED,LOW);
        digitalWrite(PIN_LED_MAN,HIGH); // manual mode ON
        digitalWrite(PIN_LED_ALARM,LOW);
        digitalWrite(PIN_LED_HDND,HIGH); // pressure ok, Indicator ON
        savebetriebsmodus=3;
    }
    // blinking LED if error occurs
    else if(mode== 4){
        Systemzustand.autobetrieb=0; // update Statusbit
        Systemzustand.reduziert=0; // update Statusbit
        Systemzustand.manbetrieb=0; // update Statusbit
        digitalWrite(PIN_LED_STBY,LOW);
        digitalWrite(PIN_LED_AUTONORM,LOW);
        digitalWrite(PIN_LED_AUTORED,LOW);
        digitalWrite(PIN_LED_MAN,LOW);
        // if pressure alarm switch is active blink Alarm and Pressure LED
        if(DiStates.status_hd || DiStates.status_nd){
            digitalWrite(PIN_LED_HDND,blink1Hz); // blink pressure & alarm LED
            digitalWrite(PIN_LED_ALARM,blink1Hz);
        }
        // if motor protection is switched on, blink alarm LED
        else if(DiStates.status_motprotect){
            digitalWrite(PIN_LED_ALARM,blink1Hz); // blink only alarm LED
        }
        // if fault state went back to normal, light alarm LED solid & blink last selected mode to restart. Also activate pressure indicator.
        // user has to ack alarm by pressing the button for mode again
        else{
            digitalWrite(PIN_LED_HDND,HIGH);
            digitalWrite(PIN_LED_ALARM,HIGH);
            if(savebetriebsmodus==0){digitalWrite(PIN_LED_STBY,blink1Hz);}
            else if(savebetriebsmodus==1){digitalWrite(PIN_LED_AUTONORM,blink1Hz);}
            else if(savebetriebsmodus==2){digitalWrite(PIN_LED_AUTORED,blink1Hz);}
            else if(savebetriebsmodus==3){digitalWrite(PIN_LED_MAN,blink1Hz);}
        }
    }
    // only for debugging Outputstates
    #ifdef DEBUG_OUTPUTVALUES
        Serial.print(F("DO: LED Standby , VALUE: "));
        Serial.println(digitalRead(PIN_LED_STBY));
        Serial.print(F("DO: LED Normalbetrieb, VALUE: "));
        Serial.println(digitalRead(PIN_LED_AUTONORM));
        Serial.print(F("DO: LED Reduziert, VALUE: "));
        Serial.println(digitalRead(PIN_LED_AUTORED));
        Serial.print(F("DO: LED Manuell , VALUE: "));
        Serial.println(digitalRead(PIN_LED_MAN));
        Serial.print(F("DO: LED HD/ND, VALUE: "));
        Serial.println(digitalRead(PIN_LED_HDND));
        Serial.print(F("DO: LED Alarm , VALUE: "));
        Serial.println(digitalRead(PIN_LED_ALARM));
        Serial.print(F("DO: LED Betrieb, VALUE: "));
        Serial.println(digitalRead(PIN_LED_KEY5));
        Serial.println();
    #endif
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: getBetriebsmodus"));
        Serial.println(F(" ->Modul: User"));
    #endif
    return mode;
}

/************************************************************************/
/**
*   @brief Benutzersteuerungs Hauptfunktion zum Aufruf im Mainloop.
*
*           Oberste Ebene der Benutzersteuerung, welche die weiteren
*           Benutzerfunktionen direkt oder indirekt aufruft.
*/
/************************************************************************/
void userMain(){
    forceLocalBedienung(); // check if local usercontrol active

    updateHMI();   // send Values to HMI every second
    // only for debugging
    #ifdef DEBUG_PROGRAM_FLOW
        Serial.print(F("Executed: userMain"));
        Serial.println(F(" ->Modul: User"));
    #endif
}
