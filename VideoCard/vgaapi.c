#include <hal.h>
#include <console.h>
#include <vga_functions.h>
#include <keyboard.h>

#define MAX_VGACALL 7

typedef struct _VGACallInfo
{
	void * VGACall;
	int    ParamCount;
} VGACallInfo;

VGACallInfo _VGACalls[MAX_VGACALL]; 

void InitialiseVGACall(int index, void * vgaCall, int paramCount)
{
	if (index >= 0 && index < MAX_VGACALL)
	{
		_VGACalls[index].VGACall = vgaCall;
		_VGACalls[index].ParamCount = paramCount;
	}
}

// The new interrupt attribute has not been used here since
// the code is so specialised.

void VGAcallDispatcher() 
{
	static int index = 0;
	// Get index into _VGACalls table from eax
	asm volatile("movl %%eax, %0" : "=r" (index));

	if (index < MAX_VGACALL)
	{
		// Temporarily save the registers that are used to pass in the parameters
		asm volatile ("push %edx\n\t"
					  "push %ecx\n\t"
					  "push %ebx\n\t");
		void * VGAFunction = _VGACalls[index].VGACall;
		int paramCount = _VGACalls[index].ParamCount;
		// Now generate the code for the user call.  There is different
		// code depending on how many parameters are passed to the function.
		// After the call to the kernel routine, we remove the parameters from teh
		// stack by adjusting the stack pointer.  This is the standard C calling convention.
		//int pointer for holding arrays passed from user.c
		int *i;
		switch (paramCount)
		{
			case 6:
				//get address stored in ebx and store in i
				asm("movl %%ebx, %0" : "=r"(i));
				//setup stack
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
 							  "pop %%edx\n\t" : :);
				//put each value from the array onto the stack one by one in reverse order.
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[5])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[4])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[3])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[2])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[1])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[0])); 
				asm volatile ("call *%0\n\t"
							  "addl $24, %%esp"
							  : : "r"(VGAFunction) 
							  );
							  break;
			case 5:
				asm("movl %%ebx, %0" : "=r"(i));
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
 							  "pop %%edx\n\t" : :);
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[4])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[3])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[2])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[1])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[0])); 
				asm volatile ("call *%0\n\t"
							  "addl $20, %%esp"
							  : : "r"(VGAFunction) 
							  );
							  break;
			case 4:
				asm("movl %%ebx, %0" : "=r"(i));
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
 							  "pop %%edx\n\t" : :);
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[3])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[2])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[1])); 
				asm volatile ("movl %0, %%ebx\n\t"
							  "push %%ebx\n\t" : : "b"(i[0])); 
				asm volatile ("call *%0\n\t"
							  "addl $16, %%esp"
							  : : "r"(VGAFunction) 
							  );
							  break;
			case 3:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
 							  "pop %%edx\n\t"
							  "push %%edx\n\t"
							  "push %%ecx\n\t"
							  "push %%ebx\n\t"
							  "call *%0\n\t"
							  "addl $12, %%esp"
							  : : "r"(VGAFunction)
							  );
							  break;
				
			case 2:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "push %%ecx\n\t"
							  "push %%ebx\n\t"
							  "call *%0\n\t"
							  "addl $8, %%esp"
							  : : "r"(VGAFunction)
							  );
							  break;
							  
			case 1:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "push %%ebx\n\t"
							  "call *%0\n\t"
							  "addl $4, %%esp"
							  : : "r"(VGAFunction)
							  );
							  break;
							  
			case 0:
				asm volatile ("pop %%ebx\n\t"
							  "pop %%ecx\n\t"
							  "pop %%edx\n\t"
							  "call *%0\n\t"
							  : : "r"(VGAFunction)
							  );
							  break;
				 
		}
	}
	asm("leave");
	asm("iret");
}

#define I86_IDT_DESC_RING3		0x60

void InitialiseVGACalls() 
{
	InitialiseVGACall(0, SetPixel, 3);
	InitialiseVGACall(1, ClearScreen, 0);
	InitialiseVGACall(2, DrawLine, 6);
	InitialiseVGACall(3, DrawCircle, 5);
	InitialiseVGACall(4, DrawPolygon, 4);
	InitialiseVGACall(5, DrawRect, 6);
	InitialiseVGACall(6, SetSinglePalette, 4);
	
	// Install interrupt handler!
	HAL_SetInterruptVector(0x81, VGAcallDispatcher, I86_IDT_DESC_RING3);
}

