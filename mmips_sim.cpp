#include <cstdio>
#include <cstdlib>

#include "Vmmips.h"
#include "verilated.h"
#ifdef TRACE
#include "verilated_vcd_c.h"
#endif

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

unsigned int ref_out[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00161716, 0x17161716, 0x18161715, 0x18171817, 0x19171800, 0x3e3d3e39, 0x443f4127, 0x65616300,
    0x00171817, 0x17161716, 0x17161716, 0x19171817, 0x18171803, 0x4e373b34, 0x433d4223, 0x775f6600,
    0x00161716, 0x17161716, 0x17161716, 0x19171817, 0x18171802, 0x4d373b34, 0x423a3f21, 0x7e636b00,
    0x00161716, 0x17161717, 0x17161716, 0x19171817, 0x17161701, 0x4d373b35, 0x433c4124, 0x82636b00,
    0x00161716, 0x18171817, 0x17161716, 0x19171818, 0x18171803, 0x4e383c34, 0x423b4023, 0x80626a00,
    0x00161716, 0x18161716, 0x17161716, 0x19171818, 0x16151600, 0x4b353931, 0x433c4123, 0x7f626a00,
    0x00161716, 0x17161716, 0x17161716, 0x19171817, 0x16161703, 0x4f393d35, 0x463e4325, 0x83666e00,
    0x00161716, 0x17161716, 0x17161716, 0x19171817, 0x17161701, 0x4b343831, 0x443c4122, 0x82646c00,
    0x00161716, 0x17161716, 0x17161716, 0x19171817, 0x17161702, 0x4d393c33, 0x443c4124, 0x83656d00,
    0x00161717, 0x17161716, 0x17161716, 0x19171817, 0x17161703, 0x4b343830, 0x443d4224, 0x80626a00,
    0x00171817, 0x17161716, 0x17161716, 0x19171817, 0x18171805, 0x4935392f, 0x443b4021, 0x80646c00,
    0x00161716, 0x17161716, 0x17161716, 0x19171818, 0x17161702, 0x4632362f, 0x443b4020, 0x82646c00,
    0x00161716, 0x17161717, 0x17161715, 0x18171817, 0x17161704, 0x4a363a2f, 0x443b4022, 0x85666e00,
    0x00161716, 0x17161716, 0x16151614, 0x19171817, 0x17161703, 0x4732362e, 0x473e4324, 0x84656d00,
    0x00161717, 0x17161716, 0x17161716, 0x19171817, 0x17161704, 0x4935392f, 0x443a3f20, 0x82656d00,
    0x00161716, 0x16151615, 0x17161716, 0x19171817, 0x17161703, 0x4632352b, 0x433b4023, 0x84646c00,
    0x00161716, 0x17161716, 0x18161716, 0x19171817, 0x17161703, 0x4835392f, 0x483e4325, 0x82646c00,
    0x00161717, 0x18161716, 0x17161716, 0x19171816, 0x16161705, 0x4934382d, 0x453c4121, 0x81656d00,
    0x00171817, 0x16151615, 0x17161716, 0x18161716, 0x18171807, 0x4632362a, 0x463d4227, 0x87687100,
    0x00161716, 0x16161716, 0x18161715, 0x18171818, 0x18161703, 0x4230342b, 0x483d4229, 0x7f5f6800,
    0x00161717, 0x17161716, 0x17161716, 0x19171817, 0x17161704, 0x4634382e, 0x473b4029, 0x765b6300,
    0x00161716, 0x16151615, 0x16151615, 0x18161717, 0x17161703, 0x4632362c, 0x453c412b, 0x72596000,
    0x00161716, 0x17161716, 0x17161716, 0x19171817, 0x16151604, 0x4633372a, 0x463c402d, 0x6f575e00,
    0x00171817, 0x17161716, 0x17161716, 0x19171817, 0x17171806, 0x4632362b, 0x4a40452c, 0x6b565d00,
    0x00161715, 0x17161717, 0x17161716, 0x19171818, 0x17161704, 0x4331352b, 0x473b4024, 0x6f5c6300,
    0x00161716, 0x18171818, 0x17161715, 0x18171817, 0x17161705, 0x4633372c, 0x473d4225, 0x785f6600,
    0x00171817, 0x19171817, 0x17161715, 0x1a181918, 0x18171807, 0x45313528, 0x473d4225, 0x7d636a00,
    0x00161716, 0x17161716, 0x17161716, 0x19171817, 0x17161707, 0x412f3326, 0x483d4224, 0x7e626a00,
    0x00171817, 0x17161717, 0x17161715, 0x18171817, 0x18171806, 0x3f2e3226, 0x4a3e4322, 0x82666e00,
    0x00161715, 0x17161717, 0x16151614, 0x19171818, 0x17161704, 0x4231352b, 0x4c3e431f, 0x86697100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
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
        printf("Unable to read 'mips_mem.bin'\n");
        free(rom);
        return 1;
    }
    printf("Read %lu bytes from file\n", fread(rom, 4, ROMSIZE/4, fin));
    fclose(fin);

    ram = (unsigned int *) calloc(RAMSIZE, 1);
    for (int i = 0; i < 256; i++) {
        ram[(0x1000 >> 2) + i] = img[i];
    }
    ram[2] = ram[3] = ram[4] = ram[5] = 0x00400000;

    Vmmips* mmips = new Vmmips;

