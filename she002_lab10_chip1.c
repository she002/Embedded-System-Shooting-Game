/*
 * she002_lab10_part1.c
 *
 * Created: 5/23/2014 10:55:18 AM
 *  Author: John
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <usart_atmega1284.h>
#include "io.c"
#include <stdio.h>


// Returns '\0' if no key pressed, else returns char '1', '2', ... '9', 'A', ...
// If multiple keys pressed, returns leftmost-topmost one
// Keypad must be connected to port C
/* Keypad arrangement
        PC4 PC5 PC6 PC7
   col  1   2   3   4
row
PC0 1   1 | 2 | 3 | A
PC1 2   4 | 5 | 6 | B
PC2 3   7 | 8 | 9 | C
PC3 4   * | 0 | # | D
*/



unsigned char GetKeypadKey() {

	PORTC = 0xEF; // Enable col 4 with 0, disable others with 1??s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('1'); }
	if (GetBit(PINC,1)==0) { return('4'); }
	if (GetBit(PINC,2)==0) { return('7'); }
	if (GetBit(PINC,3)==0) { return('*'); }

	// Check keys in col 2
	PORTC = 0xDF; // Enable col 5 with 0, disable others with 1??s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('2'); }
	if (GetBit(PINC,1)==0) { return('5'); }
	if (GetBit(PINC,2)==0) { return('8'); }
	if (GetBit(PINC,3)==0) { return('0'); }
	// ... *****FINISH*****

	// Check keys in col 3
	PORTC = 0xBF; // Enable col 6 with 0, disable others with 1??s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('3'); }
	if (GetBit(PINC,1)==0) { return('6'); }
	if (GetBit(PINC,2)==0) { return('9'); }
	if (GetBit(PINC,3)==0) { return('#'); }
	// ... *****FINISH*****

	// Check keys in col 4	
	PORTC = 0x7F;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('A'); }
	if (GetBit(PINC,1)==0) { return('B'); }
	if (GetBit(PINC,2)==0) { return('C'); }
	if (GetBit(PINC,3)==0) { return('D'); }
	// ... *****FINISH*****

	return('\0'); // default value

}




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
	unsigned char if_game_over = 0;
	
	unsigned char Jet[8] = {0x08, 0x0C, 0x17, 0x0C, 0x0C, 0x17, 0x0C, 0x08};
	unsigned char Jet_pos = 1;
	
	unsigned char bullet_pos = 0;
	unsigned char player_shot_bullet = 0;
	
	unsigned char asteroid_1[8] = {0x00, 0x00, 0x0E, 0x1F, 0x1F, 0x0E, 0x00, 0x00};
	unsigned char asteroid_1_pos = 0;
	unsigned char if_asteroid_1_was_hit = 0;
	unsigned char asteroid_2_pos = 0;
	unsigned char asteroid_2_delay_time = 0;
	unsigned char if_asteroid_2_was_hit = 0;
	
	unsigned char boss_part1[8] = {0x03, 0x06, 0x06, 0x1E, 0x06, 0x07, 0x0F, 0x1E};
	unsigned char boss_part2[8] = {0x1E, 0x0F, 0x07, 0x06, 0x1E, 0x06, 0x06, 0x03};
	unsigned char boss_delay_time = 0;
	unsigned char boss_pos = 0;
	unsigned char boss_HP = 0;
	unsigned char if_boss_died = 0;
	
	unsigned char energy_weapon_1[8] = {0x11, 0x00, 0x0A, 0x04, 0x04, 0x0A, 0x00, 0x11};
	unsigned char energy_weapon_2[8] = {0x00, 0x00, 0x04, 0x0E, 0x0E, 0x04, 0x00, 0x00};
	unsigned char energy_weapon_3[8] = {0x00, 0x00, 0x18, 0x1E, 0x1E, 0x18, 0x00, 0x00};
	unsigned char energy_weapon_4[8] = {0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00};
//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
//Enumeration of states
enum keypads{init, s0, game_over, victory};

