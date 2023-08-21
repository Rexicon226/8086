//
// Created by david on 20/08/2023.
//

#include "cpu.h"

CPU::CPU(const std::string& filename) {
    // Read bootloader into memory
    FILE* f = fopen(filename.c_str(), "rb");
    if (!f) {
        printf("Could not open file %s\n", filename.c_str());
        exit(1);
    }

    // BIOS is stored at 0xFE000 - 0xFFFFF
    fread(memory + 0xFE000, 1, 0x2000, f);

    // Close file
    fclose(f);

    ip = 0xFE000;
    running = true;
}

uint16_t CPU::getEffectiveAddress(uint16_t segment, uint16_t offset) {
    return (segment << 4) + offset;
}

uint16_t CPU::fetchNextOpcode() {
    uint16_t opcode = memory[ip];
    ip++;
    return opcode;
}

uint16_t CPU::getRegisterValue(int regNumber) {
    switch (regNumber) {
        case 0x0:
            return ax;
        case 0x01:
            return cx;
        case 0x02:
            return dx;
        case 0x03:
            return bx;
        case 0x04:
            return sp;
        case 0x05:
            return bp;
        case 0x06:
            return si;
        case 0x07:
            return di;
        default:
            printf("Unknown register: %02X\n", regNumber);
            running = false;
            return 0;
    }
}

void CPU::setRegisterValue(int regNumber, uint16_t value) {
    switch (regNumber) {
        case 0x0:
            ax = value;
            break;
        case 0x01:
            cx = value;
            break;
        case 0x02:
            dx = value;
            break;
        case 0x03:
            bx = value;
            break;
        case 0x04:
            sp = value;
            break;
        case 0x05:
            bp = value;
            break;
        case 0x06:
            si = value;
            break;
        case 0x07:
            di = value;
            break;
        default:
            printf("Unknown register: %02X\n", regNumber);
            running = false;
    }
}

int16_t CPU::convert2SComplement(int8_t value) {
    int16_t convertedValue;
    convertedValue = value;
    if (value & 0x80) {
        convertedValue = ~convertedValue + 1;
        convertedValue = -convertedValue;
    }
    else {
        convertedValue = value;
    }

    return convertedValue;
}

