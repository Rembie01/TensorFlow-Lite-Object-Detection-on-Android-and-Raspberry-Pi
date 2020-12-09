//bron code communicatie arduino rasberry pi: https://forum.arduino.cc/index.php?topic=225329.msg1810764#msg1810764
#include <Arduino.h>
#include "DRV8825.h"


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
const byte pin_dir_tilt = 2;
const byte pin_step_tilt = 3;
const byte pin_sleep_tilt = 4;
const byte pin_mode0_tilt = 5;
const byte pin_mode1_tilt = 6;
const byte pin_mode2_tilt = 7;

//pins voor sturing rotatestappenmotor
const byte pin_dir_rotate=8;
const byte pin_step_rotate=9;
const byte pin_sleep_rotate=10;
const byte pin_mode0_rotate=11;
const byte pin_mode1_rotate=12;
const byte pin_mode2_rotate=13;





//pins voor eindeloopschakelaars
//pins voor tilt eindeloopschakelaars
#define schakelaar_1  A0
#define schakelaar_2  A1
//pins voor rotate eindeloopschakelaars
#define schakelaar_3  A2
#define schakelaar_4  A3



//paramters steppers
int tilt_hoek_nieuw;
int tilt_steps;
int rotate_steps;
int rotate_hoek_nieuw;

//instelling steppers
float step_angle = 0.9;
const int steps_motor= 400;
const byte microsteps_tilt = 16;
const byte microsteps_rotate = 16;
const int frequentie_controller = 30000;
const int toerental_max = (step_angle / 360) * frequentie_controller * 60;
//int toerental;
int toerental_tilt;
int toerental_tilt_org;
int toerental_rotate;
int toerental_rotate_org;

//Setup steppers
DRV8825 motortilt(steps_motor, pin_dir_tilt,pin_step_tilt,pin_sleep_tilt,pin_mode0_tilt,pin_mode1_tilt,pin_mode2_tilt);
DRV8825 motorrotate(steps_motor,pin_dir_rotate,pin_step_rotate,pin_sleep_rotate,pin_mode0_rotate,pin_mode1_rotate,pin_mode2_rotate);



//variabelen voor hoeken tilt en rotate


//parameters overbrenging tilt
const byte z_1_tilt=12;
const byte z_2_tilt=60;
const byte een_op_overbrengingsverhouding_tilt=z_2_tilt/z_1_tilt;


//parameters overbrenging rotate
const byte z_1_rotate=12;
const byte z_2_rotate=60;
const byte een_op_overbrengingsverhouding_rotate=z_2_rotate/z_1_rotate;


//variabelen om status van schakelaars weer te geven
bool schakelaarstaat1;
bool schakelaarstaat2;
bool schakelaarstaat3;
bool schakelaarstaat4;


void setup() {



//aanzetten steppers
motortilt.enable();
motorrotate.enable();

	//setup Seriële monitor
	Serial.begin(115200);

    // tell the PC we are ready
    Serial.println("<Arduino is ready>");

	//Setup schakelaars
    
	pinMode(schakelaar_1, INPUT_PULLUP);
	pinMode(schakelaar_2, INPUT_PULLUP);
    pinMode(schakelaar_3, INPUT_PULLUP);
    pinMode(schakelaar_4, INPUT_PULLUP);
    
}
//inlezen seriële poort 
void getDataFromPC() {

    // receive data from PC and save it into inputBuffer

    if (Serial.available() > 0) {

        char x = Serial.read();

        // the order of these IF clauses is significant

        if (x == endMarker) {
            readInProgress = false;
            newDataFromPC = true;
            inputBuffer[bytesRecvd] = 0;
            parseData();
        }

        if (readInProgress) {
            inputBuffer[bytesRecvd] = x;
            bytesRecvd++;

            if (bytesRecvd == buffSize) {
                bytesRecvd = buffSize - 1;
            }

        }

        if (x == startMarker) {
            bytesRecvd = 0;
            readInProgress = true;
        }
    }
    else {
        //als er niets doorgeven wordt moet er niets worden bewogen 
        //er wordt niets bewogen als alle hoeken nul zijn en het toerental ook nul is
        tilt_hoek = 0;
        rotate_hoek = 0;
        toerental_tilt_org = 0;
        toerental_rotate_org = 0;
    }

}
// tilt_hoek,rotate_hoek en toerental worden uit de gegeven string gehaald
void parseData() {

    // split the data into its parts

    char* strtokIndx; // this is used by strtok() as an index

   //tilt,rotate,rpm_tilt,rpm_rotate

    strtokIndx = strtok(inputBuffer, ","); // this continues where the previous call left off
    tilt_hoek   = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ",");
    rotate_hoek = atoi(strtokIndx);     // convert this part to a integer

    strtokIndx = strtok(NULL, ",");
    toerental_tilt_org = atoi(strtokIndx);     // convert this part to a integer

    strtokIndx = strtok(NULL, ",");
    toerental_rotate_org = atoi(strtokIndx);     // convert this part to a integer



}
// functie om gegevens te versturen naar de Rasberry-pi
void replyToPC() {

    if (newDataFromPC) {
        newDataFromPC = false;
        Serial.print("<");
        Serial.print(tilt_hoek);
        Serial.print(",");
        Serial.print(rotate_hoek);
        Serial.print(",");
        Serial.print(toerental_tilt_org);
        Serial.print(",");
        Serial.print(toerental_rotate_org);
        Serial.println(">");



   
      
    }
}

