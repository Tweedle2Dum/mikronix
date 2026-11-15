#include <stdint.h>

#ifndef GDT_H
#define GDT_H

// ptr to be loaded into register, limit is size and base is the vaddr of gdt table
struct __attribute__((packed)) gdt_descriptor {
    uint16_t limit;
    uint64_t base;
};

// access - Code Segment ----  0x9A
// granul - Code Segment ----  0xA0
// access - Data Segment ----  0x92
// granul - Data Segment ----  0xC0
struct __attribute__((packed)) gdt_entry {
    uint16_t limit_low;  // 0xFFFF
    uint16_t base_low;   // 0x0000
    uint8_t base_middle; // 0x0000
    uint8_t access;      // varies
    uint8_t granularity; // varies - also double as part of limit_high, the lower nibble.
    uint8_t base_high;   // 0x0000
};

// This struct is for TSS which is 16 bytes in long mode
// Still needed for some specfic IST and Priv level Transitions
struct __attribute__((packed)) gdt_system_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved;
};

void gdt_init(void);

#endif
