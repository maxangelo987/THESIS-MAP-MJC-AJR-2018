#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 28
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);



#define SensorPin A1            //pH meter Analog output to Arduino Analog Input 
#define Offset 0.00             //deviation compensate
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40          //times of collection
int pHArray[ArrayLenth];        //Store the average value of the sensor feedback
int pHArrayIndex=0;
const int solepin = 22;
const int solepin1 = 23;    
const int buzzer = 24; 
int counter1 = 0;
int counter2 = 0;
int buzzercount = 0;

String inputstring = "";                                                       //a string to hold incoming data from the PC
String sensorstring = "";                                                      //a string to hold the data from the Atlas Scientific product
String DOTemp = "0.00";                                                      //a string to hold the temp data
boolean input_stringcomplete = false;                                          //have we received all the data from the PC
boolean sensor_stringcomplete = false;                                         //have we received all the data from the Atlas Scientific product

int flag = 0;


void setup()
{

  pinMode(LED,OUTPUT);  
  pinMode(2,OUTPUT); 
  pinMode(solepin,OUTPUT); //Input
  pinMode(solepin1,OUTPUT); // Output
  pinMode(buzzer,OUTPUT); // Output
  Serial.begin(9600);   
        digitalWrite(solepin,HIGH);
        digitalWrite(solepin1,HIGH);
         
  sensors.begin();

  Serial3.begin(9600);                                //set baud rate for software serial port_3 to 9600
  inputstring.reserve(5);                            //set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
  
        Serial.print("| ");
        Serial.print("Restarting");
        Serial.println(" mg/L");   
        Serial.print("| ");
        Serial.print("Restarting");
        Serial.println(" pH"); 
        Serial.print("| ");
        Serial.print("Restarting");
        Serial.println(" C"); 
        //delay(1000);    
}


void serialEvent() {                                                         //if the hardware serial port_0 receives a char              
               char inchar = (char)Serial.read();                               //get the char we just received
               inputstring += inchar;                                           //add it to the inputString
               if(inchar == '\r') {input_stringcomplete = true;}                //if the incoming character is a <CR>, set the flag
               }  

void serialEvent3(){                                                         //if the hardware serial port_3 receives a char 
              char inchar = (char)Serial3.read();                              //get the char we just received
              sensorstring += inchar;                                          //add it to the inputString
              if(inchar == '\r') {sensor_stringcomplete = true;}               //if the incoming character is a <CR>, set the flag 
              }


void loop()
{
  
  //setup();
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue, voltage;
  float Temptemp;
  float pHTrue;  
  
  counter1++;
  
/////////////////////////  Temperature Sensors (Reading) /////////////////////////////
 
  sensors.requestTemperatures(); // Send the command to get temperatures

/////////////////////////////////////////////////////////////////////////////////////


//////////////////////////  pH Sensors (Reading )////////////////////////////////////

  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      samplingTime=millis();
  }
  
   if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
        pHTrue = pHValue;
        digitalWrite(LED,digitalRead(LED)^1);
        printTime=millis();
  }
  

/////////////////////////////////////////////////////////////////////////////////////

////////////////  Dissolved Oxygen Sensors (Reading/Printing) ///////////////////////


  if (input_stringcomplete){                                                   //if a string from the PC has been received in its entirety 
      Serial3.print(inputstring);                                              //send that string to the Atlas Scientific product
      inputstring = "";                                                        //clear the string:
      input_stringcomplete = false;                                            //reset the flag used to tell if we have received a completed string from the PC
      }

  if (sensor_stringcomplete){                                                   //if a string from the Atlas Scientific product has been received in its entierty 
        Serial.print("| ");
        Serial.print(sensorstring);  // DO
        Serial.println(" mg/L");
      
      DOTemp = sensorstring;
      sensorstring = "";                                                       //clear the string:
      sensor_stringcomplete = false;                                           //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
      flag = 0;
      counter2 = 0;
      }

  else{
       
        flag++;
       
       // DOTemp = "";
       if (flag > 100)
        {
        delay(2000);
        software_Reset();          // Software Reset
        flag = 0;
        }
        
       else{
        Serial.print("| ");
        Serial.print("Reading mg/L (");   // DO
        Serial.print(counter2);
        Serial.println(" times)");
        counter2++;
       }
      }
       //Serial.print(flag);
       
/////////////////////////////////////////////////////////////////////////////////////      

    

/////////////  pH Sensors (Printing) && Controlling the Valve////////////////////////
      
   if (counter1 > 40 ){
        
        if (((pHValue < 6.5) || (pHValue < 6.7))){
          if (buzzercount < 4){
            digitalWrite(buzzer,HIGH);
            buzzercount++;
          }
          
          else {
            digitalWrite(buzzer,LOW);
          }
          
          digitalWrite(solepin,LOW);
          digitalWrite(solepin1,LOW);
          
            Serial.print("| ");
            Serial.print(pHTrue);
            Serial.println(" pH (ACIDIC)");
        }
        
        else if (((pHValue > 8.8) || (pHValue > 9))){
           if (buzzercount < 4){
            digitalWrite(buzzer,HIGH);
            buzzercount++;
          }
          
          else {
            digitalWrite(buzzer,LOW);
          }
          
          digitalWrite(solepin,LOW);
          digitalWrite(solepin1,LOW);
          
            Serial.print("| ");
            Serial.print(pHTrue);
            Serial.println(" pH (BASIC)");
        }

        else{
          
          digitalWrite(solepin,HIGH);
          digitalWrite(solepin1,HIGH);
          
          
            Serial.print("| ");
            Serial.print(pHTrue);
            Serial.println(" pH (NORMAL)");
            buzzercount = 0;
        }
   }
   
   else{
       digitalWrite(solepin,HIGH);
       digitalWrite(solepin1,HIGH);
        Serial.print("| ");
        Serial.print("Reading pH in (");
        Serial.print(40-counter1);
        Serial.println(" s)");
   }
      
/////////////////////////////////////////////////////////////////////////////////////      
      
/////////////////////////  Temperature Sensors (Printing) ///////////////////////////

       if (sensors.getTempCByIndex(2) != 85) 
        {
        Serial.print("| ");
        Serial.print(sensors.getTempCByIndex(2));  // The Real Time Sensor
        Serial.println(" C");
        Temptemp = sensors.getTempCByIndex(2);
        }

        else
        {
        Serial.print("| ");
        Serial.print(Temptemp);
        Serial.println(" C");  
        }
         
           delay(1000);
       
/////////////////////////////////////////////////////////////////////////////////////      
}





double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
//   Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
asm volatile ("  jmp 0");  
}  
