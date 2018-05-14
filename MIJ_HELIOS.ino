//**************************************************************
//  Name    : Simple Arduino Sun Tracking / Heliostat Control Program  
//  Author  : Mark IJzerman   
//  Notes   : Based on Phillip Daniel's code for controlling Servo based solar machines. Based on original code by Gabriel Miller (http://www.cerebralmeltdown.com).      
//****************************************************************  
  //////////////////////////////////////////////////  
  //Importing Libraries
  //////////////////////////////////////////////////  
    #include "Wire.h"   
    #include <Servo.h>
    #include <Helios.h>
    #include <RTClib.h>
    Helios helios;
    RTC_DS1307 RTC;

  //////////////////////////////////////////////////  
  //Declaring Variables - User Input
  //////////////////////////////////////////////////  
    //The device will act like a heliostat if this is 1 and a solar panel if this is 0
    int heliostat=1; 
  
    //Put your latitude and longitude here ... currently of Anjos
    float latitude = 38.725;
    float longitude = -9.135;
    
    //Your Heliostat target, with respect to helios's coordinates
    float targetalt = 0;
    float targetaz = 0;
    
  //////////////////////////////////////////////////  
  //Declaring Variables
  //////////////////////////////////////////////////  
    double dAzimuth; //The sun's azimuth angle
    double dElevation; // The sun's elevation/altitude angle
    double offset_Elv=90; //The angular offset to center the Helios's range of motion with the sun's range of motion
    double offset_Az=270; //The angular offset to center the Helios's range of motion with the sun's range of motion.
    float altcommand; //The helios's commanded altitute/elevation angle
    float azcommand; //The helios's commanded azimuch angle
    float altitude, azimuth, delta, h; //Some variables used in functions later
  
  //////////////////////////////////////////////////  
  //Attaching Servos
  //////////////////////////////////////////////////  
    Servo altServoMotor;
    int altServoPin = 9;
    Servo azServoMotor;
    int azServoPin = 6;
  
void setup(){
  //////////////////////////////////////////////////  
  //To set the timer chip specify these numbers based on the time that you upload your code. This is the UTC time.
  //////////////////////////////////////////////////  
    //REMOVE "//" BELOW WHEN SETTING THE TIME AND REPLACE WHEN FINISHED.
//    RTC.adjust(DateTime(__DATE__, __TIME__)); 
  
  //////////////////////////////////////////////////  
  //Initiating libraries and setting data rate
  //////////////////////////////////////////////////  
    Wire.begin();
    Serial.begin(9600);
    RTC.begin();
    Serial.println("Start MIJ.HELIOS setup");

    // This will reflect the time that your sketch was compiled
//    RTC.adjust(DateTime(__DATE__, __TIME__));
  
  //////////////////////////////////////////////////  
  //Attaching Servos
  //////////////////////////////////////////////////  
    altServoMotor.attach(altServoPin);
    azServoMotor.attach(azServoPin);
}

