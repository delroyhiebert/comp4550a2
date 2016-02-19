#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <lcd_functions.h>
#include <lcd_driver.h>

#include <stdio.h>
#include <inttypes.h>
#include <setjmp.h>

int InitButterfly( void );
int InitSound( void );
int InitTimer0( uint8_t ); 
void play_note_timer( int, uint16_t, uint16_t );
void play_note_delay( int, uint16_t, uint16_t );
void dynamic_delay( uint16_t );
void playSong();

//Keyboard
/*#define C_0 ( (uint16_t) ( 2000000UL / (16UL*2) ) )
#define CS_0 ( (uint16_t) ( 2000000UL / (17UL*2) ) )
#define D_0 ( (uint16_t) ( 2000000UL / (18UL*2) ) )
#define DS_0 ( (uint16_t) ( 2000000UL / (19UL*2) ) )
#define E_0 ( (uint16_t) ( 2000000UL / (21UL*2) ) )
#define F_0 ( (uint16_t) ( 2000000UL / (22UL*2) ) )
#define FS_0 ( (uint16_t) ( 2000000UL / (23UL*2) ) )
#define G_0 ( (uint16_t) ( 2000000UL / (24UL*2) ) )
#define GS_0 ( (uint16_t) ( 2000000UL / (26UL*2) ) )
#define A_0 ( (uint16_t) ( 2000000UL / (28UL*2) ) )
#define AS_0 ( (uint16_t) ( 2000000UL / (29UL*2) ) )
#define B_0 ( (uint16_t) ( 2000000UL / (31UL*2) ) )

#define C_1 ( (uint16_t) ( 2000000UL / (33UL*2) ) )
#define CS_1 ( (uint16_t) ( 2000000UL / (35UL*2) ) )
#define D_1 ( (uint16_t) ( 2000000UL / (37UL*2) ) )
#define DS_1 ( (uint16_t) ( 2000000UL / (39UL*2) ) )
#define E_1 ( (uint16_t) ( 2000000UL / (41UL*2) ) )
#define F_1 ( (uint16_t) ( 2000000UL / (44UL*2) ) )
#define FS_1 ( (uint16_t) ( 2000000UL / (46UL*2) ) )
#define G_1 ( (uint16_t) ( 2000000UL / (49UL*2) ) )
#define GS_1 ( (uint16_t) ( 2000000UL / (52UL*2) ) )
#define A_1 ( (uint16_t) ( 2000000UL / (55UL*2) ) )
#define AS_1 ( (uint16_t) ( 2000000UL / (58UL*2) ) )
#define B_1 ( (uint16_t) ( 2000000UL / (62UL*2) ) )
*/
#define C_2 ( (uint16_t) ( 2000000UL / (65UL*2) ) )
#define CS_2 ( (uint16_t) ( 2000000UL / (69UL*2) ) )
#define D_2 ( (uint16_t) ( 2000000UL / (73UL*2) ) )
#define DS_2 ( (uint16_t) ( 2000000UL / (78UL*2) ) )
#define E_2 ( (uint16_t) ( 2000000UL / (82UL*2) ) )
#define F_2 ( (uint16_t) ( 2000000UL / (87UL*2) ) )
#define FS_2 ( (uint16_t) ( 2000000UL / (92UL*2) ) )
#define G_2 ( (uint16_t) ( 2000000UL / (98UL*2) ) )
#define GS_2 ( (uint16_t) ( 2000000UL / (104UL*2) ) )
#define A_2 ( (uint16_t) ( 2000000UL / (110UL*2) ) )
#define AS_2 ( (uint16_t) ( 2000000UL / (117UL*2) ) )
#define B_2 ( (uint16_t) ( 2000000UL / (123UL*2) ) )

