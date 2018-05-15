#include <Servo.h> // include server library

Servo altServoMotor1; // create servo object to control a servo
Servo aziServoMotor1;
Servo altServoMotor2;
Servo aziServoMotor2;

float altServoMotor1Pos = 90; // initial position of server
float aziServoMotor1Pos = 90;
float altServoMotor2Pos = 90;
float aziServoMotor2Pos = 90; 

float val;

int sendToAziOrAlt = 0;

void setup() {

Serial.begin(9600); // Serial comm begin at 9600bps

altServoMotor1.attach(9);// server is connected at pin 9
aziServoMotor1.attach(6);
altServoMotor2.attach(3);// server is connected at pin 9
aziServoMotor2.attach(5);

}

void loop() {

  if (Serial.available()) // if serial value is available
  
  {
  
    val = Serial.read();// then read the serial value
  
       if (val == 182) //if value input is equals to 2
          if(aziServoMotor1Pos < 180) {
            
             {
      
              aziServoMotor1Pos += 0.5; //than position of servo motor increases by 1 ( anti clockwise)
              aziServoMotor1.write(aziServoMotor1Pos);// the servo will move according to position
              delayMicroseconds(10); //delay for the servo to get to the position
      
             }

          }
  
       if (val == 181) //if value input is equals to 1
          if(aziServoMotor1Pos > 0) {
            
  
            {
      
              aziServoMotor1Pos -= 0.5; //than position of servo motor decreases by 1 (clockwise)
              aziServoMotor1.write(aziServoMotor1Pos);// the servo will move according to position
              delayMicroseconds(10);//delay for the servo to get to the position
      
             }

          }

         

           if (val == 183) //if value input is equals to 3
             if(altServoMotor1Pos < 180) {
                   {
            
                    altServoMotor1Pos += 0.5; //than position of servo motor increases by 1 ( anti clockwise)
                    altServoMotor1.write(altServoMotor1Pos);// the servo will move according to position
                    delayMicroseconds(10); //delay for the servo to get to the position
            
                   }

             }
  
       if (val == 184) //if value input is equals to a
               if(altServoMotor1Pos > 0) {
                      {
                
                        altServoMotor1Pos -= 0.5; //than position of servo motor decreases by 1 (clockwise)
                        altServoMotor1.write(altServoMotor1Pos);// the servo will move according to position
                        delayMicroseconds(10);//delay for the servo to get to the position
                
                       }

               }


        // stuff for if value is from the recorded Max gestures

        if (val == 185) {
          sendToAziOrAlt = 0;
        }

        if (val == 186) {
          sendToAziOrAlt = 1;
        }

        if (val == 187) {
          sendToAziOrAlt = 3;
        }

        if (val == 188) {
          sendToAziOrAlt = 4;
        }
            
       if (val < 181) {
          if (sendToAziOrAlt == 0) {
            aziServoMotor1Pos == val;
            aziServoMotor1.write(val);
            delayMicroseconds(50);
          }
       }

        if (val < 181) {
          if (sendToAziOrAlt == 1) {
            altServoMotor1Pos == val;
            altServoMotor1.write(val);
            delayMicroseconds(50);
          }
       }

       
        if (val < 181) {
          if (sendToAziOrAlt == 3) {
            aziServoMotor2Pos == val;
            aziServoMotor2.write(val);
            delayMicroseconds(50);
          }
       }


        if (val < 181) {
          if (sendToAziOrAlt == 4) {
            altServoMotor2Pos == val;
            altServoMotor2.write(val);
            delayMicroseconds(50);
          }
       }
  
  }

}


