#include "NextionCommunication.h"

/* Variables */
boolean NextionCommunication::displayLInCelcius = true;
float NextionCommunication::setLTemperature = 22.0; 
float NextionCommunication::setLHumidity = 30; 
float NextionCommunication::curLTemperature = 30; 
float NextionCommunication::curLHumidity = 30; 
boolean NextionCommunication::displayRInCelcius = true;
float NextionCommunication::setRTemperature = 22.0; 
float NextionCommunication::setRHumidity = 30; 
float NextionCommunication::curRTemperature = 30; 
float NextionCommunication::curRHumidity = 30; 

/* DECLARE NEXTION COMPONENTS TO BE USED */
//Home screen
NexText NextionCommunication::txtLCompStat = NexText(0, 3, "txtLCompStat");
NexButton NextionCommunication::btnLComp = NexButton(0, 1, "btnLComp"); 
NexText NextionCommunication::txtLTempStat = NexText(0, 8, "txtLTempStat");
NexText NextionCommunication::txtLHumStat = NexText(0, 9, "txtLHumStat");
NexProgressBar NextionCommunication::prBarLTempStat = NexProgressBar(0, 6, "prBarLTempStat");
NexProgressBar NextionCommunication::prBarLHumStat = NexProgressBar(0, 7, "prBarLHumStat");

NexText NextionCommunication::txtRCompStat = NexText(0, 4, "txtRCompStat");
NexButton NextionCommunication::btnRComp = NexButton(0, 2, "btnRComp"); 
NexText NextionCommunication::txtRTempStat = NexText(0, 10, "txtRTempStat");
NexText NextionCommunication::txtRHumStat = NexText(0, 11, "txtRHumStat");
NexProgressBar NextionCommunication::prBarRTempStat = NexProgressBar(0, 12, "prBarRTempStat");
NexProgressBar NextionCommunication::prBarRHumStat = NexProgressBar(0, 13, "prBarRHumStat");

//Left Chamber
NexProgressBar NextionCommunication::prBarLCurTemp = NexProgressBar(1, 15, "prBarLCurTemp"); //(page#, id, objName)
NexText NextionCommunication::txtLCurTemp = NexText(1, 13, "txtLCurTemp");
NexProgressBar NextionCommunication::prBarLCurHum = NexProgressBar(1, 16, "prBarLCurHum"); //(page#, id, objName)
NexText NextionCommunication::txtLCurHum = NexText(1, 14, "txtLCurHum");

NexButton NextionCommunication::btnLTempScale = NexButton(1, 4, "btnLTempScale"); //(page#, id, objName)
NexButton NextionCommunication::btnLActivate = NexButton(1, 2, "btnLActivate"); //(page#, id, objName)

NexButton NextionCommunication::btnLTempMinus = NexButton(1, 6, "btnLTempMinus"); //(page#, id, objName)
NexButton NextionCommunication::btnLTempPlus = NexButton(1, 7, "btnLTempPlus"); 
NexText NextionCommunication::txtLSetTemp = NexText(1, 8, "txtLSetTemp");
NexSlider NextionCommunication::sldrLTemp = NexSlider(1, 3, "sldrLTemp");

NexButton NextionCommunication::btnLHumMinus = NexButton(1, 9, "btnLHumMinus"); //(page#, id, objName)
NexButton NextionCommunication::btnLHumPlus = NexButton(1, 10, "btnLHumPlus"); 
NexText NextionCommunication::txtLSetHum = NexText(1, 12, "txtLSetHum");
NexSlider NextionCommunication::sldrLHum = NexSlider(1, 11, "sldrLHum");
//Right Chamber
NexProgressBar NextionCommunication::prBarRCurTemp = NexProgressBar(2, 15, "prBarRCurTemp"); //(page#, id, objName)
NexText NextionCommunication::txtRCurTemp = NexText(2, 13, "txtRCurTemp");
NexProgressBar NextionCommunication::prBarRCurHum = NexProgressBar(2, 16, "prBarRCurHum"); //(page#, id, objName)
NexText NextionCommunication::txtRCurHum = NexText(2, 14, "txtRCurHum");

NexButton NextionCommunication::btnRTempScale = NexButton(2, 4, "btnRTempScale"); //(page#, id, objName)
NexButton NextionCommunication::btnRActivate = NexButton(2, 2, "btnRActivate"); //(page#, id, objName)

