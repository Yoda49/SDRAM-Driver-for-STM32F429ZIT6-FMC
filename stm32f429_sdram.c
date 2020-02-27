#include "stm32f4xx.h"

#define SDRAM_BASE 0xD0000000
#define SDRAM_SIZE 0x00800000

//=================================================================================================
// Macros for SDRAM Timing Register
//=================================================================================================
#define TMRD(x) (x << 0)
#define TXSR(x) (x << 4)
#define TRAS(x) (x << 8)
#define TRC(x)  (x << 12)
#define TWR(x)  (x << 16)
#define TRP(x)  (x << 20)
#define TRCD(x) (x << 24)

//=================================================================================================
// SDRAM_Init function
//=================================================================================================
void SDRAM_Init(void)
{
	volatile uint32_t ptr = 0;
	volatile uint32_t i = 0;

	RCC->AHB1ENR  |= 0x0000007E; // Enable GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, and GPIOG interface clock
	
	// PB5 PB6
	GPIOB->AFR[0]  = 0x0CC00000; // Connect PBx pins to FMC Alternate function
	GPIOB->AFR[1]  = 0x00000000; // Connect PBx pins to FMC Alternate function
	GPIOB->MODER   = 0x00002A80; // Configure PBx pins in Alternate function mode 
	GPIOB->OSPEEDR = 0x00003CC0; // Configure PBx pins speed to MAX MHz
	GPIOB->OTYPER  = 0x00000000; // Configure PBx pins Output type to push-pull
	GPIOB->PUPDR   = 0x00000100; // No pull-up, pull-down for PBx pins

	// PC0
	GPIOC->AFR[0]  = 0x0000000C; // Connect PCx pins to FMC Alternate function
	GPIOC->AFR[1]  = 0x00000000; // Connect PCx pins to FMC Alternate function
	GPIOC->MODER   = 0x00000002; // Configure PCx pins in Alternate function mode
	GPIOC->OSPEEDR = 0x00000003; // Configure PCx pins speed to MAX MHz
	GPIOC->OTYPER  = 0x00000000; // Configure PCx pins Output type to push-pull 
	GPIOC->PUPDR   = 0x00000000; // No pull-up, pull-down for PCx pins
	
	// PD0 PD1 PD8 PD9 PD10 PD14 PD15
	GPIOD->AFR[0]  = 0x000000CC; // Connect PDx pins to FMC Alternate function
	GPIOD->AFR[1]  = 0xCC000CCC; // Connect PDx pins to FMC Alternate function
	GPIOD->MODER   = 0xA02A000A; // Configure PDx pins in Alternate function mode
	GPIOD->OSPEEDR = 0xF03F000F; // Configure PDx pins speed to MAX MHz
	GPIOD->OTYPER  = 0x00000000; // Configure PDx pins Output type to push-pull
	GPIOD->PUPDR   = 0x00000000; // No pull-up, pull-down for PDx pins
	
	// PE0 PE1 PE7 PE8 PE9 PE10 PE11 PE12 PE13 PE14 PE15
	GPIOE->AFR[0]  = 0xC00000CC; // Connect PEx pins to FMC Alternate function
	GPIOE->AFR[1]  = 0xCCCCCCCC; // Connect PEx pins to FMC Alternate function
	GPIOE->MODER   = 0xAAAA800A; // Configure PEx pins in Alternate function mode
	GPIOE->OSPEEDR = 0xFFFFC00F; // Configure PEx pins speed to MAX MHz
	GPIOE->OTYPER  = 0x00000000; // Configure PEx pins Output type to push-pull
	GPIOE->PUPDR   = 0x00000000; // No pull-up, pull-down for PEx pins

	// PF0 PF1 PF2 PF3 PF4 PF5 PF11 PF12 PF13 PF14 PF15
	GPIOF->AFR[0]  = 0x00CCCCCC; // Connect PFx pins to FMC Alternate function
	GPIOF->AFR[1]  = 0xCCCCC000; // Connect PFx pins to FMC Alternate function
	GPIOF->MODER   = 0xAA800AAA; // Configure PFx pins in Alternate function mode
	GPIOF->OSPEEDR = 0xFFC00FFF; // Configure PFx pins speed to 50 MHz
	GPIOF->OTYPER  = 0x00000000; // Configure PFx pins Output type to push-pull
	GPIOF->PUPDR   = 0x00000000; // No pull-up, pull-down for PFx pins
	
	// PG0 PG1 PG4 PG5 PG8 PG15
	GPIOG->AFR[0]  = 0x00CC00CC; // Connect PGx pins to FMC Alternate function
	GPIOG->AFR[1]  = 0xC000000C; // Connect PGx pins to FMC Alternate function
	GPIOG->MODER   = 0x80020A0A; // Configure PGx pins in Alternate function mode
	GPIOG->OSPEEDR = 0xC0030F0F; // Configure PGx pins speed to 50 MHz
	GPIOG->OTYPER  = 0x00000000; // Configure PGx pins Output type to push-pull
	GPIOG->PUPDR   = 0x00000000; // No pull-up, pull-down for PGx pins
	
	
	RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;	
	
	// Initialization step 1
	FMC_Bank5_6->SDCR[0] = FMC_SDCR1_SDCLK_1 | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE_1;
	FMC_Bank5_6->SDCR[1] = FMC_SDCR1_NR_0	 | FMC_SDCR1_MWID_0 | FMC_SDCR1_NB | FMC_SDCR1_CAS;
	
	// Initialization step 2
	FMC_Bank5_6->SDTR[0] = TRC(7)  | TRP(2);
	FMC_Bank5_6->SDTR[1] = TMRD(2) | TXSR(7) | TRAS(4) | TWR(2) | TRCD(2);
	
	// Initialization step 3	
	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR 	 = 1 | FMC_SDCMR_CTB2 | (1 << 5);
	// Initialization step 4
	for(i = 0; i  < 1000000; i++);
	
	// Initialization step 5
	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR 	 = 2 | FMC_SDCMR_CTB2 | (1 << 5);
	
	// Initialization step 6
	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR 	 = 3 | FMC_SDCMR_CTB2 | (4 << 5);	
	
	// Initialization step 7
	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR 	 = 4 | FMC_SDCMR_CTB2 | (1 << 5) | (0x231 << 9);
	
	// Initialization step 8
	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);
	FMC_Bank5_6->SDRTR |= (683 << 1);
	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);
}
