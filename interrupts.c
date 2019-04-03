//
// Created by Kexin Li on 2019-04-02.
//
#include "address_map_arm.h"
#include "defines.h"
#include "interrupt_ID.h"




// configure general interrupt controller
void config_GIC(void);

// set up the KEY interrupts in the FPGA
void config_KEYs();

// Define the IRQ exception handler
void __attribute__((interrupt)) __cs3_isr_irq(void);


// interrupt service of push button
void pushbutton_ISR(void);

void set_A9_IRQ_stack(void);

void enable_A9_interrupts(void);


void configPS2(void);

void mouseISR(void);


// Define the remaining exception handlers
void __attribute__((interrupt)) __cs3_reset(void){
    while (1)
        ;
}
void __attribute__((interrupt)) __cs3_isr_undef(void){
    while (1)
        ;
}
void __attribute__((interrupt)) __cs3_isr_swi(void){
    while (1)
        ;
}
void __attribute__((interrupt)) __cs3_isr_pabort(void){
    while (1)
        ;
}
void __attribute__((interrupt)) __cs3_isr_dabort(void){
    while (1)
        ;
}
void __attribute__((interrupt)) __cs3_isr_fiq(void){
    while (1)
        ;
}

void config_GIC(void) {
    int address; // used to calculate register addresses
    /* configure the HPS timer interrupt */
    *((int *)0xFFFED8C4) = 0x01000000;
    *((int *)0xFFFED118) = 0x00000080;
    /* configure the FPGA interval timer and KEYs interrupts */
    *((int *)0xFFFED848) = 0x00000101;
    *((int *)0xFFFED108) = 0x00000300;
    // Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all // priorities
    address = MPCORE_GIC_CPUIF + ICCPMR;
    *((int *)address) = 0xFFFF;
    // Set CPU Interface Control Register (ICCICR). Enable signaling of // interrupts
    address = MPCORE_GIC_CPUIF + ICCICR;
    *((int *)address) = ENABLE;
    // Configure the Distributor Control Register (ICDDCR) to send pending // interrupts to CPUs
    address = MPCORE_GIC_DIST + ICDDCR;
    *((int *)address) = ENABLE;
}

void config_KEYs() {
    volatile int *KEY_ptr = (int *)KEY_BASE; // pushbutton KEY address
    *(KEY_ptr + 2) = 0b1111;                    // enable interrupts for KEY[1]
}

void pushbutton_ISR(void) {
    volatile int * KEY_ptr = (int *) 0xFF200050;
    int press;

    press = *(KEY_ptr + 3); // read the pushbutton interrupt register
    *(KEY_ptr + 3) = press;
    if (press & 0x1) { // KEY 0
        key_pressed = 0;
    } else if (press & 0x2) {// KEY 1
        key_pressed = 1;
    } else if (press & 0x4) {// KEY 2
        key_pressed = 2;
    } else {// KEY 3
        key_pressed = 3;
    }
    return;

}


void __attribute__((interrupt)) __cs3_isr_irq(void){
    // Read the ICCIAR from the processor interface
    int address = MPCORE_GIC_CPUIF + ICCIAR;
    int int_ID = *((int *)address);
    if (int_ID == KEYS_IRQ)
    { // check if interrupt is from the KEYs
        pushbutton_ISR();
    } else if (interrupt_ID == 79) {
        mouseISR();
    }
    else
    {
        while (1)
            ;
    } // if unexpected, then stay here
    // Write to the End of Interrupt Register (ICCEOIR)
    address = MPCORE_GIC_CPUIF + ICCEOIR;
    *((int *)address) = int_ID;
    return;
}

/*
 * Initialize the banked stack pointer register for IRQ mode
*/

void set_A9_IRQ_stack(void){
    int stack, mode;
    stack = A9_ONCHIP_END - 7; // top of A9 onchip memory, aligned to 8 bytes /* change processor to IRQ mode with interrupts disabled */
    mode = INT_DISABLE | IRQ_MODE;
    asm("msr cpsr, %[ps]"
    :
    : [ps] "r"(mode));
    /* set banked stack pointer */
    asm("mov sp, %[ps]"
    :
    : [ps] "r"(stack));
    /* go back to SVC mode before executing subroutine return! */
    mode = INT_DISABLE | SVC_MODE;
    asm("msr cpsr, %[ps]"
    :
    : [ps] "r"(mode));
}

/*
 * Turn on interrupts in the ARM processor
*/
void enable_A9_interrupts(void){
    int status = SVC_MODE | INT_ENABLE;
    asm("msr cpsr, %[ps]"
    :
    : [ps] "r"(status));
}


// Configure PS2
void configPS2(void) {
    volatile int * PS2_ptr = (int *)PS2_BASE; // PS/2 port address

    *(PS2_ptr) = 0xFF; /* reset */
    // *PS2_ptr = 0xE8;	// Set Resolution
    // *PS2_ptr = 0x00;	// Resolution
    // *PS2_ptr = 0xEA;

    *(PS2_ptr + 1) = 0x1; /* write to the PS/2 Control register to enable interrupts */

}










void mouseISR(void) {
    // Check that there are only 3 packets
    volatile int * PS2_ptr = (int *) 0xFF200100;
    int PS2_data, RAVAIL;
    PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
    RAVAIL = (PS2_data & 0xFFFF0000) >> 16;			// extract the RAVAIL field

    // byteNumber = (RAVAIL-1) % 3;
    // printf("Byte number %d\n", RAVAIL);
    if (*(PS2_ptr) == 0xFA){
        byteNumber = 0;
        return;
    }
    if (RAVAIL){
        if (byteNumber == 0) {
            byte1 = *(PS2_ptr) & 0xFF;
            if (byte1 & 0x01) {
                mouseClicked = 1;
            }
        }

        // If X movement
        if (byteNumber == 1) {
            byte2 = *(PS2_ptr) & 0xFF;
            // If no overflow
            if (!(byte1 & 0x40)) {
                if (byte1 & 0x10) {
                    // If negative, then subtract the magnitude of x movement
                    xPos -= (byte2 ^ 0xFF) + 1;
                    // if (byte2^0xFF > 4)
                    // xPos -= 2;
                }
                else {
//					// if (byte2 > 4)
//						// xPos += 2;
                    xPos += byte2;
                }
            }
        }

        // If Y movement
        if (byteNumber == 2) {
            byte3 = *(PS2_ptr) & 0xFF;
            // If no overflow
            if (!(byte1 & 0x80)) {
                if (byte1 & 0x20) {
                    // If negative, then subtract the magnitude of x movement
                    // Mouse movement up is positive, but on screen should be negative
                    // yPos += (unsigned)((byte2 ^ 0xFF) + 1);
                    // if ((byte3^0xFF) + 1 > 4)
                    // yPos += 2;
                    yPos += (unsigned)(byte3 ^ 0xFF) + 1;
                }
                else {
                    // yPos -= (unsigned)byte2;
                    // if (byte3 > 4)
                    // yPos -= 2;
                    yPos -= (unsigned) byte3;
                }
            }
        }
        if (xPos < 0) {
            xPos = 0;
        }
        else if (xPos >= 320) {
            xPos = 319;
        }

        if (yPos < 0) {
            yPos = 0;
        }
        else if (yPos >= 240) {
            yPos = 239;
        }

        byteNumber++;
        if (byteNumber == 3) {
            byteNumber = 0;
        }
    }

}