#ifdef TRACE
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    mmips->trace(tfp, 99);
    tfp->open("../mmips_trace.vcd");
#endif

    printf("Enable mMIPS\n");
    mmips->enable = 1;
    mmips->eval();

    int rom_addr = 0x00000000;
    int rom_r = 0;

    printf("Reset mMIPS\n");
    mmips->rst = 1;
    mmips->clk = 0;
    mmips->eval();
#ifdef TRACE
    tfp->dump(0);
#endif

    rom_addr = mmips->rom_addr;
    rom_r = mmips->rom_r;
    mmips->clk = 1;
    mmips->eval();
#ifdef TRACE
    tfp->dump(50);
#endif

    mmips->rst = 0;
    mmips->clk = 0;
    if (rom_r) mmips->rom_dout = bswap(rom[rom_addr >> 2]);
    mmips->eval();
#ifdef TRACE
    tfp->dump(100);
#endif

    int ram_addr = 0;
    int ram_lblw = 0;
    int ram_r = 0;
    int ram_w = 0;
    int ram_byteselect_before = 0;
    int ram_byteselect = 0;
    int ram_din = 0;
    int ram_we = 0;

    printf("Running mMIPS\n");
    int clock = 0;
    while (mmips->mMIPS__DOT__bus_pc != 0x44) {
        printf("\r>> %9d cycles", ++clock);

        /* Rising edge */
        rom_addr = mmips->rom_addr;
        rom_r = mmips->rom_r;

        ram_addr = mmips->ram_addr & 0x3fffff;
        if (ram_r == 0b10) {
            ram_lblw = 1;
        } else {
            ram_lblw = 0;
        }
        ram_r = mmips->ram_r;
        ram_w = mmips->ram_w;
        ram_byteselect = ram_byteselect_before;

        if (ram_addr < RAMSIZE) {
            ram_byteselect_before = ram_addr & 0b11;
            ram_din = mmips->ram_din;
            if (ram_w == 0b01) {
                ram_we = 0x00000000;
            } else if (ram_w == 0b10) {
                unsigned int ram_we_table[4] = {0x00ffffff, 0xff00ffff, 0xffff00ff, 0xffffff00};
                ram_we = ram_we_table[ram_byteselect_before];
                ram_din = (ram_din & 0xff) << ((3 - ram_byteselect_before) * 8);
            } else {
                ram_we = 0xffffffff;
            }
        }

        if ((ram_r || ram_w) && ram_addr < RAMSIZE) {
            unsigned int value = ram[ram_addr >> 2];
            if (ram_lblw) {
                mmips->ram_dout = (value >> ((3 - ram_byteselect) * 8)) & 0xff;
            } else {
                mmips->ram_dout = value;
            }
        } else {
            mmips->ram_dout = 0;
        }

        mmips->clk = 1;
        mmips->eval();
#ifdef TRACE
        tfp->dump(clock*100 + 50);
#endif

        /* Falling edge */
        mmips->clk = 0;
        if (rom_r) {
            assert(rom_addr < ROMSIZE);
            mmips->rom_dout = bswap(rom[rom_addr >> 2]);
        }

        if (ram_addr < RAMSIZE) {
            ram[ram_addr >> 2] = (ram[ram_addr >> 2] & ram_we) | (ram_din & ~ram_we);
        }

        mmips->eval();
#ifdef TRACE
        tfp->dump(clock*100 + 100);
#endif
    }

    printf("\nFinished\n");

    for (int i = 0; i < 256; i++) {
        unsigned int value = ram[(0x2000 >> 2) + i];
        for (int j = 0; j < 4; j++) {
            unsigned char b = (value >> ((3-j)*8)) & 0xff;
            char color[64] = {0};
            snprintf(color, 64, "\x1b[48;2;%hhu;%hhu;%hhum\x1b[38;2;%hhu;%hhu;%hhum", b, b, b, b, b, b);
            printf("%s%.2x", color, b);
        }
        if (i % 8 == 7) printf("\x1b[0m\n");
    }
    for (int i = 0; i < 256; i++) {
        unsigned int value = ram[(0x2000 >> 2) + i];
        unsigned int ref_value = ref_out[i];
        if (value != ref_value) {
            printf("Output mismatch at %.6x, expected %.8x got %.8x\n", 0x402000 + i, ref_value, value);
        }
    }

#ifdef TRACE
    tfp->close();
    delete tfp;
#endif
    delete mmips;
    free(rom);
    free(ram);
    return 0;
}
