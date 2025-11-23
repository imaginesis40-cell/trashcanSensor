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
# define numOfSensor 3
# define LOOP 12                     // 프로그램의 기본 주기. LOOP가 12이면 화재감지 12번 -> 쓰레기 양 감지 1번
                                     // LOOP 조절하여 센서 측정 속도 변경 가능
int senseMaxFlame(void);             // numOfSensor의 센서들 중 가장 큰 값 반환
float setDepthValue(void);           // 쓰레기 양을 측정하는 함수 
void EMERGENCY (void);               // 화염 감지 시 프로그램 차단 후 경보 울리는 함수


const int trigSs = 3;                // 초음파 출력,  디지털   설정 (pin1)   
const int echoSs = 2;                // 초음파 입력 , 디지털   설정 (pin2) 
int fireSs[numOfSensor] = {0};       // 화재 감지 센서

                                     // 출력장치 //
const int piezo = 9;                 // 피에조 부저,  아날로그  설정 (pin 9)
                                     //
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 디스플레이, LiquidCrystal_I2C 클래스의 이름을 "lcd"로 함
                                     // 자동으로 A4,A5 핀으로 설정됨 -> pinMode 필요없음
                                     //--------------------------------------------------------


float setDepth = 0;                  // 빈 상태의 쓰레기통 깊이
int   loopCount= 1;                  // LOOP 실행 횟수 최대 12
int flameValue = 0;                  // loop 반복하며 감지하는 화염 센서 값
int depthValue = 0;                  // loop 반복하며 측정하는 쓰레기통 빈 높이


void setup() 
{
  lcd.init();                        // A4, A5핀에 연결한 디스플레이 시작                
  lcd.backlight();                   // 화면 on
  lcd.setCursor(3,0);                // 디스플레이 커서 왼쪽에서 3번째 칸

                                     // 센서 //
for(int i = 0; i < numOfSensor; i++)
    {
     fireSs[i] = A0 + i;             // 화재감지센서, 아날로그 핀 (A0~)
    }                                
  
for(int i = 0; i < numOfSensor; i++)
    {
     pinMode(fireSs[i], INPUT);         // 화재감지센서, 아날로그 입력
    }         
    pinMode(trigSs, OUTPUT);            // 초음파 센서 세팅
    pinMode(echoSs, INPUT);

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
        }                                          // 추후에 정상 상태보다 높은 상태를,
                                                   // 정상 상태보다 유의미하게 높은 상태로 변경 필요
  if(loopCount == LOOP)                            // 화염 감지를 LOOP만큼 실행했으면
    {
    depthValue = setDepthValue();                  // 쓰레기통의 가득 찬 정도를 측정한다. 
                                                   
      if(depthValue < 7)                           // 쓰레기통이 가득 찼으면
        {
          lcd.home();
          lcd.print("TRASHCAN IS FULL");           // LCD 디스플레이 ON
          lcd.backlight();
        }
        else if(depthValue >= 7)
          {
           lcd.clear();                            // 쓰레기통이 가득 안 찼으면
           lcd.noBacklight();                      // LCD 디스플레이 OFF
          } 
     loopCount = 1;                                // 루프카운트 +1
    }

  loopCount++;
  delay(5000);                                       // LOOP = 12, delay = 5
}                                                    // LOOP * delay = 60s -> 프로그램 주기 딱 1분

int senseMaxFlame()
{
  int flameValueArr[numOfSensor] = {0};
      for(int i = 0; i < numOfSensor; i++)
        {
          flameValueArr[i] = analogRead(fireSs[i]);
        }

  int maxOfFlame = flameValueArr[0];
      for(int i = 0; i < numOfSensor; i++)
        {
          if(maxOfFlame <= flameValueArr[i])
             maxOfFlame = flameValueArr[i];
        }
    return maxOfFlame;
}

float setDepthValue()
{
  float duration = 0;
  float cmSum = 0;
  float cm = 0;
  float mean = 0;
  int count = 0;

  while(count < 10)
    {
    digitalWrite(trigSs, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigSs, LOW);

    duration = pulseIn(echoSs, HIGH);
    cm = duration / 58.0;
    cmSum += cm;

    count++;
    delay(10);
    }
  mean = cmSum/count;
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