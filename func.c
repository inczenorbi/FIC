#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

struct point {
    int x, y;
} point;

struct area {
	int r;
	struct point *o;
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
double distBetween(struct point *a, struct point *b) {
    return sqrt(0.0 + sqr(a->x - b->x) + sqr(a->y - b->y));
}

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
  * Tells us if we won the game.
  * @param e stores the position of the enemy
  * @param c stores the attributes of the circle
  * @return 1 if the enemy is outside of the circle
**/
int checkWinCondition(struct point *e, struct area *c) {
	if(distBetween(e, c->o) > c->r)
        return 1;
    else
        return 0;
}
/**
  * Tells us we are in safe zone or not.
  * @param o stores our robot's position
  * @param d stores our direction's position
  * @param c stores the attributes of the circle
  * @return 1 if we are in safe zone
**/
int checkSafeZone(struct point *o, struct point *d, struct area *c) {
    if(distBetween(o, c->o) < c->r - 2 * distBetween(o, d))
        return 1;
    else
        return 0;
}
/**
  * Tells us the distance between a point and a straight.
  * @param d stores a point's position
  * @param a stores the first point's position
  * @param b stores the second point's position
  * @return the distance between the D point and the AB straight
**/
double onStraight(struct point *d, struct point *a, struct point *b) {
    int nx, ny, c;
    nx = b->x - a->x;
    ny = -(b->y - a->y);
    c = a->x * nx + a->y * ny;
    return abs(0.0 + nx * d->x + ny * d->y + c) / sqrt(0.0 + sqr(nx) + sqr(ny));
}
/**
  * Returns the angle of 3 points
  * @param a is the first point
  * @param b is the second point which angle will be returned
  * @param c is the third point
  * @return angle of ABC
**/
double getAngle(struct point *a, struct point *b, struct point *c) {
    double atfogo, aux;

    if(distBetween(a, b) > distBetween(b, c))
        atfogo = distBetween(a, b);
    else
        atfogo = distBetween(b, c);

    aux = onStraight(b, a, c) / atfogo;
    if(aux <= -1 || aux >= 1)
        return 0;

    return asin(onStraight(b, a, c) / atfogo) * 180 / PI;
}
/**
  * Tells us what our robot need to do.
  * @param c stores the attributes of the circle
  * @param o stores our position
  * @param d stores our direction's position
  * @param e stores the enemy's position
  * @return the letter of the selected action
**/
char *chooseAction(struct area *c, struct point *o, struct point *d, struct point *e) {
    int i, index = 0, dr = distBetween(o, d);
    struct point *dless, *dmore;
    double radian = 2 * PI, DOE, EDO, DEO;

    EDO = getAngle(e, d, o);
    DEO = getAngle(d, e, o);
    if(!EDO) {
        DOE = 0;
    }
    else if(!DEO) {
        DOE = 90;
    }
    else
        DOE = 180 - getAngle(e, d, o) - getAngle(d, e, o);

    for(i = 0; i <= round(radian * dr); i++)
        if(d->x <= 0.5 + c->o->x + o->x + sin(radian * (i / (2 * dr * PI))) * dr  &&  d->y <= 0.5 + c->o->y + o->y + cos(radian * (i / (2 * dr * PI))) * dr) {
            index = i;
            break;
        }

    printf("DOE %f %f %f\n", DOE, getAngle(e, d, o), getAngle(d, e, o));



    dmore = malloc(sizeof(struct point));
    dless = malloc(sizeof(struct point));
    if(!dless) {
        printf("Allocaton error!\n");
        exit(-1);
    }

    dless->x = c->o->x + o->x + sin(2 * PI * ((index - i / 8) / (2 * dr * PI))) * dr;
    dless->y = c->o->y + o->y + cos(2 * PI * ((index - i / 8) / (2 * dr * PI))) * dr;

    dmore->x = c->o->x + o->x + sin(2 * PI * ((index + i / 8) / (2 * dr * PI))) * dr;
    dmore->y = c->o->y + o->y + cos(2 * PI * ((index + i / 8) / (2 * dr * PI))) * dr;

    if(DOE <= 5) {
        if(distBetween(d, e) < distBetween(o, e))
            return "f";
        else if(distBetween(o, e) < c->r / 2)
            return "b";
    }

    if(distBetween(dless, e) < distBetween(dmore, e)) {
        return "r";
    }
    else
        return "l";
}
/**
  * This function makes precisely our robot's action
  * @param c stores the attributes of the circle
  * @param o stores our position
  * @param d stores our direction's position
  * @param e stores the enemy's position
  * @return what our robot's need to do
**/
char *moveRobot(struct area *c, struct point *o, struct point *d, struct point *e) {
    char *moves = malloc(100 * sizeof(char));
    int i;
    double DOE = 180 - getAngle(o, e, d) - getAngle(o, d, e);

    if(!moves) {
        printf("Allocation error!\n");
        exit(-1);
    }
    strcpy(moves, chooseAction(c, o, d, e));

    if(moves[0] == 'f') {
        if(distBetween(o, e) <= c->r / 2 && distBetween(o, c->o) <= c->r / 2)
            strcat(moves, "fffff");
        else
            strcat(moves, "ff");
    }
    else if(moves[0] == 'b') {
        if(distBetween(o, c->o) <= c->r / 2)
            strcat(moves, "bbbbb");
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

int main(void) {
    struct point *o, *e, *d;
    struct area *c;

    o = malloc(sizeof(struct point));
    e = malloc(sizeof(struct point));
    d = malloc(sizeof(struct point));
    c = malloc(sizeof(struct area));

    if(!o || !e || !d || !c) {
        printf("Allocation error!\n");
        return 1;
    }

    c->o = malloc(sizeof(struct point));
    if(!c->o) {
        printf("Allocation error!\n");
        return 1;
    }

    c->o->x = 0;
    c->o->y = 0;
    c->r = 100;

    e->x = 0;
    e->y = 0;

    o->x = 50;
    o->y = 0;

    d->x = 50;
    d->y = -20;

    printf("%s\n", moveRobot(c, o, d, e));


    return 0;
}
