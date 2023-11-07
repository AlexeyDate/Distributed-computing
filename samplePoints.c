#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define gravConstant 6.6743e-11
#define points 400
#define timeSteps 1000

int main(int argc, char *argv[])
{
	int mass;
	int x, y;
	int Vx, Vy;
	FILE *fp = fopen("input.txt", "w");

	srand(time(NULL));

	fprintf(fp, "%e %d %d\n", gravConstant, points, timeSteps);
	for (int i = 0; i < timeSteps; i++)
	{
		mass = rand() % (1000000 + 1 - 0) - 0; // Range (0, 1000000)
		x = i;
		y = i;
		Vx = rand() % (100 + 1 - 0) - 0; // Range (0, 100)
		Vy = rand() % (100 + 1 - 0) - 0; // Range (0, 100)
		
		fprintf(fp, "%d\n", mass);
		fprintf(fp, "%d %d\n", x, y);
		fprintf(fp, "%d %d\n", Vx, Vy);
	}

	fclose(fp);

	return 0;
}
