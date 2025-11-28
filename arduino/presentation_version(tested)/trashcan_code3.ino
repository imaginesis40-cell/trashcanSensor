# include <Wire.h>
# include <LiquidCrystal_I2C.h>

/*             << 11.24 토론 결정사항 >>                                                             
       1. 화재센서 총 개수 3개로 결정. 센서 중 최댓값을 반환하는 알고리즘            (완료)           
       2. 초음파센서 개수 총 3개로 결정. 초음파 측정 값의 평균을 반환하는 알고리즘    (완료)           
        3. LCD 디스플레이 출력 디자인 결정.  1순위. 배터리형 UI 모방                                    
                                           2순위. 퍼센트 출력-----------------------(완료)           
***************************************************************************************//*
EMERGENCYVALUE 42 에 대해

문제상황: 화재 센서의 값으로 화재 여부를 판단해야 하지만, 기준값이 모호함

전제: 화재 감지 센서는 광원으로부터 가까울수록 오차가 커진다

해결방안: (촛불로부터 15cm 떨어진 화재센서의 반환값) - (촛불로부터 5cm 떨어진 화재센서의 최대 오차)를 화재의 기준으로 정하자

구체적 측정 방법: 1. 촛불로부터 5cm 떨어진 화재의 아날로그 값 600개를 시리얼 모니터를 통해 수집함
                 2. 아날로그 값 600개의 z-score 계산
                    >> z-score값은 정규분포 그래프의 n시그마이다
                    >> 따라서 z-score이 +3.0인 아날로그 값은 3시그마, 즉 상위 0.15%의 오차를 지닌다

                 3. (z-score이 +3.0인 아날로그 값) - (5cm일 때 측정한 아날로그 반환값의 평균) = 상위 0.15%의 오차
                 4. (15cm일 떄 측정한 아날로그 반환값의 평균) - (상위 0.15%의 오차) => 보다 엄격한 판단 기준

*****************************/

# include <Wire.h>
# include <LiquidCrystal_I2C.h>

// 피에조 부저 매크로
# define C 261
# define D 300
# define E 330
# define F 350 
# define G 400
# define A 440
# define B 490
int siren[9] = {C, D, E, F, G, F, E, D, C};

// 화재 감지 센서 기준값
# define EMERGENCYVALUE 42

// 화재감지, 초음파 센서 개수 매크로
# define numOfSensor1 3  
# define numOfSensor2 3  


/**************
|   함수 선언  |
**************/

// LCD디스플레이가 출력하는 %의 집합 (ex> 33.3 -> 40으로 매핑)
int maxDepthCase[8] = {65, 70, 75, 80, 85, 90, 95, 100};

// 화재감지 센서 3개의 반환값을 입력받아 최댓값 반환
int senseMaxFlame(void);  


// 빈 쓰레기통의 초기 깊이를 측정해 백분율 변환의 분모 값을 반환하는 함수
float setDepthValue(void);

// 초음파 센서 3개를 각각 3번 합해 개별 평균을 낸 후, 가장 큰 2개의 센서 평균의 평균 반환
// 즉, 이 함수는 가장 깊은 구간을 측정하는 센서 2개에 대한 함수이다.
// 이러한 측정 방식을 통해, 비닐 몇개가 볼록 튀어나와서 데이터를 오염시키는 경우를 방지할 수 있음
float getDepthValue(void);

// 화재감지 시 명령
void EMERGENCY (void); 

 
int trigSs[numOfSensor2] = {0}; int echoSs[numOfSensor2] = {0};                           
// 초음파 센서 핀 배열 선언
int fireSs[numOfSensor1] = {0}; 
// 화재감지 핀 배열 선언
const int piezo = 8; 
// 피에조 출력 핀 배열 선언
LiquidCrystal_I2C lcd(0x27, 16, 2); 
// LCD class 선언

float setDepth = 0;
// 쓰레기통 바닥의 깊이 (프로그램 시작 시 setup에서 1회 측정)    
float depthValue = 0; 
// 쓰레기통이 얼마나 찼는지에 대한 백분율 (가득차면 100)
int mapedValue = 0;    
// 매핑한 값 (코드 34의 maxDepthCase[9] 중 하나)


unsigned long pastmillis = 0;  
// (millis - pastmillis) => 마지막 측정으로부터 지난 시간
  
void setup() 
{
  lcd.init();          
  lcd.backlight();   
  lcd.setCursor(3,0);
  // lcd.setCursor(6,0) -> 출력값 디스플레이 정중앙 위치

//  <<화재감지센서 핀 할당>>
for(int i = 0; i < numOfSensor1; i++)
    {
     fireSs[i] = A0 + i;  
    }                                
  
for(int i = 0; i < numOfSensor1; i++)
    {
     pinMode(fireSs[i], INPUT);  
    }      

//  <<초음파센서 핀 할당>>
for(int i = 0; i < numOfSensor2; i++)
    {
    trigSs[i] = 2*i + 3;
    pinMode(trigSs[i], OUTPUT);    
    } 
 
for(int i = 0; i < numOfSensor2; i++)  
    {
      echoSs[i] = 2*i + 2;  
      pinMode(echoSs[i], INPUT);         
    }

    pinMode (piezo, OUTPUT); 

    setDepth = setDepthValue(); 
    //빈 쓰레기통의 깊이 측정 (백분율의 분모)

  lcd.print("ProgramStart"); 
  delay(1000);
  lcd.clear();
  lcd.noBacklight();
}

