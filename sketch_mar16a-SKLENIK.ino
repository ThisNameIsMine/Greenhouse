#include <dht.h>
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11); // RX | TX

#define WIN_11    13
#define WIN_12    12
#define WIN_21    7
#define WIN_22    6
#define DOOR_11    9
#define DOOR_12    8

//Sensors
#define stemp      A0  // A0
#define sWIN1CLOSE  2 // 3
#define sWIN2CLOSE  3 // 4
#define sDOORCLOSE  4 // 5
#define sDOOROPEN   5 // 6

//Movement - CODES
#define WIN1OPEN    1
#define WIN1CLOSE   2
#define WIN2OPEN    3
#define WIN2CLOSE   4
#define DOOROPEN    5
#define DOORCLOSE   6

#define ALLOPEN     10
#define ALLCLOSE    20

int hit = 0;

dht sensor;

int window[3];
double winTime[3] = {40,20,40};// W1 = 20, W2 = 16, DR = 18
double range = 2;
double tWanted = 25;
int totalPercent = 100;
int countPercent = 50;
int wait = 90000;// milliseconds


//################################################################################################################# Close Window - START
void ReverseHJ()
{
  /*
  for(int i = 8;i<14;i++)
  {
    digitalWrite(i,HIGH);
  }
  */
  digitalWrite(8,HIGH);
  digitalWrite(9,HIGH);
  digitalWrite(12,HIGH);
  digitalWrite(13,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  
  
}

void CloseAll()
{
  /*
  for(int i =0;i<3;i++)
  {
       CloseMove(8+(i*2),9+(i*2),4 - i);
       window[i] = 0;
  }
  */
//  CloseMove(8,9,4);
  //BTSerial.println("DVERE ZATVARANIE");
Move(DOOR_12,DOOR_11,sDOORCLOSE,(winTime[2])*1000 + 5*1000,2);
  window[2] = 0;
  //BTSerial.println("OKNO 2 ZATVARANIE");
  //CloseMove(6,7,3);
  Move(WIN_22,WIN_21,sWIN2CLOSE,(winTime[1])*1000 + 2000,2);
  window[1] = 0;
  //BTSerial.println("OKNO 1 ZATVARANIE");
  //CloseMove(12,13,2);   
  Move(WIN_12,WIN_11,sWIN1CLOSE,(winTime[0])*1000 + 2000,2);
  window[2] = 0;
  //BTSerial.println("SICKO ZAVRENE");
  
}

void CloseMove(int hPin,int lPin, int sensorPin) // Absolutny pohyb zatvor
{
  int activated = digitalRead(sensorPin);
  
  if(activated != hit)
  {
    while(activated != hit)
    {
      activated = digitalRead(sensorPin);
      digitalWrite(lPin,LOW);
      digitalWrite(hPin,HIGH);
      SensorValPrint();
    }
  } 
  digitalWrite(lPin,HIGH); 
}
void SensorValPrint()
{
  int val = digitalRead(sWIN1CLOSE); //A1
   
  int val2 = digitalRead(sWIN2CLOSE);//A2
  int val3 = digitalRead(sDOORCLOSE);
  int val4 = digitalRead(sDOOROPEN);

  String str = "";
  /*
  Serial.print("Win1:  ");
  Serial.print(val);//val
  Serial.print("      Win2:  ");
  Serial.print(val2);
  Serial.print("      DoorClose:  ");
  Serial.print(val3);
  Serial.print("      DoorOpen:  ");
  Serial.print(val4);
  */
  Serial.println();
  str += "Win1:  ";
  str += (String)val;
  str += "      Win2:  ";
  str += (String)val2;
  str += "      DoorClose:  ";
  str += (String)val3;
  str += "      DoorOpen:  ";
  str += (String)val4;

  Serial.println(str);
  //BTSerial.println(str);
  
}

double GetTemp()
{
  sensor.read22(stemp);
  return sensor.temperature;
}

int ChooseWindowOpen()
{
  if(window[0] < totalPercent && window[0]<=window[1]) return WIN1OPEN;
  if(window[1] < totalPercent && window[1] <= window[0]) return WIN2OPEN;
  if(window[2] < totalPercent) return DOOROPEN;

  return ALLOPEN;
}

int ChooseWindowClose()
{
  if(window[2] > 0) return DOORCLOSE;
  if(window[1] > 0 && window[1] >= window[0]) return WIN2CLOSE;
  if(window[0] > 0 && window[0] >= window[1]) return WIN1CLOSE;
  return ALLCLOSE; 
}



void Move(int pinH, int pinL, int sensorPin,long runTime, int dir)// sensor = close
{
  bool go = true;
  long tPassed = 0;

  int activated = digitalRead(sensorPin);
  
  switch(dir)
  {
    case 1:// OPEN
      while(go)
      {        
        if(pinH == DOOR_11)
        {
          activated = digitalRead(sensorPin);  
        }
        else
        {
          activated = 1;
        }
        
        if(activated != hit && tPassed<runTime)
        {
          digitalWrite(pinH,HIGH);
          digitalWrite(pinL,LOW);
          delay(100);
          tPassed += 110;
        }
        else
        {
          go = false;
        }
        
      }
      digitalWrite(pinL,HIGH);
     break;
     runTime = 0;
    case 2:// CLOSE
      while(go)
      {        
        activated = digitalRead(sensorPin);
        if(activated != hit && tPassed<runTime)
        {
          digitalWrite(pinH,HIGH);
          digitalWrite(pinL,LOW);
          delay(100);
          tPassed += 110;
        }
        else
        {
          go = false;
        }
        
      }
      digitalWrite(pinL,HIGH);
      //TODO
    break;
  }
  
}

//################################################################################################################# Close Window - END
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(WIN_11,OUTPUT);//Okno 1
  pinMode(WIN_12,OUTPUT);//Okno 1
  // LOW | HIGH -> CLose 
  // HIGH | LOW -> OPEN
  
  pinMode(WIN_21,OUTPUT);// Okno 2
  pinMode(WIN_22,OUTPUT);// Okno 2
  // LOW | HIGH -> CLOSE
  // HIGH | LOW -> OPEN
   
  pinMode(DOOR_11,OUTPUT); // Dvere
  pinMode(DOOR_12,OUTPUT);// Dvere
  // LOW | HIGH -> CLOSE
  // HIGH | LOW -> OPEN
  
    //------------------------- Sensors  --------------------
  // OKNO 1 max/min  
  pinMode(sWIN1CLOSE,INPUT_PULLUP);//min
  
  //OKNO 2 min  
  pinMode(sWIN2CLOSE,INPUT_PULLUP);
    
  //Dvere max/min
  
  pinMode(sDOOROPEN,INPUT_PULLUP);// max
  pinMode(sDOORCLOSE,INPUT_PULLUP);// min  

  
  delay(1000);
  


  ReverseHJ();
  Serial.println("Start");

  CloseAll();
  //BTSerial.begin(38400);
  
}
//-----------------------------------------------------------------------------

