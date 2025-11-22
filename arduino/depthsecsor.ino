#define maxCount 100       // 총 100번의 측정
const int echopin = 3;
const int trigpin = 4;

int count = 0;
float data[maxCount] = {0};

float duration = 0;
float cm = 0;

void setup() 
{
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  Serial.begin(9600);
  Serial.println("*****START SENSOR*****");
}

void loop() 
{
  if(count >= maxCount){while(true);}

  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  duration = pulseIn(echopin, HIGH);
  cm = duration / 58.0;

  Serial.print(count);
  Serial.print(",");
  Serial.println(cm);

  count++;
  delay(100);
}
