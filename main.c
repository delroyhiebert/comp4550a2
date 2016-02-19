/* Name: main.c
 * Author: Delroy Hiebert
 */
#include <main.h>

//uint16_t keyslist[] = { C_4, C_4, G_4, G_4, A_4, A_4, G_4, F_4, F_4, E_4, E_4, D_4, D_4, C_4, G_4, G_4, F_4, F_4, E_4, E_4, D_4, G_4, G_4, F_4, F_4, E_4, E_4, D_4, C_4, C_4, G_4, G_4, A_4, A_4, G_4, F_4, F_4, E_4, E_4, D_4, D_4, C_4};
//int noteslist[] = { q, q, q, q, q, q, h, q, q, q, q, q, q, h, q, q, q, q, q, q, h, q, q, q, q, q, q, h, q, q, q, q, q, q, h, q, q, q, q, q, q, h};
uint16_t keyslist[] = {FS_4, rest, A_4, FS_4, FS_4, B_4, FS_4, E_4, FS_4, rest, CS_5, FS_4, FS_4, D_5, CS_5, A_4, FS_4, CS_5, FS_5, FS_4, E_4, E_4, CS_4, GS_4, FS_4, rest};//, FS_2, FS_3, E_2, E_3, CS_2, CS_3, E_2, FS_2, FS_3, CS_2, CS_3, E_3, FS_3, D_2, D_3, E_2, E_3, CS_2, E_2, FS_2, FS_3, rest };
uint8_t noteslist[] = {eigth, eigth, edot, eigth, sixteenth, eigth, eigth, eigth, eigth, eigth, edot, eigth, sixteenth, eigth, eigth, eigth, eigth, eigth, eigth, sixteenth, eigth, sixteenth, eigth, eigth, qdot, hdot};//, eigth, eigth, edot, eigth, sixteenth, eigth, eigth, eigth, eigth, eigth, eigth, eigth, sixteenth, sixteenth, eigth, eigth, eigth, eigth, eigth, edot, eigth, sixteenth, eigth, eigth, eigth, quarter, quarter};

volatile uint16_t note_timer;
volatile uint16_t attack, decay, sustain, release;
volatile uint8_t state;
volatile uint16_t increment;
volatile uint16_t volume_counter;
volatile uint16_t note_amplitude;

jmp_buf task1_jb;
jmp_buf task2_jb;
jmp_buf task3_jb;
jmp_buf task4_jb;

ISR(TIMER0_OVF_vect) {

  note_timer += interrupt;
  switch( state )
  {
    case 0:
            volume_counter += increment;
            OCR1A = volume_counter;//(( note_timer / attack ) * note_amplitude);
            break;
    case 1:
            volume_counter -= increment;
            OCR1A = volume_counter;//( note_amplitude - ((note_timer / decay) * ( 0.3 * note_amplitude )) );
            break;
    case 2:
            asm volatile("nop"::);
            break;
    case 3:
            volume_counter -= increment;
            OCR1A = volume_counter;//( (1 - (note_timer / release)) * OCR1A);
            break;
  }
}

int main(void)
{
  cli();
  InitButterfly();
  LCD_Init();
  //Uncomment this line if using the timer driven play_note_timer method
  //InitTimer0( ( ( 1 << CS02 ) | ( 0 << CS01 ) | ( 0 << CS00 ) ) );
  InitSound();
  sei();
    
  for(;;)
    {
      task1();
    }
  return 0;   /* never reached */
}

void task1()
{
  for(;;)
  {
    if( setjmp( task1_jb) == 0 )
    {
      task2_tramp();
    }
    else
    {
      playSong();
      longjmp( task2_jb, 1 );
    }
  }
}

void task2_tramp()
{
  uint8_t hole[32];
  int i = 0;
  for( i; i < sizeof(hole)/sizeof(hole[0]); ++i )
  {
    hole[i] = i;
  }
  task2();
}

void task2()
{
  if( setjmp( task2_jb ) == 0 )
  {
    longjmp( task1_jb, 2 );
  }
  else
  {
    displayTimer();
    longjmp( task1_jb, 1 );
  }
}

void displayTimer()
{
  int counter = 180;
  int i = counter;
  char num[5];

  for( i; i >= 0; i-- )
  {
    itoa( i, num, 10 );
    LCD_puts( num, 0 );
    _delay_ms(100);
    //lcdputs counter
    //yield()
  }
}

void playSong()
{
  int i = 0;
  int (*play_note_ptr)(int, uint16_t, uint16_t);
  play_note_ptr = &play_note_delay;//change to delay/timer to use different functionality
  for( i; i < (sizeof(noteslist)/sizeof(uint8_t)); i++ )
  {
    switch (noteslist[i])
    {
      case quarter: //playQuarter();
              (*play_note_ptr)( 500, keyslist[i], q_count);
              break;
      case half: //playHalf();
              (*play_note_ptr)( 500, keyslist[i], h_count);
              break;
      case whole: //playWhole();
              (*play_note_ptr)( 500, keyslist[i], w_count);
              break;
      case eigth:
              (*play_note_ptr)( 500, keyslist[i], e_count);
              break;
      case sixteenth:
              (*play_note_ptr)( 500, keyslist[i], s_count);
              break;
      case edot:
              (*play_note_ptr)( 500, keyslist[i], edot_count);
              break;
      case qdot:
              (*play_note_ptr)( 500, keyslist[i], qdot_count);
              break;
      case hdot:
              (*play_note_ptr)( 500, keyslist[i], hdot_count);
              break;
    }
  }
  //yield();
}

