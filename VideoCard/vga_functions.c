#include <hal.h>
#include <vgamodes.h>
#include "vga_functions.h"
#include <math.h>
#include <console.h>

int width;
int height;
int chain;
void SetDimensions(int w, int h, int c){
	width = w;
	height = h;
	chain = c;
}
//http://www.brackeen.com/vga/basics.html
//http://www.brackeen.com/vga/source/bc31/unchain.c.html
//http://www.brackeen.com/vga/unchain.html
void SetPixel(int x, int y, uint8_t col){
	unsigned char far * pixel = (unsigned char far*)0xA0000;
	unsigned short offset;
	if(chain){
		offset = width * y + x;
	}else{
		//unsigned char far* temp1 = (unsigned char far*)0x03c4;
		//temp1 = 0x02;          /* select plane */
		//unsigned char far* temp2 = (unsigned char far*)0x03c5;
		//temp2 =  1 << (x&3);
		OutputByteToVideoController(0x03c4, 0x02);
		OutputByteToVideoController(0x03c5, 1 << (x&3));
		//offset = (y<<6)+(y<<4)+(x>>2);
		offset = (width * y + x)/4;
	}
	pixel[offset] = col;
}

//http://www.brackeen.com/vga/source/bc31/unchain.c.html
//http://www.delorie.com/djgpp/doc/ug/graphics/vga.html
void SetSinglePalette(int col, int red, int green, int blue){
	OutputByteToVideoController(0x03c8, col); //index of colour, eg 0 - 255
	OutputByteToVideoController(0x03c9, red); // simple RGB value for other colours;
	OutputByteToVideoController(0x03c9, green);
	OutputByteToVideoController(0x03c9, blue);
}

void ClearScreen(){
	for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j++){
			SetPixel(i,j,0);
		}
	}
}
//https://www.thecrazyprogrammer.com/2017/01/bresenhams-line-drawing-algorithm-c-c.html
void DrawLine(int x0, int y0, int x1, int y1, uint8_t col, int spaces){
	int deltax;
	int deltay;
	int point;
	int x;
	int y;
	int spaceCount = 0;
	
    deltax = x1 - x0;
    deltay = y1 - y0;
 
    x = x0;
    y = y0;
	//if virtical line just increment y
	if(deltax == 0){
		if(deltay < 0){
			y = y1;
			y1 = y0;
		}
		while(y < y1){
			//this same code can be seen throught this function. It just skips pixels depending on the spaces value. if 0 no pixels are skipped
			if(spaceCount < spaces || spaces == 0){
				SetPixel(x,y,col);
				spaceCount++;
			}else{
				spaceCount = 0;
			}
			
			y++;
		}
	}
	//if horizontal line just increment x
	else if(deltay == 0){
		if(deltax < 0){
			x = x1;
			x1 = x0;
		}
		while(x < x1){
			if(spaceCount < spaces || spaces == 0){
				SetPixel(x,y,col);
				spaceCount++;
			}else{
				spaceCount = 0;
			}
			x++;
		}
	}	
	//if both gradients are the same the line can be draw from either.
	else if((deltax > 0 && deltay > 0) || (deltax < 0 && deltay < 0)){
		//if the gradients are negative swap the points so its positive and the algorithm will work.
		if(deltax < 0){
			x = x1;
			y = y1;
			
			x1 = x0;
			y1 = y0;
			
			deltax = deltax*-1;
			deltay = deltay*-1;
		}
		point = 2 * deltay - deltax;
		while(x < x1)
		{
			if(point >= 0)
			{
				if(spaceCount < spaces  || spaces == 0){
				SetPixel(x,y,col);
				spaceCount++;
			}else{
				spaceCount = 0;
			}
				y++;
				point += 2 * deltay - 2 * deltax;
			}
			else
			{
				if(spaceCount < spaces  || spaces == 0){
				SetPixel(x,y,col);
				spaceCount++;
			}else{
				spaceCount = 0;
			}
				point += 2 * deltay;
			}
			x++;
		}
	}
	//if the x and y gradients are different then alter logic to decriment the right axis
	else if(deltax > 0 && deltay < 0){
		point = 2 * -deltay - deltax;
		
		while(x < x1)
		{
			if(point >= 0)
			{
				if(spaceCount < spaces || spaces == 0){
				SetPixel(x,y,col);
				spaceCount++;
			}else{
				spaceCount = 0;
			}
				y--;
				point += 2 * -deltay - 2 * deltax;
			}
			else
			{
				if(spaceCount < spaces || spaces == 0){
				SetPixel(x,y,col);
				spaceCount++;
			}else{
				spaceCount = 0;
			}
				point += 2 * -deltay;
			}
			x++;
		}
	}
	//this is the same as above but invered. the two could be merged as one but the logic would be more complex so I chose to keep them sperate for read-ability.
	else if (deltax < 0 && deltay > 0){
		point = 2 * deltay + deltax;
		
		while(x > x1)
		{
			if(point >= 0)
			{
				if(spaceCount < spaces || spaces == 0){
				SetPixel(x,y,col);
				spaceCount++;
			}else{
				spaceCount = 0;
			}
				y++;
				point += 2 * deltay + 2 * deltax;
			}
			else
			{
				if(spaceCount < spaces || spaces == 0){
				SetPixel(x,y,col);
				spaceCount++;
			}else{
				spaceCount = 0;
			}
				point += 2 * deltay;
			}
			x--;
		}
	}
}

