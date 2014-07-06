//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);
void updateLiveboard(GWindow window,GOval lifes[] ,int lives);
void initLiveboard(GWindow window,GOval lifes[], int lives);


int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    double velocityX = 0.04;
    double velocityY = 0.04;
    
    GOval lifes[lives];
    initLiveboard(window,lifes, lives);

    waitForClick();
    // keep playing until game over    
    while (lives > 0 && bricks > 0)
    {
        // TODO
        GEvent event = getNextEvent(MOUSE_EVENT);
        if (event != NULL)
        {
           if (getEventType(event) == MOUSE_MOVED)
           {
                double pw = getWidth(paddle);
                double py = getY(paddle);
                double px = getX(event) - pw / 2; 
                
                if (px < 0)
                    px = 0;

                if (px + pw > WIDTH)
                    px = WIDTH - pw;

                setLocation(paddle,px,py);
           } 
        
        }

        // move ball
        move(ball, velocityX, velocityY);
        if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
            velocityX = -velocityX;
        }        
        else if (getX(ball) <= 0)
        {
            velocityX = -velocityX;
        }
        
        if (getY(ball) + getHeight(ball) >= getHeight(window))
        {
            velocityY = -velocityY;
        }        
        else if (getY(ball) <= 0)
        {
            velocityY = -velocityY;
        }
        

        //detect collision
        GObject object = detectCollision(window, ball);
        
        if (object != NULL)
        {
            if (object == paddle)
            {
                velocityY = -velocityY;        
            }
            else if (strcmp(getType(object), "GRect") == 0)
            {
                removeGWindow(window, object);
                points += 10;
                bricks--;
                updateScoreboard(window, label, points);
                velocityY = -velocityY;       
            }
        }
        
        if (getY(ball) >= getY(paddle))
        {
            lives--;  
            removeGWindow(window, ball);  
            updateLiveboard(window,lifes, lives);       
            ball = initBall(window);
        }           
        
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // TODO
    int width=WIDTH/(COLS+2);
    int height = 10;
    int bx = 4;
    int by = 50;
    int stepy = 15;
    int stepx = 4;
    string colors[] = {"YELLOW",
                       "BLUE",
                       "CYAN",
                       "DARK_GRAY",
                       "GRAY",
                       "GREEN",
                       "LIGHT_GRAY",
                       "MAGENTA",
                       "ORANGE",
                       "PINK",
                       "RED",
                       "YELLOW"};

    srand(time(NULL));
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
             GRect brick = newGRect(bx, by, width, height);
             setColor(brick, colors[rand() % 12]);
             setFilled(brick, true);
             add(window, brick);    
             bx += width+5;
        }
        
        by += stepy;
        bx = stepx;
    }    
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    int cw = RADIUS*2;
    int ch = RADIUS*2;
    int cx = (WIDTH - cw) / 2;
    int cy = (HEIGHT - ch) / 2;
    GOval circle = newGOval(cx, cy, cw, ch);
    setColor(circle, "BLACK");
    setFilled(circle, true);
    add(window, circle);
    return circle;

}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    int width = 80, height = 10;
    int x = (WIDTH - width) / 2;
    int y = 520;
    GRect rect = newGRect(x,y,width,height);
    setFilled(rect, true);
    setColor(rect,"BLACK");
    add(window, rect);
    return rect;
//    return NULL;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{

    GLabel label;
    label = newGLabel("0");

    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;

    setFont(label, "SansSerif-40");
    setColor(label,"LIGHT_GRAY");
    setLocation(label,x,y);
    add(window,label);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}

void updateLiveboard(GWindow window,GOval lifes[] ,int lives)
{
    
    removeGWindow(window, lifes[lives]);   

}

void initLiveboard(GWindow window,GOval lifes[], int lives)
{

    int cw = RADIUS;
    int ch = RADIUS;
    int cx = 75;
    int cy = 20;
    double x = 10;  
    
    GLabel label = newGLabel("Lives ");
    setLocation(label,x,32);

  

    setFont(label, "SansSerif-20");
    setColor(label,"ORANGE");

    add(window,label);    
    
    for (int i=0; i < lives; i++) 
    {
        lifes[i] = newGOval(cx, cy, cw, ch);
        setColor(lifes[i], "GREEN");
        setFilled(lifes[i], true);
        add(window, lifes[i]);
        cx += 23;
    }

}
