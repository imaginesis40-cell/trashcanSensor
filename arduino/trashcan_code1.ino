//              << 11.24 토론 결정사항 >>
//        1. 화재센서 총 개수 3개로 결정. 센서 중 최댓값을 반환하는 알고리즘            (완료)
//        2. 초음파센서 개수 총 3개로 결정. 초음파 측정 값의 평균을 반환하는 알고리즘    (완료)
//        3. LCD 디스플레이 출력 디자인 결정.  1순위. 배터리형 UI 모방
//                                           2순위. 퍼센트 출력-----------------------(완료)
//                                            

//               <<자체 개선안 >>
//        1. 공학적 의미가 없는 화재센서5:초음파센서1 의 이중루프문 대신 milllis - prevmillis 값 사용
//            -> 이중루프문 -> 루프(분기문)으로 변경
//           개선사항: 1. delay(5000)동안 CPU를 강제중지하는 문제 해결 -> 화재에 대한 반응성 향상

# include <Wire.h>
# include <LiquidCrystal_I2C.h>

# define C 261
# define D 300
# define E 330
# define F 350 
# define G 400
# define A 440
# define B 490
int siren[9] = {C, D, E, F, G, F, E, D, C};

# define EMERGENCYVALUE 183.45       //   10cm일 떄의 센서 아날로그값 평균에서
                                     //   3시그마 일 때 5cm에서의 오차크기를 뺀 값
# define numOfSensor1 3              // 화재센서의 개수 매크로
# define numOfSensor2 3              // 초음파센서의 개수 매크로 

                                          // << 빈 쓰레기통의 깊이 매크로 >>
                                                                          // 이 값을 기준으로 측정값을 백분율로 변환한 후
int maxDepthCase[9] = {60, 65, 70, 75, 80, 85, 90, 95, 100};              // 범위에 따라 배열값 지정 -> switch문으로 알맞은 LCD 출력

int senseMaxFlame(void);             // 화재센서들 중 가장 큰 아날로그 값 반환
float setDepthValue(void);           // 쓰레기 양을 측정하는 함수
int mapPrintPercent(float);          // 쓰레기 양을 55,60,65, ..., 100 배열에 대응시키는 반복문
void EMERGENCY (void);               // 화염 감지 시 프로그램 차단 후 경보 울리는 함수

                                     // 초음파 센서
int trigSs[numOfSensor2] = {0}; int echoSs[numOfSensor2] = {0};                           

int fireSs[numOfSensor1] = {0};      // 화재 감지 센서 핀
                                     // 출력장치 //
const int piezo = 9;                 // 피에조 부저,  아날로그  설정 (pin 9)
                                     //
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 디스플레이, LiquidCrystal_I2C 클래스의 이름을 "lcd"로 함
                                     // 자동으로 A4,A5 핀으로 설정됨 -> pinMode 필요없음
                                     //--------------------------------------------------------


float setDepth = 0;                  // 빈 상태의 쓰레기통 깊이

int flameValue = 0;                  // loop 반복하며 감지하는 화염 센서 값
float depthValue = 0;                // loop 반복하며 측정하는 쓰레기통 빈 높이
int mapedValue = 0;                  // mapping하는 함수의 반환값 55, 60, 65, ..., 100

unsigned long pastmillis = 0;         // unsigned long -> int보다 바이트 크기 2~3배가 
                                     // 이론상 millis()값이 오버플로우 되어도 millis() - pastmillis() 값은 정상적으로 연산됨
                                     // (시간의 끝 - 과거시간) + 새로운 시작시간 == 정상적인 시간