int tickfct_keypad(int state)
{
		switch (state)
		{
			case init:
				LCD_Build(0, Jet);
				LCD_Cursor(Jet_pos);
				LCD_WriteData(0x00);
				state = s0;
				break;
			case s0:
			{
				if (Jet_pos == asteroid_1_pos || Jet_pos == asteroid_2_pos || Jet_pos == asteroid_1_pos-1 || Jet_pos == asteroid_2_pos-1 ||  Jet_pos == boss_pos ||  Jet_pos == boss_pos + 16)
				{
					if_game_over = 1;
					state = game_over;
				}
				else if (if_game_over)
				{
					state = game_over;
				}
				else if (if_boss_died)
				{
					state = victory;
				}
				else
				{
					state = s0;
				}
				break;
			}
			case game_over:
			{
				state = game_over;
				break;
			}
			case victory:
			{
				state = victory;
				break;
			}
			default:
				state = init;
				break;
		}
		
		switch(state)
		{
			case init:
				break;
			case s0:
			{
				unsigned char x = GetKeypadKey();
				//LCD_Cursor(Jet_pos);
				//LCD_WriteData(0x00);
				switch (x) {
					case '\0': break; // All 5 LEDs on
					case '1': break; // hex equivalent
					case '2': break;
					case '3': break;
					case '4': break;
					case '5': 
					{
						LCD_Cursor(Jet_pos);
						LCD_WriteData(' ');
						if(Jet_pos > 16)
						{
							Jet_pos = Jet_pos - 16;
						}
						LCD_Cursor(Jet_pos);
						LCD_WriteData(0x00);
						break;
					}
					case '6': break;
					case '7': 
					{
						LCD_Cursor(Jet_pos);
						LCD_WriteData(' ');
						if(Jet_pos>1 && Jet_pos<=16)
						{
							Jet_pos--;
						}
						else if (Jet_pos>17 && Jet_pos<=32)
						{
							Jet_pos--;
						}
						LCD_Cursor(Jet_pos);
						LCD_WriteData(0x00);
						break;
					}
					case '8': 
					{
						LCD_Cursor(Jet_pos);
						LCD_WriteData(' ');
						if(Jet_pos < 17)
						{
							Jet_pos = Jet_pos + 16;
						}
						LCD_Cursor(Jet_pos);
						LCD_WriteData(0x00);
						break;
					}
					case '9': 
					{
						LCD_Cursor(Jet_pos);
						LCD_WriteData(' ');
						if(Jet_pos < 16)
						{
							Jet_pos++;
						}
						else if (Jet_pos >= 17 && Jet_pos < 32)
						{
							Jet_pos++;
						}
						LCD_Cursor(Jet_pos);
						LCD_WriteData(0x00);
						break;
					}
					case 'A': break;
					case 'B': break;
					case 'C': 
					{
						if (bullet_pos == 0)
						{
							player_shot_bullet = 1;
						}
						break;
					}
					case 'D': break;
					case '*': break;
					case '0': break;
					case '#': break;
					default: break; // Should never occur. Middle LED off.
				}
				break;
			}
			case game_over:
			{
				LCD_ClearScreen();
				LCD_DisplayString(5, "YOU DIED..");
				break;
			}
			case victory:
			{
				LCD_ClearScreen();
				LCD_DisplayString(5, "YOU WIN!!");
			}
			default:
				break;
					

		}
		LCD_Cursor(0);
		return state;
}

enum asteroid_1_states{asteroid_1_negOne, asteroid_1_init, asteroid_1_moving, asteroid_1_if_hit, asteroid_1_second_moving, asteroid_1_done, asteroid_1_game_over};

