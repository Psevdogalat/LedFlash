#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define INPUT_TIMEOUT 	1
#define NUM_OF_INPUTS	4

uint8_t INPUT_STATE			=0x00;
uint8_t INPUT_TIMERS[4]		={0};

//KEYCODES
#define VKEY_1 1
#define VKEY_2 2
#define VKEY_3 3
#define VKEY_4 4

#define INPUT_BUFFER_SIZE	10
struct {
	uint8_t pHead;
	uint8_t pTail;
	uint8_t Fill;
	uint8_t Buffer[INPUT_BUFFER_SIZE];
}INPUT_BUFFER={0};

uint8_t INPUT_BUFFER_PUSH(uint8_t in_KEYCODE){
	uint8_t* pHead 	= &(INPUT_BUFFER.pHead);
	uint8_t* Fill 	= &(INPUT_BUFFER.Fill);
	
	if(*Fill > INPUT_BUFFER_SIZE) return 0x01;
	
	INPUT_BUFFER.Buffer[(*pHead)] = in_KEYCODE;
	(*pHead)++;if((*pHead) > INPUT_BUFFER_SIZE-1)(*pHead)=0;
	(*Fill)++;
	return 0x00;
}

uint8_t INPUT_BUFFER_POP(uint8_t* out_KEYCODE){
	uint8_t* pTail 	= &(INPUT_BUFFER.pTail);
	uint8_t* Fill 	= &(INPUT_BUFFER.Fill);
	
	if(*Fill < 1) return 0x01;
	
	(*out_KEYCODE) = INPUT_BUFFER.Buffer[(*pTail)];
	(*pTail)++;if((*pTail) > INPUT_BUFFER_SIZE-1)(*pTail)=0;
	(*Fill)--;
	return 0x00;
}

ISR(PCINT0_vect,ISR_BLOCK){
	uint8_t PORT_B_IN	=PINB & 0x0f;
	
	uint8_t INPUT_OFFSET=0x00;
	uint8_t INPUT_MASK	=0x00;
	for(INPUT_OFFSET=0;INPUT_OFFSET < NUM_OF_INPUTS;INPUT_OFFSET++){ 
		INPUT_MASK = 0x01 << INPUT_OFFSET; 
		if((PORT_B_IN & INPUT_MASK) == 0x00){
			if((INPUT_STATE & INPUT_MASK) == 0x00){
				INPUT_STATE		 |= INPUT_MASK;
				
				INPUT_BUFFER_PUSH((INPUT_OFFSET+1));
			}
		}
		else if ((INPUT_STATE & INPUT_MASK) != 0x00){
			INPUT_TIMERS[INPUT_OFFSET]=INPUT_TIMEOUT;
		}
	}
}

ISR(TIMER0_COMPA_vect,ISR_BLOCK){
	uint8_t PORT_B_IN	=PINB & 0x0f;
		
	uint8_t INPUT_OFFSET	=0;
	uint8_t INPUT_MASK		=0x01;
	for(INPUT_OFFSET=0;INPUT_OFFSET < NUM_OF_INPUTS;INPUT_OFFSET++){
		INPUT_MASK = 0x01 << INPUT_OFFSET;
		if(INPUT_TIMERS[INPUT_OFFSET] > 0){
			INPUT_TIMERS[INPUT_OFFSET]--;
		}
		else if ( (PORT_B_IN & INPUT_MASK) != 0x00 && (INPUT_STATE & INPUT_MASK) != 0x00  ){
			INPUT_STATE		&= ~INPUT_MASK;
			INPUT_BUFFER_PUSH((INPUT_OFFSET+1)|0x80);
		}
	}
}

int main(void){
	
	// Port configure
	PCICR	=	0x01;
	PCMSK0	=	0x0F;
	DDRB	=	0x10;
	PORTB	|=	0x0F;
	DDRD	=	0xF8;
	
	// Timer/counter 0 configure
	OCR0A	=0xFF;
	TIMSK0	=0x02;
	TCCR0A	=0x02;
	TCCR0B	=0x05;
	
	sei();

	const uint8_t MINVAL = 0x08;
	const uint8_t MAXVAL = 0x80;
	uint8_t ShiftRegister	= MINVAL;

	while (1) {
		
		
		
		uint8_t InputBufferEmpty 	= 0;
		uint8_t KeyCode				= 0;
		do{
			cli();
			InputBufferEmpty = INPUT_BUFFER_POP(&KeyCode);
			sei();
			if(InputBufferEmpty == 0x00){
				switch(KeyCode){
					case VKEY_1:
						if(ShiftRegister == MAXVAL){
							ShiftRegister = MINVAL;
						}else{
							ShiftRegister = ShiftRegister << 1;
						}
						break;
						
					case VKEY_2:
						if(ShiftRegister == MINVAL){
							ShiftRegister = MAXVAL;
						}else{
							ShiftRegister = ShiftRegister >> 1;
						}
						break;
					default:
						break;	
				}			
			}
		}while(InputBufferEmpty == 0x00);
		
		PORTD = ShiftRegister & 0xF8;
	}
	return 1;
}