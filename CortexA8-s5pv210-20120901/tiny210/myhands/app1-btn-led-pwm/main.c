#include <stdio.h>

void led_running_horse(void);

void stop_buzzer(void);
void set_buzzer_freq(int freq);

int notes[8] = {262, 294, 330, 349, 392, 440, 494, 530};

void do_some_thing(int num)
{
	int i;
	
	int freq = 1000 ;

	printf("button %d is down\n", num);

	// pwm beep
	set_buzzer_freq(notes[num] * 6);
	for (i = 0; i < 50000000; i++)
		;
	stop_buzzer();

	for (i = 0; i < num; i++)
		led_running_horse();

	return;
}

void btn_main(void);

int led_open(void);
int led_close(void);

void open_buzzer(void);

int main(void)
{
	printf("hello, my demo\n");

	led_open();
	open_buzzer();

	btn_main();

	led_close();

	return 0;
}
