#include "cpu.h"

int main() {
    CPU cpu("bootloader.bin");

    cpu.run();

    // cpu.dump_cpu();
    return 0;
}