#define C_3 ( (uint16_t) ( 2000000UL / (131UL*2) ) )
#define CS_3 ( (uint16_t) ( 2000000UL / (139UL*2) ) )
#define D_3 ( (uint16_t) ( 2000000UL / (147UL*2) ) )
#define DS_3 ( (uint16_t) ( 2000000UL / (156UL*2) ) )
#define E_3 ( (uint16_t) ( 2000000UL / (165UL*2) ) )
#define F_3 ( (uint16_t) ( 2000000UL / (175UL*2) ) )
#define FS_3 ( (uint16_t) ( 2000000UL / (185UL*2) ) )
#define G_3 ( (uint16_t) ( 2000000UL / (196UL*2) ) )
#define GS_3 ( (uint16_t) ( 2000000UL / (208UL*2) ) )
#define A_3 ( (uint16_t) ( 2000000UL / (220UL*2) ) )
#define AS_3 ( (uint16_t) ( 2000000UL / (233UL*2) ) )
#define B_3 ( (uint16_t) ( 2000000UL / (247UL*2) ) )

#define C_4 ( (uint16_t) ( 2000000UL / (262UL*2) ) )
#define CS_4 ( (uint16_t) ( 2000000UL / (277UL*2) ) )
#define D_4 ( (uint16_t) ( 2000000UL / (294UL*2) ) )
#define DS_4 ( (uint16_t) ( 2000000UL / (311UL*2) ) )
#define E_4 ( (uint16_t) ( 2000000UL / (330UL*2) ) )
#define F_4 ( (uint16_t) ( 2000000UL / (349UL*2) ) )
#define FS_4 ( (uint16_t) ( 2000000UL / (370UL*2) ) )
#define G_4 ( (uint16_t) ( 2000000UL / (392UL*2) ) )
#define GS_4 ( (uint16_t) ( 2000000UL / (415UL*2) ) )
#define A_4 ( (uint16_t) ( 2000000UL / (440UL*2) ) )
#define AS_4 ( (uint16_t) ( 2000000UL / (466UL*2) ) )
#define B_4 ( (uint16_t) ( 2000000UL / (494UL*2) ) )

#define C_5 ( (uint16_t) ( 2000000UL / (523UL*2) ) )
#define CS_5 ( (uint16_t) ( 2000000UL / (554UL*2) ) )
#define D_5 ( (uint16_t) ( 2000000UL / (587UL*2) ) )
#define DS_5 ( (uint16_t) ( 2000000UL / (622UL*2) ) )
#define E_5 ( (uint16_t) ( 2000000UL / (659UL*2) ) )
#define F_5 ( (uint16_t) ( 2000000UL / (698UL*2) ) )
#define FS_5 ( (uint16_t) ( 2000000UL / (740UL*2) ) )
#define G_5 ( (uint16_t) ( 2000000UL / (784UL*2) ) )
#define GS_5 ( (uint16_t) ( 2000000UL / (831UL*2) ) )
#define A_5 ( (uint16_t) ( 2000000UL / (880UL*2) ) )
#define AS_5 ( (uint16_t) ( 2000000UL / (932UL*2) ) )
#define B_5 ( (uint16_t) ( 2000000UL / (988UL*2) ) )

#define tempo ((int) 160) //BPM
#define BPS ((int) (tempo/60))

#define q_count ((float) (1.0 / BPS)*1000)
#define h_count ((float) (q_count * 2))
#define w_count ((float) (h_count * 2))
#define e_count ((float) (q_count / 2))
#define s_count ((float) (e_count / 2))

#define wdot_count ((float)(w_count * 1.5))
#define hdot_count ((float)(h_count * 1.5))
#define qdot_count ((float)(q_count * 1.5))
#define edot_count ((float)(e_count * 1.5))

#define Min_Amplitude 0
#define Max_Amplitude 2000

#define interrupt 32.768 //2.048Time between interrupts

enum note_types{
	whole = 0,
	half = 1,
	quarter = 2,
	eigth = 3,
	sixteenth = 4,
	rest = 5,
	edot = 6,
	hdot = 7,
	qdot = 8,
};