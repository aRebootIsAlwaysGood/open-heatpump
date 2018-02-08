/**
 *  @file wpUser.h
 *  @brief Header zum Modul Benutzersteuerung.
 *  
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

#ifndef WPUSER_H
#define WPUSER_H

typedef void(*PFENTER)();     // Pointer to Function for apply changes (Enter Key)
typedef void(*PFMENUPRINT)(); // Pointer to Function (call while menu entry visible)

// structure of navigation menu
struct LCD_MENU
{
	char * menutext;
	char * menuoption;
	int up, enter, down;
	PFMENUPRINT callwhenvisible;
	PFENTER applywithenter;
};

void wpMenu();
int8_t getKeys();
int8_t getKurvenstufe();
int8_t getParallelvs();
int8_t modeReduziert();

#endif