void CPU::run() {
    do {
        uint16_t opcode = fetchNextOpcode();

        uint16_t reg;
        uint16_t value;

        switch (opcode) {
            case 0x01: // ADD r/m16, r16
                reg = fetchNextOpcode();

                // Convert into binary, for mod-reg-rm parsing
                char binary3[9];
                for (int i = 0; i < 8; ++i) {
                    binary3[7 - i] = (reg & (1 << i)) ? '1' : '0';
                }
                binary3[8] = '\0';

                // Parse mod-reg-rm
                int mod3;
                int regField3;
                int rmField3;

                mod3 = (binary3[0] - '0') * 2 + (binary3[1] - '0');
                regField3 = (binary3[2] - '0') * 4 + (binary3[3] - '0') * 2 + (binary3[4] - '0');
                rmField3 = (binary3[5] - '0') * 4 + (binary3[6] - '0') * 2 + (binary3[7] - '0');

                // Switch on mod
                switch (mod3) {
                    case 3:
                        // Register to register
                        uint16_t srcReg3;
                        uint16_t destReg3;
                        srcReg3 = getRegisterValue(regField3);
                        destReg3 = getRegisterValue(rmField3);
                        destReg3 += srcReg3;
                        setRegisterValue(rmField3, destReg3);

                        printf("ADD %02X, %02X\n", rmField3, regField3);

                        break;

                    default:
                        printf("Unsupported mod: %02X\n", mod3);
                        running = false;
                        break;
                }

                break;

            case 0x05: // ADD AX, imm16
                value = memory[ip] | (memory[ip + 1] << 8);
                ip += 2;
                ax += value;
                printf("ADD ax, %04X\n", value);
                break;

            case 0x2D: // SUB AX, imm16
                value = memory[ip] | (memory[ip + 1] << 8);
                ip += 2;
                ax -= value;
                printf("SUB ax, %04X\n", value);
                break;

            case 0x31: // XOR r/m16, rm/16
                reg = fetchNextOpcode();

                // Convert into binary, for mod-reg-rm parsing
                char binary[9];
                for (int i = 0; i < 8; ++i) {
                    binary[7 - i] = (reg & (1 << i)) ? '1' : '0';
                }
                binary[8] = '\0';

                // Parse mod-reg-rm
                int mod;
                int regField;
                int rmField;

                mod = (binary[0] - '0') * 2 + (binary[1] - '0');
                regField = (binary[2] - '0') * 4 + (binary[3] - '0') * 2 + (binary[4] - '0');
                rmField = (binary[5] - '0') * 4 + (binary[6] - '0') * 2 + (binary[7] - '0');

                // Switch on mod
                switch (mod) {
                    case 3:
                        // Register to register
                        uint16_t srcReg;
                        uint16_t destReg;
                        srcReg = getRegisterValue(regField);
                        destReg = getRegisterValue(rmField);
                        destReg ^= srcReg;
                        setRegisterValue(rmField, destReg);

                        printf("XOR %02X, %02X\n", rmField, regField);

                        break;

                    default:
                        printf("Unsupported mod: %02X\n", mod);
                        running = false;
                        break;
                }

                break;

            case 0x39: // CMP r/m16, r16
                reg = fetchNextOpcode();

                // Convert into binary, for mod-reg-rm parsing
                char binary2[9];
                for (int i = 0; i < 8; ++i) {
                    binary2[7 - i] = (reg & (1 << i)) ? '1' : '0';
                }
                binary2[8] = '\0';

                // Parse mod-reg-rm
                int mod2;
                int regField2;
                int rmField2;

                mod2 = (binary2[0] - '0') * 2 + (binary2[1] - '0');
                regField2 = (binary2[2] - '0') * 4 + (binary2[3] - '0') * 2 + (binary2[4] - '0');
                rmField2 = (binary2[5] - '0') * 4 + (binary2[6] - '0') * 2 + (binary2[7] - '0');

                // Switch on mod
                switch (mod2) {
                    case 3:
                        // Register to register
                        uint16_t srcReg2;
                        uint16_t destReg2;
                        srcReg2 = getRegisterValue(regField2);
                        destReg2 = getRegisterValue(rmField2);

                        uint16_t result;
                        result = destReg2 - srcReg2;

                        if (result == 0) {
                            flags |= ZERO_FLAG;
                        } else {
                            flags &= ~ZERO_FLAG;
                        }

                        if (result & 0x8000) {
                            flags |= SIGN_FLAG;
                        } else {
                            flags &= ~SIGN_FLAG;
                        }

                        if (srcReg2 > destReg2) {
                            flags |= CARRY_FLAG;
                        } else {
                            flags &= ~CARRY_FLAG;
                        }

                        flags &= ~OVERFLOW_FLAG; // Clear OF flag

                        printf("CMP %02X, %02X\n", rmField2, regField2);

                        break;

                    default:
                        printf("Unsupported mod: %02X\n", mod2);
                        running = false;
                        break;
                }

                break;

            case 0x40 ... 0x47: // INC r16
                reg = opcode & 0x7;
                value = getRegisterValue(reg);
                value++;
                setRegisterValue(reg, value);

                printf("INC %02X, %04X\n", reg, value);

                break;

            case 0x48 ... 0x4F: // DEC r16
                reg = opcode & 0x7;
                value = getRegisterValue(reg);
                value--;
                setRegisterValue(reg, value);

                printf("DEC %02X, %04X\n", reg, value);

                break;


            case 0x74: // JE
                value = memory[ip];
                ip++;
                int16_t convertedValue2;
                convertedValue2 = convert2SComplement(value);

                if (flags & ZERO_FLAG) {
                    printf("JE %d\n", convertedValue2);
                    ip += convertedValue2;
                }

                break;


            case 0x81: // ADD r/m16, imm16
                // Reg is 2 bytes
                reg = fetchNextOpcode();

                value = memory[ip] | (memory[ip + 1] << 8);
                ip += 2;

                switch (reg) {
                    // Addition
                    case 0xC0:
                        ax += value;
                        printf("ADD ax, %02X\n", value);
                        break;
                    case 0xC1:
                        cx += value;
                        printf("ADD cx, %02X\n", value);
                        break;
                    case 0xC2:
                        dx += value;
                        printf("ADD dx, %02X\n", value);
                        break;
                    case 0xC3:
                        bx += value;
                        printf("ADD bx, %02X\n", value);
                        break;
                    case 0xC4:
                        sp += value;
                        printf("ADD sp, %02X\n", value);
                        break;
                    case 0xC5:
                        bp += value;
                        printf("ADD bp, %02X\n", value);
                        break;
                    case 0xC6:
                        si += value;
                        printf("ADD si, %02X\n", value);
                        break;
                    case 0xC7:
                        di += value;
                        printf("ADD di, %02X\n", value);
                        break;

                        // Subtraction
                    case 0xE8:
                        ax -= value;
                        printf("SUB ax, %02X\n", value);
                        break;
                    case 0xE9:
                        cx -= value;
                        printf("SUB cx, %02X\n", value);
                        break;
                    case 0xEA:
                        dx -= value;
                        printf("SUB dx, %02X\n", value);
                        break;
                    case 0xEB:
                        bx -= value;
                        printf("SUB bx, %02X\n", value);
                        break;
                    case 0xEC:
                        sp -= value;
                        printf("SUB sp, %02X\n", value);
                        break;
                    case 0xED:
                        bp -= value;
                        printf("SUB bp, %02X\n", value);
                        break;
                    case 0xEE:
                        si -= value;
                        printf("SUB si, %02X\n", value);
                        break;
                    case 0xEF:
                        di -= value;
                        printf("SUB di, %02X\n", value);
                        break;

                    default:
                        printf("Unknown OP extension: %02X\n", reg);
                        running = false;
                        break;
                }

                break;

            case 0x83: // ADD r/m16, imm8
                reg = fetchNextOpcode();
                value = fetchNextOpcode();

                switch (reg) {
                    // Addition
                    case 0xC0:
                        ax += value;
                        printf("ADD ax, %02X\n", value);
                        break;
                    case 0xC1:
                        cx += value;
                        printf("ADD cx, %02X\n", value);
                        break;
                    case 0xC2:
                        dx += value;
                        printf("ADD dx, %02X\n", value);
                        break;
                    case 0xC3:
                        bx += value;
                        printf("ADD bx, %02X\n", value);
                        break;
                    case 0xC4:
                        sp += value;
                        printf("ADD sp, %02X\n", value);
                        break;
                    case 0xC5:
                        bp += value;
                        printf("ADD bp, %02X\n", value);
                        break;
                    case 0xC6:
                        si += value;
                        printf("ADD si, %02X\n", value);
                        break;
                    case 0xC7:
                        di += value;
                        printf("ADD di, %02X\n", value);
                        break;

                    // Subtraction
                    case 0xE8:
                        ax -= value;
                        printf("SUB ax, %02X\n", value);
                        break;
                    case 0xE9:
                        cx -= value;
                        printf("SUB cx, %02X\n", value);
                        break;
                    case 0xEA:
                        dx -= value;
                        printf("SUB dx, %02X\n", value);
                        break;
                    case 0xEB:
                        bx -= value;
                        printf("SUB bx, %02X\n", value);
                        break;
                    case 0xEC:
                        sp -= value;
                        printf("SUB sp, %02X\n", value);
                        break;
                    case 0xED:
                        bp -= value;
                        printf("SUB bp, %02X\n", value);
                        break;
                    case 0xEE:
                        si -= value;
                        printf("SUB si, %02X\n", value);
                        break;
                    case 0xEF:
                        di -= value;
                        printf("SUB di, %02X\n", value);
                        break;

                    default:
                        printf("Unknown OP extension: %02X\n", reg);
                        running = false;
                        break;
                }
                break;

            case 0x90: // NOP
                printf("NOP\n");
                break;

            case 0xB8 ... 0xBF: // MOV
                reg = opcode & 0x7;
                value = memory[ip] | (memory[ip + 1] << 8);
                ip += 2;

                setRegisterValue(reg, value);

                printf("MOV %02X, %04X\n", reg, value);

                break;

            case 0xEB: // JMP
                value = memory[ip];
                ip++;
                int16_t convertedValue;
                convertedValue = convert2SComplement(value);

                ip += convertedValue;
                printf("JMP %d\n", convertedValue);
                break;

            case 0xF4: // HLT
                printf("HLT\n");
                running = false;
                break;


            default:
                printf("Unknown opcode: %02X at %04X\n", opcode, ip);
                running = false;
                break;
        }
    } while (running);
}

void CPU::dump_cpu() {
    printf("ax: %04X\n", ax);
    printf("bx: %04X\n", bx);
    printf("cx: %04X\n", cx);
    printf("dx: %04X\n", dx);
    printf("si: %04X\n", si);
    printf("di: %04X\n", di);
    printf("bp: %04X\n", bp);
    printf("sp: %04X\n", sp);
    printf("flags: %04X\n", flags);
    printf("cs: %04X\n", cs);
    printf("ds: %04X\n", ds);
    printf("es: %04X\n", es);
    printf("ss: %04X\n", ss);
    printf("ip: %04X\n", ip);

    // Hex dump BIOS
    printf("BIOS:\n");
    for (int i = 0xFE000; i < 0xFFFFF; i += 16) {
        printf("%05X: ", i);
        for (int j = 0; j < 16; j++) {
            printf("%02X ", memory[i + j]);
        }
        printf("\n");
    }
}
