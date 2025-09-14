#include <reg51.h>
#include <stdio.h>
#include <string.h>
#include<intrins.h>
sbit BUZZER = P2^0;  // Buzzer pin
sbit KEYPAD_ROW1 = P1^0;
sbit KEYPAD_ROW2 = P1^1;
sbit KEYPAD_ROW3 = P1^2;
sbit KEYPAD_ROW4 = P1^3;
sbit KEYPAD_COL1 = P1^4;
sbit KEYPAD_COL2 = P1^5;
sbit KEYPAD_COL3 = P1^6;
sbit MOTOR_IN1 = P3^3;  // Motor control pin 1
sbit MOTOR_IN2 = P3^1;  // Motor control pin 2
sbit MOTOR_EN  = P3^2;  // Motor enable pin
sbit FINGERPRINT_SENSOR = P0^0; // Finger Print
sbit scl=P0^6; // I2C SCL
sbit sda=P0^7; // I2C SDA

// Password & RFID Storage
unsigned char password[5] = "1234*";
unsigned char secret[6];
unsigned char stored_RFID[12] = "53000C21156B";
unsigned char input_pass[5];
unsigned char input_RFID[13];
unsigned char attempts = 3;
unsigned char slave1=0x4E;
unsigned char slave_add;

// Function Prototypes
void delay_Short(void);
void delay_Long(void);
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
void delay_ms(unsigned int);
void get_password(void);
bit check_password(void);
void ask_for_RFID(void);
void get_RFID(void);
bit check_RFID(void);
void activate_buzzer(void);
char scan_keypad(void);
void wait_for_fingerprint(void);
void ask_for_RFID(void);
void ask_for_SecretPIN(void);

