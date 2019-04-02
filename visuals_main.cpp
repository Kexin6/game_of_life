#include<time.h>
#define false 0
#define true 1
typedef int boolean; // or #define bool int
volatile int pixel_buffer_start; // global variable

void plot_pixel(int x, int y, short int line_color);
void clear_screen();
void draw_line(int xa, int xb, int ya, int yb, short int color);
void wait_for_vsync();
void plot_rect(int x, int y, short int color);
void initialize_map (boolean iMap[320][240]);
boolean check_survival(boolean map[320][240], unsigned i, unsigned j, unsigned survial);

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	srand(time(0)); 
    // initial set up e.g. initialization and etc.
    int color[10] = {0x001F, 0x07E0, 0x1111, 0xF800, 0xF81F, 0xFFFF, 0xFF11, 0xF700, 0xF911, 0xFFF0};
    int color_chosen[8];
    int color_cont = 0;
    for (; color_cont < 8; color_cont ++) {
        color_chosen[color_cont] = color[rand() % 10];
    }
    //seed random
    boolean iMap[320][240];
    boolean nMap[320][240];
	initialize_map (iMap);
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
    // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer

    while (1)
    {
        /* Erase any boxes and lines that were drawn in the last iteration */
		//clear_screen();
        //draw using iMap
        int i, j;
        for (i = 0; i < 320; i++){
        	for (j = 0; j < 240; j++){
                if (iMap[i][j] == 2) {
                    plot_pixel(i, j, 0xF800);
                }
                else if (iMap[i][j] == 1) { 
                    plot_pixel(i, j, 0xF700);
                }
                else {
                    plot_pixel(i, j, 0x0000);
                }
                nMap[i][j] = check_survival(iMap, i, j, 4);
            }
        }
        //update the iMap
        for (i = 0; i < 320; i++){
        	for (j = 0; j < 240; j++){
                iMap[i][j] = nMap[i][j];
            }
        }
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

// code for subroutines (not shown)

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// function to blank the VGA screen.
void clear_screen() {
    int y, x;
    for (x = 0; x < 320; x++) {
        for (y = 0; y < 240; y++) {
            plot_pixel(x, y, 0x0000);
        }
    }
}

// function to draw a line
void draw_line(int xa, int ya, int xb, int yb, short int color){
    int is_steep;

    if(abs(yb - ya) > abs(xb - xa)){
        is_steep = 1;
    } else {
        is_steep = 0;
    }

    if(is_steep){
        int tmp = xa;
        xa = ya;
        ya = tmp;

        tmp = xb;
        xb = yb;
        yb = tmp;
    }

    if(xa > xb){
        int tmp = xa;
        xa = xb;
        xb = tmp;

        tmp = ya;
        ya = yb;
        yb = tmp;
    }


    int deltax = xb - xa;
    int deltay = abs(yb - ya);
    int error = - (deltax / 2);

    int y = ya;
    int y_step;
    if(ya < yb){
        y_step = 1;
    } else {
        y_step = -1;
    }

    int x = xa;
    for(; x < xb; x++){
        if(is_steep){
            plot_pixel(y, x, color);
        }else{
            plot_pixel(x, y, color);
        }
        error += deltay;
        if(error >= 0){
            y += y_step;
            error -= deltax;
        }
    }

}

void wait_for_vsync() {
    volatile int* pixel_ctrl_ptr = 0xFF203020; // pixel controller
    register int status;

    *pixel_ctrl_ptr = 1;
    status = *(pixel_ctrl_ptr + 3);
    while((status & 0x01) != 0) {
        status = *(pixel_ctrl_ptr + 3);
    }
}

void plot_rect(int x, int y, short int color) {//draw a 10x10 rect
    int i, j;
    for (i = 0; i < 10; i++){
        for (j = 0; j < 10; j++){
            plot_pixel(i, j, color);
        }
    }
}

void initialize_map (boolean iMap[320][240]){
    int i, j;
    for (i = 0; i < 320; i++){
        for (j = 0; j < 240; j++){
            if (i > 100 && i <220){
                if (j >60 && j < 180){
         			iMap[i][j] = rand() % 2; //should change to srand() later
                }
                else {
                	iMap[i][j] = 0;
                }
            }
            else iMap[i][j] = 0;
        }
    }
}

boolean check_survival(boolean map[320][240], unsigned i, unsigned j, unsigned survial){
	unsigned count = 0;
    //check if is boundary
    /* case 1: corner */
    if (i == 0 && j == 0){
    	if (map[0][1]) count++;
        if (map[1][0]) count++;
        if (map[1][1]) count++;
    }
    else if (i == 0 && j == 239){ //left bottom
        if (map[0][238]) count++;
        if (map[1][239]) count++;
    	if (map[1][238]) count++;
    }
    else if (i == 319 && j == 0){
    	if (map[318][0]) count++;
        if (map[319][1]) count++;
        if (map[318][1]) count++;
    }
    else if (i == 319 && j == 239){
    	if (map[319][238]) count++;
        if (map[318][239]) count++;
        if (map[318][238]) count++;
    }
    /* case 2: edge */
    else if (i == 0){
    	if (map[0][j-1]) count++;
        if (map[0][j+1]) count++;
        if (map[1][j-1]) count++;
        if (map[1][j]) count++;
        if (map[1][j+1]) count++;
    }
    else if (i == 319){
        if (map[319][j-1]) count++;
        if (map[319][j+1]) count++;
        if (map[318][j-1]) count++;
        if (map[318][j]) count++;
        if (map[318][j+1]) count++;
    }
    else if (j == 0){
        if (map[i-1][0]) count++;
        if (map[i+1][0]) count++;
        if (map[i-1][1]) count++;
        if (map[i][1]) count++;
        if (map[i+1][1]) count++;
    }
    else if (j == 239){
        if (map[i-1][239]) count++;
        if (map[i+1][239]) count++;
        if (map[i-1][238]) count++;
        if (map[i][238]) count++;
        if (map[i+1][238]) count++;
    }    
    /* case 3: general */
    else{
    	if (map[i-1][j-1]) count++;
        if (map[i-1][j]) count++;
        if (map[i][j-1]) count++;
        if (map[i+1][j+1]) count++;
        if (map[i+1][j]) count++;
        if (map[i][j+1]) count++;
        if (map[i+1][j-1]) count++;
        if (map[i-1][j+1]) count++;
    }
    //sustain
    if(map[i][j] == 1){
        if (count ==2 || count == 3) return 2;//substain
    }
    else if (count ==3){
        return 1;//birth
    }
    return 0;
}
