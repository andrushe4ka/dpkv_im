#include "stm8s.h"
#include "debug_lib.h"

struct {
	uint8_t data[5];
	uint8_t pntr;
} ext_cmd = {{0,0,0,0,0}, 0};

uint16_t time = 10;
uint16_t delay = 1000;
uint16_t counter = 0; 
uint8_t spark = 0;

uint8_t get_timer_val() {
	return time * 25 / 4;
}


uint8_t check_symbol(char s) {
	if (s < 48 || s > 57) {
		return(0);
	}
	return(1);
}

uint16_t get_arg() {
	uint16_t r;
	uint8_t i;
	uint8_t m;
	i = 3;
	r = 0;
	m = 1;
	while (i > 1) {
		if (!check_symbol(ext_cmd.data[i])) return(1000);
		r += m * (ext_cmd.data[i] - 48);
		m *= 10;
		i--;
	}
	return(r);
}

INTERRUPT_HANDLER(IRQ_Handler_TIM4, 23) {
	if (spark) {
		spark = 0;
		GPIO_WriteLow(GPIOB, GPIO_PIN_5);
		counter = 0;
	} else {
		counter += time;
		if (counter >= delay) {
			GPIO_WriteHigh(GPIOB, GPIO_PIN_5);
			spark = 1;
		}
	}
	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}

INTERRUPT_HANDLER(IRQ_Handler_UART1, 18) {
	if (ext_cmd.pntr < 3) {
		ext_cmd.data[ext_cmd.pntr] = UART1_ReceiveData8();
		ext_cmd.pntr++;
	}
	if (ext_cmd.pntr == 3) {
		ext_cmd.pntr = 0;
		switch (ext_cmd.data[0]) {
			case 't':
			case 'T':
				uint16_t val = get_arg();
				if (val < 1000) {
					time = val;
					TIM4_SetAutoreload((uint8_t)time);
					send_str(ext_cmd.data);
				} else {
					send_str("ERR");
				}
				send_str("\n");
		}
	}
	UART1_ClearITPendingBit(UART1_IT_RXNE);
}


int main(void) {
	debug_init();

	GPIOB->CR1 &= (uint8_t)(~(GPIO_PIN_5)); //Open drain
	GPIOB->CR2 &= (uint8_t)(~(GPIO_PIN_5)); //No slope control
	GPIOB->DDR |= (uint8_t)GPIO_PIN_5;

	GPIO_WriteLow(GPIOB, GPIO_PIN_5);

	TIM4_DeInit();
	//TIM4_Cmd(DISABLE);
	TIM4_TimeBaseInit(TIM4_PRESCALER_32, get_timer_val());
	//TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

	enableInterrupts();
	
	TIM4_Cmd(ENABLE);

	// Endless loop
	while(1) {
		// Waiting for interrupt
		__asm__("WFI");
	}
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

