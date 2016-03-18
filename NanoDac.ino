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

PINC, PIN 0,1,2,3 -> low 4 bits (PORTC)
PINC, PIN 4,5,6,7 -> high 4 bits (PORTD)


*/

#include <Wire.h>
#include "Adafruit_MCP4725.h"

Adafruit_MCP4725 dac;

// Set this value to 9, 8, 7, 6 or 5 to adjust the resolution
#define DAC_RESOLUTION    (8)


#define pinInterrupt	2
#define pinStatus		8	// PORTB,PIN0 - Set to 0 for busy, 1 for ready


#define Busy	do { digitalWrite(8,HIGH) ; } while(0)
#define Ready	do { digitalWrite(8,LOW); } while(0)

//#include <Wire.h>

uint8_t volatile laserLevel = 255;
#define signal digitalRead(2)

// pour éviter une conversion coûteuse 
const PROGMEM uint16_t Volt[256] =
{
	0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 257, 273, 289, 305, 321, 337, 353, 369,
	385, 401, 417, 433, 449, 465, 481, 497, 514, 530, 546, 562, 578, 594, 610, 626, 642, 658, 674, 690,
	706, 722, 738, 754, 771, 787, 803, 819, 835, 851, 867, 883, 899, 915, 931, 947, 963, 979, 995, 1011,
	1028, 1044, 1060, 1076, 1092, 1108, 1124, 1140, 1156, 1172, 1188, 1204, 1220, 1236, 1252, 1268, 1285, 1301,
	1317, 1333, 1349, 1365, 1381, 1397, 1413, 1429, 1445, 1461, 1477, 1493, 1509, 1525, 1542, 1558,
	1574, 1590, 1606, 1622, 1638, 1654, 1670, 1686, 1702, 1718, 1734, 1750, 1766, 1782, 1799, 1815, 1831, 1847, 1863,
	1879, 1895, 1911, 1927, 1943, 1959, 1975, 1991, 2007, 2023, 2039, 2056, 2072, 2088, 2104, 2120,
	2136, 2152, 2168, 2184, 2200, 2216, 2232, 2248, 2264, 2280, 2296, 2313, 2329, 2345, 2361, 2377, 2393, 2409, 2425,
	2441, 2457, 2473, 2489, 2505, 2521, 2537, 2553, 2570, 2586, 2602, 2618, 2634, 2650, 2666, 2682,
	2698, 2714, 2730, 2746, 2762, 2778, 2794, 2810, 2827, 2843, 2859, 2875, 2891, 2907, 2923, 2939, 2955, 2971, 2987,
	3003, 3019, 3035, 3051, 3067, 3084, 3100, 3116, 3132, 3148, 3164, 3180, 3196, 3212, 3228, 3244,
	3260, 3276, 3292, 3308, 3324, 3341, 3357, 3373, 3389, 3405, 3421, 3437, 3453, 3469, 3485, 3501, 3517, 3533, 3549,
	3565, 3581, 3598, 3614, 3630, 3646, 3662, 3678, 3694, 3710, 3726, 3742, 3758, 3774, 3790, 3806,
	3822, 3838, 3855, 3871, 3887, 3903, 3919, 3935, 3951, 3967, 3983, 3999, 4015, 4031, 4047, 4063, 4079, 4096};


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
	// force I2C à 400kHz
#ifdef TWBR
	uint8_t twbrback = TWBR;
	TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz
#endif
	dac.begin(0x62);
}


void loop()
{
	while (signal == true) {}
	Busy;
	laserLevel = (PINC & B00001111) | (PIND & B11110000);
	// sent level to DAC (I2C)
	dac.setVoltage(pgm_read_word(&(Volt[laserLevel])), false);
	Serial.println(laserLevel);
	analogWrite(10, laserLevel);
	Ready; // on a fini, prêt pour prochaine demande
}

