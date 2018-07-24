#ifndef _USER_H
#define _USER_H
#include <keyboard.h>
#include <vga_functions.h>

void User_ConsoleWriteCharacter(unsigned char c); 
void User_ConsoleWriteString(char* str); 
void User_ConsoleWriteInt(unsigned int i, unsigned int base); 
void User_ConsoleClearScreen(const uint8_t c); 
void User_ConsoleGotoXY(unsigned int x, unsigned int y); 

//VGA 
void User_SetPixel(int x, int y, uint8_t col);
void User_SetSinglePalette(int col, int red, int green, int blue);
void User_ClearScreen();
void User_DrawLine(int x0, int y0, int x1, int y1, uint8_t col, int spaces);
void User_DrawRect(int x0, int y0, int x1, int y1, uint8_t col, bool fill);
void User_DrawCircle(int x0, int y0, int r, uint8_t col, bool fill);
void User_DrawPolygon(Point coords[], int length, uint8_t col, bool fill);
#endif