// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       pot_struring steppers.ino
    Created:	9/12/2020 9:44:46
    Author:     DESKTOP-53KU12I\mauro
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
//
#include <Arduino.h>
#include "DRV8825.h"
#include <math.h>


// Define Functions below here or use other .ino or cpp files
//






//instelling variabelen communicatie
int rotate_hoek;
int tilt_hoek;
const byte buffSize = 60;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;
unsigned long curMillis;
unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 1000;

//instelling pins



//pins voor sturing tiltstappenmotor
const byte pin_dir_tilt = 0;
const byte pin_step_tilt = 1;
const byte pin_sleep_tilt = 2;
const byte pin_mode0_tilt = 3;
const byte pin_mode1_tilt = 4;
const byte pin_mode2_tilt = 5;

//pins voor sturing rotatestappenmotor
const byte pin_dir_rotate = 6;
const byte pin_step_rotate = 7;
const byte pin_sleep_rotate = 8;
const byte pin_mode0_rotate = 9;
const byte pin_mode1_rotate = 10;
const byte pin_mode2_rotate = 11;





//pins voor eindeloopschakelaars
//pins voor tilt eindeloopschakelaars
#define schakelaar_1  12
#define schakelaar_2  13
//pins voor rotate eindeloopschakelaars
#define schakelaar_3  A0
#define schakelaar_4  A1

//pins voor joystick 
#define Vrx A2
#define Vry A3

//variabelen joystick;
int val_x;
int val_y;

//paramters steppers
int tilt_hoek_nieuw;
int tilt_steps;
int rotate_steps;
int rotate_hoek_nieuw;
int tilt_hoek_gewenst;
int rotate_hoek_gewenst;
int tilt_delta;
int rotate_delta;
bool x = 0;
bool y = 0;

//instelling steppers
float step_angle = 0.9;
const int steps_motor = 400;
const byte microsteps_tilt = 16;
const byte microsteps_rotate = 16;
const int frequentie_controller = 30000;
const int toerental_max = (step_angle / 360) * frequentie_controller * 60;
int toerental;
int toerental_tilt;
int toerental_rotate;

//Setup steppers
DRV8825 motortilt(steps_motor, pin_dir_tilt, pin_step_tilt, pin_sleep_tilt, pin_mode0_tilt, pin_mode1_tilt, pin_mode2_tilt);
DRV8825 motorrotate(steps_motor, pin_dir_rotate, pin_step_rotate, pin_sleep_rotate, pin_mode0_rotate, pin_mode1_rotate, pin_mode2_rotate);



//variabelen voor hoeken tilt en rotate


//parameters overbrenging tilt
const byte z_1_tilt = 12;
const byte z_2_tilt = 60;
const byte een_op_overbrengingsverhouding_tilt = z_2_tilt / z_1_tilt;


//parameters overbrenging rotate
const byte z_1_rotate = 12;
const byte z_2_rotate = 60;
const byte een_op_overbrengingsverhouding_rotate = z_2_rotate / z_1_rotate;


//variabelen om status van schakelaars weer te geven
bool schakelaarstaat1;
bool schakelaarstaat2;
bool schakelaarstaat3;
bool schakelaarstaat4;


void setup() {

    

    //aanzetten steppers
    motortilt.enable();
    motorrotate.enable();

    //Setup schakelaars

    pinMode(schakelaar_1, INPUT_PULLUP);
    pinMode(schakelaar_2, INPUT_PULLUP);
    pinMode(schakelaar_3, INPUT_PULLUP);
    pinMode(schakelaar_4, INPUT_PULLUP);

}

