//
// Created by Kexin Li on 2019-04-07.
//

unsigned char byte1 = 0;
unsigned char byte2 = 0;
unsigned char byte3 = 0;

volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port address
int wait_for_key();
int check_correct_key();
int wait_for_key() {
    int PS2_data, RVALID, current_key;

    while (1) {
        PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
        RVALID = (PS2_data & 0x8000);	// extract the RVALID field
        if (RVALID != 0)
        {
/* always save the last three bytes received */
            byte1 = byte2;
            byte2 = byte3;
            byte3 = PS2_data & 0xFF;
        }
        current_key = (byte3 << 16) | ((byte2 < 8)) | byte3;
        return current_key;
    }
}

int check_correct_key() {
    while (1) {
        int key = wait_for_key();
        if (/*key == 1*/ ) {
            //...
            break;
        } else if (/*key == 2*/) {
            //...
            break;
        } else if (/*key == 3*/) {
            //...
            break;
        } else if (/*key == 4*/) {
            //...
            break;
        } else { // the key is invalid
            key = wait_for_key(); // repeat again
            continue;
        }
    }
}

