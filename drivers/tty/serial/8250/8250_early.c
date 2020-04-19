/*
 * Early serial console for 8250/16550 devices
 *
 * (c) Copyright 2004 Hewlett-Packard Development Company, L.P.
 *	Bjorn Helgaas <bjorn.helgaas@hp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Based on the 8250.c serial driver, Copyright (C) 2001 Russell King,
 * and on early_printk.c by Andi Kleen.
 *
 * This is for use before the serial driver has initialized, in
 * particular, before the UARTs have been discovered and named.
 * Instead of specifying the console device as, e.g., "ttyS0",
 * we locate the device directly by its MMIO or I/O port address.
 *
 * The user can specify the device directly, e.g.,
 *	earlycon=uart8250,io,0x3f8,9600n8
 *	earlycon=uart8250,mmio,0xff5e0000,115200n8
 *	earlycon=uart8250,mmio32,0xff5e0000,115200n8
 * or
 *	console=uart8250,io,0x3f8,9600n8
 *	console=uart8250,mmio,0xff5e0000,115200n8
 *	console=uart8250,mmio32,0xff5e0000,115200n8
 */

#include <linux/tty.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/serial_reg.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <asm/io.h>
#include <asm/serial.h>

static unsigned int serial8250_early_in(struct uart_port *port, int offset)
{
	int reg_offset = offset;
	offset <<= port->regshift;

	switch (port->iotype) {
	case UPIO_MEM:
		return readb(port->membase + offset);
	case UPIO_MEM16:
		return readw(port->membase + offset);
	case UPIO_MEM32:
		return readl(port->membase + offset);
	case UPIO_MEM32BE:
		return ioread32be(port->membase + offset);
	case UPIO_PORT:
		return inb(port->iobase + offset);
	case UPIO_AU:
		return port->serial_in(port, reg_offset);
	default:
		return 0;
	}
}

static void serial8250_early_out(struct uart_port *port, int offset, int value)
{
	int reg_offset = offset;
	offset <<= port->regshift;

	switch (port->iotype) {
	case UPIO_MEM:
		writeb(value, port->membase + offset);
		break;
	case UPIO_MEM16:
		writew(value, port->membase + offset);
		break;
	case UPIO_MEM32:
		writel(value, port->membase + offset);
		break;
	case UPIO_MEM32BE:
		iowrite32be(value, port->membase + offset);
		break;
	case UPIO_PORT:
		outb(value, port->iobase + offset);
		break;
	case UPIO_AU:
		port->serial_out(port, reg_offset, value);
		break;
	}
}

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)

static void serial_putc(struct uart_port *port, int c)
{
	unsigned int status;

	serial8250_early_out(port, UART_TX, c);

	for (;;) {
		status = serial8250_early_in(port, UART_LSR);
		if ((status & BOTH_EMPTY) == BOTH_EMPTY)
			break;
		cpu_relax();
	}
}

static void early_serial8250_write(struct console *console,
					const char *s, unsigned int count)
{
	struct earlycon_device *device = console->data;
	struct uart_port *port = &device->port;

	uart_console_write(port, s, count, serial_putc);
}

static void __init init_port(struct earlycon_device *device)
{
	struct uart_port *port = &device->port;
	unsigned int divisor;
	unsigned char c;
	unsigned int ier;

	serial8250_early_out(port, UART_LCR, 0x3);	/* 8n1 */
	ier = serial8250_early_in(port, UART_IER);
	serial8250_early_out(port, UART_IER, ier & UART_IER_UUE); /* no interrupt */
	serial8250_early_out(port, UART_FCR, 0);	/* no fifo */
	serial8250_early_out(port, UART_MCR, 0x3);	/* DTR + RTS */

	if (port->uartclk && device->baud) {
		divisor = DIV_ROUND_CLOSEST(port->uartclk, 16 * device->baud);
		c = serial8250_early_in(port, UART_LCR);
		serial8250_early_out(port, UART_LCR, c | UART_LCR_DLAB);
		serial8250_early_out(port, UART_DLL, divisor & 0xff);
		serial8250_early_out(port, UART_DLM, (divisor >> 8) & 0xff);
		serial8250_early_out(port, UART_LCR, c & ~UART_LCR_DLAB);
	}
}

int __init early_serial8250_setup(struct earlycon_device *device,
					 const char *options)
{
	if (!(device->port.membase || device->port.iobase))
		return -ENODEV;

	if (!device->baud) {
		struct uart_port *port = &device->port;
		unsigned int ier;

		/* assume the device was initialized, only mask interrupts */
		ier = serial8250_early_in(port, UART_IER);
		serial8250_early_out(port, UART_IER, ier & UART_IER_UUE);
	} else
		init_port(device);

	device->con->write = early_serial8250_write;
	return 0;
}
EARLYCON_DECLARE(uart8250, early_serial8250_setup);
EARLYCON_DECLARE(uart, early_serial8250_setup);
OF_EARLYCON_DECLARE(ns16550, "ns16550", early_serial8250_setup);
OF_EARLYCON_DECLARE(ns16550a, "ns16550a", early_serial8250_setup);
OF_EARLYCON_DECLARE(uart, "nvidia,tegra20-uart", early_serial8250_setup);
OF_EARLYCON_DECLARE(uart, "snps,dw-apb-uart", early_serial8250_setup);

