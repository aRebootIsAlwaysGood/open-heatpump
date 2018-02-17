
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



 /***************************************************************************************/
 /**
 * @brief Liest über UART empfangene Daten ein und speichert diese.
 *        Wenn am Serialport 1 vom HMI gesendete Daten anstehen, wird die Funktion
 *        receiveSerialData() von der Funktion serialEvent1() aufgerufen.
 *        Die im Schnittstellen-Buffer enthaltene Daten werden eingelesen bis
 *        ein LINEFEED erkannt wird, das Empfangsarray @c rxbuffer
 *        voll ist oder 20ms keine weiteren Daten empfangen werden.
 *        Mittels RX_BUFFERSIZE kann die Anzahl empfangbarer ASCII-Zeichen/Bytes
 *        pro Empfangseinheit definiert werden, wobei gilt:
 *        Buffersize= empfangbare Zeichen + 3
 *        Durch die Begrenzung der Befehlslänge kann ein Buffer-Overflow effektiv
 *        verhindert werden.
 *        Die Funktion teilt empfangene Zeichenkette in Namen- und Wertteil auf
 *        wobei sie folgendes Befehlsformat erwartet:
 *        Data[<=RX_BUFFERSIZE]= PARAMNAME:PARAMVALUE\n  (PARAMVALUE < +/-16000)
 *        Der Namenteil wird anschliessend mit den Optionsnamen der, in der
 *        Struktur Usersettings enthaltenen, Einträge verglichen und bei
 *        Übereinstimmung wird der empfangene Wertteil als Optionswert
 *        übernommen.
 *        Wird ein Befehlsname ohne Wert gesendet, wird der Defaultwert
 *        -1 zugewiesen und ggf. als Optionswert übernommen.
 *
 * @return @c void
 */
 /***************************************************************************************/
void receiveSerialData() {
  char rxterminator = '\n'; // explicit initialisation for type char needed!
  char rxbuffer[RX_BUFFERSIZE] = ""; // init Receivingbuffer
  char * nameptr;
  char * splitptr;
  int16_t intvalue= -1;
  Serial1.readBytesUntil(rxterminator, rxbuffer, RX_BUFFERSIZE);
  splitptr = strtok(rxbuffer, ":");
    if (splitptr != NULL) {
      nameptr = splitptr;
      splitptr = strtok(NULL, ":");
      intvalue= atoi(splitptr);
    }
// check if received name is known, initvalue for i==Usersettings.length -1
    for(uint8_t i=14;i>=0;i--){
      if(strcmp(nameptr,Usersettings[i].action)==0){
        Usersettings[i].value= intvalue;
        #ifdef DEBUG_OVER_SERIAL
        // only for debugging
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
*	@brief Sendet Daten aus der Variable Systemsettings über die UART Schnittstelle.
*           Die Funktion transmitSerialData() ist das Gegenstück zur Funktion
*           receiveSerialData(). Sie liest die abgelegten Werte des Datenarrays
*           @c Systemsettings beim Index, welcher als Funktionsargument übergeben
*           werden muss.
*
* @param    settingindex
*           Wählt den Index der Datenstruktur Systemsettings, dessen Inhalt
*           gesendet werden soll.
*
* @return @c void
*/
/************************************************************************/
void transmitSerialData(uint8_t settingindex){
  if(Usersettings[0].value== 1 && settingindex< 8){
      if(settingindex==5){
          combineZustandbits(); /**< Auslesen der Zustände aus Systemzustand. */
      }
    Serial1<<Systemsettings[settingindex].action<<":"<<Systemsettings[settingindex].value<<"\n";
    Serial1.flush();
  }
}
  /************************************************************************/
  /**
  *	@brief Fasst die Zustände aller Bitfelder der Variable Systemzustand
  *       zusammen.
  *
  *       Mithilfe der Funktion combineZustandbits() können alle Werte (der
  *       Bitfelder in der Struktur Systemzustand) in einer 16bit Variable
  *       abgebildet werden. Dadurch können alle Felder in Einer statt 16
  *       Nachrichten über die serielle Verbindung geschickt werden und damit
  *	      erheblich Ressourcen eingespart werden.
  *       Die 16 verschiedenen Meldungsfelder werden den 16bits der Variable
  *       mittels Bitmanipulation zugewiesen. Durch die bitweise OR-Verknüpfung
  *       können die Bits der Variable @c zustaende nicht von 1 nach 0 geändert
  *       werden, weshalb es wichtig ist, dieser bei Initialisieung explizit
  *       den Wert 0 zuzuweisen.
  *       Diese Bitcodierung muss bei einer Übertragung bekannt sein damit im
  *       Empfänger das Datenpaketes auch entsprechend wieder decodiert werden
  *       kann.
  *
  * @return @c int16_t
  *         Alle Systemzustandsmeldungen in einer Variable binär codiert.
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
  zustaende |= (Systemzustand.alarm << 14);
  zustaende |= (Systemzustand.reserved_msb << 15);
  Systemsettings[5].value= zustaende;
}


/************************************************************************/
/**
*	@brief Gibt die vom User eingestellte Heizkurven-Parallelverschiebung zurück.
*           Je nach aktivem Betriebsmodus wird entweder die Parallelverschiebung für
*           den reduzierten Betrieb oder den Normalbetrieb zurückgegeben.
*           Die wählbaren Verschiebungsstufen gehen jeweils von -5...+5
*
* @return eingestellte Heizkurvenverschiebung des aktiven Betriebsmodus.
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
 *	@brief Rückgabe der eingestellten Heizkurvenstufe.
 *         Diese Funktion liest die vom Nutzer eingestellte Stufe
 *          der Heizkurve ein und gibt diese zurück. Stufen 1...11.
 *
 * @return eingestellte Heizkurvenstufe.
 */
 /************************************************************************/
 int8_t getKurvenstufe(){
	return Usersettings[4].value;
 }

 /************************************************************************/
 /**
 *	@brief Prüft ob eine reduzierte Vorlauftemperatur gewünscht wird.
 *          Die Funktion modeReduziert() gibt den Wert 1 zurück, falls
 *          mit reduzierter Vorlauftemperatur geheizt werden soll.
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
