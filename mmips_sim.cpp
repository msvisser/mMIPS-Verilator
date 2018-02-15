#include <cstdio>
#include <cstdlib>

#include "Vmmips.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define ROMSIZE (2048*64)
#define RAMSIZE (2048*64)

#define bswap(num) (((num) & 0xff000000) >> 24) | (((num) & 0x00ff0000) >> 8) | (((num) & 0x0000ff00) << 8) | ((num) << 24)

unsigned int *rom;
unsigned int *ram;

unsigned int img[] = {
    0x10101010, 0x1010100f, 0x10101010, 0x10100f0f, 0x10101010, 0x10101010, 0x10101011, 0x1010100f,
    0x10110f0f, 0x10110f0f, 0x11110f0f, 0x11111010, 0x11121010, 0x12102039, 0x3a3b3f3c, 0x40465161,
    0x0f101010, 0x1010100f, 0x10100f0f, 0x10111110, 0x10111110, 0x120f1d36, 0x3b3c3d3d, 0x3e445365,
    0x0f10100f, 0x1010100f, 0x0f100f0f, 0x10111010, 0x10111010, 0x11102236, 0x393c3c3a, 0x3e48586a,
    0x0f10100f, 0x0f100f0f, 0x10100f0f, 0x0f101010, 0x1011100f, 0x120f1b36, 0x3b3b3e3c, 0x3e48586a,
    0x10100f0f, 0x0f101010, 0x10100f0f, 0x10101010, 0x10101010, 0x110f1c37, 0x3c393d3b, 0x404a5869,
    0x0f100f0f, 0x0f100f0f, 0x0f100f0f, 0x0f101010, 0x1011100e, 0x12101834, 0x3b383c3c, 0x40485769,
    0x0f0f0f0f, 0x1010100f, 0x0f0f0f0f, 0x10101010, 0x1010100f, 0x120e1c38, 0x3d3a3e3e, 0x414b5c6d,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f101010, 0x1010100f, 0x120e1833, 0x3c3c3d3c, 0x3f48586b,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f100f0f, 0x10101010, 0x1010100f, 0x120e1a37, 0x3d3a3e3c, 0x3c495d6c,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f101010, 0x1010110f, 0x110f1633, 0x3d393c3d, 0x3d495969,
    0x10101010, 0x0f0f0f0f, 0x0f10100f, 0x0f101010, 0x10101010, 0x120f1a33, 0x3a3c3d3b, 0x404b5b6b,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f100f0f, 0x0f101010, 0x1010100f, 0x110f1631, 0x3d3b3d3b, 0x3e4a596b,
    0x0f0f0f0f, 0x0f0f0f0f, 0x10100f0f, 0x0f101010, 0x1011100f, 0x120e1934, 0x3b3b3c3b, 0x434e5d6d,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f100f0e, 0x0f101010, 0x1010100f, 0x11101431, 0x3e383c3e, 0x3f495b6c,
    0x1010100f, 0x0f0f0f0f, 0x0f100f0f, 0x0f101010, 0x1011110f, 0x120f1933, 0x393a3c3a, 0x424c5c6c,
    0x0f0f0f0f, 0x0f10100e, 0x0f0f0f0f, 0x0f101010, 0x1010100f, 0x11101430, 0x3d373a3b, 0x404b596b,
    0x0f0f0f0f, 0x0f100f0f, 0x10100f0f, 0x0f101010, 0x1010100f, 0x120f1733, 0x3e3a3d3e, 0x414c5d6b,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f101010, 0x1010100f, 0x110f1332, 0x3d393e3c, 0x3f4b576c,
    0x0f0f1010, 0x0f10100e, 0x0f100f0f, 0x0f10100f, 0x10101010, 0x12101730, 0x3b393c3d, 0x414b5f6f,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f100f0f, 0x0f101010, 0x1010100f, 0x110f142e, 0x3d393a3d, 0x3f4b5c66,
    0x0f0f100f, 0x0f0f0f0f, 0x0f10100f, 0x0f101010, 0x1010110f, 0x12101432, 0x3e393b3b, 0x404a5661,
    0x0f100f0f, 0x0f10100e, 0x0f110f0e, 0x0f10100f, 0x1011110f, 0x110f1530, 0x3c3b3b3c, 0x4046525e,
    0x0f10100f, 0x0f10100f, 0x0f100f0f, 0x0f101010, 0x1011110e, 0x11101531, 0x3d393e3c, 0x3f4a505c,
    0x0f0f0f10, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f101010, 0x10101010, 0x11101530, 0x3d373c40, 0x3e47525b,
    0x0f0f100f, 0x0f10100f, 0x0f10100f, 0x10101010, 0x1010110f, 0x1110132f, 0x3b393e3b, 0x424a5161,
    0x100f0f0f, 0x0f0f0f10, 0x10100f0f, 0x0f101010, 0x1010110f, 0x110f1531, 0x3c3c3f3d, 0x43475465,
    0x100f0f10, 0x100f1010, 0x0f0f0f0f, 0x0f0f1011, 0x11101010, 0x110f142f, 0x3c3a3c3d, 0x424a5869,
    0x0f10100f, 0x10100f0f, 0x0f10100f, 0x0f101010, 0x1010110f, 0x1111122d, 0x3d3b3b3d, 0x434d5c69,
    0x0f0f1010, 0x1010100f, 0x0f100f0f, 0x0f101010, 0x10101110, 0x1111132c, 0x3e3b3c3e, 0x424e5e6d,
    0x0f10100f, 0x0f0f0f0f, 0x0f10100e, 0x0f101010, 0x1011110f, 0x1110132f, 0x3e393c3e, 0x404e6171,
    0x1010100f, 0x0f0f0f10, 0x0f10100f, 0x0f101010, 0x1011110f, 0x110f142f, 0x3b3a3d3b, 0x424e5e74,
};