void loop() 
{
  int maxFlame = senseMaxFlame();     // 화재감지 센서의 최댓값을 입력받아

      if(maxFlame > EMERGENCYVALUE)   // 42 보다 크면 EMERGENCY            
        {                             // 코드 109 ~ 114는 매우 빠른 속도로 loop에 따라 실행됨
        EMERGENCY();  
        }                                          

  if(millis() - pastmillis >= 2000)   // 코드 116의 실행으로부터 2초가 지나면
    {                                 // 쓰레기량을 측정한 후 LCD 출력 여부를 판단
    depthValue = ((setDepth - getDepthValue())/setDepth)*100;  
    if(depthValue < 0) {depthValue = 0;}  

    mapedValue = mapPrintPercent(depthValue);
                                      // 코드 122> float백분율을 공차 5짜리 int에 매핑
      if(mapedValue != 0)                 
        {                
          lcd.home();    
          lcd.print(mapedValue);      // 매핑한 int값 LCD로 출력      
          lcd.print("% FULL"); 
          lcd.backlight();            
        }
        else
          {
           lcd.clear();               // 매핑한 int값이 55 이하면 백라이트 OFF
           lcd.noBacklight();         // 경제성 고려한 설계
          }                                        
     pastmillis = millis();
    }
}                                                  

/* <<화재감지 센서 3개의 반환값 중 최댓값 반환 함수>> */
int senseMaxFlame()
{
  int flameValueArr[numOfSensor1] = {0};
      for(int i = 0; i < numOfSensor1; i++)
        {
          flameValueArr[i] = analogRead(fireSs[i]);
        }

  int maxOfFlame = flameValueArr[0];
      for(int i = 0; i < numOfSensor1; i++)
        {
          if(maxOfFlame <= flameValueArr[i])
             maxOfFlame = flameValueArr[i];
        }
    return maxOfFlame;
}
/* <<setup에서 1회 실행되어 빈 쓰레기통의 깊이 반환 함수>> */
float setDepthValue(void)
{
  float duration = 0;
  float cmSum[numOfSensor2] = {0};
  int   cmSumcount = 0;
  float cm = 0;
  float mean = 0;
  int count = 0;

  while(count < numOfSensor2 * 3)
    {
    for(int i = 0; i < numOfSensor2; i++) 
        { 
        digitalWrite(trigSs[i], HIGH);        
        delayMicroseconds(10);
        digitalWrite(trigSs[i], LOW);

        duration = pulseIn(echoSs[i], HIGH); 
        cm = duration / 58.0;
        cmSum[cmSumcount] += cm; 

        count++;                          
        delay(10); 
        }
    cmSumcount++;  
    }            

  for(int i = 0; i < numOfSensor2; i++)
      mean += cmSum[i];                     

      mean = mean/(numOfSensor2 * 3) ;   
return mean;
}

/* <<쓰레기 양을 측정하여 평균을 반환하는 함수>> */
float getDepthValue()
{
  float duration = 0;
  float cmSum[numOfSensor2] = {0};
  int   cmSumcount = 0;
  float cm = 0;
  float mean = 0;
  int count = 0;

  while(count < numOfSensor2 * 3)
    {
    for(int i = 0; i < 3; i++)  // 해석순서1. 각 센서당 연속해서 3번씩 값을 측정하여 합한 후
      {
      digitalWrite(trigSs[cmSumcount], HIGH);        
      delayMicroseconds(10);
      digitalWrite(trigSs[cmSumcount], LOW);

      duration = pulseIn(echoSs[cmSumcount], HIGH, 20000); // 20000마이크로초 -> 20밀리초 -> 0.02초
      cm = duration / 58.0;
      cmSum[cmSumcount] += cm;  // 해석순서2. 각 센서에 1대1로 대응하는 배열에 저장한다
                                // ex) 센서"1"의 1st, 2nd, 3d의 합은 cmSum["1"]에 저장한다
      count++;                          
      delay(10); 
      }
    cmSumcount++;
    }   

    float ind = 0;

    for(int j = 0; j < numOfSensor2 - 1; j++)     // 버블정렬 -> 프기실 내용
      {                                           // 크기가 n인 배열에 대해 n-1번 <@>을 시행하면 모든 값 정렬 가능
        for(int i = 0; i < numOfSensor2 - 1; i++) // 이번 정렬은 내림차순이다  
          {
            if(cmSum[i+1] > cmSum[i])
            {
              ind = cmSum[i+1]; cmSum[i+1] = cmSum[i];  cmSum[i] = ind; // <@>
            }
          } 
      }   

  for(int i = 0; i < numOfSensor2 - 1; i++)       // 값이 
      mean += cmSum[i];                     

      mean = mean/((numOfSensor2 - 1) * 3);  

return mean;
}

/* <<화재감지 시 비상 사이렌>> */
void EMERGENCY ()
{
 while(senseMaxFlame() >= EMERGENCYVALUE)
    {
      for(int i = 0; i < 9; i++)
        {
         if(senseMaxFlame() < EMERGENCYVALUE) 
          {noTone(piezo); return;}
        tone(piezo, siren[i], 300);
        delay(300);
        }
    }
}

/*  <<LCD 디스플레이 출력값 매핑>>  */
int mapPrintPercent(float percentDepth)
{
  int percentToPrint = 0;
   if(percentDepth < 60.0) return 0;

  for(int i = 0; i < 7; i++)
    {
      if(percentDepth <= 65.0 + 5*i)
        {
        percentToPrint = maxDepthCase[i]; 
        return percentToPrint;
        }
    }
  return 100;   
}
