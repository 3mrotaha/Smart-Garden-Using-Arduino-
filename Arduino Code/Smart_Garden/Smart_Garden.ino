#include <TimeLib.h>
#include<LiquidCrystal.h>

#define SOIL_PIN    A1
#define LM35_PIN    A0
#define RAIN_PIN    8
#define MOTOR_PIN   12
#define LED_PIN     3

#define RELAY_ON    LOW
#define RELAY_OFF   HIGH

#define RAINING     LOW
#define NO_RAIN     HIGH


const int rs = 10, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

void setup() {
    Serial.begin(9600);
    lcd.begin(16,2);
   setTime(6,0,0,30,12,21);
    pinMode(SOIL_PIN,INPUT);
    pinMode(LM35_PIN,INPUT);
    pinMode(MOTOR_PIN,OUTPUT);
    pinMode(LED_PIN,OUTPUT);
    pinMode(RAIN_PIN,INPUT);
}

void loop() {
    LCD_Print_Sensors_Values();
    Time_Settings();
    Pumb_Settings();
}


/****************************************************
 * Soil_Moisture() :                                *
 *    used for measuring the moisture of the soil   *  
 ****************************************************/
int Soil_Moisture(){
  int moisture = 0, value = 0;
  value = analogRead(SOIL_PIN);
  moisture = map(value, 1023, 0, 0, 100);
  return moisture;
}

/****************************************************
 * LM35_Temp() :                                    *
 *    used for measuring the Temperature            *  
 ****************************************************/
int LM35_Temp(){
  int temp = 0, value = 0;
  float volt = 0;
  value = analogRead(LM35_PIN);
  volt = (5.0 / 1023.0) * value;
  temp = volt / 0.01;
  return temp;
}

/****************************************************
 * Rain_Check() :                                   *
 *    used for Checking if there is a rain or not   *  
 ****************************************************/
int Rain_Check(){
  int rain = digitalRead(RAIN_PIN);
  return rain;
}

/*****************************************************
 * Time_Settings() :                                 *
 *    used for Displaying the time and setting the   *
 *    logic based on that time.                      *
 *    such as displaying the morning, Afternoon or   *
 *    Evening and Turning the Lights On if its Dark  *
 *    outside                                        *
 ****************************************************/
void Time_Settings()
{ 
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.println(second());
  
  lcd.setCursor(0,1);
  lcd.print(hour());
  printDigits(minute());
  if(hour() < 11 && hour() > 5) {digitalWrite(LED_PIN, LOW); lcd.print("  Morning");}
  else if(hour() <= 5) {digitalWrite(LED_PIN, HIGH); lcd.print("  Morning");}
  else if(hour() > 11 && hour() < 16) {digitalWrite(LED_PIN, LOW); lcd.print(" Afternoon");}
  else {lcd.print("  Evening"); digitalWrite(LED_PIN, HIGH);}
}

/******************************************************
 * printDigits(int digits) :                          *
 *    Displaying the minutes in the form "0m" if      *                                   
 *    it's less than 10 and "mm" if it's greater than *
 *    or equal 10                                     *
 ******************************************************/
void printDigits(int digits)
{
  lcd.print(":");
  if(digits < 10)
    lcd.print('0');
  lcd.print(digits);  
}

/******************************************************
 * LCD_Print_Sensors_Values() :                       *
 *   Displays the values of the sensors on the lcd    *
 ******************************************************/
void LCD_Print_Sensors_Values(){
  lcd.setCursor(0,0);
  lcd.print(LM35_Temp());
  lcd.print("C | ");
  lcd.setCursor(6,0);
  lcd.print(Soil_Moisture());
  Serial.print("M = ");
  Serial.println(Soil_Moisture());
  lcd.print("%| ");
  lcd.setCursor(12,0);
  if(Rain_Check() == RAINING){Serial.println("rain");lcd.print("Rain");}
  else if(Rain_Check() == NO_RAIN) {Serial.println("dry");lcd.print(" Dry");} 
}

/******************************************************
 * Pumb_Settings() :                                  *
 *   Adjusts the Watering System as follows :         *
 *   - if temperature is high the water must be opend * 
 *      3 times in the day                            * 
 *   - if temperature is low the water must be opened *
 *      only 2 times in the day                       *
 *   - if the soil is dry the water must be opend     *
 *   - if it rains the next round is cancelled        *
 *   - moisture must be between 18 % - 20 %           *
 *   - Water is opened 3 seconds for each round       *
 *   - Water is opened in the morning 6 AM or 6:30 AM *
 *     or SunSet 18:00 PM                             *
 ******************************************************/
void Pumb_Settings(){
  if(LM35_Temp() >= 37){
     if(Soil_Moisture() < 20){
       if(Rain_Check() == NO_RAIN){
                if(hour() == 6 && minute() == 0 && second() > 0 && second() < 4){
                  digitalWrite(MOTOR_PIN, RELAY_ON);
                }
                else if(hour() == 6 && minute() == 30 && second() > 0 && second() < 4){
                  digitalWrite(MOTOR_PIN, RELAY_ON);
                }
                else if(hour() == 18 && minute() == 0 && second() > 0 && second() < 4){
                  digitalWrite(MOTOR_PIN, RELAY_ON);
                }
                else{
                  digitalWrite(MOTOR_PIN, RELAY_OFF);
                }
       }
       else if(Rain_Check() == RAINING){
          digitalWrite(MOTOR_PIN, RELAY_OFF);
       }
     }
    else{
            digitalWrite(MOTOR_PIN, RELAY_OFF);

    }
  }
  else if(LM35_Temp() < 37){
     if(Soil_Moisture() < 18){
       if(Rain_Check() == NO_RAIN){
                if(hour() == 6 && minute() == 0 && second() > 0 && second() < 4){
                  digitalWrite(MOTOR_PIN, RELAY_ON);
                }
                else if(hour() == 18 && minute() == 0 && second() > 0 && second() < 4){
                  digitalWrite(MOTOR_PIN, RELAY_ON);
                }
                else{
                  digitalWrite(MOTOR_PIN, RELAY_OFF);
                }
       }
       else if(Rain_Check() == RAINING){
          digitalWrite(MOTOR_PIN, RELAY_OFF);
       }
     }
     else{
          digitalWrite(MOTOR_PIN, RELAY_OFF);
      }
  }
}
