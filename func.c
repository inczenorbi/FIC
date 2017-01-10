#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

typedef struct point {
    int x, y;
} point;

typedef struct area {
	int r;
	point *o;
} area;
/**
  * Returns the square of a number
  * @param x is the input number
  * @return the square of the {@code x}
**/
int sqr(int x) {
    return x * x;
}
/**
  * Returns the distance between two points.
  * @param a stores the position of the first point
  * @param b stores the position of the second point
  * @return the distance between A and B
**/
double distBetween(point *a, point *b) {
    return sqrt(0.0 + sqr(a->x - b->x) + sqr(a->y - b->y));
}
/**
  * Returnes color's code (Hmin Hmax Smin Smax Vmin Vmax)
  * @param color is the name of the color with lowercase
  * @param v stores the color's code
**/
void getColor(char *color, int *v) {
	if(!strcmp(color, "blue")) {
			v[0] = 83;
			v[1] = 165;
			v[2] = 35;
			v[3] = 256;
			v[4] = 228;
			v[5] = 256;
	}
	else if(!strcmp(color, "red")) {
			v[0] = 0;
			v[1] = 6;
			v[2] = 158;
			v[3] = 256;
			v[4] = 0;
			v[5] = 256;
	}
	else if(!strcmp(color, "green")) {
			v[0] = 46;
			v[1] = 256;
			v[2] = 0;
			v[3] = 256;
			v[4] = 100;
			v[5] = 210;
	}
	else if(!strcmp(color, "yellow")) {
			v[0] = 0;
			v[1] = 256;
			v[2] = 113;
			v[3] = 163;
			v[4] = 231;
			v[5] = 256;
	}
}
/**
  * Tells us if we won or we lose the game.
  * @param e stores the position of the enemy
  * @param o stores our robot's position
  * @param c stores the attributes of the circle
  * @return 1 if the enemy or our robot is outside of the circle
**/
int checkWinLoseCondition(point *e, point *o, area *c) {
	if(distBetween(e, c->o) > c->r || distBetween(o, c->o) > c->r)
        return 1;
    else
        return 0;
}
/**
  * Returns the angle of the 3 points
  * @param a is the first point
  * @param b is the second point which angle will be returned
  * @param c is the third point
  * @return angle of ABC
**/
double getAngle(point *a, point *b, point *c) {
    double angle = abs((atan2(a->y - b->y, a->x - b->x) - atan2(c->y - b->y, c->x - b->x)) * 180 / PI);

    return (angle > 180) ? (360 - angle) : angle;
}
/**
  * Tells us what our robot need to do.
  * @param c stores the attributes of the circle
  * @param o stores our position
  * @param d stores our direction's position
  * @param e stores the enemy's position
  * @return the letter of the selected action
**/
char *chooseAction(area *c, point *o, point *d, point *e) {
    int i, index = 0, dr = distBetween(o, d);
    point *dless, *dmore;
    double radian = 2 * PI, DOE = getAngle(e, o, d);
    char *action = new char[100];

    for(i = 0; i <= round(radian * dr); i++)
        if(d->x <= 0.25 + c->o->x + o->x + sin(radian * (i / (2 * dr * PI))) * dr  &&  d->y <= 0.25 + c->o->y + o->y + cos(radian * (i / (2 * dr * PI))) * dr) {
            index = i;
            break;
        }

    dmore = new point();
    dless = new point();
    if(!dless || !dmore) {
        printf("Allocaton error!\n");
        exit(-1);
    }

    dless->x = c->o->x + o->x + sin(2 * PI * ((index - i / 8) / (2 * dr * PI))) * dr;
    dless->y = c->o->y + o->y + cos(2 * PI * ((index - i / 8) / (2 * dr * PI))) * dr;
    dmore->x = c->o->x + o->x + sin(2 * PI * ((index + i / 8) / (2 * dr * PI))) * dr;
    dmore->y = c->o->y + o->y + cos(2 * PI * ((index + i / 8) / (2 * dr * PI))) * dr;

    if(DOE > 40 && DOE < 140 && distBetween(o, e) <= distBetween(o, d) * 6) { // Our robot is in danger by the enemy robot
        if(distBetween(d, c->o) < distBetween(o, c->o)) {
            strcpy(action, "f");
            return action;
        }
        else {
            strcpy(action, "b");
            return action;
        }
    }
    else if(DOE <= 5 || (DOE <= 10 && distBetween(o, e) < c->r / 2)) { // We will hit him if we go forward
        strcpy(action, "f");
        return action;
    }
    else if(DOE >= 175 || (DOE >= 170 && distBetween(o, e) < c->r / 2)) { // We will hit him if we go backward
        strcpy(action, "b");
        return action;
    }
    else if(distBetween(dless, e) < distBetween(dmore, e)) { // Turning to the right position
        strcpy(action, "r");
        return action;
    }
    else {
        strcpy(action, "l");
        return action;
    }
}
/**
  * This function makes precisely our robot's action
  * @param c stores the attributes of the circle
  * @param o stores our position
  * @param d stores our direction's position
  * @param e stores the enemy's position
  * @return what our robot's need to do
**/
char *moveRobot(area *c, point *o, point *d, point *e) {        // FIXIT WITH TIMER
    char *moves = new char[100];
    int i;
    double DOE = getAngle(d, o, e);

    if(!moves) {
        printf("Allocation error!\n");
        exit(-1);
    }
    sprintf(moves, "%s", chooseAction(c, o, d, e));

    if(moves[0] == 'f') {
        if((distBetween(o, e) <= c->r / 2 && distBetween(o, c->o) <= c->r / 2) || distBetween(d, c->o) < distBetween(o, c->o))
            strcat(moves, "ffffffff");
        else
            strcat(moves, "ff");
    }
    else if(moves[0] == 'b') {
        if((distBetween(o, e) <= c->r / 2 && distBetween(o, c->o) <= c->r / 2) || distBetween(d, c->o) > distBetween(o, c->o))
            strcat(moves, "bbbbbbbb");
        else
            strcat(moves, "bb");
    }
    else {
        for(i = 20; i < DOE; i += 20)
            if(moves[0] == 'l')
                strcat(moves, "l");
            else
                strcat(moves, "r");
    }

    strcat(moves, "s");
    return moves;
}