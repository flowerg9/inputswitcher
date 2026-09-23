#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

/* Shiftreg pins */
#define PS_PIN PB0 // P/S pin
#define CLK_PIN PB2 // shiftreg CLK
#define DATA_PIN PC5 // 2. shiftreg Q4

/* MAC08A address pins */
#define ADDR0 PC2 /* A0 */
#define ADDR1 PC3 /* A1 */
#define ADDR2 PC4 /* A2 */

static uint8_t read_shiftregs(void)
{
    uint8_t data = 0; // erase data

    PORTB |= (1 << PS_PIN); // set P/S HI for paralel load
    _delay_us(2);
    PORTB |=  (1 << CLK_PIN); // CLK pulse to load state into shiftregs
    _delay_us(1);
    PORTB &= ~(1 << CLK_PIN); // end of CLK pulse
    _delay_us(1);

    PORTB &= ~(1 << PS_PIN); // set P/S LO for serial output
    _delay_us(1);

    /* send 8 CLK pulses to read the shiftregs */
    for (int8_t i = 7; i >= 0; i--) {
        data <<= 1;
        if (PINC & (1 << DATA_PIN)) data |= 1; // check if there is a bit on PC5 if yes write it to data
        PORTB |=  (1 << CLK_PIN); // CLK pulse to send next bit
        _delay_us(1);
        PORTB &= ~(1 << CLK_PIN); // end if CLK pulse
        _delay_us(1);
    }

    return data;
}

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
	DDRC &= ~(1 << DATA_PIN); // set as input
	
	DDRB |= (1 << PS_PIN) | (1 << CLK_PIN); // set as output
	PORTB &= ~((1 << PS_PIN) | (1 << CLK_PIN)); 

	uint8_t current = 0;
	set_input(current);
	
	/* debounce stuff */
	uint8_t last = 0xFF;
	uint8_t stable = 0xFF;
	uint8_t stable_cnt = 0;

	while(1)
	{
		uint8_t now = read_shiftregs();

		if (now != last) {
			stable_cnt = 0;
			last = now;
		} else if (stable_cnt < 3) {
			stable_cnt++;
		} else if  (now != stable) {
			stable = now;

			for (uint8_t i=0; i < 8; i++) {
				if (!(stable & (1 << i))) {
					if (i != current) {
						current = i;
						set_input(current);	
					}
					break;
				}
			}
		}

		_delay_ms(10);
	
	}
	
	return 0;
}