NexButton NextionCommunication::btnRTempMinus = NexButton(2, 6, "btnRTempMinus"); //(page#, id, objName)
NexButton NextionCommunication::btnRTempPlus = NexButton(2, 7, "btnRTempPlus"); 
NexText NextionCommunication::txtRSetTemp = NexText(2, 8, "txtRSetTemp");
NexSlider NextionCommunication::sldrRTemp = NexSlider(2, 3, "sldrRTemp");

NexButton NextionCommunication::btnRHumMinus = NexButton(2, 9, "btnRHumMinus"); //(page#, id, objName)
NexButton NextionCommunication::btnRHumPlus = NexButton(2, 10, "btnRHumPlus"); 
NexText NextionCommunication::txtRSetHum = NexText(2, 12, "txtRSetHum");
NexSlider NextionCommunication::sldrRHum = NexSlider(2, 11, "sldrRHum");


/* TOUCH EVENTS*/
    NexTouch *NextionCommunication::nex_listen_list[] =
    {
        //HOME SCREEN
        &btnLComp,
        &btnRComp,
        //Left Chamber
        &btnLTempScale,
        &btnLActivate,
        &btnLTempMinus,
        &btnLTempPlus,
        &sldrLTemp,
        &btnLHumMinus,
        &btnLHumPlus,
        &sldrLHum,
        //right chamber
        &btnRTempScale,
        &btnRActivate,
        &btnRTempMinus,
        &btnRTempPlus,
        &sldrRTemp,
        &btnRHumMinus,
        &btnRHumPlus,
        &sldrRHum
    };


NextionCommunication::NextionCommunication(){}

void NextionCommunication::init() {
    Serial.println("Initialized Nextion.\n");

    //initialize static variables:
    displayLInCelcius = true;
    setLTemperature = 30.0;
    setLHumidity = 30;
    displayRInCelcius = true;
    setRTemperature = 20.0;
    setRHumidity = 20;

    //Home Screen attatch button functions. 
    btnLComp.attachPop(btnLCompPopCallback);
    btnRComp.attachPop(btnRCompPopCallback);

    //Left Chamber attatch button fuctions.
    btnLTempScale.attachPop(btnLTempScalePopCallback);
    btnLActivate.attachPop(btnLActivatePopCallback);
    btnLTempMinus.attachPop(btnLTempMinusPopCallback);
    btnLTempPlus.attachPop(btnLTempPlusPopCallback);
    sldrLTemp.attachPop(sldrLTempPopCallback);
    btnLHumMinus.attachPop(btnLHumMinusPopCallback);
    btnLHumPlus.attachPop(btnLHumPlusPopCallback);
    sldrLHum.attachPop(sldrLHumPopCallback);

    //right chamber attatch button functions
    btnRTempScale.attachPop(btnRTempScalePopCallback);
    btnRActivate.attachPop(btnRActivatePopCallback);
    btnRTempMinus.attachPop(btnRTempMinusPopCallback);
    btnRTempPlus.attachPop(btnRTempPlusPopCallback);
    sldrRTemp.attachPop(sldrRTempPopCallback);
    btnRHumMinus.attachPop(btnRHumMinusPopCallback);
    btnRHumPlus.attachPop(btnRHumPlusPopCallback);
    sldrRHum.attachPop(sldrRHumPopCallback);

    //set default values:
    updateLAllSetItems();
    delay(50);
    updateRAllSetItems();
    delay(50);
}


/*************************** Public Functions *******************************/
//funtion updating the current status in the chamber. called by arduino. 
void NextionCommunication::updateCurrentStatus(float curLTemp, float curLHum, float curRTemp, float curRHum) {
    curLHumidity = curLHum;
    curLTemperature = curLTemp;
    curRHumidity = curRHum;
    curRTemperature = curRTemp;
    updateLAllCurrentItems(curLTemp, curLHum);
    updateRAllCurrentItems(curRTemp, curRHum);
    updateHomeScreen();
}

/*************************** Utility Functions *******************************/

float NextionCommunication::CelciusToFahrenheit(float celsius) {
    return (celsius * 9.0 / 5.0) + 32;
}
float NextionCommunication::FahrenheitToCelcius(float fahrenheit) {
    return ((fahrenheit - 32)*5.0)/9.0;
}



