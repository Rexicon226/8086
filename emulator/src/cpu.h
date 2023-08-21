//
// Created by david on 20/08/2023.
//

#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

#include <iostream>

// Configs
#define MEMORY_SIZE (1024 * 1024)
#define MAX_MEMORY (MEMORY_SIZE - 1)

// Flags
#define ZERO_FLAG (1 << 0)
#define CARRY_FLAG (1 << 1)
#define SIGN_FLAG (1 << 2)
#define OVERFLOW_FLAG (1 << 3)

class CPU {
    // Accumulators
    uint16_t ax{};
    uint16_t bx{};
    uint16_t cx{};
    uint16_t dx{};

    // Index Registers
    uint16_t si{};
    uint16_t di{};
    uint16_t bp{};
    uint16_t sp{};

    // Status Flags
    uint16_t flags{};

    // Segments
    uint16_t cs{};
    uint16_t ds{};
    uint16_t es{};
    uint16_t ss{};

    // Instruction Pointer
    int ip{};

    // Memory
    uint8_t memory[MEMORY_SIZE]{};

    bool running{false};

    // Functions
    uint16_t getEffectiveAddress(uint16_t segment, uint16_t offset);
    uint16_t fetchNextOpcode();

public:
    explicit CPU(const std::string& filename);
    // Functions


    void dump_cpu();
    void run();

    uint16_t getRegisterValue(int regNumber);

    void setRegisterValue(int regNumber, uint16_t value);

    int16_t convert2SComplement(uint8_t value);
};