//Just calls drawline for you if you give it points. This does not support the dotted/dashed lines.
void DrawLinePoints(Point p0, Point p1, uint8_t col){
	DrawLine(p0.x, p0.y, p1.x, p1.y, col, 0);
}

//simply draws rectangle from top left point and width and height.
void DrawRect(int x, int y, int width, int height, uint8_t col, bool fill){
	if(!fill){
		DrawLine(x,y,x+width,y,col, 0);
		DrawLine(x+width,y,x+width,y+height,col, 0);
		DrawLine(x+width,y+height,x,y+height,col, 0);
		DrawLine(x,y+height,x,y,col, 0);
	}else{
		for(int tx0 = x; tx0 < x+width; tx0++){
			for(int ty0 = y; ty0 < y+height; ty0++){
				SetPixel(tx0,ty0,col);
			}
		}
	}
	
}

//https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
//draw circle algorithm
void DrawCircle(int x0, int y0, int r, uint8_t col, bool fill){
	int x = r-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (r << 1);
	if(!fill){
		//just outline
		while (x >= y)
		{
			SetPixel(x0 + x, y0 + y, col);
			SetPixel(x0 + y, y0 + x, col);
			SetPixel(x0 - y, y0 + x, col);
			SetPixel(x0 - x, y0 + y, col);
			SetPixel(x0 - x, y0 - y, col);
			SetPixel(x0 - y, y0 - x, col);
			SetPixel(x0 + y, y0 - x, col);
			SetPixel(x0 + x, y0 - y, col);

			if (err <= 0)
			{
				y++;
				err += dy;
				dy += 2;
			}
			if (err > 0)
			{
				x--;
				dx += 2;
				err += (-r << 1) + dx;
			}
		}
	}else{
		//brute force algorithm for filling the circle, slightly inefficent as it loops through more pixel than needed but not too bad.
		for(int i = x0-r; i <=x0+r; i++){
			for(int j = y0-r; j <= y0+r; j++){
				int fdx = x0 - i; // horizontal offset
				int fdy = y0 - j; // vertical offset
				if ( (fdx*fdx + fdy*fdy) <= (r*r) )
				{
					SetPixel(i,j,col);
				}
			}
		}
	}
}

//draws polygon from points. Points have to be in order i.e. 1234, 2341, 3412 ect...
void DrawPolygon(Point coords[], int length, uint8_t col, bool fill){
	if(fill){
		int minY = 10000;
		int maxY = 0;
		//could have issues if shape has more than 200 intersection points.
		Point intersection[200];
		//gets min and max y values for the shape
		for(int i = 0; i < length; i++){
			if(coords[i].y < minY){
				minY = coords[i].y;
			}else if(coords[length-1].y > maxY){
				maxY = coords[length-1].y;
			}			
		}
		int count = 0;
		for(int scanY = minY; scanY < maxY; scanY++){
			for(int i = 0; i < length; i++){
				//room for imporvment here. could fins min and max x for the shape.
				Point scanPoint1 = {0,scanY};
				Point scanPoint2 = {width,scanY};
				Point edgePoint1 = {0,0};
				Point edgePoint2 = {0,0};
				if(i != length-1){
					edgePoint1.x = coords[i].x;
					edgePoint1.y = coords[i].y;
					edgePoint2.x = coords[i+1].x;
					edgePoint2.y = coords[i+1].y;
				}else{
					edgePoint1.x = coords[0].x;
					edgePoint1.y = coords[0].y;
					edgePoint2.x = coords[i].x;
					edgePoint2.y = coords[i].y;
				}
				Point lines[4] = {scanPoint1, scanPoint2, edgePoint1, edgePoint2};
				//gets intersection point for the scanline and the shapes edge
				Point intercept = GetIntersectionPoint(lines);
				
				if(intercept.x != -1){
					if(count < 200)
					intersection[count] = intercept;
					count++;
				}
			}
		}
		int count2 = 0;
		while(count2 < count){
			DrawLinePoints(intersection[count2], intersection[count2+1], col);
			count2 += 2;
		}
	}
	//draws the outline last so it is always on top, this is so if you fill it with a different colour you always get the outline 
	for(int i = 0; i < length; i++){
		if(i+1 < length){
			DrawLinePoints(coords[i], coords[i+1], col);
		}
		else{
			DrawLinePoints(coords[i], coords[0], col);
		}
	}	
	
	
}

//http://www.geeksforgeeks.org/program-for-point-of-intersection-of-two-lines/
//simple algorithm for finding intersection point of two lines.
Point GetIntersectionPoint(Point coords[]){
	
	int x1 = coords[0].x - coords[1].x; 
	int y1 = coords[1].y - coords[0].y; 
	int c1 = y1*(coords[0].x) + x1*(coords[0].y); 
	
	int x2 = coords[2].x - coords[3].x; 
	int y2 = coords[3].y - coords[2].y; 
	int c2 = y2*(coords[2].x) + x2*(coords[2].y); 
	
	int d = (y1*x2) - (y2*x1); 
	if(d == 0){
		//is there is not interection, set to -1
		Point p = {-1,-1};
		return p;
	}
	else{
		int x = (x2*c1 - x1*c2)/d;
		int y = (y1*c2 - y2*c1)/d;
		Point p = {x,y};
		return p;
	}
}