/*************************** UPDATE CALLS *******************************/
void NextionCommunication::updateHomeScreen() {
    //hanle on/off functinoallity. TODO

    //left chamber
    updateLTempText("txtLTempStat.txt=", curLTemperature);
    updateTextItemWithVar("txtLHumStat.txt=", curLHumidity, "%");

    float val = (curLTemperature/MAX_TEMP) * 100;
    updateItemValWithVar("prBarLTempStat.val=", val);
    updateItemValWithVar("prBarLHumStat.val=", curLHumidity);

    //Right Chamber
    updateRTempText("txtRTempStat.txt=", curRTemperature);
    updateTextItemWithVar("txtRHumStat.txt=", curRHumidity, "%");

    val = (curRTemperature/MAX_TEMP) * 100;
    updateItemValWithVar("prBarRTempStat.val=", val);
    updateItemValWithVar("prBarRHumStat.val=", curRHumidity);
}
void NextionCommunication::updateLTempText(String item, float temperature) {
    float temp = temperature;

    if (!displayLInCelcius) {//if needed in F, convert. 
        temp = CelciusToFahrenheit(temp);
        updateTextItemWithVar(item, temp, " F");  
    } else
        updateTextItemWithVar(item, temp, " C");  
}
void NextionCommunication::updateRTempText(String item, float temperature) {
    float temp = temperature;

    if (!displayRInCelcius) {//if needed in F, convert. 
        temp = CelciusToFahrenheit(temp);
        updateTextItemWithVar(item, temp, " F");  
    } else
        updateTextItemWithVar(item, temp, " C");  
}
void NextionCommunication::updateTextItemWithVar(String item, float val, String optional) {
    //update display
    nexSerial.print(item);
    nexSerial.print("\"");
    nexSerial.print(val);
    if (optional != NULL && optional != "")
        nexSerial.print(optional);
    nexSerial.print("\"");
    nexSerial.write(0xff);
    nexSerial.write(0xff);
    nexSerial.write(0xff);
}
void NextionCommunication::updateLTempItemValWithVar(String item, float temperature) {
    float temp = (displayLInCelcius) ? temperature : CelciusToFahrenheit(temperature);
    updateItemValWithVar(item, temp); 
}
void NextionCommunication::updateRTempItemValWithVar(String item, float temperature) {
    float temp = (displayRInCelcius) ? temperature : CelciusToFahrenheit(temperature);
    updateItemValWithVar(item, temp); 
}
void NextionCommunication::updateItemValWithVar(String item, int val) {
    nexSerial.print(item);
    nexSerial.print(val);
    nexSerial.write(0xff);
    nexSerial.write(0xff);
    nexSerial.write(0xff);
}

void NextionCommunication::updateLAllCurrentItems(int tempL, int humL) {
    //temp
    updateItemValWithVar("prBarLCurTemp.val=", tempL);
    updateLTempText("txtLCurTemp.txt=", tempL);

    //humidity
    updateItemValWithVar("prBarLCurHum.val=", humL);
    updateTextItemWithVar("txtLCurHum.txt=", humL, "%");
}
void NextionCommunication::updateRAllCurrentItems(int tempR, int humR) {
    //temp
    updateItemValWithVar("prBarRCurTemp.val=", tempR);
    updateRTempText("txtRCurTemp.txt=", tempR);

    //humidity
    updateItemValWithVar("prBarRCurHum.val=", humR);
    updateTextItemWithVar("txtRCurHum.txt=", humR, "%");
}

//updates the "USER SET" sliders and texts to reflect the current 
void NextionCommunication::updateLAllSetItems() {
    //Reinitialize sliders
    tempLDisplayBarAdjustment();

    //Left Chamber
        //update set Temp:
        updateLTempText("txtLSetTemp.txt=", setLTemperature);  
        //update temp slider
        updateLTempItemValWithVar("sldrLTemp.val=", setLTemperature);
    
        //update set Humidity
        updateTextItemWithVar("txtLSetHum.txt=", setLHumidity, "%");
        //update humidity slider
        updateItemValWithVar("sldrLHum.val=", setLHumidity);
}
//updates the "USER SET" sliders and texts to reflect the current 
void NextionCommunication::updateRAllSetItems() {
    //Right Chamberr
        //update set Temp:
        updateRTempText("txtRSetTemp.txt=", setRTemperature);  
        //update temp slider
        updateRTempItemValWithVar("sldrRTemp.val=", setRTemperature);
    
        //update set Humidity
        updateTextItemWithVar("txtRSetHum.txt=", setRHumidity, "%");
        //update humidity slider
        updateItemValWithVar("sldrRHum.val=", setRHumidity);

    //Reinitialize sliders
    tempRDisplayBarAdjustment();
}

