//Object for each chamber object

#include <Arduino.h>
#include <Chamber.h>


/* TODO:
    1. Override temperature from the main chamber. This can be used when the chamber is off, or anything really to turn off the chamber. 
    2. Override Humidity from the main chamber. This can be used when the chamber is off, or if the water disposal unit is full. 
    3. 
*/

    

Chamber::Chamber(LM75A sensorOne,int sensorOneAddr,
                LM75A sensorTwo,int sensorTwoAddr,
                LM75A sensorThree,int sensorThreeAddr,
                int heaterSSRPinNum)
{
    // Initialise the LM75A.
    lm75a_sensor_one = sensorOne;
    lm75a_sensor_two = sensorTwo;
    lm75a_sensor_three_heater = sensorThree;

    sensorAddrOne = sensorOneAddr;
    sensorAddrTwo = sensorTwoAddr;
    sensorAddrThree = sensorThreeAddr;

    //heater ssr
    HeaterSSRPin = heaterSSRPinNum;


    //set temperatures 
    setTemperature = 22.0;
    setHumidity = 30.0;
}

void Chamber::init() {
    //Initialize the sensors. 
    //first
    Wire.beginTransmission(sensorAddrOne);
    Wire.write(0x01);              // Configuration register.
    Wire.write(0x18 | 0x02);       // Fault queue: 6; Interrupt Mode on.
    Wire.endTransmission();
    delay(5);
    //second
    Wire.beginTransmission(sensorAddrTwo);
    Wire.write(0x01);              // Configuration register.
    Wire.write(0x18 | 0x02);       // Fault queue: 6; Interrupt Mode on.
    Wire.endTransmission();
    delay(5);
    //third
    Wire.beginTransmission(sensorAddrThree);
    Wire.write(0x01);              // Configuration register.
    Wire.write(0x18 | 0x02);       // Fault queue: 6; Interrupt Mode on.
    Wire.endTransmission();
    delay(5);

    //Setup the heater ssr pin as an output. Turn the heating element off to start.  
    pinMode(HeaterSSRPin, OUTPUT);
    delay(5);

    //TODO: Set up the dehumidifier. 

}


/*Function called via a loop. EVERY LOOP:
    1. update current temperature readings, in chamber and on heater. 
    2. check for any OVERHEATING or violations in the chamber heat. 
    3. handle the heater accordingly (influenced by currentTemp, heaterTemp, setTemp, chamberStatus)
    4. update current Humidity readings in chamber. 
    5. Handle the dehumidifier accordingly. 
*/
void Chamber::chamberRounds(){
    //1. update temperature 
    updateCurrentTemperature();

    //2. check for any OVERHEATING or violations in the chamber heat. 
    // Check heater temperature:
    if (currentHeaterTemp >= HEATER_OVERHEAT_TEMP) {
        heaterState = HEATER_OVERHEAT;
        if (digitalRead(HeaterSSRPin) == HIGH)
            turnOffHeatingElement();
    }
    if (currentTemperature >= CHAMBER_OVERHEAT_TEMP) {
        heaterState = CHAMBER_OVERHEAT;
        if (digitalRead(HeaterSSRPin) == HIGH)
            turnOffHeatingElement();
    }
    //TODO: Check for overheating of dehumidifier. 

    //3. check if temperature should be changed.
    // If the chamber is set to heat and nothing has overheated. 
    if (heaterState != OFF && heaterState != HEATER_OVERHEAT && heaterState != CHAMBER_OVERHEAT) {
        //If heater is off and below (setTemperature-Margin) TURN ON.
        if (digitalRead(HeaterSSRPin) == LOW && currentTemperature <= (setTemperature - TEMP_MARGIN_ERROR)) {
            heaterState = OPERATING;
            turnOnHeatingElement();
        }
        //If heater is on and above (setTemp+Margin) TURN OFF.
        else if (digitalRead(HeaterSSRPin) == HIGH && currentTemperature >= (setTemperature + TEMP_MARGIN_ERROR)) {
            heaterState = IDLE;
            turnOffHeatingElement();
        }

    }


    //update humidity

    //check if the humidity should be changed

}

//reads from the sensor to update current chamber temp and humidity status. 
void Chamber::updateCurrentTemperature(){
    //get the temperatures from each sensor, check each are valid to be safe. 
    // 11-bit ADC that offers a temperature resolution of 0.125 °C
    float tempOne = lm75a_sensor_one.getTemperatureInDegrees();
    tempOne = temperatureValid(tempOne); 
    float tempTwo = lm75a_sensor_two.getTemperatureInDegrees();
    tempTwo = temperatureValid(tempTwo); 
    //Do any calculations or averages to find end temperature (highest?)
    float finalTemp = (tempOne + tempTwo)/2;

    //update current temperature. 
    currentTemperature = finalTemp;


    //Update heater temp. 
    float tempThree = lm75a_sensor_three_heater.getTemperatureInDegrees();
    currentHeaterTemp = temperatureValid(tempThree); 
}
//checks that the temperature reading is valid
float Chamber::temperatureValid(float temp) {
    if (temp == INVALID_LM75A_TEMPERATURE) {  
        Serial.println("Did you create lm75a.chip.c?");
        Serial.println("See the steps in the header of the \"lm75a-chip.ino\" file.");
        return 0.0;
    }
    // Bug fix. This will work with the library whether they fix it or not.
    uint16_t i_temperature = temp * 256.0f;
    temp = float(int16_t(i_temperature)) / 256.0f;

    // Round temperature to 2 decimal places.
    temp = round2dp(temp);
    return temp;
}
void updateCurrentHumidity(){

}

//Function that rounds a float to 2 decimal places for printing. 
float Chamber::round2dp(float f)
{
  return round(f * 100.0f) / 100.0f;
}

// Subroutine to turn on the heating element
void Chamber::turnOnHeatingElement() {
    digitalWrite(HeaterSSRPin, HIGH);
}

// Subroutine to turn off the heating element
void Chamber::turnOffHeatingElement() {

    digitalWrite(HeaterSSRPin, LOW);
}