
/**
 *  @file wpUser.ino
 *  @brief Benutzersteuerung und Kommunikation mit Frontendcontroller.
 *  
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

 #include "wpUser.h"
 
 int8_t getKurvenstufe(){
	return 5; 
 }
 
 int8_t getParallelvs(){
	return 0;
 }
 
 int8_t modeReduziert(){
	 return 0;
 }
 
 int8_t getKeys(){
	 
 }
 
 
 void wpMenu() {
	 static int8_t userinput;
	 static int8_t menu;
	 static int8_t newmenu;
	 
	 
	 if (usermenu[menu].callwhenvisible != NULL) {
		 // call function
		 usermenu[menu].callwhenvisible();
	 }
	 userinput = getKeys();
	 
	 switch (userinput) {
		 default:
		 case NO_KEY:
		 newmenu = -1;
		 break;
		 case T1_UP:
		 newmenu = usermenu[menu].up;
		 break;
		 case T2_ENTER: {
			 if (usermenu[menu].applywithenter != 0) {
				 usermenu[menu].applywithenter();
			 }
			 newmenu = usermenu[menu].enter;
			 break;
		 }
		 case T3_DOWN:
		 newmenu = usermenu[menu].down;
		 break;
	 }
	 if ((newmenu >= 0) && (newmenu != menu))
	 {
		 menu = newmenu;
		 lcd.clear();
		 lcd.print(usermenu[menu].menutext);
		 lcd.setCursor(0, 1);
		 lcd.print(usermenu[menu].menuoption);
	 }
 }