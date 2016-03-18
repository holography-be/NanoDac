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
	0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256, 273, 289, 305, 321, 337, 353,
	369, 385, 401, 417, 433, 449, 465, 481, 497, 513, 529, 546, 562, 578, 594, 610, 626, 642, 658,
	674, 690, 706, 722, 738, 754, 770, 786, 802, 819, 835, 851, 867, 883, 899, 915, 931, 947, 963, 979, 995,
	1011, 1027, 1043, 1059, 1075, 1092, 1108, 1124, 1140, 1156, 1172, 1188, 1204, 1220, 1236,
	1252, 1268, 1284, 1300, 1316, 1332, 1348, 1365, 1381, 1397, 1413, 1429, 1445, 1461, 1477, 1493, 1509,
	1525, 1541, 1557, 1573, 1589, 1605, 1621, 1638, 1654, 1670, 1686, 1702, 1718, 1734, 1750,
	1766, 1782, 1798, 1814, 1830, 1846, 1862, 1878, 1894, 1911, 1927, 1943, 1959, 1975, 1991, 2007, 2023,
	2039, 2055, 2071, 2087, 2103, 2119, 2135, 2151, 2167, 2184, 2200, 2216, 2232, 2248, 2264,
	2280, 2296, 2312, 2328, 2344, 2360, 2376, 2392, 2408, 2424, 2440, 2457, 2473, 2489, 2505, 2521, 2537,
	2553, 2569, 2585, 2601, 2617, 2633, 2649, 2665, 2681, 2697, 2713, 2730, 2746, 2762, 2778,
	2794, 2810, 2826, 2842, 2858, 2874, 2890, 2906, 2922, 2938, 2954, 2970, 2986, 3003, 3019, 3035, 3051,
	3067, 3083, 3099, 3115, 3131, 3147, 3163, 3179, 3195, 3211, 3227, 3243, 3259, 3276, 3292,
	3308, 3324, 3340, 3356, 3372, 3388, 3404, 3420, 3436, 3452, 3468, 3484, 3500, 3516, 3532, 3549, 3565,
	3581, 3597, 3613, 3629, 3645, 3661, 3677, 3693, 3709, 3725, 3741, 3757, 3773, 3789, 3805,
	3822, 3838, 3854, 3870, 3886, 3902, 3918, 3934, 3950, 3966, 3982, 3998, 4014, 4030, 4046, 4062, 4078, 4095};


void setup()
{
	DDRD &= (B00001111);	// set pin 4,5,6,7 as input
	DDRC &= (B11110000);	// set pin A0,A1,A2,A3 as input
	DDRB |= B00000001;	// set pin 8 as output
	pinMode(2, INPUT);
	Serial.begin(115200);
	Serial.println("OK");
	// on signale démarrage
	for (uint8_t i = 0; i < 5; i++) {
		pinMode(9, OUTPUT);
		digitalWrite(9, HIGH);
		delay(500);
		digitalWrite(9, LOW);
		delay(500);
	}
	Busy;	// on est occupé
	Ready;
	pinMode(13, OUTPUT);
	// force I2C à 400kHz
#ifdef TWBR
	uint8_t twbrback = TWBR;
	TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz
#endif
	// For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
	// For MCP4725A0 the address is 0x60 or 0x61
	// For MCP4725A2 the address is 0x64 or 0x65
	dac.begin(0x60);
}


void loop()
{
	while (signal == true) {}
	Busy;
	laserLevel = (PINC & B00001111) | (PIND & B11110000);
	// sent level to DAC (I2C)
	dac.setVoltage(pgm_read_word(&(Volt[laserLevel])), false);
	Serial.println(laserLevel);
	Ready; // on a fini, prêt pour prochaine demande
}