int tickfct_asteroid_1(int state)
{
	static unsigned char asteroid_1_tmp = 0;
	switch (state)
	{
		case asteroid_1_negOne:
		{
			//asteroid_1_pos = 16;
			state = asteroid_1_init;
			break;
		}
		case asteroid_1_init:
		{
			state = asteroid_1_moving;
			break;
		}
		case asteroid_1_moving:
		{
			if (if_game_over)
			{
				state = asteroid_1_game_over;
			}
			else if(asteroid_1_pos <= 0)
			{
				asteroid_1_pos = 16;
				state = asteroid_1_second_moving;
			}
			else if(if_asteroid_1_was_hit)
			{
				asteroid_1_tmp = asteroid_1_pos;
				state = asteroid_1_if_hit;
				LCD_Cursor(asteroid_1_pos);
				LCD_WriteData(' ');
				asteroid_1_pos = 0;
				
			}
			else
			{
				state = asteroid_1_moving;
			}
			
			break;
		}
		case asteroid_1_if_hit:
		{
			if (if_game_over)
			{
				state = asteroid_1_game_over;
			}
			else if (asteroid_1_tmp <= 0)
			{
				if_asteroid_1_was_hit = 0;
				asteroid_1_pos = 16;
				state = asteroid_1_second_moving;
			}
			else
			{
				state = asteroid_1_if_hit;
			}
			break;
		}
		case asteroid_1_second_moving:
		{
			if (if_game_over)
			{
				state = asteroid_1_game_over;
			}
			else if(asteroid_1_pos <= 0)
			{
				LCD_Cursor(0);
				state = asteroid_1_done;
			}
			else if(if_asteroid_1_was_hit)
			{
				LCD_Cursor(asteroid_1_pos);
				LCD_WriteData(' ');
				state = asteroid_1_done;
			}
			else
			{
				state = asteroid_1_second_moving;
			}
			break;
			
		}
		case asteroid_1_done:
		{
			state = asteroid_1_done;
			break;
		}
		case asteroid_1_game_over:
		{
			state = asteroid_1_game_over;
			break;
		}
		default:
		{
			state = asteroid_1_negOne;
			break;
		}
	}
	switch(state)
	{
		case asteroid_1_negOne:
		{
			break;
		}
		case asteroid_1_init:
		{
			LCD_Build(1, asteroid_1);
			asteroid_1_pos = 16;
			LCD_Cursor(asteroid_1_pos);
			LCD_WriteData(0x01);
			//LCD_Cursor(0);
			break;
		}
		case asteroid_1_moving:
		{
			LCD_Cursor(asteroid_1_pos);
			LCD_WriteData(' ');
			if(asteroid_1_pos > 0)
			{
				asteroid_1_pos--;
			}
			LCD_Cursor(asteroid_1_pos);
			LCD_WriteData(0x01);
			//LCD_Cursor(0);
			break;
			
		}
		case asteroid_1_if_hit:
		{
			asteroid_1_tmp--;
			break;
		}
		case asteroid_1_second_moving:
		{
			LCD_Cursor(asteroid_1_pos);
			LCD_WriteData(' ');
			if(asteroid_1_pos > 0)
			{
				asteroid_1_pos--;
			}
			LCD_Cursor(asteroid_1_pos);
			LCD_WriteData(0x01);
			//LCD_Cursor(0);
			break;
		}
		case asteroid_1_done:
		{
			asteroid_1_pos = 0xFF;
			break;
		}
		case asteroid_1_game_over:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	LCD_Cursor(0);
	return state;
}

enum asteroid_2_states{asteroid_2_negOne, asteroid_2_init, asteroid_2_moving,asteroid_2_if_hit, asteroid_2_second_moving, asteroid_2_done, asteroid_2_game_over};

int tickfct_asteroid_2(int state)
{
	static unsigned char asteroid_2_tmp = 0;
	switch (state)
	{
		case asteroid_2_negOne:
		{
			if (asteroid_2_delay_time < 10)
			{
				state = asteroid_2_negOne;
				asteroid_2_delay_time++;
			}
			else
			{
				state = asteroid_2_init;
			}
			break;
		}
		case asteroid_2_init:
		{
			
			state = asteroid_2_moving;
			break;
		}
		case asteroid_2_moving:
		{
			if (if_game_over)
			{
				state = asteroid_2_game_over;
			}
			else if(asteroid_2_pos <= 17)
			{
				LCD_Cursor(asteroid_2_pos);
				LCD_WriteData(' ');
				state = asteroid_2_second_moving;
				asteroid_2_pos = 33;
			}
			else if(if_asteroid_2_was_hit)
			{
				asteroid_2_tmp = asteroid_2_pos;
				state = asteroid_2_if_hit;
				LCD_Cursor(asteroid_2_pos);
				LCD_WriteData(' ');
				asteroid_2_pos = 0;
				
			}
			else
			{
				state = asteroid_2_moving;
			}
			
			break;
		}
		case asteroid_2_if_hit:
		{
			if(if_game_over)
			{
				state = asteroid_2_game_over;
			}
			else if (asteroid_2_tmp <= 16)
			{
				if_asteroid_2_was_hit = 0;
				asteroid_2_pos = 32;
				state = asteroid_2_second_moving;
			}
			else
			{
				state = asteroid_2_if_hit;
			}
			break;
			
		}
		case asteroid_2_second_moving:
		{
			if (if_game_over)
			{
				state = asteroid_2_game_over;
			}
			else if(asteroid_2_pos <= 17)
			{
				LCD_Cursor(asteroid_2_pos);
				LCD_WriteData(' ');
				asteroid_2_pos = 0;
				state = asteroid_2_done;
			}
			else if(if_asteroid_2_was_hit)
			{
				LCD_Cursor(asteroid_2_pos);
				LCD_WriteData(' ');
				asteroid_2_pos = 0;
				state = asteroid_2_done;
			}
			else
			{
				state = asteroid_2_second_moving;
			}
			break;
			
		}
		case asteroid_2_done:
		{
			state = asteroid_2_done;
			break;
		}
		case asteroid_2_game_over:
		{
			state = asteroid_2_game_over;
			break;
		}
		default:
		{
			state = asteroid_2_negOne;
			break;
		}
	}
	switch(state)
	{
		case asteroid_2_negOne:
		{
			break;
		}
		case asteroid_2_init:
		{
			asteroid_2_pos = 32;
			//LCD_Build(1, asteroid_1);
			LCD_Cursor(asteroid_2_pos);
			LCD_WriteData(0x01);
			//LCD_Cursor(0);
			break;
		}
		case asteroid_2_moving:
		{
			LCD_Cursor(asteroid_2_pos);
			LCD_WriteData(' ');
			if(asteroid_2_pos > 17)
			{
				asteroid_2_pos--;
			}
			LCD_Cursor(asteroid_2_pos);
			LCD_WriteData(0x01);
			//LCD_Cursor(0);
			break;
			
		}
		case asteroid_2_if_hit:
		{
			asteroid_2_tmp--;
			break;
		
		}
		case asteroid_2_second_moving:
		{
			LCD_Cursor(asteroid_2_pos);
			LCD_WriteData(' ');
			if(asteroid_2_pos > 17)
			{
				asteroid_2_pos--;
			}
			LCD_Cursor(asteroid_2_pos);
			LCD_WriteData(0x01);
			//LCD_Cursor(0);
			break;
		}
		case asteroid_2_done:
		{
			//LCD_Cursor(0);
			asteroid_2_pos = 0xFF;
			break;
		}
		case asteroid_2_game_over:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	LCD_Cursor(0);
	return state;
}

enum bullet_states{bullet_negOne, bullet_init, bullet_moving, bullet_gone, bullet_game_over};
	
int tickfct_bullet(int state)
{
	switch (state)
	{
		case bullet_negOne:
		{
			state = bullet_init;
			break;
		}
		case bullet_init:
		{
			if(if_game_over)
			{
				state = bullet_game_over;
			}
			else if(Jet_pos == 16 || Jet_pos == 32)
			{
				state = bullet_init;
				player_shot_bullet = 0;
			}
			else if (!player_shot_bullet)
			{
				state = bullet_init;
			}
			else
			{
				bullet_pos = Jet_pos;
				state = bullet_moving;
			}
			break;
		}
		case bullet_moving:
		{
			LCD_Cursor(bullet_pos);
			LCD_WriteData(' ');
			if(if_game_over)
			{
				state = bullet_game_over;
			}
			else if(bullet_pos == 16 || bullet_pos == 32)
			{
				state = bullet_gone;
				if (boss_pos)
				{
					boss_HP--;
				}
			}
			else if (bullet_pos == asteroid_1_pos || bullet_pos == asteroid_1_pos - 1)
			{
				if_asteroid_1_was_hit = 1;
				state = bullet_gone;
			}
			else if(bullet_pos == asteroid_2_pos || bullet_pos == asteroid_2_pos - 1)
			{
				if_asteroid_2_was_hit = 1;
				state = bullet_gone;
			}
			else
			{
				state = bullet_moving;
			}
			break;
		}
		case bullet_gone:
		{
			player_shot_bullet = 0;
			bullet_pos = 0;
			state = bullet_init;
			break;
		}
		case bullet_game_over:
		{
			state = bullet_game_over;
			break;
		}
		default:
		{
			state = bullet_negOne;
			break;
		}
	}
	switch(state)
	{
		case bullet_negOne:
		{
			break;
		}
		case bullet_init:
		{
			break;
		}
		case bullet_moving:
		{
			bullet_pos++;
			LCD_Cursor(bullet_pos);
			LCD_WriteData(0x3D);
			//LCD_Cursor(0);	
			break;
		}
		case bullet_gone:
		{
			
			break;
		}
		default:
		{
			break;
		}
		
	}
	LCD_Cursor(0);
	return state;
}

enum Boss_states{Boss_negOne, Boss_init, Boss_alive, Boss_died, Boss_game_over};
	
int tickfct_Boss(int state)
{
	switch(state)
	{
		case Boss_negOne:
		{
			if(if_game_over)
			{
				state = Boss_game_over;
			}
			else if(asteroid_1_pos == 0xFF && asteroid_2_pos == 0xFF)
			{
				if (boss_delay_time < 10)
				{
					state = Boss_negOne;
					boss_delay_time++;
				}
				else
				{
					boss_pos = 16;
					boss_HP = 10;
					state = Boss_init;
				}
			}
			else
			{
				state = Boss_negOne;
			}
			break;
		}
		case Boss_init:
		{
			if(if_game_over)
			{
				state = Boss_game_over;
			}
			else
			{
				state = Boss_alive;
			}
			break;
		}
		case Boss_alive:
		{
			if(if_game_over)
			{
				state = Boss_game_over;
			}
			else if (boss_HP <= 0)
			{
				if_boss_died = 1;
				LCD_Cursor(boss_pos);
				LCD_WriteData(' ');
				LCD_Cursor(boss_pos + 16);
				LCD_WriteData(' ');
				state = Boss_died;
			}
			else
			{
				state = Boss_alive;
			}
			break;
		}
		case Boss_died:
		{
			state = Boss_died;
			break;
		}
		case Boss_game_over:
		{
			state = Boss_game_over;
			break;
		}
		default:
		{
			state = Boss_negOne;
			break;
		}
		
	}
	switch(state)
	{
		case Boss_negOne: break;
		case Boss_init:
		{
			LCD_Build(2, boss_part1);
			LCD_Build(3, boss_part2);
			LCD_Cursor(boss_pos);
			LCD_WriteData(0x02);
			LCD_Cursor(boss_pos + 16);
			LCD_WriteData(0x03);
		}
		case Boss_alive:
		{
			LCD_Cursor(boss_pos);
			LCD_WriteData(0x02);
			LCD_Cursor(boss_pos + 16);
			LCD_WriteData(0x03);
			break;
		}
		case Boss_died:
		{
			boss_pos = 0;
			break;
		}
		case Boss_game_over:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	if(USART_IsSendReady(0))
	{
		USART_Send(boss_HP, 0);
	}
	LCD_Cursor(0);
	return state;
}

enum weapon_system_states{weapon_negOne, weapon_init, gathering_energy_1, laser_ready_1, laser_shot_1, weapon_cooldown_1, gathering_energy_2, laser_ready_2, laser_shot_2,weapon_cooldown_2, weapon_game_over};

int tickfct_weapon_system(int state)
{
	static unsigned char weapon_timer = 0;
	switch (state)
	{
		case weapon_negOne:
		{
			if(if_game_over || if_boss_died)
			{
				state = weapon_game_over;
			}
			else if (boss_HP > 0)
			{
				state = weapon_init;
			}
			else
			{
				state = weapon_negOne;
			}
			break;
		}
		case weapon_init:
		{
			if(if_game_over || if_boss_died)
			{
				state = weapon_game_over;
			}
			else
			{
				weapon_timer = 10;
				state = weapon_cooldown_2;
			}
			break;
		}
		case weapon_cooldown_2:
		{
			if(if_game_over || if_boss_died)
			{
				state = weapon_game_over;
			}
			else if (weapon_timer > 0)
			{
				weapon_timer--;
				state = weapon_cooldown_2;
			}
			else
			{
				weapon_timer = 5;
				state = gathering_energy_1;
			}
			break;
		}
		case gathering_energy_1:
		{
			if(if_game_over || if_boss_died)
			{
				state = weapon_game_over;
			}
			else if (weapon_timer > 0)
			{
				weapon_timer--;
				state = gathering_energy_1;
			}
			else
			{
				weapon_timer = 5;
				state = laser_ready_1;
			}
			break;
		}
		case laser_ready_1:
		{
			if(if_game_over || if_boss_died)
			{
				state = weapon_game_over;
			}
			else if(weapon_timer > 0)
			{
				weapon_timer--;
				state = laser_ready_1;
			}
			else
			{
				weapon_timer = 10;
				state = laser_shot_1;
			}
			break;
		}
		case laser_shot_1:
		{
			if(if_game_over || if_boss_died)
			{
				state = weapon_game_over;
			}
			else if (Jet_pos >= 1 && Jet_pos < 16)
			{
				if_game_over = 1;
				state = weapon_game_over;
			}
			else if (weapon_timer > 0)
			{
				weapon_timer--;
				state = laser_shot_1;
			}
			else
			{
				weapon_timer = 10;
				for (int i = 1; i < 16; i++)
				{
					LCD_Cursor(i);
					LCD_WriteData(' ');
				}
				state = weapon_cooldown_1;
			}
			break;
		}
		case weapon_cooldown_1:
		{
			if(if_game_over || if_boss_died)
			{
				state = weapon_game_over;
			}
			else if (weapon_timer > 0)
			{
				weapon_timer--;
				state = weapon_cooldown_1;
			}
			else
			{
				weapon_timer = 5;
				state = gathering_energy_2;
			}
			break;
		}
		case gathering_energy_2:
		{
			if(if_game_over || if_boss_died)
			{
				state = weapon_game_over;
			}
			else if (weapon_timer > 0)
			{
				weapon_timer--;
				state = gathering_energy_2;
			}
			else
			{
				weapon_timer = 5;
				state = laser_ready_2;
			}
			break;
		}
		case laser_ready_2:
		{
			if(if_game_over || if_boss_died)
			{
				state = weapon_game_over;
			}
			else if(weapon_timer > 0)
			{
				weapon_timer--;
				state = laser_ready_2;
			}
			else
			{
				weapon_timer = 10;
				state = laser_shot_2;
			}
			break;
		}
		case laser_shot_2:
		{
			if(if_game_over || if_boss_died)
			{
				state = weapon_game_over;
			}
			else if (Jet_pos >= 17 && Jet_pos < 32)
			{
				if_game_over = 1;
				state = weapon_game_over;
			}
			else if (weapon_timer > 0)
			{
				weapon_timer--;
				state = laser_shot_2;
			}
			else
			{
				weapon_timer = 10;
				for (int i = 17; i<32; i++)
				{
					LCD_Cursor(i);
					LCD_WriteData(' ');
				}
				state = weapon_cooldown_2;
			}
			break;
		}
		case weapon_game_over:
		{
			state = weapon_game_over;
			break;
		}
		default:
		{
			state = weapon_negOne;
			break;
		}
		
		
	}
	switch (state)
	{
		case weapon_negOne:
		{
			break;
		}
		case weapon_init:
		{
			LCD_Build(4, energy_weapon_1);
			LCD_Build(5, energy_weapon_2);
			LCD_Build(6, energy_weapon_3);
			LCD_Build(7, energy_weapon_4);
			break;
		}
		case weapon_cooldown_2:
		{
			break;
		}
		case gathering_energy_1:
		{
			LCD_Cursor(15);
			LCD_WriteData(0x04);
			break;
		}
		case laser_ready_1:
		{
			LCD_Cursor(15);
			LCD_WriteData(0x05);
			break;
		}
		case laser_shot_1:
		{
			LCD_Cursor(15);
			LCD_WriteData(0x06);
			for (int i = 1; i<15; i++)
			{
				LCD_Cursor(i);
				LCD_WriteData(0x07);
			}
			break;
		}
		case weapon_cooldown_1:
		{
			break;
		}
		case gathering_energy_2:
		{
			LCD_Cursor(31);
			LCD_WriteData(0x04);
			break;
		}
		case laser_ready_2:
		{
			LCD_Cursor(31);
			LCD_WriteData(0x05);
			break;
		}
		case laser_shot_2:
		{
			LCD_Cursor(31);
			LCD_WriteData(0x06);
			for (int i = 17; i<31; i++)
			{
				LCD_Cursor(i);
				LCD_WriteData(0x07);
			}
			break;
		}
		case weapon_game_over:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	LCD_Cursor(0);
	return state;
}
// --------END User defined FSMs-----------------------------------------------

// Implement scheduler code from PES.
int main()
{
// Set Data Direction Registers
// Buttons PORTA[0-7], set AVR PORTA to pull down logic
DDRA = 0xFF; PORTA = 0x00;
DDRC = 0xF0; PORTC = 0x0F;
DDRB = 0xFF; PORTB = 0x00;
// . . . etc

// Period for the tasks
unsigned long int SMTick1_calc = 100;
unsigned long int SMTick2_calc = 100;
unsigned long int SMTick3_calc = 300;
unsigned long int SMTick4_calc = 200;
unsigned long int SMTick5_calc = 100;
unsigned long int SMTick6_calc = 100;

//Calculating GCD

unsigned long int tmpGCD = 1;
tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
tmpGCD = findGCD(tmpGCD, SMTick3_calc);
tmpGCD = findGCD(tmpGCD, SMTick4_calc);
tmpGCD = findGCD(tmpGCD, SMTick5_calc);
tmpGCD = findGCD(tmpGCD, SMTick6_calc);
//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc/GCD;
unsigned long int SMTick2_period = SMTick2_calc/GCD;
unsigned long int SMTick3_period = SMTick3_calc/GCD;
unsigned long int SMTick4_period = SMTick4_calc/GCD;
unsigned long int SMTick5_period = SMTick5_calc/GCD;
unsigned long int SMTick6_period = SMTick6_calc/GCD;
//Declare an array of tasks 
static task task1, task2, task3, task4, task5, task6;
task *tasks[] = { &task1, &task2, &task3, &task4, &task5, &task6};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = init;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &tickfct_keypad;//Function pointer for the tick.

// Task 2
task2.state = bullet_negOne;//Task initial state.
task2.period = SMTick2_period;//Task Period.
task2.elapsedTime = SMTick2_period; // Task current elasped time.
task2.TickFct = &tickfct_bullet; // Function pointer for the tick.
 //Task 3
task3.state = asteroid_1_negOne;//Task initial state.
task3.period = SMTick3_period;//Task Period.
task3.elapsedTime = SMTick3_period;//Task current elapsed time.
task3.TickFct = &tickfct_asteroid_1;//function pointer for the tick.

// Task 4
task4.state = asteroid_2_negOne;//Task initial state.
task4.period = SMTick4_period;//Task Period.
task4.elapsedTime = SMTick4_period; // Task current elasped time.
task4.TickFct = &tickfct_asteroid_2; // Function pointer for the tick.

// Task 5
task5.state = Boss_negOne;//Task initial state.
task5.period = SMTick5_period;//Task Period.
task5.elapsedTime = SMTick5_period; // Task current elasped time.
task5.TickFct = &tickfct_Boss; // Function pointer for the tick.

// Task 6
task6.state = weapon_negOne;//Task initial state.
task6.period = SMTick6_period;//Task Period.
task6.elapsedTime = SMTick6_period; // Task current elasped time.
task6.TickFct = &tickfct_weapon_system; // Function pointer for the tick.
// Set the timer and turn it on
TimerSet(GCD);
TimerOn();
LCD_init();
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
