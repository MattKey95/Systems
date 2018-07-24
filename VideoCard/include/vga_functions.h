#ifndef _VGA_FUNCTIONS_H
#define _VGA_FUNCTIONS_H
#include <stdint.h>

typedef struct PointStrc{
	int x;
	int y;
} Point; //easy way to pass an define points

void SetDimensions(int w, int h, int c);//used to change the size/resolution of the screen
void SetPixel(int x, int y, uint8_t col);
void SetSinglePalette(int col, int red, int green, int blue);//sets colour number in vga palette to RGB value
void ClearScreen();
void DrawLine(int x0, int y0, int x1, int y1, uint8_t col, int spaces);//spaces indicates the amount of pixel before one pixel is missed. If 0 no pixels are missed. If 1 a dotted line it created. Only avalible on this function.
void DrawLinePoints(Point p0, Point p1, uint8_t col);//used for ease of use so you can type less. not added in user mode.
void DrawRect(int x0, int y0, int x1, int y1, uint8_t col, bool fill);
void DrawCircle(int x0, int y0, int r, uint8_t col, bool fill);
void DrawPolygon(Point coords[], int length, uint8_t col, bool fill);
Point GetIntersectionPoint(Point coords[]); // used for scanline algorithm
#endif