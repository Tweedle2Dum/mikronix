extern "C" void kmain()
{
    asm volatile("cli");

    for (;;)
        asm volatile("hlt");
}
