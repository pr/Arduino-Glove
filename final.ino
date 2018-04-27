//Peter Richards
//pfr2109

#include <LiquidCrystal.h>

//Pin initializations
LiquidCrystal lcd(7, 8, 5, 10, 11, 6); //Lcd Screen
const uint8_t button_pin = 13; //pin for finger button
const uint8_t motor_a = 12; //Motor Channel A pin
const uint8_t brake_a = 9; //Brake Channel A pin
const uint8_t analog_a = 3; //analog pin for Channel A
const uint8_t clkPin = 2; //Actuation Adjustment Knob (clk pin)
const uint8_t dtPin = 0; //Actuation Adjustment Knob (dt pin)
const uint8_t swPin = 4;//Actuation Adjustment Knob (sw pin)
const uint8_t switchPin = 1;  // pin for thumb switch

//Values to persist beyond one while loop
const bool switchOn  = false;
const bool switchOff = true;
bool last_reverse_state = switchOff;
bool new_reverse_state = switchOff;
bool last_locked_state = switchOff;
bool new_locked_state = switchOff;
bool reverse = false;
bool locked = false;

double act_fac = 2.0; //This can be whatever you want
const uint32_t debounceTime = 5;

void setup() {

  pinMode(button_pin, INPUT); //Initialize the finger button
  pinMode(switchPin, INPUT_PULLUP); //Initialize the thumb switch
  pinMode(motor_a, OUTPUT); //Initiates Motor Channel A pin
  pinMode(brake_a, OUTPUT); //Initiates Brake Channel A pin
  lcd.begin(16, 2); //Initialize lcd screen
  pinMode(clkPin, INPUT); //Initialize the Actuation Adjustment Knob
  pinMode(dtPin, INPUT); //Initialize the Actuation Adjustment Knob
  pinMode(swPin, INPUT_PULLUP); //Initialize the Actuation Adjustment Knob

}


void loop() {

  //Decides what direction to run the motor
  new_reverse_state = digitalRead( switchPin );
  if( last_reverse_state != new_reverse_state ) {

    delay( debounceTime );
    last_reverse_state = new_reverse_state;

    if( new_reverse_state == switchOn && reverse == false ) {
      reverse = true;
    }
    else if( new_reverse_state == switchOn && reverse == true ) {
      reverse = false;
    }

  }

  //Decides whether or not to lock the actuation factor
  new_locked_state = digitalRead( swPin );
  if( last_locked_state != new_locked_state ) {

    delay( debounceTime );
    last_locked_state = new_locked_state;

    if( new_locked_state == switchOn && locked == false ) {
      locked = true;
    }
    else if( new_locked_state == switchOn && locked == true ) {
      locked = false;
    }

  }

  //If the knob is spun, change the actuation factor
  double change = upordown();
  if (change != 0 && locked == false) {
     act_fac = act_fac + change;
  }
  if (act_fac > 2.55) {
      act_fac = 2.55;
  }
  if (act_fac < 1.00) {
      act_fac = 1.00;
  }

  //if the sensor in the finger is triggered, move the motor
  //otherwise, engage brake
  int buttonState = digitalRead(button_pin);
  if (buttonState == 0) {
    if (reverse == false) {
         moveforward(act_fac);
    }
    else {
        movebackward(act_fac);
    }

  }
  else {
     digitalWrite(brake_a, HIGH);
  }

  //Print actuation factor on screen
  lcd.print("Act. Factor: 2.0");
  if (change != 0) {
      lcd.setCursor(13,0);
      lcd.print(" ");
      lcd.setCursor(14,0);
      lcd.print(" ");
      lcd.setCursor(15,0);
      lcd.print(" ");
      lcd.setCursor(13,0);
      lcd.print(act_fac);
  }

  //if moving the motor, light up the actuation indicator
  lcd.setCursor(0, 1);
  if (buttonState == 0) {
     lcd.print((char)255);
  }
  else {
    lcd.print(" ");
  }

  lcd.setCursor(1, 1);
  lcd.print(" ");

  //If motor set to reverse, light up "REVERSE"
  lcd.setCursor(2, 1);
  if (reverse == false) {
      lcd.print("        ");
  }
  else {
      lcd.print("REVERSE ");
  }

  //If actuation factor is locked, light up "LOCKED"
  lcd.setCursor(10, 1);
  if (locked == false) {
      lcd.print("       ");
  }
  else {
      lcd.print("LOCKED ");
  }

}

//function to move motor forwards
void moveforward(double act_factor){

    double motor_power = 100 * act_factor;
    digitalWrite(motor_a, HIGH);
    digitalWrite(brake_a, LOW);
    analogWrite(analog_a, motor_power);

}

//function to move motor backwards
void movebackward(double act_factor) {

    double motor_power = 100 * act_factor;
    digitalWrite(motor_a, LOW);
    digitalWrite(brake_a, LOW);
    analogWrite(analog_a, motor_power);
}

//function to adjust the Actuation Adjustment Knob
double upordown(void) {

    static int oldA = HIGH; //set the oldA as HIGH
    static int oldB = HIGH; //set the oldB as HIGH
    double result = 0;
    int newA = digitalRead(clkPin);//read the value of clkPin to newA
    int newB = digitalRead(dtPin);//read the value of dtPin to newB

    if (newA != oldA || newB != oldB) {

        if (oldA == HIGH && newA == LOW) {

              if (newB != newA) {
                result = -0.1;
              }
              else {
                result = 0.1;
              }
        }

    }

    oldA = newA;
    oldB = newB;

    return result;

}
