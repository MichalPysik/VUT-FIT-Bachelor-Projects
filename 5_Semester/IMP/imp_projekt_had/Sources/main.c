// IMP Project - Snake game on ARM-FITkit3 platform
//
// Author: Michal Pyšík (xpysik00)
//
// Note: Some parts of code were taken from Fitkit3-demo
// (c) 2019 Michal Bidlo, BUT FIT, bidlom@fit.vutbr.cz

#include "MK60D10.h"
#include <stdbool.h>

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))

/* Mapping of buttons, matrix display leds and column selectors to specific port pins: */
#define BTN_SW2 0x400     // Port E, bit 10
#define BTN_SW3 0x1000    // Port E, bit 12
#define BTN_SW4 0x8000000 // Port E, bit 27
#define BTN_SW5 0x4000000 // Port E, bit 26
#define BTN_SW6 0x800     // Port E, bit 11

#define COL_A0 0x100 //Port A, bit 8
#define COL_A1 0x400 //Port A, bit 10
#define COL_A2 0x40 //Port A, bit 6
#define COL_A3 0x800 //Port A, bit 11

#define ROW_R0 0x4000000 //Port A, bit 26
#define ROW_R1 0x1000000 //Port A, bit 24
#define ROW_R2 0x200 //Port A, bit 9
#define ROW_R3 0x2000000 //Port A, bit 25
#define ROW_R4 0x10000000 //Port A, bit 28
#define ROW_R5 0x80 //Port A, bit 7
#define ROW_R6 0x8000000 //Port A, bit 27
#define ROW_R7 0x20000000 //Port A, bit 29

#define NEG_EN 0x10000000 //Port E, bit 28 (negation of matrix display's EN signal)


#define PERIODS_PER_MOVE 384 //LPTMR periods per game event (snake move) - should be divisible by 16 (row count)
#define SNAKE_LEN 8 //length of snake

//snake move direction
typedef enum
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
} DIRECTION_T;

//snake has 6 bodyparts, each has x and y coord.
typedef struct snake_struct
{
	unsigned char x[SNAKE_LEN];
	unsigned char y[SNAKE_LEN];
} Snake;

Snake snake; //our snake instance

int pressed_up = 0, pressed_down = 0, pressed_left = 0, pressed_right = 0, pressed_reset = 0;
int period_cnt = 0; //LPTMR period counter
int col_select = 0; //column selector for multiplexing

bool smatrix[16][8] = {0}; //game field
DIRECTION_T snake_dir = UP; //snake moving direction

bool gameOver = false;


//fills the whole game field (smatrix) with true or false values (depending on the fill argument)
void clearGameField(bool fill)
{
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			smatrix[i][j] = fill;
		}
	}
}


//sets default position of snake
void setDefaultSnake()
{
	for(int i = 0; i < SNAKE_LEN; i++)
	{
		snake.x[i] = 3; //horizontal position
		snake.y[i] = (16 - SNAKE_LEN) + i; //vertical position
		smatrix[ snake.y[i] ][ snake.x[i] ] = true; //activate cell in game field
	}

	snake_dir = UP; //set default move direction
}


//moves the snake in the active direction
void moveSnake()
{
	smatrix[ snake.y[SNAKE_LEN-1] ][ snake.x[SNAKE_LEN-1] ] = false; //get rid of tail

	for(int i = SNAKE_LEN-1; i > 0; i--) //move body of the snake (no need to re-draw)
	{
		snake.x[i] = snake.x[i-1];
		snake.y[i] = snake.y[i-1];
	}

	if(snake_dir == UP)
	{
		if(snake.y[0] == 0) gameOver = true; //upper boundary collision
		else if(smatrix[ snake.y[0]-1 ][ snake.x[0] ]) gameOver = true; //snake bodypart collision
		else snake.y[0] -= 1; //ok, moving the snake up
	}
	else if(snake_dir == DOWN)
	{
		if(snake.y[0] == 15) gameOver = true; //lower boundary collision
		else if(smatrix[ snake.y[0]+1 ][ snake.x[0] ]) gameOver = true; //snake bodypart collision
		else snake.y[0] += 1; //ok, moving the snake down
	}
	else if(snake_dir == LEFT)
	{
		if(snake.x[0] == 0) gameOver = true; //left boundary collision
		else if(smatrix[ snake.y[0] ][ snake.x[0]-1 ]) gameOver = true; //snake bodypart collision
		else snake.x[0] -= 1; //ok, moving the snake left
	}
	else //snake_dir == RIGHT
	{
		if(snake.x[0] == 7) gameOver = true; //right boundary collision
		else if(smatrix [snake.y[0] ][ snake.x[0]+1 ]) gameOver = true; //snake bodypart collision
		else snake.x[0] += 1; //ok, moving the snake right
	}

	if(gameOver) clearGameField(true); //fill the whole field if the game is over
	else smatrix[ snake.y[0] ][ snake.x[0] ] = true; //else draw the new snake head
}


/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(void)  {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}


