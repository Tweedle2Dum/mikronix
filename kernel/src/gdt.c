#include "gdt.h"
#include <stdint.h>

static struct gdt_entry entries[3];
static struct gdt_descriptor desc;

static inline void set_gdt_entry(struct gdt_entry* entry, uint32_t base, uint32_t limit,
                                 uint8_t access, uint8_t flags) {
    entry->limit_low = limit & 0xFFFF;
    entry->base_low = base & 0xFFFF;
    entry->base_middle = (base >> 16) & 0xFF;
    entry->access = access;
    // Pack flags (upper nibble) and limit high (lower nibble) into granularity byte
    entry->granularity = (flags & 0xF0) | ((limit >> 16) & 0x0F);
    entry->base_high = (base >> 24) & 0xFF;
}

static void reload_segments() {
    __asm__ volatile("push $0x08\n\t"
                     "leaq 1f(%%rip), %%rax\n\t"
                     "pushq %%rax\n\t"
                     "lretq\n\t"
                     "1:\n\t"
                     "movw $0x10, %%ax\n\t"
                     "movw %%ax, %%ds\n\t"
                     "movw %%ax, %%es\n\t"
                     "movw %%ax, %%fs\n\t"
                     "movw %%ax, %%gs\n\t"
                     "movw %%ax, %%ss\n\t"
                     "\n\t"
                     :
                     :
                     : "rax", "memory");
}

void gdt_init(void) {
    // 0: Null Descriptor
    entries[0] = (struct gdt_entry){0};
    set_gdt_entry(&entries[1], 0, 0xFFFFF, 0x9A, 0xA0);
    set_gdt_entry(&entries[2], 0, 0xFFFFF, 0x92, 0xC0);

    desc.limit = sizeof(entries) - 1;
    desc.base = (uint64_t)&entries;

    // load gdt
    __asm__ volatile("lgdt (%0)" : : "m"(desc) : "memory");
    reload_segments();
}
