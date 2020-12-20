//bron code communicatie Arduino Rasberry Pi: https://forum.arduino.cc/index.php?topic=225329.msg1810764#msg1810764
#include <Servo.h>
#include <math.h>

//instelling servo's
Servo tilt_servo;
Servo rotate_servo;



//instelling variabelen communicatie
const byte buffSize = 16;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;
unsigned long curMillis;
unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 10;

//instelling pins


//instelling pins servo's
const int pin_tilt_servo = 2;
const int pin_rotate_servo = 3;


//pins voor eindeloopschakelaars
//pins voor tilt eindeloopschakelaars
#define schakelaar_1  A0
#define schakelaar_2  A1
//pins voor rotate eindeloopschakelaars
#define schakelaar_3  A2
#define schakelaar_4  A3



//paramters steppers

int tilt_steps;
int rotate_steps;
int rotate_hoek_nieuw;


//int toerental;
int toerental_tilt;
int toerental_tilt_org;
int toerental_rotate;
int toerental_rotate_org;



//variabelen voor hoeken tilt en rotate
int rotate_hoek;
int tilt_hoek;
int tilt_hoek_nieuw=90;
int tilt_hoek_vorige=90;
int rotate_hoek_vorige;
int tilt_hoek_hiervoor;
int rotate_hoek_hiervoor;
bool x=0;
bool y = 0;




//variabelen om status van schakelaars weer te geven
bool schakelaarstaat1;
bool schakelaarstaat2;
bool schakelaarstaat3;
bool schakelaarstaat4;


