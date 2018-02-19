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
    0x001a1818, 0x191b1917, 0x1b1b1818, 0x1a1a191a, 0x1a1b1a18, 0x1d11193e, 0x42434945, 0x45464e00,
    0x001a1919, 0x1a1a1919, 0x1a1a1718, 0x1a1b1a19, 0x191b1a19, 0x1d0f244a, 0x43464943, 0x44475800,
    0x001a1a18, 0x191a1917, 0x191a1818, 0x1a1b1919, 0x191b1a18, 0x1c122648, 0x44454742, 0x434c6000,
    0x001b1918, 0x18191818, 0x1a1a1818, 0x191a1919, 0x191b1918, 0x1d131f45, 0x48444842, 0x44506100,
    0x001a1818, 0x181a191a, 0x191a1818, 0x191a1919, 0x191a1a17, 0x1c131f49, 0x49414841, 0x47526100,
    0x00191818, 0x181a1919, 0x18191818, 0x19191919, 0x191b1a16, 0x1e131a47, 0x483e4743, 0x474e5f00,
    0x00191818, 0x1a1a1918, 0x19191818, 0x191a1919, 0x1a1a1916, 0x1f111d4a, 0x49414a46, 0x48506400,
    0x00181819, 0x19181818, 0x18191918, 0x19191919, 0x19191a17, 0x1f101a46, 0x49434946, 0x444c6200,
    0x00181818, 0x18181818, 0x181a1818, 0x191a1919, 0x19191a17, 0x1e121b48, 0x4d424745, 0x414f6700,
    0x00181819, 0x19181818, 0x18191819, 0x18191919, 0x19191a17, 0x1d121947, 0x48404843, 0x41526400,
    0x00191919, 0x18181818, 0x181a1a18, 0x181a1919, 0x191a1a18, 0x1d141b44, 0x48434943, 0x44536300,
    0x00181818, 0x18181818, 0x181b1818, 0x181a1919, 0x191a1a17, 0x1d121742, 0x4a454740, 0x44526200,
    0x00181818, 0x18181818, 0x191a1818, 0x181a1919, 0x191b1a17, 0x1d141944, 0x4c424542, 0x4a556500,
    0x00181919, 0x18181819, 0x181a1817, 0x181a1919, 0x191a1a17, 0x1d141644, 0x4b404744, 0x47506400,
    0x00191918, 0x18181918, 0x181a1818, 0x181a1919, 0x191a1b17, 0x1d151844, 0x4a404741, 0x47546600,
    0x00181818, 0x181a1a17, 0x18191818, 0x181a1919, 0x1a1a1a16, 0x1d15153f, 0x4c404342, 0x48516300,
    0x00181818, 0x181a1817, 0x1a191818, 0x181a1919, 0x19191a17, 0x1e161444, 0x4f3e4947, 0x47556400,
    0x00181819, 0x18191918, 0x18191818, 0x181a1a19, 0x19191917, 0x1d141444, 0x4c3f4944, 0x45515f00,
    0x00181919, 0x18191a17, 0x181a1818, 0x181a1918, 0x19191a18, 0x1d161741, 0x4b414644, 0x47516a00,
    0x00181918, 0x18191817, 0x181a1819, 0x181a1919, 0x1a191a17, 0x1c16133c, 0x4e414545, 0x45546b00,
    0x00191a18, 0x18181918, 0x171b1a18, 0x181a1a19, 0x19191c17, 0x1d161342, 0x4f404544, 0x48556200,
    0x001a1918, 0x18191a17, 0x181c1817, 0x181a1918, 0x191a1c16, 0x1c151442, 0x4c404645, 0x47515c00,
    0x001a1819, 0x191a1917, 0x191a1718, 0x181a1919, 0x1a1b1a16, 0x1c161541, 0x4f3f4547, 0x46525b00,
    0x00181919, 0x18191918, 0x18191818, 0x181a1919, 0x1a191a17, 0x1c181441, 0x4d3c4946, 0x44525700,
    0x00181a19, 0x18191918, 0x181a1a18, 0x191a1919, 0x19191c17, 0x1c161140, 0x4a3e4c43, 0x49525600,
    0x00191818, 0x19191819, 0x1a1a1818, 0x19191919, 0x1a1a1b17, 0x1d151341, 0x4b444a42, 0x4c4c5800,
    0x00181919, 0x19191a1a, 0x18181819, 0x1819191a, 0x1a191b18, 0x1b17153d, 0x4b444642, 0x494d6100,
    0x00181919, 0x19191918, 0x171a1a18, 0x18191a1a, 0x19181b18, 0x1b19123a, 0x4d434445, 0x49526500,
    0x00191a19, 0x1a191918, 0x181a1918, 0x181a1919, 0x19191c18, 0x1c19103b, 0x52424448, 0x48526600,
    0x001a1b18, 0x18181818, 0x181a1a17, 0x181a1919, 0x191a1c17, 0x1c17103f, 0x513f4746, 0x45556700,
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
    printf("Read %lu bytes from file\n", fread(rom, 1, ROMSIZE, fin));
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

    unsigned int rom_addr = 0;
    unsigned int rom_r = 0;

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

    unsigned int ram_addr = 0;
    unsigned int ram_lblw = 0;
    unsigned int ram_r = 0;
    unsigned int ram_w = 0;
    unsigned int ram_byteselect_before = 0;
    unsigned int ram_byteselect = 0;
    unsigned int ram_din = 0;
    unsigned int ram_we = 0;

    printf("Running mMIPS\n");
    unsigned int clock = 0;
    while (mmips->mMIPS__DOT__bus_pc != 0x44) {
        printf("\r>> %9d cycles", ++clock);

        /* Rising edge */
        rom_addr = mmips->rom_addr;
        rom_r = mmips->rom_r;

        ram_addr = mmips->ram_addr & 0x3fffff;
        if (ram_addr < RAMSIZE) {
            ram_byteselect_before = ram_addr & 0b11;
        }
        ram_r = mmips->ram_r;
        ram_w = mmips->ram_w;
        ram_byteselect = ram_byteselect_before;
        if (ram_r == 0b10) {
            ram_lblw = 1;
        } else {
            ram_lblw = 0;
        }

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
        if (rom_r) {
            assert(rom_addr < ROMSIZE);
            mmips->rom_dout = bswap(rom[rom_addr >> 2]);
        }

        if (ram_addr < RAMSIZE) {
            ram[ram_addr >> 2] = (ram[ram_addr >> 2] & ram_we) | (ram_din & ~ram_we);
        }

        mmips->clk = 0;
        mmips->eval();
#ifdef TRACE
        tfp->dump(clock*100 + 100);
#endif
    }

    printf("\nFinished\n");

    for (int i = 0; i < 256; i++) {
        unsigned int value = ram[(0x2000 >> 2) + i];
        unsigned int ref_value = ref_out[i];
        for (int j = 0; j < 4; j++) {
            unsigned char b = (value >> ((3-j)*8)) & 0xff;
            unsigned char r = (ref_value >> ((3-j)*8)) & 0xff;
            if (b == r) {
                printf("\x1b[48;2;%1$u;%1$u;%1$u;38;2;%1$u;%1$u;%1$um%1$.2x", b);
            } else {
                printf("\x1b[48;2;%1$u;%1$u;%1$u;38;2;%2$u;%3$u;%3$um%1$.2x", b, 255, 0);
            }
        }
        if (i % 8 == 7) printf("\x1b[0m\n");
    }
    bool identical = true;
    for (int i = 0; i < 256; i++) {
        unsigned int value = ram[(0x2000 >> 2) + i];
        unsigned int ref_value = ref_out[i];
        if (value != ref_value) {
            printf("Output mismatch at %.6x, expected %.8x got %.8x\n", 0x402000 + i, ref_value, value);
            identical = false;
        }
    }
    if (identical) {
        printf("Output is identical to reference\n");
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
