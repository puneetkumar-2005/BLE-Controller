
//  CREATION OF LORD PUNEET
#include<SoftwareSerial.h>
SoftwareSerial btserial(2,3);
//long int transfer=0;
//int ru,rd,lu,ld,ls,rs,us,ux,uy,lx,ly,rx,ry,pot,but3,but4,val;
void setup()
{
Serial.begin(9600);
btserial.begin(9600);
pinMode(13,INPUT);
pinMode(12,INPUT);
pinMode(11,INPUT);
pinMode(10,INPUT);
pinMode(9,INPUT);
pinMode(8,INPUT);
pinMode(7,INPUT);
pinMode(6,OUTPUT);
pinMode(5,OUTPUT);
pinMode(4,OUTPUT);


digitalWrite(4,LOW);
delay(100);
digitalWrite(4,HIGH);
digitalWrite(5,LOW);
delay(100);
digitalWrite(5,HIGH);
digitalWrite(6,LOW);
delay(100);
digitalWrite(6,HIGH);
delay(100);

}
void loop()
{
long int transfer=0;
int ru,rd,lu,ld,ls,rs,us,ux,uy,lx,ly,rx,ry,pot,but3,but4,val;

ru=digitalRead(7);
rd=digitalRead(10);
lu=digitalRead(9);
ld=digitalRead(8);
ls=digitalRead(13);
rs=digitalRead(12);
us=digitalRead(11);
ux=map(analogRead(A2),0,1023,1,9);
uy=map(analogRead(A1),0,1023,1,9);
lx=map(analogRead(A3),0,1023,1,9);
ly=map(analogRead(A5),0,1023,1,9);
rx=map(analogRead(A6),0,1023,1,9);
ry=map(analogRead(A7),0,1023,1,9);
pot=map(analogRead(A4),0,1023,1,9);//a4 or a5

but3=(us*4+rs*2+ls+1);
val=(lu*8+ld*4+ru*2+rd);
if(val<3)
but4=(val+1);
else if(val>7)
but4=(val-1);
else
but4=val;

transfer+=but4;transfer*=10;
transfer+=but3;transfer*=10;
transfer+=pot;transfer*=10;
transfer+=ry;transfer*=10;
transfer+=rx;transfer*=10;
transfer+=ly;transfer*=10;
transfer+=lx;transfer*=10;
transfer+=ux;transfer*=10;
transfer+=uy;

btserial.println(transfer);
delay(10);
}
