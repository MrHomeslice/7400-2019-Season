#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

#define FRONT_LEFT  0
#define FRONT_RIGHT 1
#define BACK_RIGHT  2
#define BACK_LEFT   3

typedef enum
{
	eRotateCenter = -1,
	eRotateFrontLeft = FRONT_LEFT,
	eRotateFrontRight = FRONT_RIGHT,
	eRotateBackRight = BACK_RIGHT,
	eRotateBackLeft = BACK_LEFT
} eRotationPoint;

struct DoubleXY
{
   double x;
   double y;

   inline DoubleXY operator - (DoubleXY a) {
       return {x-a.x, y-a.y};
   }

   inline DoubleXY operator + (DoubleXY a) {
       return {x+a.x, y+a.y};
   }
};

#endif
