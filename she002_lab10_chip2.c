#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <usart_atmega1284.h>
#include <stdio.h>

//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------

//--------Shared Variables----------------------------------------------------

unsigned char temp = 0x00;
//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
//Enumeration of states.


enum LED_States { negOne, LED_USART_Receiver };

// Monitors button connected to PA0. 
// When button is pressed, shared variable "pause" is toggled.
int SMTick1(int state) {
	switch(state){
		case negOne:
		{
			state = LED_USART_Receiver;
			break;
		}
		case LED_USART_Receiver:
		{
			state = LED_USART_Receiver;
			break;
		}
		default:
		{
			state = negOne;
			break;
		}
	}
	switch(state){
		case negOne:
		{
			break;
		}
		case LED_USART_Receiver:
		{
			
			if(USART_HasReceived(0))
			{
				temp = USART_Receive(0);
			}
			
			PORTB = SetBit(PORTB, 0, (temp >= 1));
			PORTB = SetBit(PORTB, 1, (temp >= 2));
			PORTB = SetBit(PORTB, 2, (temp >= 3));
			PORTB = SetBit(PORTB, 3, (temp >= 4));
			PORTB = SetBit(PORTB, 4, (temp >= 5));
			PORTB = SetBit(PORTB, 5, (temp >= 6));
			PORTB = SetBit(PORTB, 6, (temp >= 7));
			PORTB = SetBit(PORTB, 7, (temp >= 8));
			PORTD = SetBit(PORTD, 2, (temp >= 9));
			PORTD = SetBit(PORTD, 3, (temp >= 10));
			break;
			
		}
		default:
		{
			break;
		}
	}
	return state;

}
enum celebrate_states{celebrate_init, celebrate_wait, all_on, all_off};
int SMTick2(int state)
{
	static unsigned char counter = 0;
	switch(state)
	{
		case celebrate_init:
		{
			if (temp > 0)
			{
				state = celebrate_wait;
			}
			else
			{
				state = celebrate_init;
			}
			break;
		}
		case celebrate_wait:
		{
			if(temp == 0)
			{
				counter = 10;
				state = all_on;
			}
			else
			{
				state = celebrate_wait;
			}
			break;
		}
		case all_on:
		{
			if (counter > 0)
			{
				counter --;
				state = all_on;
			}
			else
			{
				counter = 10;
				state = all_off;
			}
			break;
		}
		case all_off:
		{
			if (counter > 0)
			{
				counter --;
				state = all_off;
			}
			else
			{
				counter = 10;
				state = all_on;
			}
			break;
		}
		default:
		{
			state = celebrate_init;
			break;
		}
		
	}
	switch(state)
	{
		case celebrate_init:
		{
			break;
		}
		case celebrate_wait:
		{
			break;
		}
		case all_on:
		{
			PORTB = 0xFF;
			PORTD = 0x0C;
			break;
		}
		case all_off:
		{
			PORTB = 0x00;
			PORTD = 0x00;
			break;
		}
		default:
		{
			break;
		}
		
	}
	return state;
}

// --------END User defined FSMs-----------------------------------------------

// Implement scheduler code from PES.
int main()
{
// Set Data Direction Registers
// Buttons PORTA[0-7], set AVR PORTA to pull down logic
//DDRD = 0xFE; PORTD = 0x01;
DDRB = 0xFF; PORTB = 0x00;
DDRD = 0xFF; PORTD = 0x00;
// . . . etc

// Period for the tasks
unsigned long int SMTick1_calc = 50;
unsigned long int SMTick2_calc = 50;

//Calculating GCD
unsigned long int tmpGCD = 50;


//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc/GCD;
unsigned long int SMTick2_period = SMTick2_calc/GCD;


//Declare an array of tasks 
static task task1, task2;
task *tasks[] = { &task1, &task2};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = negOne;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &SMTick1;//Function pointer for the tick.

// Task 2
task2.state = celebrate_init;//Task initial state.
task2.period = SMTick2_period;//Task Period.
task2.elapsedTime = SMTick2_period;//Task current elapsed time.
task2.TickFct = &SMTick2;//Function pointer for the tick.

// Set the timer and turn it on
TimerSet(GCD);
TimerOn();
initUSART(0);
unsigned short i; // Scheduler for-loop iterator
while(1) {
	// Scheduler code
	for ( i = 0; i < numTasks; i++ ) {
		// Task is ready to tick
		if ( tasks[i]->elapsedTime == tasks[i]->period ) {
			// Setting next state for task
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			// Reset the elapsed time for next tick.
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += 1;
	}
	while(!TimerFlag);
	TimerFlag = 0;
}

// Error: Program should not exit!
return 0;
}
