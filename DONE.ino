#include <LiquidCrystal.h>

//Pin intialiazations 
LiquidCrystal lcd(7, 8, 5, 10, 11, 6); 
const uint8_t button_pin = 13; //only pin for button
const uint8_t motor_a = 12; //Motor Channel A pin
const uint8_t brake_a = 9; //Brake Channel A pin
const uint8_t analog_a = 3; //analog pin for Channel A
const uint8_t clkPin = 2; //the clk attach to pin 2
const uint8_t dtPin = 0; //the dt pin attach to pin 3
const uint8_t swPin = 4;//the sw pin attach to pin 4
const uint8_t switchPin = 1;  // with n.o. momentary pb switch to ground

const bool switchOn  = false;    
const bool switchOff = true;
bool last_reverse_state = switchOff;
bool new_reverse_state = switchOff;
bool last_locked_state = switchOff;
bool new_locked_state = switchOff;
bool reverse = false;
bool locked = false;

double act_fac = 2.0;
const uint32_t debounceTime = 5;

void setup() {

  pinMode(button_pin, INPUT); 
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(motor_a, OUTPUT); //Initiates Motor Channel A pin
  pinMode(brake_a, OUTPUT); //Initiates Brake Channel A pin
  lcd.begin(16, 2); //initalize lcd 
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);

}


void loop() {

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


  lcd.setCursor(0, 1);
  if (buttonState == 0) {
     lcd.print((char)255);
  }
  else {
    lcd.print(" ");
  }
  
  lcd.setCursor(1, 1);
  lcd.print(" "); 
   
  lcd.setCursor(2, 1);
  if (reverse == false) {
      lcd.print("        ");
  }
  else {
      lcd.print("REVERSE ");     
  }
  
  lcd.setCursor(10, 1);
  if (locked == false) {
      lcd.print("       ");
  }
  else {
      lcd.print("LOCKED ");     
  }

}

void moveforward(double act_factor){

    double motor_power = 100 * act_factor;
    digitalWrite(motor_a, HIGH); //Establishes forward direction of Channel A
    digitalWrite(brake_a, LOW);   //Disengage the Brake for Channel A
    analogWrite(analog_a, motor_power);   //Spins the motor on Channel A at full speed
  
}

void movebackward(double act_factor) {

    double motor_power = 100 * act_factor;
    digitalWrite(motor_a, LOW); //Establishes forward direction of Channel A
    digitalWrite(brake_a, LOW);   //Disengage the Brake for Channel A
    analogWrite(analog_a, motor_power);   //Spins the motor on Channel A at full speed
}

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