void main(void) {
    init_Sys();
    
    lcd_cmd(0x80);  // Move cursor to first row
		wait_for_fingerprint();
    lcd_string("Enter Password:");

    while (1) {
        attempts = 3;  // Reset attempts
        while (attempts > 0) {
            get_password();
            if (check_password()) {
                lcd_cmd(0x01);  // Clear screen
                lcd_string("ACCESS GRANTED !");
								delay_Long();
								lcd_cmd(0x01);  // Clear screen
                lcd_string("OPENING GATE !");
								delay_Long();
								delay_Long();
							  /*MOTOR_EN = 1;   // Enable motor driver
								MOTOR_IN1 = 1;  // Set motor direction
								MOTOR_IN2 = 0;
								delay_Long();
								MOTOR_EN = 0;   
								MOTOR_IN1 = 0;  
								MOTOR_IN2 = 0;*/
								lcd_cmd(0x01);  // Clear screen
								lcd_string("Thank You !");
								delay_Long();delay_Long();
                return;
            }
            attempts--;
						
						lcd_cmd(0x01);  // Clear screen
            lcd_string("Incorrect Pass");
            lcd_cmd(0xC0);  // Move to the second line
            lcd_string("Please Try Again");
            delay_Long();   // Give user time to read
            lcd_cmd(0x01);  // Clear screen for new input
            lcd_string("Enter Password:");
        }
        ask_for_RFID();
    }
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
void init_Sys(void){
		lcd_slave(slave1);
    BUZZER = 0;  // Ensure buzzer is OFF
    
    delay_Long();
    
		lcd_cmd(0x33);
		lcd_cmd(0x32);
    lcd_cmd(0x02);	// Return home
		lcd_cmd(0x28);	// 4 bit mode
    lcd_cmd(0x0C); // Display ON, Cursor OFF
    lcd_cmd(0x06); // Entry mode: Increment cursor
    lcd_cmd(0x01); // Clear display
		init_UART();
}

//Finger print
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
void delay_Short(void) {
    unsigned int i;
    for(i=0; i<5000; i++);
}

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
// Read 5-digit password from keypad without masking
void get_password() {
    unsigned char i;
    lcd_cmd(0xC0); // Move cursor to the second line
    for (i = 0; i < 5; i++) {
        input_pass[i] = scan_keypad();
        lcd_data(input_pass[i]); // Display the entered digit
				delay_ms(10);
    }
}

// Compare entered password with stored password
bit check_password() {
    unsigned char i;
    for (i = 0; i < 5; i++) {
        if (input_pass[i] != password[i]) return 0;
    }
    return 1;
}

// Prompt for RFID if password fails
void ask_for_RFID() {
    char key;

    lcd_cmd(0x01); // Clear screen
    lcd_string("Enter RFID:");
    lcd_cmd(0xC0); // Move to second line

    // Wait for key press (using keypad)
    key = scan_keypad(); // Replace with your keypad function
    if (key == '*') {
				secret[0]=key;
        ask_for_SecretPIN();  // Go to PIN backup
        return;
    } else {
				input_RFID[0]=key;
        get_RFID();  // Reads RFID via UART
        lcd_string(input_RFID); // Show the scanned RFID
        delay_Long(); delay_Long();

        if (check_RFID()) {
            lcd_cmd(0x01);
            lcd_string("ACCESS GRANTED!");
            delay_Long();
            lcd_cmd(0x01);
            lcd_string("OPENING GATE!");
            delay_Long(); delay_Long();

            // Uncomment below if you control motor
            /*
            MOTOR_EN = 1;
            MOTOR_IN1 = 1;
            MOTOR_IN2 = 0;
            delay_Long();
            MOTOR_EN = 0;*/

            lcd_cmd(0x01);
            lcd_string("Thank You!");
            delay_Long(); delay_Long();
            return;
        } else {
            lcd_cmd(0x01);
            lcd_string("Invalid Tag");
            activate_buzzer();
        }
    }
}

// Read RFID input via UART

void get_RFID(void) {
    unsigned char i;
    lcd_cmd(0xC0); // Move cursor to second line
    for (i = 1; i < 13; i++) {
        while (!RI);  // Wait for data to be received
        input_RFID[i] = SBUF;  // Read received character
        RI = 0;  // Clear receive interrupt flag
    }
		input_RFID[13]='\0'; // Null-terminate the RFID string
		
}

// Compare entered RFID with stored RFID
bit check_RFID() {
    if(strcmp(input_RFID,"#53000C21156B\0")==0)return 1;
		else return 0;
}

// Activate buzzer on incorrect RFID
void activate_buzzer() {
    BUZZER = 1;
}

//Secret Pin
void ask_for_SecretPIN() {
    unsigned char i;

    lcd_cmd(0x01);
    lcd_string("Enter PIN:");
		lcd_cmd(0xC0);

    for (i = 1; i < 5; i++) {
        secret[i] = scan_keypad();  // Read key
        lcd_data(secret[i]);        
    }
		secret[6]='\0';

    if (strcmp(secret, "*2468\0") == 0) {
        lcd_cmd(0x01);
        lcd_string("PIN Accepted!");
        delay_Long();
        lcd_cmd(0x01);
        lcd_string("OPENING GATE!");
        delay_Long(); delay_Long();

        // Uncomment motor control if needed
        /*
        MOTOR_EN = 1;
        MOTOR_IN1 = 1;
        MOTOR_IN2 = 0;
        delay_Long();
        MOTOR_EN = 0;*/

        lcd_cmd(0x01);
        lcd_string("Thank You!");
        delay_Long(); delay_Long();
    } else {
        lcd_cmd(0x01);
        lcd_string("Invalid PIN");
        activate_buzzer();
    }
}


char scan_keypad() {
    while (1) {
        KEYPAD_ROW1 = 0; KEYPAD_ROW2 = 1; KEYPAD_ROW3 = 1; KEYPAD_ROW4 = 1;
        if (KEYPAD_COL1 == 0) { delay_Short(); while (KEYPAD_COL1 == 0); return '1'; }
        if (KEYPAD_COL2 == 0) { delay_Short(); while (KEYPAD_COL2 == 0); return '2'; }
        if (KEYPAD_COL3 == 0) { delay_Short(); while (KEYPAD_COL3 == 0); return '3'; }
        
        KEYPAD_ROW1 = 1; KEYPAD_ROW2 = 0; KEYPAD_ROW3 = 1; KEYPAD_ROW4 = 1;
        if (KEYPAD_COL1 == 0) { delay_Short(); while (KEYPAD_COL1 == 0); return '4'; }
        if (KEYPAD_COL2 == 0) { delay_Short(); while (KEYPAD_COL2 == 0); return '5'; }
        if (KEYPAD_COL3 == 0) { delay_Short(); while (KEYPAD_COL3 == 0); return '6'; }
        
        KEYPAD_ROW1 = 1; KEYPAD_ROW2 = 1; KEYPAD_ROW3 = 0; KEYPAD_ROW4 = 1;
        if (KEYPAD_COL1 == 0) { delay_Short(); while (KEYPAD_COL1 == 0); return '7'; }
        if (KEYPAD_COL2 == 0) { delay_Short(); while (KEYPAD_COL2 == 0); return '8'; }
        if (KEYPAD_COL3 == 0) { delay_Short(); while (KEYPAD_COL3 == 0); return '9'; }
        
        KEYPAD_ROW1 = 1; KEYPAD_ROW2 = 1; KEYPAD_ROW3 = 1; KEYPAD_ROW4 = 0;
        if (KEYPAD_COL1 == 0) { delay_Short(); while (KEYPAD_COL1 == 0); return '*'; }
        if (KEYPAD_COL2 == 0) { delay_Short(); while (KEYPAD_COL2 == 0); return '0'; }
        if (KEYPAD_COL3 == 0) { delay_Short(); while (KEYPAD_COL3 == 0); return '#'; }
    }
}
