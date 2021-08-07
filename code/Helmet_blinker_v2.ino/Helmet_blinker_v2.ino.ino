#include <avr/sleep.h>
#include <avr/interrupt.h>


const int LedPin = 2;
volatile int button_interrupt = 0;


// the setup function runs once when you press reset or power the board
void setup() {

  // Configure pins
  //DDRB |= 1<<LedPin;// set PB2 as output
  //PORTB = 0;      // All low


  //goToSleep();
  
  //CCP = 0xD8;                       // Allow change of protected register
  //CLKPSR = 0;                       // Switch to 8MHz clock

}

// the loop function runs over and over again forever
void loop() {
  button_interrupt = 2;
  defaultProgram();   
  goToSleep();//go to sleep  
}

ISR(PCINT0_vect) {// This is called when the interrupt occurs, 
  PCICR = 0x00;    // Pin Change Interrupt Control Register set to DISABLE PCINT Interrupt 

  if(button_interrupt == 0){
    button_interrupt = 1; //the interrupt has happened
    
    DDRB |= 1<<LedPin;// set PB2 as output
    
    while( bit_is_clear(PINB, PINB1) ){//while button is pressed
      PORTB ^= 1<<LedPin; // toggle the LED output to make it blink
      delay(200);
    }  
  }
}
  
void delay (int millis) {
  for (volatile unsigned int i = 34*millis; i>0; i--);
}

void defaultProgram(){
  DDRB |= 1<<LedPin;// set PB2 as output

  //interrupt has happened, do something
  PORTB |= 1<<LedPin;//LED high
  delay(500);  
  
  while(button_interrupt == 2){//LED is on, watch for next press
    PORTB ^= 1<<LedPin; // toggle the LED output to make it blink
    if( bit_is_clear(PINB, PINB1) ){//if button is pressed
      PORTB &= ~(1<<LedPin); // shut the LED off
      button_interrupt = 3;
      delay(200);    
    }
    delay(40);   
  }
    
  while(button_interrupt == 3){//LED is in blinky mode
    PORTB ^= 1<<LedPin; // toggle the LED output to make it blink
    for(int i=0; i<50;i++){
      delay(10);// wait for some time
      if( bit_is_clear(PINB, PINB1) ){//if button is pressed
        PORTB &= ~(1<<LedPin); // shut the LED off
        button_interrupt = 4;
        delay(200);  
      }                  
    }
  }
        
 
  while(button_interrupt == 4){//LED is in fast blinky mode
    PORTB |= 1<<LedPin;//LED high
    for(int i=0; i<5;i++){
      delay(10);// wait for some time
      if( bit_is_clear(PINB, PINB1) ){//if button is pressed
        PORTB &= ~(1<<LedPin); // shut the LED off
        button_interrupt = 5;
        delay(200);   
      }                  
    }  
    PORTB &= ~(1<<LedPin); // shut the LED off
    for(int i=0; i<25;i++){
      delay(10);// wait for some time
      if( bit_is_clear(PINB, PINB1) ){//if button is pressed
        PORTB &= ~(1<<LedPin); // shut the LED off
        button_interrupt = 5;
        delay(200); 
      }                  
    }

  }
    
  while(button_interrupt == 5){//LED is in fast energysaving blinky mode
    PORTB |= 1<<LedPin;//LED high
    for(int i=0; i<3;i++){
      delay(10);// wait for some time
      if( bit_is_clear(PINB, PINB1) ){//if button is pressed
        PORTB &= ~(1<<LedPin); // shut the LED off
        button_interrupt = 6;
        //goToSleep();//go to sleep  
      }                  
    }  
    PORTB &= ~(1<<LedPin); // shut the LED off
    for(int i=0; i<100;i++){
      delay(10);// wait for some time
      if( bit_is_clear(PINB, PINB1) ){//if button is pressed
        PORTB &= ~(1<<LedPin); // shut the LED off
        button_interrupt = 6;
        //goToSleep();//go to sleep  
      }                  
    }
  }
}

void goToSleep(){
  
  DDRB |= 1<<LedPin;// set PB2 as output
  for(int i=0; i<100; i++){
    PORTB ^= 1<<LedPin; // toggle the LED output to make it blink
    delay(20);// wait for some time                 
  }
  PORTB &= ~(1<<LedPin); // shut the LED off
  DDRB &= ~(1<<LedPin); //set pin as floating




  
  //Interrupt preparation for button
  cli();                  //no interrupts
  PRR  |= (1<<PRADC);     //disable ADC
  PRR  |= (1<<PRTIM0);    //disable Timer0
  ACSR |= (1<<ACD);       //disable analog comparator
  PUEB  = 0b00000010;     //enable pullup for PB1 
  //EIMSK = 0b00000001;     //External Interrupt Request 0 Enable 
  PCICR = 0b00000001;     //Pin Change Interrupt Control Register set to enable PCINT Interrupt  
  PCMSK = 0b00000010;     //enable PB1 as interrupt source
  //EICRA = 0b00000010;     //enable INT0 falling edge interrupt  
  //EICRA = 0b00000000;     //enable INT0 low level interrupt  
  PCIFR = 0; //clear external interrupt flag register
  sei();                  // Enable interrupts
/*
          for(int i=0; i<10; i++){
      PORTB ^= 1<<LedPin; // toggle the LED output to make it blink
      delay(100);// wait for some time                 
      }*/

  button_interrupt = 0; //clear interrupt


  //PORTB &= ~(1<<LedPin); // shut the LED off
  //DDRB &= ~(1<<LedPin); //set pin as floating
  //sleep mode
  SMCR = 0b00000011;  //Sleep mode is power down AND sleep is enabled
  //set_sleep_mode(SLEEP_MODE_PWR_DOWN);  //Sleep mode is power down  
  //sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sleep_cpu();                            // sleep
}