void loop() {
    //we lezen hier potmeter uit
    //valx gebruiken we voor de deltarotatehoek uit te lezen 
    val_x = analogRead(Vrx);
    //valy gebruiken we om de detaltilthoek uit te lezen
    val_x = analogRead(Vry);

    //toerental instellen
    toerental = 200;
    schakelaarstaat1 = digitalRead(schakelaar_1);
    schakelaarstaat2 = digitalRead(schakelaar_2);
    schakelaarstaat3 = digitalRead(schakelaar_3);
    schakelaarstaat4 = digitalRead(schakelaar_4);
    


    //kalibratiehoek tilt
    while (schakelaarstaat1 == HIGH && x!=1);
    {
        motortilt.setMicrostep(1);
        motortilt.move(-2);

    }
    tilt_hoek = 0;
    x = 1;

    //kalibratiehoek rotate 
    while (schakelaarstaat3 == HIGH && y != 1);
    {
        motorrotate.setMicrostep(1);
        motortilt.move(-2);
    }
    rotate_hoek = 0;
    y = 1;

    
    //Serial.println("een_op_overbrengingsverhouding_tilt=");
    //Serial.println(een_op_overbrengingsverhouding_tilt);

    //we moeten ook opletten dat de camera zich niet boven de maximale grenzen bewegen daarom hier inlezing eindeloopschakelaars
    //alleen als de camera zich binnen de aanvaarbaare grenzen voor tilt bevind zal de tilt zich mogen bewegen als dit niet het geval is
    //zal deze een ERROR:WRONG TILT VALUE geven op de seri�le monitor
    //de eerste 2 eindelooschakelaars zijn voor de stepper tilt
    schakelaarstaat1 = digitalRead(schakelaar_1);
    schakelaarstaat2 = digitalRead(schakelaar_2);
    schakelaarstaat3 = digitalRead(schakelaar_3);
    schakelaarstaat4 = digitalRead(schakelaar_4);


    if (schakelaarstaat1 == HIGH && schakelaarstaat2 == HIGH) {
        //steppers aansturen
        //tilt

        //setup tilt stepper
        //toerental berkenen plus rekening houding met het maximale toerental van de stappenmotor
        toerental_tilt = toerental * een_op_overbrengingsverhouding_tilt;
        if (toerental_tilt > 4500) {
            toerental_tilt = toerental_max;
        }

        motortilt.setRPM(toerental_tilt);
        motortilt.setMicrostep(microsteps_tilt);

        tilt_hoek_gewenst = map(val_y, 0, 1023, 0, 180);
        tilt_delta = tilt_hoek_gewenst - tilt_hoek;
        
        //berekening aantal steps tilt
        tilt_hoek_nieuw = tilt_delta * een_op_overbrengingsverhouding_tilt * microsteps_tilt;
        tilt_steps = tilt_hoek_nieuw / step_angle;
        /*
        //seri�le monitor voor debugging tilt
        
        if (tilt_hoek_nieuw != 0) {
            Serial.println("tilt_hoek_nieuw=");
            Serial.println(tilt_hoek_nieuw);
            Serial.println("tilt_steps=");
            Serial.println(tilt_steps);
            Serial.println("toerental_int=");
            Serial.println(toerental_tilt);
            delay(1000);
        }
        */

       

        //aansturing stepper tilt
        motortilt.move(tilt_steps);

        //aanpassen tilt hoek
        tilt_hoek = tilt_hoek_nieuw;






    }
    //doorsturen ERROR:WRONG TILT VALUE boodschap op de seri�le monitor
    else if (schakelaarstaat1 != HIGH || schakelaarstaat2 != HIGH) {
        //doorgeven rotate & tilt-hoek en toerental aan Rasberry-Pi
        //we sturen nullen door omdat er niks zal gebeuren 
        //tilt_hoek = 0;
        //rotate_hoek = 0;
        toerental = 0;
        //setup stappen motor 
        motortilt.setRPM(toerental);
        motortilt.setMicrostep(1);

 

        //aansturen stappen motor tilt 
        motortilt.move(0);
    }
    //we moeten ook opletten dat de camera zich niet boven de maximale grenzen bewegen daarom hier inlezing eindeloopschakelaars
    //alleen als de camera zich binnen de aanvaarbaare grenzen voor tilt bevind zal de tilt zich mogen bewegen als dit niet het geval is
    //zal deze een ERROR:WRONG ROTATE VALUE geven op de seri�le monitor
    //de laatse  2 eindelooschakelaars zijn voor de stepper tilt
    schakelaarstaat1 = digitalRead(schakelaar_1);
    schakelaarstaat2 = digitalRead(schakelaar_2);
    schakelaarstaat3 = digitalRead(schakelaar_3);
    schakelaarstaat4 = digitalRead(schakelaar_4);


    if (schakelaarstaat3 == HIGH && schakelaarstaat4 == HIGH) {
        //rotate

        //setup rotate stepper
        //toerental bereken en rekening houden met het maximale toerental van de motor 
        toerental_rotate = toerental * een_op_overbrengingsverhouding_rotate;
        if (toerental_rotate > 4500) {
            toerental_rotate = toerental_max;
        }
        motorrotate.setRPM(toerental_rotate);
        motorrotate.setMicrostep(microsteps_rotate);

        //omzetten aanloge waarde naar te bewegen hoek
        rotate_hoek_gewenst = map(val_x, 0, 1023, 0, 360);
        rotate_delta = rotate_hoek_gewenst - rotate_hoek;
        
        //berekening aantal steps rotate
        rotate_hoek_nieuw = rotate_delta * een_op_overbrengingsverhouding_rotate * microsteps_rotate;
        rotate_steps = rotate_hoek_nieuw / step_angle;
        /*
        //seri�le monitor voor debugging rotate
        if (rotate_hoek_nieuw != 0) {
            Serial.println("rotate_hoek_nieuw=");
            Serial.println(rotate_hoek_nieuw);
            Serial.println("rotate_steps=");
            Serial.println(rotate_steps);
            Serial.println("toerental_rotate=");
            Serial.println(toerental_rotate);
        }
        */

       
        //aansturing stepper rotate
        //gegeven string doorsturen
        motorrotate.move(rotate_steps);

        //rotate_hoek updaten 
        rotate_hoek = rotate_hoek_gewenst;
    }
    //doorsturen ERROR:WRONG ROTATE VALUE boodschap op de seri�le monitor
    //motoren niets laten doen 
    //lege string doorsturen , zelfde format 00,00,0000
    else if (schakelaarstaat3 != HIGH || schakelaarstaat4 != HIGH) {
        //doorgeven rotate & tilt-hoek en toerental aan Rasberry-Pi
        //we sturen nullen door omdat er niks zal gebeuren 
        //tilt_hoek = 0;
        //rotate_hoek = 0;
        toerental = 0;
        //setup stappen motor 
        motorrotate.setRPM(toerental);
        motorrotate.setMicrostep(1);

        //aansturen stappen motor tilt 
        motorrotate.move(0);
    }









}