//Initializes the sliders with the max and min values defined in the header. 
void NextionCommunication::tempLDisplayBarAdjustment() {
    //left chamber
    int min = (displayLInCelcius) ? MIN_TEMP : CelciusToFahrenheit(MIN_TEMP);
    int max = (displayLInCelcius) ? MAX_TEMP : CelciusToFahrenheit(MAX_TEMP);

    //temp slider
    updateItemValWithVar("sldrLTemp.minval=", min);
    updateItemValWithVar("sldrLTemp.maxval=", max);

    //current conditions:
    float val = (curLTemperature/MAX_TEMP) * 100;
    updateItemValWithVar("prBarLCurTemp.val=", val);
    updateLTempText("txtLCurTemp.txt=", curLTemperature);
    //humidity sliders are already set to begin with. 
}
//Initializes the sliders with the max and min values defined in the header. 
void NextionCommunication::tempRDisplayBarAdjustment() {
    //right chamber
    int min = (displayRInCelcius) ? MIN_TEMP : CelciusToFahrenheit(MIN_TEMP);
    int max = (displayRInCelcius) ? MAX_TEMP : CelciusToFahrenheit(MAX_TEMP);

    //temp slider
    updateItemValWithVar("sldrRTemp.minval=", min);
    updateItemValWithVar("sldrRTemp.maxval=", max);

    //current conditions:
    int val = (curRTemperature/MAX_TEMP) * 100;
    updateItemValWithVar("prBarRCurTemp.val=", val);
    updateRTempText("txtRCurTemp.txt=", curRTemperature);
}

/* ************************** BUTTON EVENTS *******************************/
//HOME SCREEN
//update the chamber info before entering. 
void NextionCommunication::btnLCompPopCallback(void *ptr){
    updateLAllSetItems();
    updateLAllCurrentItems(curLTemperature, curLHumidity);
}
void NextionCommunication::btnRCompPopCallback(void *ptr){
    updateRAllSetItems();
    updateRAllCurrentItems(curRTemperature, curRHumidity);
}


//LEFT CHAMBER
void NextionCommunication::btnLTempScalePopCallback(void *ptr) {
    //update flag.
    displayLInCelcius = !displayLInCelcius;

    //update all displays TODO!
    tempLDisplayBarAdjustment();
    updateLAllSetItems();
    updateLAllCurrentItems(curLTemperature, curLHumidity);
}
void NextionCommunication::btnLActivatePopCallback(void *ptr) {
    Serial.print("SYSTEM STARTING");
}
void NextionCommunication::btnLTempMinusPopCallback(void *ptr) {
    if (setLTemperature > MIN_TEMP) {
        //update temp.
        setLTemperature -= 1.0;
        updateLTempText("txtLSetTemp.txt=", setLTemperature);
        //update the slider:
        updateLTempItemValWithVar("sldrLTemp.val=", setLTemperature);
    }

}
void NextionCommunication::btnLTempPlusPopCallback(void *ptr) {
    if (setLTemperature < MAX_TEMP) {
        //update temp.
        setLTemperature += 1.0;
        updateLTempText("txtLSetTemp.txt=", setLTemperature);  
        //update the slider:
        updateLTempItemValWithVar("sldrLTemp.val=", setLTemperature);
    }

}
void NextionCommunication::sldrLTempPopCallback(void *ptr) {
    // slider value
    uint32_t val = 0;
    sldrLTemp.getValue(&val);

    if (!displayLInCelcius)
        val = FahrenheitToCelcius(val);
    //ISSUE: Sometimes the slider will return 0 due to the sending time. 
    // increasing the baud rate can help with this.
    // alternatively, one can do multiple checks or delay. 
    // Find a better solution!

    //update temp val:
    setLTemperature = val;
    updateLTempText("txtLSetTemp.txt=", setLTemperature);  
}
void NextionCommunication::btnLHumMinusPopCallback(void *ptr) {
    if (setLHumidity > MIN_HUM){
        //update Humidity.
        setLHumidity -= 1;
        updateTextItemWithVar("txtLSetHum.txt=", setLHumidity, "%");
        //update slider
        updateItemValWithVar("sldrLHum.val=", setLHumidity);
    }
}
void NextionCommunication::btnLHumPlusPopCallback(void *ptr) {
    if (setLHumidity < MAX_HUM){
        //update Humidity.
        setLHumidity += 1;
        updateTextItemWithVar("txtLSetHum.txt=", setLHumidity, "%");
        //update slider
        updateItemValWithVar("sldrLHum.val=", setLHumidity);
    }
}
void NextionCommunication::sldrLHumPopCallback(void *ptr) {
    // slider value
    uint32_t val = 0;
    sldrLHum.getValue(&val);

    //ISSUE: Sometimes the slider will return 0 due to the sending time. 
    // increasing the baud rate can help with this.
    // alternatively, one can do multiple checks or delay. 
    // Find a better solution!

    //update temp val:
    setLHumidity = val;
    updateTextItemWithVar("txtLSetHum.txt=", setLHumidity, "%");
}


