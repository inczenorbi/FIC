#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct area {
	int Ox, Oy, r;
} area;

struct pos {
    int x, y;
} pos;

int sqr(int x) {
    return x * x;
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
  * Tells us if we win the game.
  * @param e stores the position of the enemy
  * @param c stores the attributes of the circle
  * @return 1 if the enemy is outside of the circle
**/
int checkWinCondition(struct pos e, struct area c) {
	if(c.r * c.r < (e.x - c.Ox) * (e.x - c.Ox) + (e.y - c.Oy) * (e.y - c.Oy))
        return 1;
    else
        return 0;
}

int checkSafeZone(struct pos o, struct area c) {
    if(sqr((c.r - 15)) > sqr((o.x - c.Ox)) + sqr((o.y - c.Oy)))
        return 1;
    else
        return 0;
}
/**
  * Tells us the distance between the D point and the OP straight.
  * @param o stores our position
  * @param p stores the enemy's position
  * @param d stores a point's position
  * @return the distance between the D point and the OP straight
**/
int onStraight(struct pos o, struct pos p, struct pos d) {
    int a = p.x - o.x, b = -(p.y - o.y), c = o.x * a + o.y * b;
    return abs(a * d.x + b * d.y + c) / sqrt(a * a + b * b);
}
/**
  * Tells us the distance between two points.
  * @param a stores the position of the first point
  * @param b stores the position of the second point
  * @return the distance between A and B
**/
int distBetween(struct pos a, struct pos b) {
    return sqrt(sqr(a.x - b.x) + sqr(a.y - b.y));
}

int getAngle(int befogo, int atfogo) {
    return befogo / atfogo;
}

char *chooseAction(struct area c, struct pos o, struct pos d, struct pos e) {
    int dist, eps;

    if((dist = distBetween(o, e)) > 3 * c.r / 4) //
        eps = 15;
    else if(dist > c.r / 2)
        eps = 10;
    else
        eps = 5;

    if(onStraight(o, e, d) - eps <= 0) {

    }

}

char *moveRobot(struct area c, struct pos o, struct pos d, struct pos e) {
    int dr = sqrt(sqr(o.x - c.Ox + d.x - c.Ox) + sqr(o.y - c.Oy + d.y - c.Oy));
    int dir = onStraight(o, e, d);


    return "s";
}

int main(void) {
    return 0;
}
