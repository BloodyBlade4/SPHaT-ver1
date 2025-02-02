/*example:
#include "Arduino.h"
#include "HDC302x.h" // Include the header

HDC302x hdc = HDC302x();    //Preselected I2C Address

void setup()
{
    Serial.begin(115200);                       // Init the serial device
    Serial.println("Ola! Feeling chill?");      // Welcome message

    if (!hdc.Initialize(HDC302X_ADDRESS1))      // Did the initialization fail?
    {
        Serial.println("Sorry, cannot find sensor with the selected address"); 
    } 
}

void loop()
{
    HDC302xDataResult result = hdc.ReadData();  // Calculate the temperature

    Serial.print("Temperature is ");            // Begin printing output
    Serial.print(result.Temperature);           // Print the Temperature
    Serial.print("°C, Humidity is ");           // Print the degrees plus a newline
    Serial.println(result.Humidity);            // Print the Temperature

    delay(5000);                                // Wait 5 seconds
}
*/

#include "HDC302x.h"

bool HDC302x::Initialize(uint8_t address)
{
    this->address = address;

    wire->begin();

    wire->beginTransmission(address);
    if (wire->endTransmission() == 0)
    {
        isInitialized = true;
    }
    else
    {
        isInitialized = false;
    }
    return isInitialized;
}

HDC302xDataResult HDC302x::ReadData()
{
    readDataResult.Temperature = 0.0f;
    readDataResult.Humidity = 0.0f;

    if (!isInitialized)
        return readDataResult;

    // See (8.5.7) on page 18: Trigger-On Demand Mode Low power and low noise!
    wire->beginTransmission(address);
    wire->write(0x24);       // 0x24 => read MSB - no clock stretching
    wire->write(0x00);       // 0x00 => read LSB - HIGH REPEATABILITY
    wire->endTransmission(); // stop transmitting

    delay(HDC302X_READDELAY_MS);

    wire->requestFrom(address, static_cast<uint8_t>(6), static_cast<uint8_t>(1));           // request 6 bytes from HDC3x device, device serial number

    while (wire->available())
    {
        // See (8.5.5) Read single data result
        wire->readBytes(buffer, 6);

        HDC302x_hum_MSB = buffer[3] << 8;                                                   // shift left
        HDC302x_hum_DEC = (HDC302x_hum_MSB + buffer[4]);                                    // get value for calculation, made from iBuff index array values 0 and 1 for humidity reading.
        readDataResult.Humidity = (((float)(HDC302x_hum_DEC)) / 65535) * 100;               // do math according to the HDC3x datasheet

        if (getHumidityCorrection() != 0.0f)                                                // is there a neeed to apply any correction on the RH? (issue reported by user jxl77 on 2024-02-06)
        {
             readDataResult.Humidity = readDataResult.Humidity +  getHumidityCorrection();  // apply and add a correction to the humidity
        }
       
        HDC302x_temp_MSB = (buffer[0] << 8);                                                // shift left
        HDC302x_temp_DEC = (HDC302x_temp_MSB + buffer[1]);                                  // get value for calculation, made from iBuff index array values 0 and 1 for temp reading.
        readDataResult.Temperature = ((float)(HDC302x_temp_DEC) / 65535) * (175) - 45;      // do math according to the HDC3x datasheet
    }
    delay(HDC302X_RETURNDELAY_MS);

    return readDataResult;
}