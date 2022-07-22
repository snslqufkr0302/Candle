//캔들바닥과 초음파 사이의 거리 13.5cm. 캔들은 10cm로 잡고 캔들이 1cm남으면 교체시기.

//lcd를 제어하기 위한 라이브러리
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

int trigPin = 7;  //센서1 출력 핀번호
int echoPin = 6;  //센서1 입력 핀번호
int bt1 = 4;  //기본 on/off 슬라이드버튼1 입력 핀번호
int bt2 = 3;  //조도센서 이용한 on/off 슬라이드버튼2 입력  핀번호
int buz = 2;  //부저 출력 핀번호
int lig = 5;  //전구 핀번호
int ch = 0; //부저소리가 났는지에 관한 변수(0이 안난거)
unsigned long past = 0; //과거 시간 저장 변수
unsigned long now = 0;  //현재 시간 저장 변수
int count = 0; //과거시간을 1번만 저장하기위한 변수
LiquidCrystal_I2C lcd(0x27, 16, 2);  //lcd 주소 저장
int sw1 = 11; //숫자 바꾸기 위한 버튼
int sw2 = 10; //커서 넘기기 위한 버튼
int time = 0; //시간 설정 변수
unsigned long pasttime = 0; //시계용 과거 시간 변수
unsigned long timexx = 0; //시간 차이 없애기위해
int ok = 0; //시간을 제대로 설정했는지 알수있는 변수
int a = 0;  //시계 첫번째 자리
int b = 0;  //시계 두번째 자리
int c = 0;  //시계 세번째 자리
int d = 0;  //시계 네번째 자리

void setup() {
  Serial.begin(9600); //시리얼 속도 설정

  lcd.init();  //lcd 초기화
  lcd.backlight();  //lcd 백라이트 킴
  
  for(int i=0;i<4;i++){ //처음 시간 정하는부분. 4자리 숫자를 입력받음
    time = 0;
    ok=0;
    while(ok==0){
      if(i==0){
        lcd.print(time);
        lcd.print("0:00");
        lcd.setCursor(i,0);
        lcd.cursor();
      }
      else if(i==1){
        lcd.setCursor(0,0);
        lcd.print(a);
        lcd.print(time);
        lcd.print(":00");
        lcd.setCursor(i,0);
        lcd.cursor();
      }
      else if(i==2){
        lcd.setCursor(0,0);
        lcd.print(a);
        lcd.print(b);
        lcd.print(":");
        lcd.print(time);
        lcd.print(0);
        lcd.setCursor(i+1,0);
        lcd.cursor();
      }
      else if(i==3){
        lcd.setCursor(0,0);
        lcd.print(a);
        lcd.print(b);
        lcd.print(":");
        lcd.print(c);
        lcd.print(time);
        lcd.setCursor(i+1,0);
        lcd.cursor();
      }
      lcd.noCursor();
      if(digitalRead(sw1)==HIGH){ //버튼1을 누르면 숫자가 올라감
        if(time<9)
          time++;
        else
          time=0;
        lcd.init();
      }
      else if(digitalRead(sw2)==HIGH){  //버튼2를 누르면 커서가 넘어감
        if(i==0)
          a=time;
        else if(i==1)
          b=time;
        else if(i==2)
          c=time;
        else if(i==3)
          d=time;
        lcd.init();
        ok=1;
      }
    }
  }

  pinMode(sw1, INPUT);  //스위치1 입력 저항
  pinMode(sw2, INPUT);  //스위치2 입력 저항

  pinMode(trigPin, OUTPUT); //초음파센서
  pinMode(echoPin, INPUT);
  
  pinMode(lig, OUTPUT); //led 핀모드 설정
  analogWrite(lig, LOW); //처음시작할때 전구 꺼져있게 설정
  
  pinMode(bt1, INPUT_PULLUP);  //슬라이드버튼1 디지털 핀 입력모드 설정
  pinMode(bt2, INPUT_PULLUP);  //슬라이드버튼2 디지털 핀 입력모드 설정
  
  pinMode(buz, OUTPUT); //부저 출력 설정
  
  pasttime=millis();  //시간을 저장한 시간을 과거시간으로 정함
}