void setup() {



	//setup Seri�le monitor
	Serial.begin(115200);

    // tell the PC we are ready
    Serial.println("<Arduino is ready>");

	//Setup schakelaars
    
	pinMode(schakelaar_1, INPUT_PULLUP);
	pinMode(schakelaar_2, INPUT_PULLUP);
    pinMode(schakelaar_3, INPUT_PULLUP);
    pinMode(schakelaar_4, INPUT_PULLUP);

    //setup servo's    

    tilt_servo.attach(pin_tilt_servo);
    //tilt_servo.write(90);
    rotate_servo.attach(pin_rotate_servo);
    rotate_servo.write(90);
    
}
//inlezen seri�le poort 
void getDataFromPC() {

    // receive data from PC and save it into inputBuffer
    //minimale string is <0,0,0,0>
    // de lengte hiervan is 9 dus er moet maar iets uitgevoerd worden als de inglezen string langer is dan 9

    if (Serial.available() > 9) {

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
    // hier wordt dus niets uitgevoerd;
    /*
    else
    {
        z = 0;
    }
    */
    /*
    else {
        //als er niets doorgeven wordt moet er niets worden bewogen 
        //er wordt niets bewogen als alle hoeken nul zijn en het toerental ook nul is
        tilt_hoek = 0;
        rotate_hoek = 0;
        toerental_tilt_org = 0;
        toerental_rotate_org = 0;
    }
    */
}
// tilt_hoek,rotate_hoek en toerental worden uit de gegeven string gehaald
void parseData() {
   

    // split the data into its parts

    char* strtokIndx; // this is used by strtok() as an index

   //<tilt,rotate,rpm_tilt,rpm_rotate>
   //<XX,XX,XXX,XXX>

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
void move_sevo_tilt(){
  if (x!=1){
    tilt_servo.write(90);
  }
  x=1;
  
  if (tilt_hoek_hiervoor!=tilt_hoek){
      tilt_hoek_nieuw=tilt_hoek_vorige+tilt_hoek; 
  if (tilt_hoek_nieuw<0) {
    tilt_hoek_nieuw=0; 
  }
  if (tilt_hoek_nieuw>180){
    tilt_hoek_nieuw=180;

  }
  tilt_servo.write(tilt_hoek_nieuw);
  tilt_hoek_hiervoor=tilt_hoek;
  tilt_hoek_vorige=tilt_hoek_nieuw;
  }
  

/*
        if (tilt_hoek <0) {
          
    
            //tilt_servo.attach(pin_tilt_servo);
            //delay(15);
            tilt_hoek_nieuw = map(tilt_hoek, 0, -19, 0, 90);
            tilt_servo.write(tilt_hoek_nieuw);
            //delay(100);
            //tilt_servo.detach();
            //delay(100);
            
        }
        if (tilt_hoek > 0) {
            //tilt_servo.attach(pin_tilt_servo);
            //delay(15);
            tilt_hoek_nieuw = map(tilt_hoek, 0, 19, 90, 180);
            tilt_servo.write(tilt_hoek_nieuw);
            //delay(100);
            //tilt_servo.detach();
            //delay(100);
   

    }
    */

      
     
    }
  
  

void move_servo_rotate() {
    //als er een nieuwe hoek wordt doorgestuurd beweegt de servo naar de nieuwe hoek
    if (rotate_hoek != rotate_hoek_vorige) {
        if (rotate_hoek_nieuw  >=0 && rotate_hoek_nieuw <= 180) {
            rotate_hoek_nieuw = rotate_hoek_nieuw + rotate_hoek;
            rotate_servo.write(rotate_hoek_nieuw);
            rotate_hoek_vorige = rotate_hoek;
            //Serial.println(rotate_hoek_nieuw);
        }
        


   

    }
    //de gecommentende code is  zijn alle vorige versies van deze functie
    /*
    if (rotate_hoek_vorige != rotate_hoek) {
        Serial.println(rotate_hoek);
        rotate_hoek_nieuw += rotate_hoek;
        if (rotate_hoek_nieuw > 180) {
            rotate_hoek_nieuw = 180;
        }
        if (rotate_hoek_nieuw < 0) {
            rotate_hoek_nieuw = 0;
        }
        rotate_servo.write(rotate_hoek_nieuw);
        rotate_hoek_vorige = rotate_hoek;
        
    }
    */
  
    /*
    //Serial.println("lal");
    if (0 <= rotate_hoek_nieuw <= 180) {
    
    }
    */

    
    /*
        if (rotate_hoek < 0) {
         
        //rotate_servo.attach(pin_rotate_servo);
        //delay(15);
        rotate_hoek_nieuw = map(rotate_hoek, 0, -32, 0, 90);
        rotate_servo.write(rotate_hoek_nieuw);
        //delay(100);
        //rotate_servo.detach();
        //delay(100);
        
    }
   
    if (rotate_hoek > 0) {
    
        //rotate_servo.attach(pin_rotate_servo);
        //delay(15);
        rotate_hoek_nieuw = map(rotate_hoek, 0, 32, 90, 180);
        rotate_servo.write(rotate_hoek_nieuw);
        //delay(100);
        //rotate_servo.detach();
        //delay(100);
        
    } 
    */
    }
    


void loop() {
    curMillis = millis();



    //we lezen hier de gewenste hoek en toerental binnen van de Rasberry-pi binnen
    //inlezen seri�le poort
    getDataFromPC();
    
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
            /*
            toerental_tilt = toerental_tilt_org * een_op_overbrengingsverhouding_tilt;
            if (toerental_tilt > 4500) {
                toerental_tilt = toerental_max;
            }
*/
            /*
            motortilt.setRPM(toerental_tilt);
            motortilt.setMicrostep(microsteps_tilt);
            */

            //berekening aantal steps tilt
            /*
            tilt_hoek_nieuw = tilt_hoek * een_op_overbrengingsverhouding_tilt * microsteps_tilt;
            tilt_steps = tilt_hoek_nieuw / step_angle;
            */
            //seri�le monitor voor debugging tilt
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
            //replyToPC();


            /*
            //aansturing stepper tilt
            motortilt.move(tilt_steps);
            */

            //servo aansturen 
            move_sevo_tilt();





        }
        //doorsturen ERROR:WRONG TILT VALUE boodschap op de seri�le monitor
        else if (schakelaarstaat1 != HIGH || schakelaarstaat2 != HIGH) {
            //doorgeven rotate & tilt-hoek en toerental aan Rasberry-Pi
            //we sturen nullen door omdat er niks zal gebeuren 
            tilt_hoek = 0;
            rotate_hoek = 0;
            toerental_tilt_org = 0;
            toerental_rotate_org = 0;
            //toerental = 0;
            //setup stappen motor 
            /*
            motortilt.setRPM(toerental_tilt_org);
            motortilt.setMicrostep(1);
            */
            //doorsturen tilt_hoek,rotate_heok en toerental aan Rasberry-Pi
            //replyToPC();

            /*
            //aansturen stappen motor tilt
            motortilt.move(tilt_hoek);
            */
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
            /*
            toerental_rotate = toerental_rotate_org * een_op_overbrengingsverhouding_rotate;
            if (toerental_rotate > 4500) {
                toerental_rotate = toerental_max;
            }
            */
            /*
            motorrotate.setRPM(toerental_rotate);
            motorrotate.setMicrostep(microsteps_rotate);
            */
            /*
            //berekening aantal steps rotate
            rotate_hoek_nieuw = rotate_hoek * een_op_overbrengingsverhouding_rotate * microsteps_rotate;
            rotate_steps = rotate_hoek_nieuw / step_angle;
            */
            //seri�le monitor voor debugging rotate
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
            //replyToPC();


            /*
            //aansturing stepper rotate
            //gegeven string doorsturen
            motorrotate.move(rotate_steps);
            */

            //servo's aansturen 
      
            move_servo_rotate();

        }
        //doorsturen ERROR:WRONG ROTATE VALUE boodschap op de seri�le monitor
        //motoren niets laten doen 
        //lege string doorsturen , zelfde format 00,00,0000
        else if (schakelaarstaat3 != HIGH || schakelaarstaat4 != HIGH) {
            //doorgeven rotate & tilt-hoek en toerental aan Rasberry-Pi
            //we sturen nullen door omdat er niks zal gebeuren 
            tilt_hoek = 0;
            rotate_hoek = 0;
            //toerental = 0;
            toerental_rotate_org = 0;
            toerental_tilt_org = 0;
            /*
            //setup stappen motor
            motorrotate.setRPM(toerental_rotate_org);
            motorrotate.setMicrostep(1);
            */
            //doorsturen tilt_hoek,rotate_heok en toerental aan Rasberry-Pi
            //replyToPC();

            /*
            //aansturen stappen motor tilt
            motorrotate.move(tilt_hoek);
            */
        }




        //doorgeven tilt_hoek,rotate_hoek,tilt_toerental_org,rotate_hoek_org naar RPI
       //replyToPC();


   
    
    
    
}  
