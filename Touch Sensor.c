#include <reg51.h>
#include <stdio.h>
#include <string.h>
#include<intrins.h>

sbit FINGERPRINT_SENSOR = P0^0; // Finger Print
sbit scl=P0^6; // I2C SCL
sbit sda=P0^7; // I2C SDA

unsigned char slave1=0x4E;
unsigned char slave_add;

void init_Sys(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_ACK(void);
void i2c_write(unsigned char);
void i2c_DevWrite(unsigned char);
void lcd_cmd(unsigned char);
void lcd_data(unsigned char);
void lcd_string(unsigned char *);
void lcd_slave(unsigned char );
void wait_for_fingerprint(void);
void delay_Long(void);
void delay_ms(unsigned int);

void main(void){
	init_Sys(); // Initialize system and LCD
  lcd_cmd(0x01); // Clear screen
	wait_for_fingerprint();
}

void init_UART() {
    SCON = 0x50;  // UART Mode 1, 8-bit data, Enable Reception
    TMOD = 0x20; // Timer1 Mode 2, 8-bit auto-reload
    TH1 = 0xFD;   // Baud rate 9600 for 11.0592 MHz crystal
    TR1 = 1;      // Start Timer1
    TI = 1;       // Enable transmission
    ES = 1;       // Enable UART interrupt
    EA = 1;       // Enable global interrupts
}

// Initialize LCD and System
void init_Sys(void) {
    lcd_slave(slave1);  // Set slave address
    delay_ms(500);      // Delay to allow the system to stabilize
    lcd_cmd(0x33);      // Initialize LCD
    lcd_cmd(0x32);      // 4-bit mode
    lcd_cmd(0x02);      // Return home
    lcd_cmd(0x28);      // 4-bit mode
    lcd_cmd(0x0C);      // Display ON, Cursor OFF
    lcd_cmd(0x06);      // Entry mode: Increment cursor
    lcd_cmd(0x01);      // Clear display
    init_UART();        // Initialize UART for EM-18 RFID communication
}

void wait_for_fingerprint() {
    lcd_cmd(0x01);
    lcd_string("Enter Fingerprint");
    lcd_cmd(0xC0);
    lcd_string("Touch Sensor now");
		lcd_cmd(0x01);
		lcd_string("Waiting...");

    while (1) {
        if (FINGERPRINT_SENSOR == 1) {
            lcd_cmd(0x01);
            lcd_string("Fingerprint OK");
            delay_Long();
            lcd_cmd(0x01);
            return;
        }
    }
}

//I2C Interface
void i2c_start(void)
{
sda=1;_nop_();_nop_();
scl=1;_nop_();_nop_();
sda=0;_nop_();_nop_();
}

void i2c_stop(void)
{
scl=0;
sda=0;
scl=1;
sda=1;
}

void lcd_slave(unsigned char slave)
{
slave_add=slave;
}

void i2c_ACK(void)
{
scl=0;
sda=1;
scl=1;
while(sda);
}

void i2c_write(unsigned char dat)
{
unsigned char i;
for(i=0;i<8;i++)
{
scl=0;
sda=(dat&(0x80)>>i)?1:0;
scl=1;
}
}
// Send command to LCD
void lcd_cmd(unsigned char cmd)
{
unsigned char cmd_l,cmd_u;
cmd_l=(cmd<<4)&0xf0;
cmd_u=(cmd &0xf0);
i2c_start();			 //BL EN RW RS 1 1 0 0
i2c_write(slave_add);
i2c_ACK();
i2c_write(cmd_u|0x0C);
i2c_ACK();
delay_ms(1);
i2c_write(cmd_u|0x08); // 1 0 0 0
i2c_ACK();
delay_ms(10);
i2c_write(cmd_l|0x0C);	 // 1 1 0 0
i2c_ACK();
delay_ms(1);
i2c_write(cmd_l|0x08);
i2c_ACK();
delay_ms(10);
i2c_stop();
}


// Send character to LCD
void lcd_data(unsigned char dataw)// 1 1 0 1
{
unsigned char dataw_l,dataw_u;
dataw_l=(dataw<<4)&0xf0;
dataw_u=(dataw &0xf0);
i2c_start();
i2c_write(slave_add);
i2c_ACK();
i2c_write(dataw_u|0x0D);//BL EN RW RS   1 1 0 1
i2c_ACK();
delay_ms(1);
i2c_write(dataw_u|0x09);// 1 0 0 1
i2c_ACK();
delay_ms(10);
i2c_write(dataw_l|0x0D);
i2c_ACK();
delay_ms(1);
i2c_write(dataw_l|0x09);
i2c_ACK();
delay_ms(10);
i2c_stop();
}

// Send string to LCD
void lcd_string(unsigned char *p)
{
while(*p != '\0')
lcd_data(*p++);
}

// Delay Functions
void delay_Long(void) {
    unsigned int i;
    for(i=0; i<30000; i++);
}

void delay_ms(unsigned int n)
{
unsigned int m;
for(n;n>0;n--)
{
  for(m=121;m>0;m--);
    _nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}
}
