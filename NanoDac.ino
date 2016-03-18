/*

I2C
A4 (SDA)
A5 (SCL)

SPI
10 (SS)
11 (MOSI)
12 (MISO)
13 (SCK)

SERIAL
0 (TX)
1 (RX)

INT
2
3

FREE pins
Analog  : 0,1,2,3,6,7 (6 & 7 analog only)
Digital : 4,5,6,7,8,9

PINC, PIN 0,1,2,3 -> low 4 bits
PINC, PIN 4,5,6,7 -> high 4 bits


*/


#define pinInterrupt	2
#define pinStatus		8	// PORTB,PIN0 - Set to 0 for busy, 1 for ready


#define Busy	do { digitalWrite(8,HIGH) ; } while(0)
#define Ready	do { digitalWrite(8,LOW); } while(0)

//#include <Wire.h>

uint8_t volatile laserLevel = 255;
#define signal digitalRead(2)


void setup()
{
	DDRD &= (B00001111);	// set pin 4,5,6,7 as input
	DDRC &= (B11110000);	// set pin A0,A1,A2,A3 as input
	DDRB |= B00000001;	// set pin 8 as output
	//pinMode(4, INPUT);
	//pinMode(5, INPUT);
	//pinMode(6, INPUT);
	//pinMode(7, INPUT);
	//pinMode(A0, INPUT);
	//pinMode(A1, INPUT);
	//pinMode(A2, INPUT);
	//pinMode(A3, INPUT);
	//pinMode(pinStatus, OUTPUT);
	pinMode(2, INPUT);
	Serial.begin(115200);
	Serial.println("OK");
	//pinMode(pinInterrupt, INPUT_PULLUP);
	pinMode(10, OUTPUT);
	analogWrite(10, 255);
	delay(500);
	analogWrite(10, 0);
	Busy;	// on est occupé
	Ready;
	laserLevel = 125;
	pinMode(13, OUTPUT);
}


void loop()
{
	while (signal == true) {}
	Busy;
	laserLevel = (PINC & B00001111) | (PIND & B11110000);
	Serial.println(laserLevel);
	analogWrite(10, laserLevel);
	Ready; // on a fini, prêt pour prochaine demande
}