//RIGHT CHAMBER
void NextionCommunication::btnRTempScalePopCallback(void *ptr) {
    //update flag.
    displayRInCelcius = !displayRInCelcius;

    //update all displays TODO!
    tempRDisplayBarAdjustment();
    updateRAllSetItems();
    updateRAllCurrentItems(curRTemperature, curRHumidity);
}
void NextionCommunication::btnRActivatePopCallback(void *ptr) {
    Serial.print("RIGHT SYSTEM STARTING");
}
void NextionCommunication::btnRTempMinusPopCallback(void *ptr) {
    if (setRTemperature > MIN_TEMP) {
        //update temp.
        setRTemperature -= 1.0;
        updateRTempText("txtRSetTemp.txt=", setRTemperature);
        //update the slider:
        updateRTempItemValWithVar("sldrRTemp.val=", setRTemperature);
    }

}
void NextionCommunication::btnRTempPlusPopCallback(void *ptr) {
    if (setRTemperature < MAX_TEMP) {
        //update temp.
        setRTemperature += 1.0;
        updateRTempText("txtRSetTemp.txt=", setRTemperature);  
        //update the slider:
        updateRTempItemValWithVar("sldrRTemp.val=", setRTemperature);
    }

}
void NextionCommunication::sldrRTempPopCallback(void *ptr) {
    // slider value
    uint32_t val = 0;
    sldrRTemp.getValue(&val);

    if (!displayRInCelcius)
        val = FahrenheitToCelcius(val);
    //ISSUE: Sometimes the slider will return 0 due to the sending time. 
    // increasing the baud rate can help with this.
    // alternatively, one can do multiple checks or delay. 
    // Find a better solution!

    //update temp val:
    setRTemperature = val;
    updateRTempText("txtRSetTemp.txt=", setRTemperature);  
}
void NextionCommunication::btnRHumMinusPopCallback(void *ptr) {
    if (setRHumidity > MIN_HUM){
        //update Humidity.
        setRHumidity -= 1;
        updateTextItemWithVar("txtRSetHum.txt=", setRHumidity, "%");
        //update slider
        updateItemValWithVar("sldrRHum.val=", setRHumidity);
    }
}
void NextionCommunication::btnRHumPlusPopCallback(void *ptr) {
    if (setRHumidity < MAX_HUM){
        //update Humidity.
        setRHumidity += 1;
        updateTextItemWithVar("txtRSetHum.txt=", setRHumidity, "%");
        //update slider
        updateItemValWithVar("sldrRHum.val=", setRHumidity);
    }
}
void NextionCommunication::sldrRHumPopCallback(void *ptr) {
    // slider value
    uint32_t val = 0;
    sldrRHum.getValue(&val);

    //ISSUE: Sometimes the slider will return 0 due to the sending time. 
    // increasing the baud rate can help with this.
    // alternatively, one can do multiple checks or delay. 
    // Find a better solution!

    //update temp val:
    setRHumidity = val;
    updateTextItemWithVar("txtRSetHum.txt=", setRHumidity, "%");
}

