/**
 *  @file wpSensorik.h
 *  @brief Header zum Sensorikmodul.
 *   
 *  @author Daniel Schmissrauter
 *  @date 	09.01.2018
 */

#ifndef WPSENSORIK_H
#define WPSENSORIK_H


int16_t getAussentemp();
int16_t getKondenstemp();
int16_t getSpeichertemp();
int16_t getVorlauftemp();

#endif