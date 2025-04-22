#include <xc.h>

// Configuration settings
#pragma config FOSC = HS    // High-Speed Oscillator mode
#pragma config WDTE = OFF   // Watchdog Timer disabled
#pragma config PWRTE = ON   // Power-up Timer enabled
#pragma config CP = OFF     // Code protection disabled
#pragma config BOREN = OFF  // Brown-out Reset disabled
#pragma config LVP = OFF    // Low-Voltage Programming disabled
#pragma config CPD = OFF    // Data EEPROM code protection disabled
#pragma config WRT = OFF    // Write protection disabled
#pragma config DEBUG = OFF  // Debug mode disabled

//__CONFIG( FOSC_HS & WDTE_OFF & PWRTE_OFF & CP_OFF & BOREN_ON & LVP_OFF & CPD_OFF & WRT_OFF & DEBUG_OFF);

#define IN1 RD0
#define IN2 RD1
#define IN3 RD2
#define IN4 RD3

void init_UART();
void tx(unsigned char);
unsigned char rx();

void main()
{
    TRISC6 = TRISC7 = 1;   // Configure TX and RX pins
    TRISD = 0x00;             // Port D as Output
    IN1 = IN2 = IN3 = IN4 = 0;  // Initialize motor control pins to 0
    
    unsigned char get = 0; // Variable to store command
    init_UART();

    while(1) {
        if (RCIF) {               // Check if UART received data
            get = rx();           // Get UART command
            tx(get);              // Echo command back for debugging
            
            if (get == 'F') {
                /* Forward */
                IN1 = 1;
                IN3 = 1;
                IN2 = 0; 
                IN4 = 0;
            } else if (get == 'R') {
                /* Reverse */
                IN2 = 1;
                IN4 = 1;
                IN1 = 0;
                IN3 = 0;
            } else if (get == 'W') {
                /* Right */
                IN1 = 1;
                IN2 = 0;
                IN3 = 0;
                IN4 = 0;
            } else if (get == 'L') {
                /* Left */
                IN3 = 1;
                IN2 = 0;
                IN1 = 0;
                IN4 = 0;
            } else if (get == 'S') {
                /* Off */
                IN1 = IN2 = IN3 = IN4 = 0;
            }
        }
        // If no command is received, maintain current state
    }   
}   

void init_UART()
{
    TXSTA = 0x24; // 0b00100100, BRGH=1, TXEN=1, Asynchronous Mode, 8-bit mode
    RCSTA = 0b10010000; // Serial Port enabled, 8-bit reception
    SPBRG = 129; // 9600 baudrate for 20Mhz
    TXIF = RCIF = 0;
}
    
void tx(unsigned char a)
{
    TXREG = a;
    while(!TXIF);
    TXIF = 0;
}

unsigned char rx()
{
    while(!RCIF);   // Wait for UART data
    RCIF = 0;
    return RCREG;   // Return received byte
}
