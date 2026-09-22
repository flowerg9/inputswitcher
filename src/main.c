#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

static void set_input(uint8_t idx)
{

	PORTD = (1 << idx);
	PORTC = (PORTC & ~((1 << 2) | (1 << 3) | (1 << 4))) | ((idx & 0x07) << 2);

}

int main (void)
{
	DDRD = 0xFF; //set DirRegD to out
	PORTD = 0x00; //set PORTD to 0
	
	DDRC |= (1 << 2) | (1 << 3) | (1 << 4);	
	
	uint8_t current = 0;
	set_input(current);

	while(1)
	{
		_delay_ms(500);
		
		current = (current + 1) & 0x07; //simulate button press
		set_input(current);

	}
	
	return 0;
}
