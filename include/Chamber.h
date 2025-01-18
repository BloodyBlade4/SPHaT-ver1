#ifndef CHAMBER_H
#define CHAMBER_H

#include "Arduino.h"
#include <Wire.h>
#include <LM75A.h>



#define TEMP_MARGIN_ERROR 2.0
#define CHAMBER_OVERHEAT_TEMP 80
#define HEATER_OVERHEAT_TEMP 90


class Chamber
{
public:
    //Settings from the user. 
    float setTemperature = 0.0f;
    float setHumidity = 0.0f;

    //current chamber status. 
    float currentTemperature = 0.0f;
    float currentHumidity = 0.0f;

    float currentHeaterTemp = 0.0f;

    enum STATE {
        OFF,                //chamber off. 
        IDLE,           // waiting. at designated temp/humidity
        OPERATING,        // heater/dehumidifier currently on.
        CHAMBER_OVERHEAT,// Chamber is over user settings + margin
        HEATER_OVERHEAT, // Heater is past it's designated temperature. 
        UNKNOWN_ERROR    // unknown error. This shouldn't be a thing. 
    };
    enum STATE heaterState = OFF;
    enum STATE dehumidifierState = OFF;

    

    Chamber(LM75A sensorOne,int sensorOneAddr,
                LM75A sensorTwo,int sensorTwoAddr,
                LM75A sensorThree,int sensorThreeAddr,
                int HeaterSSRPin);
    void init();

    //Function called via a loop. does all calculations. 
    void chamberRounds();

    //reads from the sensor to update current chamber temp and humidity status. 
    void updateCurrentTemperature();
    void updateCurrentHumidity();

    private: 
    // Create I2C LM75A instance
    LM75A lm75a_sensor_one;
    LM75A lm75a_sensor_two;
    LM75A lm75a_sensor_three_heater;
    //addresses
    int sensorAddrOne, sensorAddrTwo, sensorAddrThree;
    int HeaterSSRPin;

    //Function that rounds a float to 2 decimal places for printing. 
    float round2dp(float f);
    //checks that the temperature is valid and returns it in a valid format. 
    float temperatureValid(float temp);
    // Subroutine to turn on the heating element
    void turnOnHeatingElement();
    // Subroutine to turn off the heating element
    void turnOffHeatingElement();


    //Variables  



};

#endif //Chamber_H
