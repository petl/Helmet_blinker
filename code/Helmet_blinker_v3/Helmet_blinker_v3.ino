#include <avr/sleep.h>
#include <avr/interrupt.h>


const int LedPin = 2;
volatile int button_interrupt = 0;


// the setup function runs once when you press reset or power the board
void setup() {

  //goToSleep();
  
  //CCP = 0xD8;                       // Allow change of protected register
  //CLKPSR = 0;                       // Switch to 8MHz clock

}

// the loop function runs over and over again forever
void loop() {
  DDRB |= 1<<LedPin;// set PB2 as output
  switch(button_interrupt){

    case 0://no interrupt has happened, go to sleep 
      //PORTB &= ~(1<<LedPin); // shut the LED off
      //DDRB = 0<<LedPin;// set PB2 as floating
      delay(5);
      goToSleep();
      delay(5);
      break;
  
    case 1://interrupt has happened, do something
      //PCICR = 0x00;    // Pin Change Interrupt Control Register set to DISABLE PCINT Interrupt 
      PORTB |= 1<<LedPin;//LED high
      delay(500);  
      button_interrupt = 2;
      break;
  
    case 2://LED is on, watch for next press
      while( bit_is_clear(PINB, PINB1) ){//if button is pressed
        PORTB &= ~(1<<LedPin); // shut the LED off
        button_interrupt = 3;
        delay(200);    
      }
      delay(50);   
      break;
      
    case 3://LED is in blinky mode
      while(button_interrupt == 3){
        PORTB ^= 1<<LedPin; // toggle the LED output to make it blink
        for(int i=0; i<50;i++){
          delay(10);// wait for some time
          while( bit_is_clear(PINB, PINB1) ){//if button is pressed
            PORTB &= ~(1<<LedPin); // shut the LED off
            button_interrupt = 4;
            delay(200); 
            break;   
          }                  
        }
      }
      break;
          
    case 4://LED is in fast blinky mode
      while(button_interrupt == 4){
        PORTB |= 1<<LedPin;//LED high
        for(int i=0; i<5;i++){
          delay(10);// wait for some time
          while( bit_is_clear(PINB, PINB1) ){//if button is pressed
            PORTB &= ~(1<<LedPin); // shut the LED off
            button_interrupt = 5;
            delay(200); 
            break;   
          }                  
        }  
        PORTB &= ~(1<<LedPin); // shut the LED off
        for(int i=0; i<25;i++){
          delay(10);// wait for some time
          while( bit_is_clear(PINB, PINB1) ){//if button is pressed
            PORTB &= ~(1<<LedPin); // shut the LED off
            button_interrupt = 5;
            delay(200); 
            break;   
          }                  
        }

      }
      break;
      
    case 5://LED is in fast energysaving blinky mode
      while(button_interrupt == 5){
        PORTB |= 1<<LedPin;//LED high
        for(int i=0; i<3;i++){
          delay(10);// wait for some time
          while( bit_is_clear(PINB, PINB1) ){//if button is pressed
            PORTB &= ~(1<<LedPin); // shut the LED off
            //button_interrupt = 0;
            delay(5);
            goToSleep();
            delay(5);
            //goToSleep();//go to sleep   
          }                  
        }  
        PORTB &= ~(1<<LedPin); // shut the LED off
        for(int i=0; i<100;i++){
          delay(10);// wait for some time
          while( bit_is_clear(PINB, PINB1) ){//if button is pressed
            PORTB &= ~(1<<LedPin); // shut the LED off
            //button_interrupt = 0;
            delay(5);
            goToSleep();
            delay(5);
            //goToSleep();//go to sleep 
          }                  
        }

      }
      break;
  } 
}

ISR(PCINT0_vect) {// This is called when the interrupt occurs, 
  //cli(); //no interrupts
  //PCICR = 0x00;    // Pin Change Interrupt Control Register set to DISABLE PCINT Interrupt 

  if(button_interrupt == 0){
    button_interrupt = 1; //the interrupt has happened
    //PCIFR = 0; //clear external interrupt flag register
    
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

void goToSleep(){

  DDRB |= 1<<LedPin;// set PB2 as output
  for(int i=0; i<10;i++){
    delay(30);// wait for some time
    PORTB ^= 1<<LedPin; // toggle the LED output to make it blink                 
  }
  PORTB &= ~(1<<LedPin); // shut the LED off
  delay(500);

  //Interrupt preparation for button
  cli();                  //no interrupts
  PRR  |= (1<<PRADC);     //disable ADC
  PRR  |= (1<<PRTIM0);    //disable Timer0
  ACSR |= (1<<ACD);       //disable analog comparator
  PUEB  = 0b00000010;     //enable pullup for PB1 
  PCICR = 0b00000001;     //Pin Change Interrupt Control Register set to enable PCINT Interrupt  
  PCMSK = 0b00000010;     //enable PB1 as interrupt source
  PCIFR = 0;              //clear external interrupt flag register
  button_interrupt = 0;
  sei();                  // Enable interrupts

  //PORTB |= 1<<LedPin;//LED high
  delay(5);
  
  //sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    
  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sleep_cpu();                            // sleep
}
