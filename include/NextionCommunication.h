#ifndef NEXTIONCOMMUNICATION_H
#define NEXTIONCOMMUNICATION_H

#include "Arduino.h"
#include <nextion/Nextion.h>



/* TODO:
    1. Remove decimal places on homescreen. this is hiding the 'C' and 'F" and '%'. decimal aren't needed
    2. Add set temp/humidity to homescreen?
    3. Implement the chamber on and off. 
    4. Extract settings to be sent off to the arduino. 
    5. Settings?
    6. Notifications, such as "OVERHEATING!, sutting donw." etc. 
    7. time memory, notifying the user of part replacement? This seems excessive.. a 5 year timer? 
*/

class NextionCommunication 
{
public: 
    //constants:
    static const int MIN_TEMP = 20;
    static const int MAX_TEMP = 70;
    static const int MIN_HUM = 0;
    static const int MAX_HUM = 100;
    
    //variables
    static boolean displayLInCelcius;
    static float curLTemperature;
    static float curLHumidity;
    static float setLTemperature; 
    static float setLHumidity;
    static boolean displayRInCelcius;
    static float curRTemperature;
    static float curRHumidity;
    static float setRTemperature; 
    static float setRHumidity;

    //public functions
    
    NextionCommunication();
    void init();
    static void updateCurrentStatus(float curLTemp, float curLHum, float curRTemp, float curRHum);

    //Utilities
    static float CelciusToFahrenheit(float C);
    static float FahrenheitToCelcius(float fahrenheit);

    /* TOUCH EVENTS*/
    static NexTouch *nex_listen_list[];


private:
    //gui updates
    static void updateHomeScreen();
    static void updateTextItemWithVar(String item, float var, String optional);
    static void updateLTempText(String item, float temperature);
    static void updateRTempText(String item, float temperature);
    static void updateLTempItemValWithVar(String item, float val);
    static void updateRTempItemValWithVar(String item, float val);
    static void updateItemValWithVar(String item, int val);
    static void updateLAllSetItems();
    static void updateRAllSetItems();
    static void updateLAllCurrentItems(int tempL, int humL);
    static void updateRAllCurrentItems(int tempR, int humR);
    static void tempLDisplayBarAdjustment();
    static void tempRDisplayBarAdjustment();


    //buttons and gui interactions
    //homescreen
        static void btnLCompPopCallback(void *ptr);
        static void btnRCompPopCallback(void *ptr);
    //Left Chamber
        static void btnLTempScalePopCallback(void *ptr);
        static void btnLActivatePopCallback(void *ptr);
        static void btnLTempMinusPopCallback(void *ptr);
        static void btnLTempPlusPopCallback(void *ptr);
        static void sldrLTempPopCallback(void *ptr);
        static void btnLHumMinusPopCallback(void *ptr);
        static void btnLHumPlusPopCallback(void *ptr);
        static void sldrLHumPopCallback(void *ptr);
    //Right Chamber
        static void btnRTempScalePopCallback(void *ptr);
        static void btnRActivatePopCallback(void *ptr);
        static void btnRTempMinusPopCallback(void *ptr);
        static void btnRTempPlusPopCallback(void *ptr);
        static void sldrRTempPopCallback(void *ptr);
        static void btnRHumMinusPopCallback(void *ptr);
        static void btnRHumPlusPopCallback(void *ptr);
        static void sldrRHumPopCallback(void *ptr);

/* DECLARE NEXTION COMPONENTS TO BE USED
    */
   //HOME SCREEN
        static NexText txtLCompStat;
        static NexButton btnLComp;
        static NexText txtLTempStat;
        static NexText txtLHumStat;
        static NexProgressBar prBarLTempStat;
        static NexProgressBar prBarLHumStat;
        static NexText txtRCompStat;
        static NexButton btnRComp;
        static NexText txtRTempStat;
        static NexText txtRHumStat;
        static NexProgressBar prBarRTempStat;
        static NexProgressBar prBarRHumStat;

   //LEFT CHAMBER
            static NexButton btnLTempScale;
            static NexButton btnLActivate;
        // Current Conditions
            static NexProgressBar prBarLCurTemp;
            static NexText txtLCurTemp;
            static NexProgressBar prBarLCurHum;
            static NexText txtLCurHum;
        // Set conditions temp
            static NexButton btnLTempMinus;
            static NexButton btnLTempPlus;
            static NexText txtLSetTemp;
            static NexSlider sldrLTemp;
        // Set conditions Humidity
            static NexButton btnLHumMinus;
            static NexButton btnLHumPlus;
            static NexText txtLSetHum;
            static NexSlider sldrLHum;
    //RIGHT CHAMBER
            static NexButton btnRTempScale;
            static NexButton btnRActivate;
        // Current Conditions
            static NexProgressBar prBarRCurTemp;
            static NexText txtRCurTemp;
            static NexProgressBar prBarRCurHum;
            static NexText txtRCurHum;
        // Set conditions temp
            static NexButton btnRTempMinus;
            static NexButton btnRTempPlus;
            static NexText txtRSetTemp;
            static NexSlider sldrRTemp;
        // Set conditions Humidity
            static NexButton btnRHumMinus;
            static NexButton btnRHumPlus;
            static NexText txtRSetHum;
            static NexSlider sldrRHum;
};


#endif //NEXTIONCOMMUNICATION_H