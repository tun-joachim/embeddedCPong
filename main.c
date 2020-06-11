/*
 ===================================================================================================
 Name        :  main.c
 Author      :  Tun Joachim Bin Tun Fizi
 Version     :  1.0.0
 Copyright   :  This work is licensed under the WTFPL
 Description :  This program is a mock game from one of the first commercial games, if not the first
                games released. This game is named Pong and it features two paddles and a ball, the
                goal is to defeat your opponent by being the first one to gain 10 point, a player
                gets a point once the opponent misses a ball. The game can be played with two human
                players, or one player against a computer controlled paddle. The game was originally
                developed by Allan Alcorn and released in 1972 by Atari corporations.
 ===================================================================================================
*/
/*
====================================================================================================
Compilation note :  (avr-gcc -mmcu=atmega644p -DF_CPU=12000000 -Wall -L -Os main.c \
                    -o main.elf liblcd.a
                    avr-objcopy -O ihex main.elf main.hex
                    avrdude -c c232hm -p m644p -U flash:w:main.hex 
                    # This requires the C232hm programmer cable and a patched avrdude version 6.3
                    )
                    
                    This statement needs to be added when compiling in order for the llcd.a to be 
                    included when compiliing or else the compiler will produce the undefined 
                    variable error.
====================================================================================================
*/
#include <avr/io.h>

#include <util/delay.h>
#include "lcd.h"

void _init_(void);

int main(void)
{
    _init_();

    int8_t state=0, mov_l=1, mov_bx=1, mov_by=1;
    //state 0:welcome,1:play ball,2: game over

    for(;;)
    {
        switch(state)
        {
//=======================Welcome Screen=============================================================
        case 0: //Welcome screen
            clear_screen();
            display.x = 65;
            display.y = 150;
            display_string("Welcome to a game of");
            display.x = 110;
            display.y = 170;
            display_string("Pong!");
            _delay_ms(2000);

            clear_screen();
            display.x = 65;
            display.y = 160;
            display_string("Press when ready");
            while(state == 0)//wait for user input
            {
                if(((PIND & _BV(3)) == 0)|((PIND & _BV(2)) == 0))
                {
                    state = 1;
                }
            }
            break;
//=======================Start Game=================================================================
        case 1:
            clear_screen();
                            //left,right,top,bottom
            rectangle paddle = {95,145,305,310};
            rectangle line_l = {95,95,305,310};//Paddle edges
            rectangle line_r = {145,145,305,310};//Paddle edges

            rectangle ball = {5,10,5,10};
            rectangle old_ball = ball;

            fill_rectangle(paddle, WHITE);

            while(state == 1)
            {
                _delay_ms(7);
                old_ball = ball;

                //test for paddle
                if((PIND & _BV(2)) == 0)
                {
                    mov_l = 1;
                    if((paddle.right >= 239) | (line_r.right >= 239))
                    {
                        paddle.left = 189;
                        paddle.right = 239;
                        line_l.left = 189;
                        line_l.right = 189;
                        line_r.left = 239;
                        line_r.right = 239;
                    }
                    else
                    {
                        fill_rectangle(line_l, BLACK);
                        paddle.left += mov_l;
                        paddle.right += mov_l;
                        line_l.left += mov_l;
                        line_l.right += mov_l;
                        line_r.left += mov_l;
                        line_r.right += mov_l;
                        fill_rectangle(line_r, WHITE);
                    }
                }
                else if((PIND & _BV(3)) == 0)
                {
                    mov_l=-1;
                    if((paddle.left <= 1) | (line_l.left <= 1))
                    {
                        paddle.left = 1;
                        paddle.right = 51;
                        line_l.left = 1;
                        line_l.right = 1;
                        line_r.left = 51;
                        line_r.right = 51;
                    }
                    else
                    {
                        fill_rectangle(line_r, BLACK);
                        paddle.left += mov_l;
                        paddle.right += mov_l;
                        line_l.left += mov_l;
                        line_l.right += mov_l;
                        line_r.left += mov_l;
                        line_r.right += mov_l;
                        fill_rectangle(line_l, WHITE);
                    }
                }
                else if((PIND & _BV(2))&(PIND & _BV(3)))
                {
                    mov_l=0;
                }
                else
                {
                    mov_l=0;
                }

                //test for ball
                if(ball.bottom <=315)
                {
                    if(ball.left <=0)
                    {
                        mov_bx = 1;
                    }
                    else if(ball.right >=238)
                    {
                        mov_bx = -1;
                    }
                    else if(ball.top <=2)
                    {
                        mov_by =1;
                    }
                    else if(ball.bottom >= (paddle.top - 2))
                    {
                        if ((ball.left <= paddle.right - 1)&&(ball.right >= paddle.left - 1))
                        {
                            mov_by = -1;
                        }
                    }
                }
                else
                {
                    state = 2;
                }

                //Now move ball
                ball.left += mov_bx;
                ball.right +=mov_bx;
                ball.bottom += mov_by;
                ball.top += mov_by;
                fill_rectangle(old_ball,BLACK);
                fill_rectangle(ball,WHITE);

            }
            break;
//=======================Game Over=================================================================
        case 2:
            clear_screen();
            display.x = 90;
            display.y = 160;
            display_string("Game Over");
            _delay_ms(2000);

            clear_screen();
            display.x = 70;
            display.y = 160;
            display_string("Press for rematch");

            while(state == 2)//wait for user input
            {
                if(((PIND & _BV(3)) == 0)|((PIND & _BV(2)) == 0))
                {
                    state = 1;
                }
            }

            break;
        default:
            clear_screen();
            display_string("ERROR. CURRENT STATE UNKNOWN");// error if outside of {0,1,2} range
            while(1);
        }
    }
}

void _init_(void)
{
    init_lcd();
    set_orientation(North);
    DDRD |= 0xFF ^ (_BV(PB2) | _BV(PB3)); // set all pins to output except pin 2 & 3
    PORTD |= _BV(PB2) | _BV(PB3); //set pullup

}
