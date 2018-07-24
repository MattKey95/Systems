#include <console.h>
#include <vga_functions.h>
#include <keyboard.h>

void User_ConsoleWriteCharacter(unsigned char c)
{
	asm volatile("movl $1, %%eax\n\t"
				 "movzx %0, %%ebx\n\t"
				 "int $0x80\n"
				 : : "b"(c)
				);
}

void User_ConsoleWriteString(char* str)
{
	asm volatile("xor %%eax, %%eax\n\t"
				 "leal (%0), %%ebx\n\t"
				 "int $0x80\n"
				 : : "b"(str)
				);
}

void User_ConsoleWriteInt(unsigned int i, unsigned int base)
{
	asm volatile("movl $2, %%eax\n\t"
				 "movl %0, %%ecx\n\t"
				 "movl %1, %%ebx\n\t"
				 "int $0x80\n"
				 : : "c"(base), "b"(i)
				);
}

void User_ConsoleClearScreen(const uint8_t c)
{
	asm volatile("movl $3, %%eax\n\t"
				 "movzx %0, %%ebx\n\t"
				 "int $0x80\n"
				 : : "b"(c)
				);
}

void User_ConsoleGotoXY(unsigned int x, unsigned int y) 
{
	asm volatile("movl $4, %%eax\n\t"
				 "movl %0, %%ecx\n\t"
				 "movl %1, %%ebx\n\t"
				 "int $0x80\n"
				 : : "c"(y), "b"(x)
				);
}

void User_SetPixel(int x, int y, uint8_t col){
	asm volatile("movl $0, %%eax\n\t"
				 "movl %0, %%edx\n\t"
				 "movl %1, %%ecx\n\t"
				 "movl %2, %%ebx\n\t"
				 "int $0x81\n"
				 : : "d"((int)col), "c"(y), "b"(x)
				);				
}

void User_SetSinglePalette(int col, int red, int green, int blue){
	int param[4] = {col, red, green, blue};
	asm volatile("movl $6, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "int $0x81\n"
				 : : "b"(&param)
				);
}
void User_ClearScreen(){
	asm volatile("movl $1, %%eax\n\t"
				 "int $0x81\n"
				 : :
				);
}

void User_DrawLine(int x0, int y0, int x1, int y1, uint8_t col, int spaces){
	int param[6] = {x0, y0, x1, y1, col, spaces};
	asm volatile("movl $2, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "int $0x81\n"
				 : : "b"(&param)
				);
}


void User_DrawCircle(int x0, int y0, int r, uint8_t col, bool fill){
	int param[5] = {x0, y0, r, col, fill};
	asm volatile("movl $3, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "int $0x81\n"
				 : : "b"(&param)
				);
}
void User_DrawPolygon(Point coords[], int length, uint8_t col, bool fill){
	
	int param[5] = {coords, length, col, fill};
	asm volatile("movl $4, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "int $0x81\n"
				 : : "b"(&param)
				);
}

void User_DrawRect(int x0, int y0, int x1, int y1, uint8_t col, bool fill){
	int param[6] = {x0, y0, x1, y1, col, fill};
	asm volatile("movl $5, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "int $0x81\n"
				 : : "b"(&param)
				);
}