float dis(){  //초음파센서 거리 함수
  float duration, distance; //초음파센서 거리
  digitalWrite(trigPin, HIGH); 
  delay(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = ((float)(340 * duration)/10000)/2;
  return distance;
}

float blight(){ //조도센서 조도 함수
  float bn=analogRead(A0);  //조도 저장 변수
  return bn;
}

void sound(){ //캔들 잔여량 적을때 나는 소리
  tone(buz, 1319, 250);
  delay(250);
  tone(buz, 1109, 250);
  delay(250);
  tone(buz, 988, 250);
  delay(600);
  tone(buz, 880, 250);
  delay(250);
  tone(buz, 740, 250);
  delay(250);
  tone(buz, 622, 250);
  delay(600);
  tone(buz, 784, 250);
  delay(250);
  tone(buz, 659, 250);
  delay(250);
  tone(buz, 554, 250 );
  delay(600);
  tone(buz, 659, 1000);
  delay(1000);
}

int candle(){ //캔들 잔여량 수치화(1 단위로 0~100)
  int can = (13-dis())*10;
  if(can<0)
    can=0;
  else if(can>100)
    can=100;
  return can;
}

void ledon(){ // 잔여량에따라 전구 키는 함수
  analogWrite(lig, 155+candle());
}

void ledoff(){  //전구 끄는 함수
  analogWrite(lig, LOW);
}

void lcdcan(){  //잔여량 lcd 두번째줄에 출력하는 함수
  lcd.setCursor(0,1);
  lcd.print(candle());
  lcd.print("%");
}

void prtime(){  //시간을 lcd 첫째줄에 출력하는 함수
  lcd.setCursor(0,0);
  lcd.print(a);
  lcd.print(b);
  lcd.print(":");
  lcd.print(c);
  lcd.print(d);
}

void loop(){
  //조도 출력
  Serial.print("\nBlightness:");
  Serial.print(blight());
  
  //센서1거리 출력
  Serial.print("\nDistance:");
  Serial.print(dis());
  Serial.print("\n");

  int bt1v = digitalRead(bt1);  //버튼1의 값
  int bt2v = digitalRead(bt2);  //버튼2의 값

  Serial.print(bt1v);
  Serial.print("\n");
  Serial.print(bt2v);
  
  if(bt2v == HIGH){ //조도센서 on/off 버튼이 켜져있으면 밝기에따라 켜짐
    if(blight()>500)  //밝기가 500이상이면 조명 꺼짐
      ledoff();
    else  //밝기가 500이하면 조명 켜짐
      ledon();
  }
  else{ //그게 아니라면 기본 on/off스위치 값대로 작동
    if(bt1v == LOW)
      ledoff();
    else
      ledon();
  }
  Serial.print(digitalRead(5));
  
  if(dis()>=14&&ch==0){ //캔들 잔여량이 적으면 부저소리남.(=초음파 거리가 10이상이면 소리남.)
    sound();
    ch = 1;
    
  }
  
  if(ch==1&&dis()<12){  //부저소리가 났었고, 캔들 잔여량이 3분동안 많으면 캔들을 교체했다고 인식. 따라서 그 캔들을 다쓰면 부저 다시 울림.
    now=millis();
    if(count==0){
      past=now;
      count=1;
    }
    if(now-past>=180000){ //1분이 60000. 즉 1초에 1000
      ch=0;
      count=0;
    }
  }

  now=millis();  //과거시간으로부터 1분이 지나면 분 숫자가 올라감
  timexx=now-pasttime;
  if(timexx>=60000){
    if(d<9)
      d=d+1;
    else{
      d=0;
      if(c<5)
        c=c+1;
      else{
        c=0;
        if(a<1){
          if(b<9)
            b=b+1;
          else
            a=a+1;
        }
        else{
          if(b<3)
            b=b+1;
          else{
            a=0;
            b=0;
          }
        }
      }
    }
    pasttime=now-(timexx-60000);
  }

  Serial.print(now);
  Serial.print("\n");
  Serial.print(pasttime);

  lcd.init();
  prtime();
  lcdcan();
  
  delay(1000);
}
