#define GPIO_BASE 0x20200000
#define GPIO_LENGTH 0x80

// Dividimos por 4 porque la suma de punteros se hace respecto a sizeof(int)

#define OFFSET_SET0 7
#define OFFSET_SET1 8
#define OFFSET_CLR0 10
#define OFFSET_CLR1 11
#define OFFSET_LEVEL0 13
#define OFFSET_LEVEL1 14

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

enum gpio_function {GPIO_INPUT, GPIO_OUTPUT};

static volatile uint32_t *gpio_addr;

int gpio_setup()
{
	int fd = open("/dev/mem", O_RDWR);

	if (fd < 0)
		return -1;

	gpio_addr = (uint32_t*)mmap(NULL, GPIO_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);

	if (gpio_addr == MAP_FAILED)
		return -1;

	return 0;
}

inline void gpio_set0(uint32_t mask)
{
	*(gpio_addr + OFFSET_SET0) = mask;
}

inline void gpio_set1(uint32_t mask)
{
	*(gpio_addr + OFFSET_SET1) = mask;
}

inline void gpio_set_pin(uint32_t pin)
{
	*(gpio_addr + OFFSET_SET0 + pin / 32) = 1 << (pin % 32);
}

inline void gpio_clear0(uint32_t mask)
{
	*(gpio_addr + OFFSET_CLR0) = mask;
}

inline void gpio_clear1(uint32_t mask)
{
	*(gpio_addr + OFFSET_CLR1) = mask;
}

inline void gpio_clear_pin(uint32_t pin)
{
	*(gpio_addr + OFFSET_CLR0 + pin / 32) = 1 << (pin % 32);
}

inline uint32_t gpio_level0()
{
	return *(gpio_addr + OFFSET_LEVEL0);
}

inline uint32_t gpio_level1()
{
	return *(gpio_addr + OFFSET_LEVEL1);
}

inline uint32_t gpio_level_pin(uint32_t pin)
{
	return *(gpio_addr + OFFSET_LEVEL0 + pin / 32) & (1 << (pin % 32));
}

void gpio_fsel(uint32_t pin, enum gpio_function func)
{
	*(gpio_addr + pin / 10) = func << pin % 10 * 3;
}

int main()
{
	if (gpio_setup()) {
		fprintf(stderr, "Error setup()\n");
		return EXIT_FAILURE;
	}

	////////////////////////////////////////////////////////////////////////////

	// Ponemos los pines 2 y 3 como salida
	gpio_fsel(2, GPIO_OUTPUT);
	gpio_fsel(3, GPIO_OUTPUT);
	
	// Ponemos el pin 4 como entrada
	gpio_fsel(4, GPIO_INPUT);
	
	// Emitimos un 1 por GPIO2 y un 0 por un GPIO3
	gpio_set_pin(2);
	gpio_clear_pin(3);
	
	// Leemos valor del GPIO4
	printf("GPIO4: %d\n", gpio_level_pin(4));

	////////////////////////////////////////////////////////////////////////////

	printf("pulsa una tecla para salir...");
	getchar();
	return EXIT_SUCCESS;
}
