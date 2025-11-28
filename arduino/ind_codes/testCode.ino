// 코드 점검용
// 화재의 아날로그 값 입력 -> 조이스틱의 x 값으로 대체
// 초음파 센서의 출력장치는 led로 변경

// 삭제한 함수 2개 외 코드의 오류 점검용

# include <Wire.h>
# include <LiquidCrystal_I2C.h>


# define EMERGENCYVALUE 900    
                                    
# define numOfSensor1 3              
# define numOfSensor2 3              
                                             
                                                                          
int maxDepthCase[9] = {60, 65, 70, 75, 80, 85, 90, 95, 100};                        
float depth(void);           
int mapPrintPercent(float);          
void EMERGENCY (void);               
                       
int trigSs = 4; int echoSs = 3;                           
int ledpin = 9;
int fireSs = A3;   
                                 
LiquidCrystal_I2C lcd(0x27, 16, 2);  

float setDepth = 0;                 
int flameValue = 0;                 
float depthValue = 0;               
int mapedValue = 0;                

unsigned long pastmillis = 0;        
                                
void setup() 
{
lcd.init();                       
lcd.backlight();                 
lcd.setCursor(3,0);              

  pinMode(fireSs, INPUT);        
    
  pinMode(trigSs, OUTPUT);
  pinMode(echoSs, INPUT);

  setDepth = depth();        

  lcd.print("ProgramStart"); 
  delay(1000);
  lcd.clear();
  lcd.noBacklight();
}

void loop() 
{
  int maxFlame = analogRead(fireSs);

      if(maxFlame > EMERGENCYVALUE)                
        {
        EMERGENCY();                              
        }                                          

  if(millis() - pastmillis >= 5000)            
    {
    depthValue = ((setDepth - depth())/setDepth)*100;   
    if(depthValue < 0) {depthValue = 0;}        

    mapedValue = mapPrintPercent(depthValue);
      if(mapedValue != 0)                  
        {                                   
          lcd.home();                             
          lcd.print(mapedValue);                   
          lcd.print("% FULL"); 
          lcd.backlight();                         
        }
        else
          {
           lcd.clear();                          
           lcd.noBacklight();                   
          }                                        
     pastmillis = millis();
    }
}                                                  
float depth()
{
  float duration = 0;

  float cm = 0;
        digitalWrite(trigSs, HIGH);        
        delayMicroseconds(10);
        digitalWrite(trigSs, LOW);

        duration = pulseIn(echoSs, HIGH); 
        cm = duration / 58.0;
return cm;
}

void EMERGENCY ()
{
 while(analogRead(fireSs) >= EMERGENCYVALUE)
    {
      digitalWrite(ledpin, HIGH);
      delay(100);
    }
  digitalWrite(ledpin, LOW);
}

int mapPrintPercent(float percentDepth)
{
  int percentToPrint = 0;
   if(percentDepth < 55.0) return 0;

  for(int i = 0; i < 9; i++)
    {
      if(percentDepth <= 60.0 + 5*i)
        {
        percentToPrint = maxDepthCase[i];  
        return percentToPrint;
        }
    }
  return 100;                             
}
