
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
        #ifdef DEBUG_OVER_SERIAL
          Serial.print("Updated ");
          Serial.print(Usersettings[i].action);
          Serial.print(" mit Wert: ");
          Serial.println(Usersettings[i].value);
        #endif
        break;
      }
    }
  #ifdef DEBUG_OVER_SERIAL
    Serial.print("Splitted Namenteil: ");
    Serial.println(nameptr);
    Serial.print("Splitted Wertteil: ");
    Serial.println(splitptr);
    Serial.print("Wertteil als Intwert: ");
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
  int16_t zustaende=0b0;
  zustaende |= (Systemzustand.sumpfheizung << 0);
  zustaende |= (Systemzustand.kompressor << 1);
  zustaende |= (Systemzustand.ventilator << 2);
  zustaende |= (Systemzustand.bypass << 3);
  zustaende |= (Systemzustand.ladepumpe << 4);
  zustaende |= (Systemzustand.vorlaufregler << 5);
  zustaende |= (Systemzustand.heizpumpe << 6);
  zustaende |= (Systemzustand.defrost << 7);
  zustaende |= (Systemzustand.autobetrieb << 8);
  zustaende |= (Systemzustand.reduziert << 9);
  zustaende |= (Systemzustand.manbetrieb << 10);
  zustaende |= (Systemzustand.drucktief << 11);
  zustaende |= (Systemzustand.druckhoch << 12);
  zustaende |= (Systemzustand.motorschutz << 13);
  zustaende |= (Systemzustand.tarifsperre << 14);
  zustaende |= (Systemzustand.reserved_msb << 15);
  Systemsettings[5].value= zustaende;
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
     if(Usersettings[1].value==2){
         return Usersettings[3].value; // reduziert
     }
     else{
         return Usersettings[2].value; // Normalbetrieb
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

    if(enabled || reqForce){
        int8_t buttonread= analogRead(PIN_LOCALBUTTONS);
        // set to Stby
        if(buttonread<50){Usersettings[1].value=0;}
        // set to Auto Normal
        else if(buttonread>100 && buttonread< 200){Usersettings[1].value=1;}
        // set to Auto reduziert
        else if(buttonread>200 && buttonread< 300){Usersettings[1].value=2;}
        // set to Manual Mode
        else if(buttonread>300 && buttonread< 400){Usersettings[1].value=3;}
        // reserve Button
        else if(buttonread>400 && buttonread< 500){;}
        // reserve Button
        else if(buttonread>500 && buttonread< 600){;}

        Usersettings[4].value= readLocalKurvenstufe(); // overwrite Heizkurvenstufe
        Usersettings[3].value= readLocalParallelvsRed(); // overwrite Verschiebung reduzierter Betrieb
        Usersettings[2].value= readLocalParallelvsNorm(); // overwrite Verschiebung Normalbetrieb
    }
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
    int8_t mode= Usersettings[1].value; // 0 Stdby, 1 AutoN, 2 AutoR, 3 Man, 4 Error
    // Standby mode
    if(mode==0){
        Systemzustand.autobetrieb=0;
        Systemzustand.manbetrieb=0;
        Systemzustand.reduziert=0;
        digitalWrite(PIN_LED_STBY,HIGH);
        digitalWrite(PIN_LED_AUTONORM,LOW);
        digitalWrite(PIN_LED_AUTORED,LOW);
        digitalWrite(PIN_LED_MAN,LOW);
        digitalWrite(PIN_LED_ALARM,LOW);
    }
    // auto mode normal
    else if(mode==1){
        Systemzustand.autobetrieb=1;
        Systemzustand.reduziert=0;
        Systemzustand.manbetrieb=0;
        digitalWrite(PIN_LED_STBY,LOW);
        digitalWrite(PIN_LED_AUTONORM,HIGH);
        digitalWrite(PIN_LED_AUTORED,LOW);
        digitalWrite(PIN_LED_MAN,LOW);
        digitalWrite(PIN_LED_ALARM,LOW);
    }
    // auto red. mode
    else if(mode==2){
        Systemzustand.autobetrieb=1;
        Systemzustand.reduziert=1;
        Systemzustand.manbetrieb=0;
        digitalWrite(PIN_LED_STBY,LOW);
        digitalWrite(PIN_LED_AUTONORM,LOW);
        digitalWrite(PIN_LED_AUTORED,HIGH);
        digitalWrite(PIN_LED_MAN,LOW);
        digitalWrite(PIN_LED_ALARM,LOW);
    }
    // manual mode, turn on LED
    else if(mode==3){
        Systemzustand.autobetrieb=0;
        Systemzustand.reduziert=0;
        Systemzustand.manbetrieb=1;
        digitalWrite(PIN_LED_STBY,LOW);
        digitalWrite(PIN_LED_AUTONORM,LOW);
        digitalWrite(PIN_LED_AUTORED,LOW);
        digitalWrite(PIN_LED_MAN,HIGH);
        digitalWrite(PIN_LED_ALARM,LOW);
    }
    // blinking LED if error occurs
    else if(mode== 4){
        digitalWrite(PIN_LED_ALARM,blink1Hz);
    }
    // check pressure and blink if too high/low
    if(Systemzustand.druckhoch || Systemzustand.drucktief){
        Systemzustand.autobetrieb=0;
        Systemzustand.reduziert=0;
        Systemzustand.manbetrieb=0;
        digitalWrite(PIN_LED_STBY,LOW);
        digitalWrite(PIN_LED_AUTONORM,LOW);
        digitalWrite(PIN_LED_AUTORED,LOW);
        digitalWrite(PIN_LED_MAN,LOW);
        digitalWrite(PIN_LED_HDND,blink1Hz);
    }
    // activate pressure indicator if pressure within range
    else{digitalWrite(PIN_LED_HDND,HIGH);}
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
    getBetriebsmodus(); // Update Mode to Systemsettings and show LEDs
    updateHMI();   // send Values to HMI every second
}