void loop() {
    curMillis = millis();



    //we lezen hier de gewenste hoek en toerental binnen van de Rasberry-pi binnen
    //inlezen seriële poort
    getDataFromPC();

    //Serial.println("een_op_overbrengingsverhouding_tilt=");
    //Serial.println(een_op_overbrengingsverhouding_tilt);

    //we moeten ook opletten dat de camera zich niet boven de maximale grenzen bewegen daarom hier inlezing eindeloopschakelaars
    //alleen als de camera zich binnen de aanvaarbaare grenzen voor tilt bevind zal de tilt zich mogen bewegen als dit niet het geval is
    //zal deze een ERROR:WRONG TILT VALUE geven op de seriële monitor
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
        toerental_tilt = toerental_tilt_org * een_op_overbrengingsverhouding_tilt;
        if (toerental_tilt > 4500) {
            toerental_tilt = toerental_max;
        }

        motortilt.setRPM(toerental_tilt);
        motortilt.setMicrostep(microsteps_tilt);


        //berekening aantal steps tilt
        tilt_hoek_nieuw = tilt_hoek * een_op_overbrengingsverhouding_tilt * microsteps_tilt;
        tilt_steps = tilt_hoek_nieuw / step_angle;

        //seriële monitor voor debugging tilt
        /*
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
        
        //doorgeven rotate & tilt-hoek en toerental aan Rasberry-Pi
        replyToPC();

        //aansturing stepper tilt
        motortilt.move(tilt_steps);

        




    }
    //doorsturen ERROR:WRONG TILT VALUE boodschap op de seriële monitor
    else if (schakelaarstaat1 != HIGH || schakelaarstaat2 != HIGH) {
        //doorgeven rotate & tilt-hoek en toerental aan Rasberry-Pi
        //we sturen nullen door omdat er niks zal gebeuren 
        tilt_hoek = 0;
        rotate_hoek = 0;
        toerental_tilt_org = 0;
        toerental_rotate_org = 0;
        //toerental = 0;
        //setup stappen motor 
        motortilt.setRPM(toerental_tilt_org);
        motortilt.setMicrostep(1);

        //doorsturen tilt_hoek,rotate_heok en toerental aan Rasberry-Pi
        replyToPC();
        
        //aansturen stappen motor tilt 
        motortilt.move(tilt_hoek);
    }
    //we moeten ook opletten dat de camera zich niet boven de maximale grenzen bewegen daarom hier inlezing eindeloopschakelaars
    //alleen als de camera zich binnen de aanvaarbaare grenzen voor tilt bevind zal de tilt zich mogen bewegen als dit niet het geval is
    //zal deze een ERROR:WRONG ROTATE VALUE geven op de seriële monitor
    //de laatse  2 eindelooschakelaars zijn voor de stepper tilt
    schakelaarstaat1 = digitalRead(schakelaar_1);
    schakelaarstaat2 = digitalRead(schakelaar_2);
    schakelaarstaat3 = digitalRead(schakelaar_3);
    schakelaarstaat4 = digitalRead(schakelaar_4);


    if (schakelaarstaat3 == HIGH && schakelaarstaat4 == HIGH) {
        //rotate

        //setup rotate stepper
        //toerental bereken en rekening houden met het maximale toerental van de motor 
        toerental_rotate = toerental_rotate_org * een_op_overbrengingsverhouding_rotate;
        if (toerental_rotate > 4500) {
            toerental_rotate = toerental_max;
        }
        motorrotate.setRPM(toerental_rotate);
        motorrotate.setMicrostep(microsteps_rotate);

        //berekening aantal steps rotate
        rotate_hoek_nieuw = rotate_hoek * een_op_overbrengingsverhouding_rotate * microsteps_rotate;
        rotate_steps = rotate_hoek_nieuw / step_angle;

        //seriële monitor voor debugging rotate
       /* if (rotate_hoek_nieuw != 0) {
            Serial.println("rotate_hoek_nieuw=");
            Serial.println(rotate_hoek_nieuw);
            Serial.println("rotate_steps=");
            Serial.println(rotate_steps);
            Serial.println("toerental_rotate=");
            Serial.println(toerental_rotate);
        }
        */

        //doorgeven rotate & tilt-hoek en toerental aan Rasberry-Pi
        replyToPC();

        //aansturing stepper rotate
        //gegeven string doorsturen
        motorrotate.move(rotate_steps);

    }
    //doorsturen ERROR:WRONG ROTATE VALUE boodschap op de seriële monitor
    //motoren niets laten doen 
    //lege string doorsturen , zelfde format 00,00,0000
    else if (schakelaarstaat3!=HIGH || schakelaarstaat4!=HIGH) {
        //doorgeven rotate & tilt-hoek en toerental aan Rasberry-Pi
        //we sturen nullen door omdat er niks zal gebeuren 
        tilt_hoek = 0;
        rotate_hoek = 0;
        //toerental = 0;
        toerental_rotate_org = 0;
        toerental_tilt_org = 0;
        //setup stappen motor 
        motorrotate.setRPM(toerental_rotate_org);
        motorrotate.setMicrostep(1);

        //doorsturen tilt_hoek,rotate_heok en toerental aan Rasberry-Pi
        replyToPC();

        //aansturen stappen motor tilt 
        motorrotate.move(tilt_hoek);
    }

    
   
    
    
    
    
    
    
}  