void setup() 
{
  lcd.init();                        // A4, A5핀에 연결한 디스플레이 시작                
  lcd.backlight();                   // 화면 on
  lcd.setCursor(3,0);                // 디스플레이 커서 왼쪽에서 3번째 칸

// <<화재감지센서 세팅>>
for(int i = 0; i < numOfSensor1; i++)
    {
     fireSs[i] = A0 + i;                // 화재감지센서, 아날로그 핀 (A0, A1, A2)
    }                                
  
for(int i = 0; i < numOfSensor1; i++)
    {
     pinMode(fireSs[i], INPUT);         // 화재감지센서, 아날로그 입력
    }      

// <<초음파센서 세팅>>
for(int i = 0; i < numOfSensor2; i++)
    {
    trigSs[i] = 2*i + 3;                // 초음파센서0 => 3,2번핀  
    pinMode(trigSs[i], OUTPUT);    
    } 
                                        // 초음파센서1 => 5,4번핀
for(int i = 0; i < numOfSensor2; i++)   // 초음파센서2 => 7,6번핀
    {
      echoSs[i] = 2*i + 2;  
      pinMode(echoSs[i], INPUT);         
    }

    pinMode (piezo, OUTPUT);            // 출력장치 세팅

    setDepth = setDepthValue();         // 쓰레기통의 바닥까지의 거리 설정

  lcd.print("ProgramStart"); 
  delay(1000);
  lcd.clear();
  lcd.noBacklight();
}

void loop() 
{
  int maxFlame = senseMaxFlame();

      if(maxFlame > EMERGENCYVALUE)                // 정상 상태보다 높으면
        {
        EMERGENCY();                               // 비상 울림
        }                                          

  if(millis() - pastmillis >= 5000)                // 마지막 쓰레기통의 쓰레기량 측정으로부터 5초가 지나면
    {
    depthValue = ((setDepth - setDepthValue())/setDepth)*100;   // 쓰레기통의 가득 찬 정도를 백분율로 표현
    if(depthValue < 0) {depthValue = 0;}        // 바닥까지 거리가 음수일 때 100%로 인식

    mapedValue = mapPrintPercent(depthValue);
      if(mapedValue != 0)                          // mapedeValue -> 55%이하면 0 처리
        {                                          // 55 이상이면 60, 65, ..., 100, 즉 5 단위로 올림처리
          lcd.home();                              // 따라서 코드 122는  쓰레기통이 약 55% 이상 찼는지 확인하는 조건문
          lcd.print(mapedValue);                   // LCD 디스플레이 ON
          lcd.print("% FULL"); 
          lcd.backlight();                         // %백분율 텍스트로 출력
        }
        else
          {
           lcd.clear();                            // 쓰레기통이 55%이하면 
           lcd.noBacklight();                      // LCD 디스플레이 OFF
          }                                        
     pastmillis = millis();
    }
}                                                  

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

float setDepthValue()
{
  float duration = 0;
  float cmSum[numOfSensor2] = {0};
  int   cmSumcount = 0;
  float cm = 0;
  float mean = 0;
  int count = 0;

  while(count < numOfSensor2 * 3)
    {
    for(int i = 0; i < numOfSensor2; i++)     // while(for) 이중루프문
        {                                     // 센서 3개에 대한 합을 3번 구한다
        digitalWrite(trigSs[i], HIGH);        
        delayMicroseconds(10);
        digitalWrite(trigSs[i], LOW);

        duration = pulseIn(echoSs[i], HIGH); 
        cm = duration / 58.0;
        cmSum[cmSumcount] += cm;              // 센서 3개에 대한 합 측정 1세트       

        count++;                          
        delay(10); 
        }
    cmSumcount++;                             // 센서 3개에 대한 합 인덱스 이동
    }                                         // 3 * 3 합 연산완료
                                              // 현재 배열: [3개합], [3개합], [3개합] 구조
  for(int i = 0; i < numOfSensor2; i++)
      mean += cmSum[i];                       // 배열 합하여 총합 연산

      mean = mean/(numOfSensor2 * 3) ;        //  총합  (센서의 개수*루프횟수)
return mean;
}

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

int mapPrintPercent(float percentDepth)
{
  int percentToPrint = 0;
   if(percentDepth < 55.0) return 0;

  for(int i = 0; i < 9; i++)
    {
      if(percentDepth <= 60.0 + 5*i)
        {
        percentToPrint = maxDepthCase[i];  // 55~60 -> 60, 65~70 -> 70, ..., 95~100 -> 100
        return percentToPrint;
        }
    }
  return 100;                              // percentDepth가 100% 이상일 때 100으로 상한선
}