int counter = 0;
bool connection = false;
int a = 0;
//-----------------------------------------------------------------------------
void loop() 
{
  // put your main code here, to run repeatedly:
 
  
  int win = 0;  
/*
  if (BTSerial.available())
  {
    connection = true;
    Serial.println();
  }
  */
    
// MK I - working
/*
if(connection)
{  
  */
  /*
  if(a == 0)
  {
    BTSerial.println("Connected");
    delay(5000);
    a = 17;
    CloseAll();
  }
  */
  double temp = GetTemp();  
  String tempest = (String)temp;
  
  Serial.println();
  Serial.println("Teplota je: ");
  Serial.print(temp);
  //BTSerial.println(tempest);

  
  if(temp > tWanted+range)
  {//############################################################################################### OPEN - START
    win = ChooseWindowOpen();
    switch(win)
    {
        case WIN1OPEN:
            //TODO
            //BTSerial.println("Okno 1 sa Otvara. Win1. ");
            //BTSerial.print(window[0]);
            
            Move(WIN_11,WIN_12,sDOOROPEN,(winTime[0]/2)*1000,1);
            window[0] += countPercent;
            
            Serial.println("Okno 1");
            Serial.print(window[0]);

            //BTSerial.println("Win1");
            //BTSerial.print(window[0]);
            
            
            
        break;
        
        case WIN2OPEN:
            //BTSerial.println("Okno 2 sa Otvara. Win2. ");
           // BTSerial.print(window[1]);
            
            Move(WIN_21,WIN_22,sDOOROPEN,(winTime[1]/2)*1000,1);
            window[1] += countPercent;
            
            Serial.println("Okno 2 sa Otvara.Win2. ");
            Serial.print(window[1]);

            //BTSerial.println("Win2");
            //BTSerial.print(window[1]);
                                   
        break;

        case DOOROPEN:
            //BTSerial.println("Dvere sa Otvaraju.DOOR.   ");
            //BTSerial.print(window[2]);
            
            window[2] += countPercent;
            //if(window[2] == 100){Move(DOOR_11,DOOR_12,sDOOROPEN,(winTime[1]/2)*1000 + 5000,1);}else{Move(DOOR_11,DOOR_12,sDOOROPEN,(winTime[1]/2)*1000,1);} 
            Move(DOOR_11,DOOR_12,sDOOROPEN,(winTime[1]/2)*1000,1);  
                      
            Serial.println("Dvere sa Otvaraju.DOOR. ");
            Serial.print(window[2]);
            
            //BTSerial.println("DOOR. ");
            //BTSerial.print(window[2]);
        break;
    }
  }//############################################################################################### OPEN - END
  //############################################################################################### CLOSE - START
  else if(temp + range < tWanted)
  {
      win = ChooseWindowClose();
      switch(win)
      {
          case WIN1CLOSE:
               // BTSerial.println("Okno 1 sa Zatvara.Win1. ");
               // BTSerial.print(window[0]);
                
                window[0] -= countPercent;
                //if(window[0] == 0){Move(WIN_12,WIN_11,sWIN1CLOSE,(winTime[0]/2)*1000 + 2000,2);}else{Move(WIN_12,WIN_11,sWIN1CLOSE,(winTime[0]/2)*1000,2);}  
                
                Move(WIN_12,WIN_11,sWIN1CLOSE,(winTime[0]/2)*1000 + 2000,2);
                
                Serial.println("Okno 1 sa Zatvara.Win1. ");
                Serial.print(window[0]);      
                
               // BTSerial.println("Win1. ");
               // BTSerial.print(window[0]);                             
          break;
          case WIN2CLOSE:
               // BTSerial.println("Okno 2 sa Zatvara.Win2. ");
               // BTSerial.print(window[1]);   
                
                window[1] -= countPercent;                
                //if(window[1] == 0){Move(WIN_22,WIN_21,sWIN2CLOSE,(winTime[1]/2)*1000 + 2000,2);}else{Move(WIN_22,WIN_21,sWIN2CLOSE,(winTime[1]/2)*1000,2);}            

                Move(WIN_22,WIN_21,sWIN2CLOSE,(winTime[1]/2)*1000 + 2000,2);
                
                Serial.println("Okno 2 sa Zatvara.Win2. ");
                Serial.print(window[1]);      
                
                //BTSerial.println("Win2. ");
               // BTSerial.print(window[1]);                   
          break;
          
          case DOORCLOSE:
               // BTSerial.println("Dvere sa Zatvaraju.DOOR. ");
               // BTSerial.print(window[2]); 
                
                window[2] -= countPercent;
                //if(window[2] == 0){CloseMove(DOOR_12,DOOR_11,sDOORCLOSE);}else{Move(DOOR_12,DOOR_11,sDOORCLOSE,(winTime[2]/2)*1000,2);}        
                //if(window[2] == 0){Move(DOOR_12,DOOR_11,sDOORCLOSE,(winTime[2]/2)*1000 + 5*1000,2);}else{Move(DOOR_12,DOOR_11,sDOORCLOSE,(winTime[2]/2)*1000,2);}        
                Move(DOOR_12,DOOR_11,sDOORCLOSE,(winTime[2]/2)*1000 + 5*1000,2);
                Serial.println("Dvere sa Zatvaraju.DOOR. ");
                Serial.print(window[2]);      
                
               // BTSerial.println("DOOR. ");
               // BTSerial.print(window[2]);                       
          break;
      }
   }//############################################################################################### CLOSE - END  
//}
/*
else{
  counter ++;
  String s = (String)counter;
  Serial.println(s);
  connection = false;
}
*/
  
  //delay(wait);
  await(wait);
    
  }

  void await(int _time)
  {
    int tpassed = 0;
    
    
    
    while (tpassed < _time)
    {
      if (BTSerial.available())
      {
        connection = true;
        Serial.println();
      }
      
      delay(100);
      tpassed += 110;
    }
  }



  

  