void play_note_timer( int amplitude, uint16_t pitch, uint16_t duration )
{//play notes with a timer interrupt and an ADSR envelope
  ICR1 = pitch;
  attack = duration / 4;
  decay = attack / 2;
  sustain = attack * 2;
  release = decay;

  note_amplitude = amplitude;

  increment = (amplitude / (duration / interrupt));
  volume_counter = 0;
  
  note_timer = 0;
  state = 0;
  while( note_timer < attack )
    asm volatile("nop"::);
  note_timer = 0;
  state = 1;
  while( note_timer < decay)
    asm volatile("nop"::);
  note_timer = 0;
  state = 2;
  while( note_timer < sustain )
    asm volatile("nop"::);
  note_timer = 0;
  state = 3;
  while( note_timer < release )
    asm volatile("nop"::);
}

void play_note_delay( int amplitude, uint16_t pitch, uint16_t duration )
{//Play note with delay calculated ADSR envelope
  
  ICR1 = pitch;
  int attack = duration / 4;
  int decay = attack / 2;
  int sustain = attack * 2;
  int release = decay;
  int increment = amplitude / attack;

  int i = 0;
  int counter = 0;
  for( i; i < attack; i++ )
  {
    counter += increment;
    OCR1A = counter;
    _delay_ms(1);
  }

  i = 0;
  for( i; i< decay; i++)
  {
    counter -= increment;
    OCR1A = counter;
    _delay_ms(1);
  }

  i = 0;
  for( i; i < sustain; i++ )
  {
    _delay_ms(1);
  }

  i = 0;
  for( i; i < release; i++)
  {
    counter -= increment;
    OCR1A = counter;
    _delay_ms(1);
  }

}

/*void play_note( int amplitude, uint16_t pitch, uint16_t duration )
{//Play note with no ADSR envelope
  ICR1 = pitch;
  OCR1A = amplitude;
  dynamic_delay(duration);
  OCR1A = 0;
  _delay_ms(25);
}*/

int InitTimer0( uint8_t preScalar ) {
  uint8_t preScalarMask = ~( ( 1 << CS02 ) | ( 1 << CS01 ) | ( 1 << CS00 ) );
  uint8_t wgmMask = ~( ( 1 << WGM01 ) | ( 1 << WGM00 ) );
  uint8_t ocMask = ~ ( ( 1 << COM0A1 ) | ( 1 << COM0A0 ) );
  TCCR0A = ( TCCR0A & ( preScalarMask & wgmMask & ocMask ) ) | preScalar;
  
  TCNT0 = 0;
  TIFR0 = TIFR0 & ~( ( 1 << TOV0 ) | ( 1 << OCF0A ) );
  //Enable interrupts
  TIMSK0 = ( TIMSK0 & ~( 1 << OCIE0A ) ) | ( 1 << TOIE0 );
  
  return 0;
}

void dynamic_delay( uint16_t count )
{
  while( count-- )
  {
    _delay_ms(1);
  }
}

int InitButterfly( void )
{
  PCMSK0 = 0x00;
  PCMSK1 = 0x00;

  DIDR0 = 0x00;
  DIDR1 = 0x00;
  return 0;
}

int InitSound( void )
{
  // Enable PortB5 output
  DDRB = DDRB | ( 1 << PB5 );
  // Set output pin Port B5 to 0
  PORTB = ( PORTB & ( ~ ( 1 << PB5 ) ) ) | ( 0 << PB5 );
  // Enable output compare toggle mode
  TCCR1A = ( TCCR1A & (~ ( ( 1 << COM1A1 ) | (1 << COM1A0) ) ) ) | ( ( 1 << COM1A1 ) | ( 0 << COM1A0 ) );
  // Enable phase and frequency correct mode, WGM=1000
  TCCR1B = ( TCCR1B & (~ ( ( 1 << WGM13 ) | (1 << WGM12) ) ) ) | ( ( 1 << WGM13 ) | ( 0 << WGM12 ) );
  TCCR1A = ( TCCR1A & (~ ( ( 1 << WGM11 ) | (1 << WGM10) ) ) ) | ( ( 0 << WGM11 ) | ( 0 << WGM10 ) );

  // Set clock to prescalar of 1
  TCCR1B = ( TCCR1B & (~ ( ( 1 << CS12 ) | ( 1 << CS11 ) | ( 1 << CS10 ) ) ) ) | ( ( 0 << CS12 ) | ( 0 << CS11 ) | ( 1 << CS10 ) );
  return 0;
}