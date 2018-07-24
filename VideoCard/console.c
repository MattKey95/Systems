//	Basic Console Output.

#include <stdint.h>
#include <string.h>
#include <console.h>

// Video memory
uint16_t *_videoMemory = (uint16_t *)0xB8000;

#define CONSOLE_HEIGHT		25
#define CONSOLE_WIDTH		80

// Current cursor position
uint8_t _cursorX = 0;
uint8_t _cursorY = 0;

// Current color
uint8_t	_colour = 15;

// Write byte to device through port mapped io
void  OutputByteToVideoController(unsigned short portid, unsigned char value) 
{
	asm volatile("movb	%1, %%al \n\t"
				 "movw	%0, %%dx \n\t"
				  "out	%%al, %%dx"
				 : : "m" (portid), "m" (value));
				 
}

// Update hardware cursor position

void UpdateCursorPosition(int x, int y) 
{
    uint16_t cursorLocation = y * 80 + x;

	// Send location to VGA controller to set cursor
	// Send the high byte
	OutputByteToVideoController(0x3D4, 14);
	OutputByteToVideoController(0x3D5, cursorLocation >> 8); 
	// Send the low byte
	OutputByteToVideoController(0x3D4, 15);
	OutputByteToVideoController(0x3D5, cursorLocation);      
}

// Displays a character
void ConsoleWriteCharacter(unsigned char c) 
{
    uint16_t attribute = _colour << 8;
	
	if(c == '\n'){
		_cursorY++;
		_cursorX = 0;
	}else{
		// Assume printable characters
		int16_t* location = _videoMemory + (_cursorY * CONSOLE_WIDTH + _cursorX);
		*location = c | attribute;
		_cursorX++;
	}
}

// Display specified string

void ConsoleWriteString(char* str) 
{
	if (!str)
	{
		return;
	}
	while (*str)
	{
		ConsoleWriteCharacter(*str++);
	}
}

//http://www.geeksforgeeks.org/implement-itoa/
void ConsoleWriteInt(unsigned int num, unsigned int base)
{
	char str[10];
    int i = 0;
	char base_digits[16] =
	 {'0', '1', '2', '3', '4', '5', '6', '7',
	  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
		str[i++] = base_digits[rem];
        num = num/base;
    }
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    ConsoleWriteString(str);
} 

void ConsoleClearScreen(const uint8_t c){
	ConsoleSetColour(c);
	uint16_t attribute = _colour << 8;
	_cursorX = 0;
	_cursorY = 0;
	for(int i = 0; i < CONSOLE_WIDTH; i++){
		for(int j = 0; j < CONSOLE_HEIGHT; j++){
			int16_t* l = _videoMemory + (_cursorY * CONSOLE_WIDTH + _cursorX);
			*l = ' ' | attribute;
			_cursorY++;
		}
		_cursorX++;
		_cursorY = 0;
	}
	_cursorX = 0;
	_cursorY = 0;
}

/* A utility function to reverse a string  */
void reverse(char* str, int length)
{
    /* skip null */
    if (str == 0)
    {
        return;
    }

    /* skip empty string */
    if (*str == 0)
    {
        return;
    }

    /* get range */
    char *start = str;
    char *end = start + strlen(str) - 1; /* -1 for \0 */
    char temp;

    /* reverse */
    while (end > start)
    {
        /* swap */
        temp = *start;
        *start = *end;
        *end = temp;

        /* move */
        ++start;
        --end;
    }
}

unsigned int ConsoleSetColour(const uint8_t c){
	_colour = c;
}	

// Position the cursor at the specified X and Y position. All subsequent calls to 
// ConsoleWriteXXXX routines will start at this position.

void ConsoleGotoXY(unsigned int x, unsigned int y){
	_cursorX = x;
	_cursorY = y;
}	

// Get the current X and Y position of the cursor

void ConsoleGetXY(unsigned* x, unsigned* y){
	x = _cursorX;
	y = _cursorY;
}

// Get the width of the console window

int ConsoleGetWidth(){
	return CONSOLE_WIDTH;
}

// Get the height of the console window

int ConsoleGetHeight(){
	return CONSOLE_HEIGHT;
}	