#ifdef CONFIG_SERIAL_8250_OMAP

static int __init early_omap8250_setup(struct earlycon_device *device,
				       const char *options)
{
	struct uart_port *port = &device->port;

	if (!(device->port.membase || device->port.iobase))
		return -ENODEV;

	port->regshift = 2;
	device->con->write = early_serial8250_write;
	return 0;
}

OF_EARLYCON_DECLARE(omap8250, "ti,omap2-uart", early_omap8250_setup);
OF_EARLYCON_DECLARE(omap8250, "ti,omap3-uart", early_omap8250_setup);
OF_EARLYCON_DECLARE(omap8250, "ti,omap4-uart", early_omap8250_setup);

#endif

#ifdef CONFIG_SERIAL_8250_RT288X

unsigned int au_serial_in(struct uart_port *p, int offset);
void au_serial_out(struct uart_port *p, int offset, int value);

static int __init early_au_setup(struct earlycon_device *dev, const char *opt)
{
	dev->port.serial_in = au_serial_in;
	dev->port.serial_out = au_serial_out;
	dev->port.iotype = UPIO_AU;
	dev->con->write = early_serial8250_write;
	return 0;
}
OF_EARLYCON_DECLARE(palmchip, "ralink,rt2880-uart", early_au_setup);

#endif

#ifdef CONFIG_SERIAL_8250_EARLYLOG

#include <linux/io.h>

/* logging signature */
#define LOG_SIG_OFFSET       0x0000
#define LOG_SIG_VAL          0x75767971

/* current logging offset that points to where new logs should be added */
#define LOG_OFF_OFFSET       0x0004

/* current logging length (excluding header) */
#define LOG_LEN_OFFSET       0x0008

#define LOG_HEADER_LEN       12

/*
 * @base: CPU virtual address of the memory where log is saved
 * @is_initialized: flag that indicates logging has been initialized
 * @addr: physical address of the memory where log is saved
 * @max_size: maximum log buffer size
 */
struct uart8250_early_log {
	void *base;
	bool is_initialized;
	phys_addr_t addr;
	u32 max_size;
};

static struct uart8250_early_log early_log;

static int __init param_setup_earlylog(char *buf)
{
	struct uart8250_early_log *log = &early_log;
	phys_addr_t addr;
	u32 size;
	char *end;
	int ret = -EINVAL;

	if (!buf || !buf[0])
		return ret;

	addr = memparse(buf, &end);
	if (*end == ',') {
		size = memparse(end + 1, NULL);

		if (addr && size) {
			log->addr = addr;
			log->max_size = size;
			return 0;
		}
	}

	return ret;
}
early_param("earlylog", param_setup_earlylog);

static void uart8250_early_log_putc(struct uart_port *port, int c)
{
	struct uart8250_early_log *log = &early_log;
	u32 offset, len;

	/* always dump to serial port */
	serial_putc(port, c);

	/* do not save any log if it's not yet initialized */
	if (!log->is_initialized)
		return;

	offset = readl(log->base + LOG_OFF_OFFSET);
	len = readl(log->base + LOG_LEN_OFFSET);
	writeb(c, log->base + offset);
	offset++;

	/* log buffer is now full and need to wrap around */
	if (offset >= log->max_size)
		offset = LOG_HEADER_LEN;

	/* only increment length when log buffer is not full */
	if (len < log->max_size - LOG_HEADER_LEN)
		len++;

	writel(offset, log->base + LOG_OFF_OFFSET);
	writel(len, log->base + LOG_LEN_OFFSET);
}

static void uart8250_early_log_write(struct console *console, const char *s,
				     unsigned int count)
{
	struct earlycon_device *dev = console->data;
	struct uart_port *port = &dev->port;
	unsigned int i;

	for (i = 0; i < count; i++, s++) {
		if (*s == '\n')
			uart8250_early_log_putc(port, '\r');
		uart8250_early_log_putc(port, *s);
	}
}

static int __init uart8250_early_log_setup(struct earlycon_device *dev,
					   const char *opt)
{
	if (!(dev->port.membase || dev->port.iobase))
		return -ENODEV;

	if (!dev->baud) {
		struct uart_port *port = &dev->port;
		unsigned int ier;

		/* assume the device was initialized, only mask interrupts */
		ier = serial8250_early_in(port, UART_IER);
		serial8250_early_out(port, UART_IER, ier & UART_IER_UUE);
	} else
		init_port(dev);

	dev->con->write = uart8250_early_log_write;

	return 0;
}
EARLYCON_DECLARE(uart8250_log, uart8250_early_log_setup);

static int uart8250_early_log_init(void)
{
	struct uart8250_early_log *log = &early_log;
	u32 val;

	/* silently bail out if invaliad parameters are seen */
	if (log->is_initialized || !log->addr || !log->max_size)
		return 0;

	log->base = memremap(log->addr, log->max_size, MEMREMAP_WB);
	if (!log->base)
		return -ENOMEM;

	/* Silently bail out if no header signature can be found */
	val = readl(log->base + LOG_SIG_OFFSET);
	if (val != LOG_SIG_VAL) {
		memunmap(log->base);
		return 0;
	}

	log->is_initialized = true;

	return 0;
}
arch_initcall(uart8250_early_log_init);

#endif /* CONFIG_SERIAL_8250_EARLYLOG */