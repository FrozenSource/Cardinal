#include <core/io/isr.h>
#include <std/convert.h>
#include <core/io/ports.h>
#include <std/io.h>

#define PIC1        0x20 // Master PIC
#define PIC2        0xA0 // Slave PIC
#define PIC1_DATA   (PIC1 + 1)
#define PIC2_DATA   (PIC2 + 1)
#define PIC_EOI     0x20 // end of interrupt
#define IRQ_BASE    0x20

// These functions are declared in interrupts.asm file
extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();

extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();

isr_t interrupt_handlers[256];

const cstring exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_init() {
    static bool gbISR_Init = false;
    if (gbISR_Init) return;

    // start initialization
    port_byte_out(PIC1, 0x11);
    port_byte_out(PIC2, 0x11);

    // set IRQ base numbers for each PIC
    port_byte_out(PIC1_DATA, IRQ_BASE);
    port_byte_out(PIC2_DATA, IRQ_BASE + 8);

    // use IRQ number 2 to relay IRQs from the slave PIC
    port_byte_out(PIC1_DATA, 0x04);
    port_byte_out(PIC2_DATA, 0x02);

    // finish initialization
    port_byte_out(PIC1_DATA, 0x01);
    port_byte_out(PIC2_DATA, 0x01);

    // mask all IRQs
    port_byte_out(PIC1_DATA, 0x00);
    port_byte_out(PIC2_DATA, 0x00);

    set_idt_gate(0, (uint64_t) isr0);
    set_idt_gate(1, (uint64_t) isr1);
    set_idt_gate(2, (uint64_t) isr2);
    set_idt_gate(3, (uint64_t) isr3);
    set_idt_gate(4, (uint64_t) isr4);
    set_idt_gate(5, (uint64_t) isr5);
    set_idt_gate(6, (uint64_t) isr6);
    set_idt_gate(7, (uint64_t) isr7);
    set_idt_gate(8, (uint64_t) isr8);
    set_idt_gate(9, (uint64_t) isr9);
    set_idt_gate(10, (uint64_t) isr10);
    set_idt_gate(11, (uint64_t) isr11);
    set_idt_gate(12, (uint64_t) isr12);
    set_idt_gate(13, (uint64_t) isr13);
    set_idt_gate(14, (uint64_t) isr14);
    set_idt_gate(15, (uint64_t) isr15);
    set_idt_gate(16, (uint64_t) isr16);
    set_idt_gate(17, (uint64_t) isr17);
    set_idt_gate(18, (uint64_t) isr18);
    set_idt_gate(19, (uint64_t) isr19);
    set_idt_gate(20, (uint64_t) isr20);
    set_idt_gate(21, (uint64_t) isr21);
    set_idt_gate(22, (uint64_t) isr22);
    set_idt_gate(23, (uint64_t) isr23);
    set_idt_gate(24, (uint64_t) isr24);
    set_idt_gate(25, (uint64_t) isr25);
    set_idt_gate(26, (uint64_t) isr26);
    set_idt_gate(27, (uint64_t) isr27);
    set_idt_gate(28, (uint64_t) isr28);
    set_idt_gate(29, (uint64_t) isr29);
    set_idt_gate(30, (uint64_t) isr30);
    set_idt_gate(31, (uint64_t) isr31);

    set_idt_gate(IRQ0, (uint64_t) irq0);
    set_idt_gate(IRQ1, (uint64_t) irq1);
    set_idt_gate(IRQ2, (uint64_t) irq2);
    set_idt_gate(IRQ3, (uint64_t) irq3);
    set_idt_gate(IRQ4, (uint64_t) irq4);
    
    set_idt();
    
    gbISR_Init = true;
}

void irq_init()
{
    static bool gbIRQ_Init = false;
    if (gbIRQ_Init) return;

    isr_init();
    __asm__("sti");

    gbIRQ_Init = true;
}

void isr_handler(uint64_t id, uint64_t stack) __asm__("isr_handler");
void isr_handler(uint64_t id, UNUSED uint64_t stack) {
    printf("Received interrupt: %i\n%s\n", id, exception_messages[id]);
    __asm__("hlt");
}

void irq_handler(uint64_t id, uint64_t stack) __asm__("irq_handler");
void irq_handler(uint64_t id, uint64_t stack) {
    if (id >= 40)     {
        port_byte_out(PIC2, PIC_EOI);
    }

    port_byte_out(PIC1, PIC_EOI);

    if (interrupt_handlers[id] != 0) {
        isr_t handler = interrupt_handlers[id];
        handler(stack);
    }
}

void register_interrupt_handler(uint64_t id, isr_t handler) {
    irq_init();
    interrupt_handlers[id] = handler;
}

void Setup_Interrupts() {
    isr_init();
    irq_init();
}