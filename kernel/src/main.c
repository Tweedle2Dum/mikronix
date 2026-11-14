#include <limine.h>
#include <stddef.h>
#include <stdint.h>

__attribute__((
    used,
    section(".limine_requests_start"))) static volatile uint64_t limine_requests_start_marker[] =
    LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests"))) static volatile uint64_t limine_base_revision[] =
    LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request
    framebuffer_request = {
        .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
        .revision = 0,
        .response = nullptr,
};

__attribute__((
    used, section(".limine_requests_end"))) static volatile uint64_t limine_requests_end_marker[] =
    LIMINE_REQUESTS_END_MARKER;

[[noreturn]] static void halt_forever(void) {
    for (;;)
        __asm__ volatile("hlt");
}

void kmain(void) {
    __asm__ volatile("cli");

    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision))
        halt_forever();

    if (framebuffer_request.response == nullptr ||
        framebuffer_request.response->framebuffer_count == 0) {
        halt_forever();
    }

    struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];

    volatile uint32_t* pixels = (volatile uint32_t*)framebuffer->address;

    for (size_t i = 0; i < 100 && i < framebuffer->width; ++i)
        pixels[i] = 0x00FFFFFF;

    halt_forever();
}
