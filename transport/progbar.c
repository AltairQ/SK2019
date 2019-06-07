#include "common.h"
#include "progbar.h"

static char* quadrants[] = {"▖", "▘", "▝", "▗"};

static int c_quad = 0;

void prog_tick()
{
	c_quad++;
}

void render_progbar(int a, int b)
{
	int percent = a*100/b;

	putchar('\r');
	printf("%s", quadrants[c_quad/64 % 4] );
	putchar('[');

	int cprog = percent * STEPS / 100;

	for (int x = 0; x < cprog; ++x)
	{
		printf("█");
	}
	for(int x = 0; x < STEPS-cprog; x++)
		putchar(' ');
	putchar(']');
	printf("  done %.2f %% (%d/%d bytes)", ((float)a) * 100.0f / (float) b,  a, b );
	
	fflush(stdout);
}