#define maxCount 600       // 5cm, 10cm 각각 1분에 걸쳐 600번 측정 
   
const int Ss = A0;
int data[maxCount] = {0};
int count = 0;


void setup() 
{
  pinMode(Ss, INPUT);
  Serial.begin(9600);
}

void loop() 
{
  if(count >= maxCount) {while(true);}
  data[count] = analogRead(Ss);
  Serial.print(count);
  Serial.print(",");
  Serial.println(data[count]);
  delay(100);

  count++;
}
