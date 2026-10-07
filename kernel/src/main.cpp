#include <limine.h>
#include <stddef.h>

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] =
    LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] =
    LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
    .response = nullptr,
};

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] =
    LIMINE_REQUESTS_END_MARKER;

extern "C" void kmain()
{
    asm volatile("cli");

    if(!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
        for (;;)
            asm volatile("hlt");
    }

    if (framebuffer_request.response == nullptr){
            for (;;)
                asm volatile("hlt");
    }

    auto framebuffer = framebuffer_request.response->framebuffers[0];

        volatile uint32_t* pixels =
            static_cast<volatile uint32_t*>(framebuffer->address);

    for(size_t i = 0; i<=100; i++){
        pixels[0] = 0x00FFFFFF;
    }

    for (;;)
        asm volatile("hlt");
}
