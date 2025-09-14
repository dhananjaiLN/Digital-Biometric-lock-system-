#include <reg51.h>
#include <stdio.h>
#include <string.h>
#include <intrins.h>

sbit scl = P0^6; // I2C SCL
sbit sda = P0^7; // I2C SDA

unsigned char input_RFID[12]; // Array to store the RFID data
unsigned char stored_RFID[12] = "53000C21156B";
unsigned char slave1 = 0x4E;  // LCD slave address
unsigned char slave_add;      // To store slave address

void init_Sys(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_ACK(void);
void i2c_write(unsigned char);
void i2c_DevWrite(unsigned char);
void lcd_cmd(unsigned char);
void lcd_data(unsigned char);
void lcd_string(unsigned char *);
void lcd_slave(unsigned char);
void delay_ms(unsigned int);
void ask_for_RFID(void);
void get_RFID(void);
void init_UART(void);
void read_RFID(void); // Function to read the RFID from the UART

// Main function
void main(void) {
    init_Sys(); // Initialize system and LCD
    lcd_cmd(0x01); // Clear screen
    lcd_string("Enter RFID: ");
    lcd_cmd(0xC0); // Move cursor to second line

    while (1) {
        read_RFID(); // Read the RFID input
        lcd_cmd(0x01); // Clear screen
        lcd_string("RFID: "); // Display "RFID:" text
				lcd_cmd(0xC0);
        lcd_string(input_RFID); // Display the RFID input
        delay_ms(1000); // Delay to allow viewing the RFID
    }
}

// UART initialization function
void init_UART() {
    SCON = 0x50;  // UART Mode 1, 8-bit data, Enable Reception
    TMOD = 0x20; // Timer1 Mode 2, 8-bit auto-reload
    TH1 = 0xFD;   // Baud rate 9600 for 11.0592 MHz crystal
    TR1 = 1;      // Start Timer1
    TI = 1;       // Enable transmission
    ES = 1;       // Enable UART interrupt
    EA = 1;       // Enable global interrupts
}

// I2C initialization function
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

// Function to read the RFID data
void read_RFID(void) {
    unsigned char i;
    for (i = 0; i < 12; i++) {
        while (!RI);  // Wait for data to be received
        input_RFID[i] = SBUF; // Read the received byte from UART buffer
        RI = 0; // Clear the receive interrupt flag
    }
    input_RFID[12] = '\0'; // Null-terminate the RFID string
		if(strcmp(input_RFID,"53000C21156B\0")==0){
			lcd_cmd(0x01); // Clear screen
			lcd_string("   Successful  ");
			delay_ms(100);
		}
		else{
			lcd_cmd(0x01); // Clear screen
			lcd_string(" Not Successful  ");
			delay_ms(100);
		}
}

// I2C functions (already in your code)
void i2c_start(void) {
    sda = 1; _nop_(); _nop_();
    scl = 1; _nop_(); _nop_();
    sda = 0; _nop_(); _nop_();
}

void i2c_stop(void) {
    scl = 0;
    sda = 0;
    scl = 1;
    sda = 1;
}

void lcd_slave(unsigned char slave) {
    slave_add = slave;
}

void i2c_ACK(void) {
    scl = 0;
    sda = 1;
    scl = 1;
    while (sda);
}

void i2c_write(unsigned char dat) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        scl = 0;
        sda = (dat & (0x80 >> i)) ? 1 : 0;
        scl = 1;
    }
}

void lcd_cmd(unsigned char cmd) {
    unsigned char cmd_l, cmd_u;
    cmd_l = (cmd << 4) & 0xf0;
    cmd_u = (cmd & 0xf0);
    i2c_start();
    i2c_write(slave_add);
    i2c_ACK();
    i2c_write(cmd_u | 0x0C);
    i2c_ACK();
    delay_ms(1);
    i2c_write(cmd_u | 0x08);
    i2c_ACK();
    delay_ms(10);
    i2c_write(cmd_l | 0x0C);
    i2c_ACK();
    delay_ms(1);
    i2c_write(cmd_l | 0x08);
    i2c_ACK();
    delay_ms(10);
    i2c_stop();
}

void lcd_data(unsigned char dataw) {
    unsigned char dataw_l, dataw_u;
    dataw_l = (dataw << 4) & 0xf0;
    dataw_u = (dataw & 0xf0);
    i2c_start();
    i2c_write(slave_add);
    i2c_ACK();
    i2c_write(dataw_u | 0x0D);
    i2c_ACK();
    delay_ms(1);
    i2c_write(dataw_u | 0x09);
    i2c_ACK();
    delay_ms(10);
    i2c_write(dataw_l | 0x0D);
    i2c_ACK();
    delay_ms(1);
    i2c_write(dataw_l | 0x09);
    i2c_ACK();
    delay_ms(10);
    i2c_stop();
}

void lcd_string(unsigned char *p) {
    while (*p != '\0')
        lcd_data(*p++);
}

void delay_ms(unsigned int n) {
    unsigned int m;
    for (n; n > 0; n--) {
        for (m = 121; m > 0; m--);
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }
}