void PortsInit(void)
{
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTA_MASK;

    /* Set corresponding PTA and PTE pins for GPIO functionality */
    PORTE->PCR[10] = PORT_PCR_MUX(0x01); // SW2
    PORTE->PCR[12] = PORT_PCR_MUX(0x01); // SW3
    PORTE->PCR[27] = PORT_PCR_MUX(0x01); // SW4
    PORTE->PCR[26] = PORT_PCR_MUX(0x01); // SW5
    PORTE->PCR[11] = PORT_PCR_MUX(0x01); // SW6

    // Display columns multiplex selectors
    PORTA->PCR[8] = PORT_PCR_MUX(0x01);  // A0
    PORTA->PCR[10] = PORT_PCR_MUX(0x01); // A1
    PORTA->PCR[6] = PORT_PCR_MUX(0x01);  // A2
    PORTA->PCR[11] = PORT_PCR_MUX(0x01); // A3

    // Display rows
    PORTA->PCR[26] = PORT_PCR_MUX(0x01); // R0
    PORTA->PCR[24] = PORT_PCR_MUX(0x01); // R1
    PORTA->PCR[9] = PORT_PCR_MUX(0x01);  // R2
    PORTA->PCR[25] = PORT_PCR_MUX(0x01); // R3
    PORTA->PCR[28] = PORT_PCR_MUX(0x01); // R4
    PORTA->PCR[7] = PORT_PCR_MUX(0x01);  // R5
    PORTA->PCR[27] = PORT_PCR_MUX(0x01); // R6
    PORTA->PCR[29] = PORT_PCR_MUX(0x01); // R7

    PORTE->PCR[28] = PORT_PCR_MUX(0x01); // NEG_EN

    /* Change corresponding Port A and Port E pins as outputs */
    PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0); // Display rows and cols as outputs
    PTE->PDDR = GPIO_PDDR_PDD(NEG_EN); // Display EN negation

    PTA->PDOR &= ~(0x3F000FC0); //turn off all matrix display rows and reset column index
    PTE->PDOR &= ~(NEG_EN); //turn on matrix display (just in case)
}


void LPTMR0_IRQHandler(void)
{
    PTA->PDOR &= ~(0x3F000FC0); //turn off all matrix display rows and reset column index

    //select the column we are currently on
    if(col_select & 0x1) PTA->PDOR |= COL_A0;
    if(col_select & 0x2) PTA->PDOR |= COL_A1;
    if(col_select & 0x4) PTA->PDOR |= COL_A2;
    if(col_select & 0x8) PTA->PDOR |= COL_A3;

    //turn on leds in selected rows
    if(smatrix[col_select][0]) PTA->PDOR |= ROW_R7;
    if(smatrix[col_select][1]) PTA->PDOR |= ROW_R6;
    if(smatrix[col_select][2]) PTA->PDOR |= ROW_R5;
    if(smatrix[col_select][3]) PTA->PDOR |= ROW_R4;
    if(smatrix[col_select][4]) PTA->PDOR |= ROW_R3;
    if(smatrix[col_select][5]) PTA->PDOR |= ROW_R2;
    if(smatrix[col_select][6]) PTA->PDOR |= ROW_R1;
    if(smatrix[col_select][7]) PTA->PDOR |= ROW_R0;

    col_select = (col_select + 1) % 16; //move to the next column (using multiplexing)

    if(++period_cnt == PERIODS_PER_MOVE) //snake move
    {
    	period_cnt = 0;
    	if(!gameOver) moveSnake();
    }

    LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;   // writing 1 to TCF to clear the flag
}


void LPTMR0Init(int count)
{
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK; // Enable clock to LPTMR
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;   // Turn OFF LPTMR to perform setup
    LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
                 | LPTMR_PSR_PBYP_MASK   // LPO feeds directly to LPT
                 | LPTMR_PSR_PCS(1)) ;   // use the choice of clock
    LPTMR0_CMR = count;                  // Set compare value
    LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK    // Clear any pending interrupt (now)
                 | LPTMR_CSR_TIE_MASK    // LPT interrupt enabled
                );
    NVIC_EnableIRQ(LPTMR0_IRQn);         // enable interrupts from LPTMR0
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;    // Turn ON LPTMR0 and start counting
}


int main(void)
{
    MCUInit();
    PortsInit();
    LPTMR0Init(0x00); //fastest timer setting - 0 compare value

    clearGameField(false);
    setDefaultSnake();
    gameOver = false;

    while (1) {
        // button up
        if(!pressed_up && !(GPIOE_PDIR & BTN_SW5))
        {
            pressed_up = 1;
            snake_dir = UP;
        }
        else if(GPIOE_PDIR & BTN_SW5) pressed_up = 0;

        // button down
        if(!pressed_down && !(GPIOE_PDIR & BTN_SW3))
        {
            pressed_down = 1;
            snake_dir = DOWN;
        }
        else if(GPIOE_PDIR & BTN_SW3) pressed_down = 0;

        // button left
        if(!pressed_left && !(GPIOE_PDIR & BTN_SW4))
        {
            pressed_left = 1;
            snake_dir = LEFT;
        }
        else if(GPIOE_PDIR & BTN_SW4) pressed_left = 0;

        // button right
        if(!pressed_right && !(GPIOE_PDIR & BTN_SW2))
        {
            pressed_right = 1;
            snake_dir = RIGHT;
        }
        else if(GPIOE_PDIR & BTN_SW2) pressed_right = 0;

        // reset button (SW6)
        if(!pressed_reset && !(GPIOE_PDIR & BTN_SW6))
        {
        	pressed_reset = 1;
        	clearGameField(false);
        	setDefaultSnake();
        	gameOver = false;
        }
        else if(GPIOE_PDIR & BTN_SW6) pressed_reset = 0;
    }

    return 0;
}
