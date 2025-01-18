#include <Arduino.h>



#include <LM75A.h>
#include <LiquidCrystal_I2C.h>
#include "NextionCommunication.h"
#include "Chamber.h"



/* TODO:
  - Nextion todo's listed in the header. 
  - Connect the nextion communication to the main loop. updating variables. 
  - Volitile variables?? I know we need them, but it's been a while. 
  - What are we doing for dehumidification? The area is open on in the Chamber object and waiting for control. 

  Problems to address
  - The current temp/Humidity sensor, the HDC302x only has 4 I2C addresses. 
      (we can only use 4. currently we use 8. 2 for each chamber, 1 on each heater, and MAYBE 2 in the dehumidifier.)
      Solutions: 
        - Get an I2C expander. we can then have 8. 
        - Utilize a different temp sensor on the heaters, which only needs the temp sensing element. 
        - Choose a new temp/humidity sensor. 


  Once parts are recieved
    Pressure sensor. 

*/



//Arduino/c++ libraies





float temperature = 0.0f;
uint8_t DegreeBitmap[] = { 0x6, 0x9, 0x9, 0x6, 0x0, 0, 0, 0 };       





// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//PIN DECLARATIONS!!!!
/*Pin layout declaration: 
* The default address, with A0, A1, and A2 set to 0 is 1001000 = 1001(A2)(A1)(A0)= 72 = 0x48. 
* to enable different addresses you must enable A0-A3 on the different chip delarations. 
* These are the addresses that I2C uses to communicate and diffrientiate the different items. 
* LM75a documentation: https://www.ti.com/lit/ds/symlink/lm75a.pdf?ts=1735705295933 
*/
const int CH_ONE_SENSOR_ONE = 0x48, //72
          CH_ONE_SENSOR_TWO = 0b1001001, //73
          CH_ONE_SENSOR_THREE = 0x4A; //74

const int CH_ONE_HEATER_SSR = 23; //77, Digital pin 23.


const int CH_TWO_SENSOR_ONE = 0b1001100,  //75
          CH_TWO_SENSOR_TWO = 0b1001101,  //76
          CH_TWO_SENSOR_THREE = 0b1001110;  //77

const int CH_TWO_HEATER_SSR = 24;

//Chambers
/*
  Temperature Sensor initiliazation
  LM75A Custom Wokwi Chip Example

  See for more info: https://docs.wokwi.com/chips-api/getting-started
*/
Chamber chamberOne(LM75A(false,  // A0 LM75A pin state (connected to ground = false)
            false,  // A1 LM75A pin state (connected to ground = false)
            false), // A2 LM75A pin state (connected to ground = false)
            CH_ONE_SENSOR_ONE,
            LM75A(true,  // A0 LM75A pin state (connected to ground = false)
            false,  // A1 LM75A pin state (connected to ground = false)
            false), // A2 LM75A pin state (connected to ground = false)
            CH_ONE_SENSOR_TWO,
            LM75A(false,  // A0 LM75A pin state (connected to ground = false)
            true,  // A1 LM75A pin state (connected to ground = false)
            false), // A2 LM75A pin state (connected to ground = false)
            CH_ONE_SENSOR_THREE,
            CH_ONE_HEATER_SSR //arduino pin address for the Heater SSR. 
            );
Chamber chamberTwo(LM75A(false,  // A0 LM75A pin state (connected to ground = false)
            false,  // A1 LM75A pin state (connected to ground = false)
            true), // A2 LM75A pin state (connected to ground = false)
            CH_TWO_SENSOR_ONE,
            LM75A(true,  // A0 LM75A pin state (connected to ground = false)
            false,  // A1 LM75A pin state (connected to ground = false)
            true), // A2 LM75A pin state (connected to ground = false)
            CH_TWO_SENSOR_TWO,
            LM75A(false,  // A0 LM75A pin state (connected to ground = false)
            true,  // A1 LM75A pin state (connected to ground = false)
            true), // A2 LM75A pin state (connected to ground = false)
            CH_TWO_SENSOR_THREE,
            CH_TWO_HEATER_SSR //arduino pin address for the Heater SSR. 
            );


//Display
NextionCommunication nextionDisplay;

void setup() {
  nexInit();
  Serial.begin(9600);
  nexSerial.begin(9600);

  printf("initialized");
  //chamberOne.init();
  //chamberTwo.init();


  //Initialize the nextion Display
  Serial.println("setup");
  nextionDisplay = NextionCommunication();
  delay(500);
  nextionDisplay.init();
  delay(500);




  // Initialize the lcd
  lcd.init();

  // Storage the custom degree symbol as character #1
  lcd.createChar(1, DegreeBitmap);

  // Print a message to the LCD
  lcd.backlight();
  lcd.setCursor(5, 0);
  lcd.print("LM75A");
  lcd.setCursor(2, 1);
  lcd.print("Address 0x48");
  delay(1000);
  lcd.clear();
}

/* Every loop:
  1. check for input from the nextion display. 
  2. Check water disposal unit levels. Turn off dehumidifiers and prompt user if needed. 
  3. chamber rounds: (Handled in chamber: turn on/off dehumidifier/heater accordingly, check for overheat). 
  4. IF USING DESICCANT WHEEL: Handle condensing chamber. 
  5. Handle statuses of all items. Handle error, overheating, prompting user, updating display. 

*/

float temp = 20;
float hum = 0;

void loop() {

  //1. Communicate with Nextion Display
  //Temp variables to show function. 
  if (temp >= 90)
    temp = 20;
  temp++;
  if (hum >= 100)
    hum = 0;
  hum++;

  nextionDisplay.updateCurrentStatus(temp, hum, temp-10, hum-10);
  nexLoop(nextionDisplay.nex_listen_list);
  delay(500);

  //2. Check water disposal unit levels. 

  //3. Chamber rounds. 
  /*chamberOne.chamberRounds();
  chamberTwo.chamberRounds();

  // 4. Dehumidifier?

  // 5. Handle statuses
  //chamber 1 status:
  






  
  lcd.setCursor(4, 1);
  lcd.print(chamberOne.currentTemperature);
  printf("%6.4lf\n", chamberOne.currentTemperature);
  lcd.print("\001C   "); // Print char 1 (which is the degree) followed by C
  delay(1000);
  */


}

  