const char *reg_names[] = {
    "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", "s8", "s9", "gp", "sp", "fp", "ra"
};

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    rom = (unsigned int *) malloc(ROMSIZE);
    FILE *fin = fopen("../mips_mem.bin", "rb");
    if (fin == NULL) {
        free(rom);
        return 1;
    }
    printf("Read %lu bytes from file\n", fread(rom, 4, ROMSIZE/4, fin));
    ram = (unsigned int *) calloc(RAMSIZE, 1);
    for (int i = 0; i < 256; i++) {
        ram[(0x1000 >> 2) + i] = img[i];
    }
    ram[2] = ram[3] = ram[4] = ram[5] = 0x00400000;

    Vmmips* mmips = new Vmmips;
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    mmips->trace(tfp, 99);
    tfp->open("simx.vcd");

    printf("Enable mMIPS\n");
    mmips->enable = 1;
    mmips->eval();

    int rom_addr = 0x00000000;
    int rom_r = 0;
    int ram_addr = 0x00000000;
    int ram_din = 0x00000000;
    int ram_r = 0;
    int ram_w = 0;

    printf("Reset mMIPS\n");
    mmips->rst = 1;
    mmips->clk = 0;
    mmips->eval();
    rom_addr = mmips->rom_addr;
    rom_r = mmips->rom_r;
    mmips->clk = 1;
    mmips->eval();
    tfp->dump(0);
    mmips->rst = 0;
    mmips->clk = 0;
    if (rom_r) {
        printf("rom read at %.8x\n", rom_addr);
        mmips->rom_dout = bswap(rom[rom_addr >> 2]);
    }
    mmips->eval();
    tfp->dump(50);

    int clock = 0;
    while (clock < 80 || mmips->mMIPS__DOT__bus_pc != 0x44) {
        // if (false && mmips->mMIPS__DOT__bus_pc == 0x364) {
        //     printf("\n---- BREAKPOINT ----\n");
        //     for (int r = 0; r < 4; r++) {
        //         for (int c = 0; c < 8; c++) {
        //             printf("%8s ", reg_names[r*8+c]);
        //         }
        //         printf("\n");
        //         for (int c = 0; c < 8; c++) {
        //             printf("%.8x ", mmips->mMIPS__DOT__registers__DOT__regs[r*8+c]);
        //         }
        //         printf("\n");
        //     }
        //     printf("--------------------\n");

        //     getchar();
        // }

        printf(">> %d: %s\n", ++clock, mmips->mMIPS__DOT__bus_hazard_hazard ? "hazard" : "");

        /* Rising edge */
        rom_addr = mmips->rom_addr;
        rom_r = mmips->rom_r;

        ram_addr = mmips->ram_addr;
        ram_r = mmips->ram_r;
        ram_w = mmips->ram_w;
        ram_din = mmips->ram_din;

        mmips->clk = 1;
        mmips->eval();
        tfp->dump(clock*100);

        /* Falling edge */
        mmips->clk = 0;
        if (rom_r) {
            printf("rom read at %.8x\n", rom_addr);
            assert(rom_addr < ROMSIZE);
            mmips->rom_dout = bswap(rom[rom_addr >> 2]);
        }
        unsigned int ram_offset = ram_addr & 0x3fffff;
        if (ram_offset < RAMSIZE) {
            if (ram_r == 1) {
                mmips->ram_dout = ram[ram_offset >> 2];
                printf("ram read word %.8x at %.8x\n", mmips->ram_dout, ram_addr);
            } else if (ram_r == 2) {
                mmips->ram_dout = ram[ram_offset >> 2];
                // mmips->ram_dout = *(((unsigned char *) ram) + ram_offset);
                printf("ram read byte %.2x at %.8x\n", mmips->ram_dout, ram_addr);
            }

            if (ram_w == 1) {
                printf("ram write word %.8x at %.8x\n", ram_din, ram_addr);
                ram[ram_offset >> 2] = ram_din;
            } else if (ram_w == 2) {
                printf("ram write byte %.2hhx at %.8x\n", (unsigned char) ram_din, ram_addr);
                *(((unsigned char *) ram) + ram_offset) = ram_din;
            }
        } else if (ram_r) {
            printf(">>>> read at invalid addr: %.8x\n", ram_addr);
        } else if (ram_w) {
            printf(">>>> write at invalid addr: %.8x\n", ram_addr);
        }

        mmips->eval();
        tfp->dump(clock*100 + 50);
    }

    printf("Finished\n");

    for (int i = 0; i < 256; i++) {
        unsigned int value = ram[(0x2000 >> 2) + i];
        for (int j = 0; j < 4; j++) {
            unsigned char b = (value >> (j*8)) & 0xff;
            char color[64] = {0};
            snprintf(color, 64, "\x1b[48;2;%hhu;%hhu;%hhum\x1b[38;2;%hhu;%hhu;%hhum", b, b, b, b, b, b);
            printf("%s%.2x", color, b);
        }
        if (i % 8 == 7) printf("\x1b[0m\n");
    }
    printf("Clean\n");

    tfp->close();
    delete mmips;
    free(rom);
    free(ram);
    return 0;
}
