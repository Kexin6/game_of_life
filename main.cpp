volatile int pixel_buffer_start; // global variable

void plot_pixel(int x, int y, short int line_color);
void clear_screen();
void draw_line(int xa, int xb, int ya, int yb, short int color);
void wait_for_vsync();
void plot_rect(int x, int y, short int color);


int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)

    int x_Xs[8], y_Xs[8], dx_Xs[8], dy_Xs[8], color_Xs[8];
    int color[10] = {0x001F, 0x07E0, 0x1111, 0xF800, 0xF81F, 0xFFFF, 0xFF11, 0xF700, 0xF911, 0xFFF0};
    // initialize location and direction of rectangles(not shown)
    int i = 0;
    for (; i < 8; i ++) {
        x_Xs[i] = rand() % 319;
        y_Xs[i] = rand() % 239;
        dx_Xs[i] = rand() % 2 * 2 -1;
        dy_Xs[i] = rand() % 2 * 2 -1;
        color_Xs[i] = color[rand() % 10];
    }
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

        clear_screen();

        int count = 0;
        // code for drawing the boxes and lines (not shown)
        for (; count < 8; count ++) {
            plot_rect(x_Xs[count], y_Xs[count], color_Xs[count]);
            draw_line(x_Xs[count], y_Xs[count], x_Xs[(count + 1) % 8], y_Xs[(count + 1) % 8], color_Xs[count]);

            // code for updating position
            x_Xs[count] += dx_Xs[count];
            y_Xs[count] += dy_Xs[count];
            // adjust for when object hits the edge
            if (x_Xs[count] == 1) {
                dx_Xs[count] = 1;
            }
            if (x_Xs[count] == 318) {
                dx_Xs[count] = -1;
            }
            if (y_Xs[count] == 1) {
                dy_Xs[count] = 1;
            }
            if (y_Xs[count] == 238) {
                dy_Xs[count] = -1;
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

void plot_rect(int x, int y, short int color) {
    plot_pixel(x + 1, y + 1, color);
    plot_pixel(x, y, color);
    plot_pixel(x - 1, y - 1, color);
    plot_pixel(x, y - 1, color);
    plot_pixel(x - 1, y, color);
    plot_pixel(x, y + 1, color);
    plot_pixel(x + 1, y,  color);
    plot_pixel(x - 1, y + 1, color);
    plot_pixel(x + 1, y - 1, color);
}
