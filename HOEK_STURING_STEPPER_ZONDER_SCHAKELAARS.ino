//bron code communicatie arduino raspberry pi: https://forum.arduino.cc/index.php?topic=225329.msg1810764#msg1810764
#include "AccelStepper.h"
#include "MultiStepper.h"
#include "Arduino.h"




/*
steppers tegelijktijd laten bewegen https://www.airspayce.com/mikem/arduino/AccelStepper/MultiStepper_8pde-example.html

*/

//instelling variabelen communicatie
int rotate_hoek;
int tilt_hoek;
const unsigned long baudrate=115200;
const byte buffSize = 60;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;


//instelling pins

//pins voor sturing tiltstappenmotor
const byte pin_dir_tilt = 5;
const byte pin_step_tilt = 2;

//pins voor sturing rotatestappenmotor
const byte pin_dir_rotate = 6;
const byte pin_step_rotate = 3;


//parameters steppers
int tilt_hoek_nieuw;
int tilt_steps;
int toerental_tilt;
int rotate_hoek_nieuw;
int rotate_steps;
int toerental_rotate;

//instelling steppers
float step_angle = 1.8;
const int steps_motor= 200;
const byte microsteps = 16;
const int toerental_max = 4500;


//tilt
int rpm_tilt =2000;
int sps_tilt=(rpm_tilt/60)*steps_motor;
float versnelling_tilt= 1000;
float sps_versnelling_tilt=(versnelling_tilt/60)*steps_motor;

//rotate
int rpm_rotate = 2000;
int sps_rotate=(rpm_rotate/60)*steps_motor;
float versnelling_rotate= 1000;
float sps_versnelling_rotate=(versnelling_rotate/60)*steps_motor;

//voor multistepper 
long positions[2];

//Setup steppers
AccelStepper  motortilt(AccelStepper::DRIVER, pin_step_tilt, pin_dir_tilt);
AccelStepper motorrotate(AccelStepper::DRIVER, pin_step_rotate, pin_step_tilt );

//multistepepr controler aanmaken
MultiStepper stappen_motoren;
void setup() {

    //aanzetten steppers
    //zijn nu gewoon effe voorbeeld waarden
    motortilt.setMaxSpeed(sps_tilt);
    motortilt.setAcceleration(sps_versnelling_tilt);
    motorrotate.setMaxSpeed(sps_rotate);
    motorrotate.setAcceleration(sps_versnelling_rotate);

    //stappen motoren toevoegen aan multistepper controller 
    stappen_motoren.addStepper(motortilt);
    stappen_motoren.addStepper(motorrotate);


	//setup Seriële monitor
	Serial.begin(baudrate);


    //Debug voor instelling steppers
    /*
    Serial.println(rpm_tilt);
    Serial.println(sps_tilt);
    Serial.println(sps_versnelling_tilt);
    Serial.println(sps_rotate);
    Serial.println(sps_versnelling_rotate);
    */

    // tell the RPI we are ready
    Serial.println("<Arduino is ready>");
}



//inlezen seriële poort
void getDataFromPC() {

    // receive data from RPI and save it into inputBuffer
    if (Serial.available() > 5) {

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

}



// tilt_hoek,rotate_hoek en toerental worden uit de gegeven string gehaald
void parseData() {

    // split the data into its parts

    char* strtokIndx; // this is used by strtok() as an index

   //<tilt,rotate,rpm_tilt,rpm_rotate>
   //<XX,XX,XXX,XXX>

    strtokIndx = strtok(inputBuffer, ",");      // this continues where the previous call left off
    tilt_hoek   = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ",");
    rotate_hoek = atoi(strtokIndx);     // convert this part to a integer

    strtokIndx = strtok(NULL, ",");
    toerental_tilt = atoi(strtokIndx);     // convert this part to a integer

    strtokIndx = strtok(NULL, ",");
    toerental_rotate = atoi(strtokIndx);     // convert this part to a integer
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
        Serial.print(toerental_tilt);
        Serial.print(",");
        Serial.print(toerental_rotate);
        Serial.println(">");
    }
}



void loop() {

    //we lezen hier de gewenste hoek en toerental binnen van de Raspberry-pi binnen
    getDataFromPC();

    //steppers aansturen

    //tilt

    //toerental berekenen plus rekening houding met het maximale toerental van de stappenmotor
    if (toerental_tilt > toerental_max) {
        toerental_tilt = toerental_max;    }

    //motortilt.setRPM(toerental_tilt);

    //berekening aantal steps tilt
    tilt_hoek_nieuw = tilt_hoek * microsteps;
    tilt_steps = round(tilt_hoek_nieuw / step_angle);


    //tilt steps toevoegen aan positions array
    positions[0]=tilt_steps;
    





    //rotate


    //toerental bereken en rekening houden met het maximale toerental van de motor
    if (toerental_rotate > toerental_max) {
        toerental_rotate = toerental_max;
    }

    //motorrotate.setRPM(toerental_rotate);

    //berekening aantal steps rotate
    rotate_hoek_nieuw = rotate_hoek * microsteps;
    rotate_steps = round(rotate_hoek_nieuw / step_angle);

    //rotate steps toevoegen aan positie array
    positions[1]=rotate_steps;

    //multistepper aansturen
    //beide stappenmotoren bewegen naar de opgeven posities, tot dat gebeurd is gaat het programma niet verder
    stappen_motoren.moveTo(positions);
    stappen_motoren.runSpeedToPosition();
    //Serial.println(positions[0]);
    //Serial.println(positions[1]);



    //doorgeven tilt_hoek, rotate_hoek, tilt_toerental_org, rotate_hoek_org naar RPI
    replyToPC();
    

}
