#include <reg51.h>

sbit servo = P3^3; // Servo Signal Pin

void msdelay(unsigned int time) {
    unsigned int i, j;
    for (i = 0; i < time; i++)
        for (j = 0; j < 112; j++); // Approx 1ms delay at 11.0592 MHz
}

void servo_pulse(unsigned int time_high) {
    servo = 1;
    {
        unsigned char i;
        for(i=0; i<time_high; i++);
    }
    servo = 0;
}

void main() {
    unsigned char i;
    while (1) {
        for (i = 0; i < 50; i++) { // Repeat for 1 second (50*20ms)
            servo_pulse(200); // High-precision 1.5ms pulse
            msdelay(18); // 20ms period - 1.5ms - few us = ~18ms
        }
        msdelay(1000); // Wait 1 second at 90 deg

        for (i = 0; i < 50; i++) {
            servo_pulse(100); // High-precision 1.0ms pulse
            msdelay(19); // 20ms period - 1.0ms - few us = ~19ms
        }
        msdelay(1000); // Wait 1 second at 0 deg
    }
}
