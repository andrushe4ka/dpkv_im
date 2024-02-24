#include "stm8s.h"
#include "debug_lib.h"

#define DPKV_PIN GPIO_PIN_5
#define IGN_PIN GPIO_PIN_4

typedef struct {
	uint8_t cntr;
	uint8_t init;
} CNTR_T;

/* RPM depnds on init
	1 - 5000 rpm
	2 - 2500 rpm
	4 - 1250 rpm
	5 - 1000 rpm
	10 - 500 rpm
*/
CNTR_T dpkv = {0, 10};
/* Igniton coil charge time depends on init
	1 - 0.1 ms
	10 - 1 ms
	100 - 10 ms
*/
CNTR_T ign = {0, 10};

uint8_t check_symbol(char s) {
	if (s < 48 || s > 57) {
		return(0);
	}
	return(1);
}

uint16_t get_arg(char* offset) {
	uint8_t i = 0;
	while (*offset) {
		i++;
		offset++;
	}
	offset--;
	uint16_t r;
	uint8_t m;
	r = 0;
	m = 1;
	while (i--) {
		r += m * (*offset - 48);
		m *= 10;
	}
	return(r);
}

// This interrupt called each 0.1 ms
INTERRUPT_HANDLER(IRQ_Handler_TIM4, 23) {
	// Manage counter for ignition signal
	if (ign.cntr) {
		ign.cntr--;
		if (!ign.cntr) GPIO_WriteLow(GPIOB, IGN_PIN);
	}

	static uint8_t front = 0;

	// Manage counter for dpkv signal
	dpkv.cntr--;
	if (!dpkv.cntr) {
		switch (++front) {
			case 115:
			case 116:
			case 118:
			case 119:
				break;
			default:
				GPIO_WriteReverse(GPIOB, DPKV_PIN);
				break;
		}
		if (front == 120) {
			front = 0;
			ign.cntr = ign.init;
			GPIO_WriteHigh(GPIOB, IGN_PIN);
		}
		dpkv.cntr = dpkv.init;
	}

	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}

INTERRUPT_HANDLER(IRQ_Handler_UART1, 18) {
	static int8_t pntr = 0;

	static char cmd[4] = {0, 0, 0, 0};

	char s = UART1_ReceiveData8();

	switch (pntr) {
		case 0:
			switch (s) {
				case 't':
				case 'T':
				case 'r':
				case 'R':
					cmd[0] = s;
					pntr++;
					break;
				default:
					cmd[0] = 0;
					pntr = 3;
			}
			break;
		case 1:
		case 2:
			if (check_symbol(s)) {
				cmd[pntr] = s;
				pntr++;
			} else {
				cmd[0] = 0;
				pntr = 3;
			}
			break;
		case 3:
			if (s == '\n') {
				if (cmd) {
					uint16_t val = get_arg(cmd + 1);
					switch (cmd[0]) {
						case 't':
						case 'T':
							ign.init = val;
							break;
						case 'r':
						case 'R':
							dpkv.init = 50 / val;
							break;
					}
					send_str(cmd);
				} else {
					send_str("ERR");
				}
				send_str("\n");
				pntr = -1;
			}
			break;
	}

	UART1_ClearITPendingBit(UART1_IT_RXNE);
}


int main(void) {
	debug_init();

	// Init DPKV pin
	GPIOB->CR1 &= (uint8_t)(~(DPKV_PIN)); //Open drain
	GPIOB->CR2 &= (uint8_t)(~(DPKV_PIN)); //No slope control
	GPIOB->DDR |= (uint8_t)DPKV_PIN;

	// Init IGN pin
	GPIOB->CR1 &= (uint8_t)(~(IGN_PIN)); //Open drain
	GPIOB->CR2 &= (uint8_t)(~(IGN_PIN)); //No slope control
	GPIOB->DDR |= (uint8_t)IGN_PIN;

	GPIO_WriteLow(GPIOB, DPKV_PIN);
	GPIO_WriteLow(GPIOB, IGN_PIN);

	TIM4_DeInit();
	//TIM4_Cmd(DISABLE);
	// set the period 100 microseconds
	TIM4_TimeBaseInit(TIM4_PRESCALER_2, 99);
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