void loop(){ 
  //////////////////////////////////////////////////  
  //Find the location of the sun
  //////////////////////////////////////////////////  
    DateTime now = RTC.now();  
    DateTime future = (now + TimeSpan(0,1,0,0)); // Do this to get UT in currect location. Lisbon is UTC+1.

    helios.calcSunPos(future.year(),future.month(),future.day(),
    future.hour(), future.minute(),future.second(),longitude,latitude); 
    
    showTime();
    
    dAzimuth=helios.dAzimuth;show("dAzimuth",dAzimuth,true);
    dElevation=helios.dElevation;show("dElevation",dElevation,true);       

  //////////////////////////////////////////////////  
  //Act like solar panel 
  ////////////////////////////////////////////////// 
    if (heliostat==0){
      altcommand=offset_Elv-dElevation; //So that the servo's range of motion is aligned with the sun's visable range of motion
      azcommand=offset_Az-dAzimuth; //So that the servo's range of motion is aligned with the sun's visable range of motion
      altServoMotor.write(altcommand); //Command the altitude servo to the reported position
      Serial.println(" ");
      Serial.print("Commanded Elevation =");
      Serial.println( altServoMotor.read()); //Print the angle the servo moved to
      Serial.println(" ");
      delay(3000); //SHould be 5000 Give the Altitude servo time to move to position, so the source voltage doesn't drop below the 4.5 volt limit of the servo
      azServoMotor.write(azcommand); //Command the azimuth servo to the reported position
      Serial.print("Commanded Azimuth =");
      Serial.println(azServoMotor.read()); //Print the angle the servo moved to
      Serial.println(" ");
      delay(3000); //Should be 5000 Give the Azimuth servo time to move to position, so the source voltage doesn't drop below the 4.5 volt limit of the servo
     }
      
  //////////////////////////////////////////////////  
  //Act like heliostat
  ////////////////////////////////////////////////// 
    if (heliostat==1) {
    altcommand= FindHeliostatAngle(dElevation, dAzimuth, targetalt, targetaz, 1); //Shifted so that the servo's zero is aligned with the coord. sys zero
    azcommand= FindHeliostatAngle(dElevation, dAzimuth, targetalt, targetaz, 2); //Shifted so that the servo's zero is centered on the coordinate systems values for of 180degrees
    altServoMotor.write(altcommand); //Command the altitude servo to the reported position
    Serial.println(" ");
    Serial.print("Commanded Elevation =");
    Serial.println( altServoMotor.read()); //Print the angle the servo was told to move to
    Serial.println(" ");
    delay(5000); //Give the Altitude servo time to move to position, so the source voltage doesn't drop below 4.5 volts
    azServoMotor.write(azcommand); //Command the azimuth servo to the reported position
    Serial.print("Commanded Azimuth =");
    Serial.println(azServoMotor.read()); //Print the angle the servo moved to
    Serial.println(" ");
    delay(5000); //Give the Altitude servo time to move to position, so the source voltage doesn't drop below 4.5 volts
    }  
}
 
void show(char nameStr[], double val, boolean newline) {
  Serial.print(nameStr);  
  Serial.print("=");
  if (newline)
    Serial.println(val);
  else Serial.print(val);
}

void showTime() {
  DateTime now = RTC.now();  
  Serial.print("UT ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print("  ");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print("/"); 
  Serial.print(now.year(), DEC); 
  Serial.println("");
}
//////////////////////////////////////////////////  
//This code calculates the angles for the heliostat
//(returnaltaz = 1 will return alt, 2 returns az)
////////////////////////////////////////////////// 
  float FindHeliostatAngle(float altitude, float azimuth, float targetalt, float targetaz, int returnAltAz){
    float x,y,z,z1,z2,x1,x2,y1,y2,hyp,dist,machinealt,machineaz;
    
      //////////////////////////////////////////////////  
      //The cartisian location of the sun with respect to Helios
      ////////////////////////////////////////////////// 
        z1 = cos(to_rad(offset_Elv-altitude)); //The altitude with respect to helios
        hyp =sin(to_rad(offset_Elv-altitude));//The altitude with respect to helios
        x1 = hyp*cos(to_rad(offset_Az-azimuth));//The azimuth with respect to helios
        y1 = hyp*sin(to_rad(offset_Az-azimuth));//The azimuth with respect to helios
        
      //////////////////////////////////////////////////  
      //The cartisian location of the target with respect to Helios
      ////////////////////////////////////////////////// 
        z2 = cos(to_rad(targetalt)); //The target with respect to helios
        hyp = sin(to_rad(targetalt)); //The target with respect to helios
        x2 = hyp*cos(to_rad(targetaz)); //The target with respect to helios
        y2 = hyp*sin(to_rad(targetaz)); //The target with respect to helios
    
      //////////////////////////////////////////////////  
      //The cartisian definition for the vector pointing to along 
      //the bisector of the sun's location and the target's location
      //////////////////////////////////////////////////     
        x=(x1+x2);
        y=(y1+y2);
        z=(z1+z2);
        dist=sqrt(x*x+y*y+z*z);
        
      //////////////////////////////////////////////////  
      //The spherical definition of the vector pointing along 
      //the bisector of the sun's location and the target's location
      ////////////////////////////////////////////////// 
        if ((dist>-0.00000001) && (dist <0.00000001)){
          Serial.println('Impossible to reflect to target with these angles');
        }else{
          machinealt=to_deg(acos(z/dist)); //Angle below the vertical, how this servo is controlled.
        }
        
        if (x==0){x=0.000001;}
        machineaz=to_deg(atan2(y,x));
        if (returnAltAz == 1){return machinealt;}
        if (returnAltAz == 2){return machineaz;} 
  }
        
  float to_rad(float angle){
    return angle*(pi/180);
  }
  
  float to_deg(float angle){
    return angle*(180/pi);
}
