// Modified from https://www.geekering.com/categories/embedded-sytems/arduino/joaotarquinio/how-to-make-a-swimming-stopwatch-using-shock-sensor-ky-002/

#include <LiquidCrystal.h>

const int shockSensor = 12; //Defines shock sensor digital pin (12)
const int buzzer = 11;      //Defines buzzer digital pin (11)

// Push-Buttons related variables
const int startButton = 1;  //Defines start button digital pin (1)
const int resetButton = 2;  //Defines reset button digital pin (2)
const int hitButton = 13;  //Defines hit button digital pin (13)
bool stopwatch = 0;         //Variable to manage the stopwatch
bool getReady = 0;          //Variable to manage the buzzer warning

// Chronometer related variables
int cents = 0;                          //Counter for the hundredths of seconds
int seconds = 0;                        //Counter for the seconds
int minutes = 0;                        //Counter for the minutes      
int hours = 0;                          //Counter for the hours 
int c1, c2, c3, s1, s2, m1, m2, h1;         //Variables used to put the running time in the form h1:m2m1:s2s1:c2c1
int c_1, c_2, c_3, s_1, s_2, m_1, m_2, h_1;  //Variables used to put the split time in the form h_1:m_2m_1:s_2s_1:c_2c_1
const int milisToCent = 10;             //Every 10 milliseconds there is an increase one hundredth of a second
const int showInterval = 1000;          //It shows the split number and time for a few seconds
unsigned long currentMillis = 0;        //Variable to ensure timing
unsigned long previousMillis = 0;       //Variable to ensure timing
int showTime = 0;                       //Counter of the time spent showing the split time
int countSplits = 1;                    //Counter of splits
bool showSplit = 0;                     //Auxiliary variable for the split time display
bool splitTime = 0;                     //Auxiliary variable to get the split time digits
bool playerTime = 1;
// Defines the LCD's parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

void setup() {
  
  //Definition of the system's inputs and outputs
  pinMode(startButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);
  pinMode(hitButton, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  // Set up the LCD's number of columns and rows
  lcd.begin(16,2);
  
}

void loop() {

  //If the reset button is pressed, the timer is reset
  if (digitalRead(resetButton) == LOW) {
    playerTime = 1;
    lcd.clear();
    stopwatch = 0;
    getReady = 0;
    cents = 0;
    seconds = 0;
    minutes = 0;
    hours = 0;
    showTime = 0;
    countSplits = 1;
    splitTime = 0;
    showSplit = 0;
   }
  
  //Keeps displaying "PRESS START BUTTON..." while the start button is not pressed
  if (stopwatch == 0){
   playerTime = 1;
    while(digitalRead(startButton) == HIGH) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("READY");
      //lcd.setCursor(0, 1);
      //lcd.print("BUTTON ...");
      delay(200);
     };  
  }

  //Starts the buzzer count down sequence
  if (getReady == 0){
    countDown();
  }
  
  //Starts the chronometer 
  if(playerTime){
  chronometer();
  }
}

// Buzzer warning 3 seconds before the chronometer starts
void countDown(void){

    //Initial 
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("STARTING IN");
    lcd.setCursor(7, 1);
    lcd.print("3");
    delay(1000);
    lcd.setCursor(7, 1);
    lcd.print("2");
    delay(1000);
    lcd.setCursor(7, 1);
    lcd.print("1");
    
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(500);

    lcd.clear();

    //For the buzzer count down sequence to happen only once after the start button is pressed
    getReady = 1;
 
}

//Display of the current time and split times
void chronometer(void){   
  
  stopwatch = 1;
  currentMillis = millis();  //Initiates timer variable

  //If 10 milliseconds have passed, it is incremented one hundredth of a second to the chronometer
  if (currentMillis - previousMillis >= milisToCent) {  
    previousMillis = currentMillis;
    cents++;
    //100 hundredths of a second = 1 second
    if (cents == 100){
      cents = 0;
      seconds++;
      //60 seconds = 1 minute
      if (seconds == 60) {
        seconds = 0;
        minutes++;
        //60 minutes = 1 hour
        if (minutes == 60){
          minutes = 0; 
          hours++;
          //Just resets the hours if it ever reaches 24 hours
          if (hours == 24){
            hours = 0;
          }
        }
      }
    }

    //Time digits division for it to be displayed as intended (h1:m2m1:s2s1:c2c1)
    int c = cents;
    int s = seconds;
    int m = minutes; 
    int h = hours;  
    
 
    c1 = c%10;
    c /= 10;
    c2 = c%10;
    
    s1 = s%10;
    s /= 10;
    s2 = s%10;
    m1 = m%10;
    m /= 10;
    m2 = m%10;

    //Current time display
    lcd.setCursor(0, 0);
    lcd.print("SHOOTING");
    lcd.setCursor(3, 1);
    lcd.print(h1);
    lcd.print(':');
    lcd.print(m2);
    lcd.print(m1);
    lcd.print(':');
    lcd.print(s2);
    lcd.print(s1);
    lcd.print(':');
    lcd.print(c2);
    lcd.print(c1);
    
  }

  //If the shock sensor detects a vibration 
  if (digitalRead(shockSensor) == 1 || digitalRead(hitButton) == LOW){
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(500);
    showSplit = 1;
  }
   

  //Get the time digits at the moment the shock sensor detected a vibration
  if (showSplit == 1){
    if(splitTime == 0){
      c_1 = c1;
      c_2 = c2;
      s_1 = s1;
      s_2 = s2;
      m_1 = m1;
      m_2 = m2;
      h_1 = h1;
	  splitTime = 1;
    }  

    //Split time display
    if(showTime <= showInterval){
      lcd.setCursor(0, 0);
      //lcd.print(countSplits);
      lcd.print("SHOOTER TIME");
      lcd.setCursor(3, 1);
      lcd.print(h_1);
      lcd.print(':');
      lcd.print(m_2);
      lcd.print(m_1);
      lcd.print(':');
      lcd.print(s_2);
      lcd.print(s_1);
      lcd.print(':');
      lcd.print(c_2);
      lcd.print(c_1); 
      showTime++; 
      playerTime = 0;
    }

    //Increment split count and resets split time display process
    else {
      showTime = 0;
      countSplits++;
      splitTime = 0;
      showSplit = 0;
      lcd.clear();
    }
  }
}
