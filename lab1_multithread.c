#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <float.h>
#include "timer.h"

#define DT 0.05
#define EPS 1e-6

typedef struct
{
    double x, y;
} vector;

pthread_cond_t cond_var;
pthread_mutex_t mutex;
int thread_count;
int thread_flag_output;
int counter = 0;

int bodies, timeSteps;
double *masses, GravConstant;
vector *positions, *velocities, *accelerations;
vector *new_positions;

double start_time;
double end_time;

pthread_barrier_t barrier;

vector addVectors(vector a, vector b)
{
    vector c = {a.x + b.x, a.y + b.y};

    return c;
}

vector scaleVector(double b, vector a)
{
    vector c = {b * a.x, b * a.y};

    return c;
}

vector subtractVectors(vector a, vector b)
{
    vector c = {a.x - b.x, a.y - b.y};

    return c;
}

double mod(vector a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

void initiateSystem(char *fileName)
{
    int i;
    FILE *fp = fopen(fileName, "r");

    fscanf(fp, "%lf%d%d", &GravConstant, &bodies, &timeSteps);

    masses = (double *)malloc(bodies * sizeof(double));
    positions = (vector *)malloc(bodies * sizeof(vector));
    velocities = (vector *)malloc(bodies * sizeof(vector));
    accelerations = (vector *)malloc(bodies * sizeof(vector));
    
    new_positions = (vector *)malloc(bodies * sizeof(vector));
    
    for (i = 0; i < bodies; i++)
    {
        fscanf(fp, "%lf", &masses[i]);
        fscanf(fp, "%lf%lf", &positions[i].x, &positions[i].y);
        fscanf(fp, "%lf%lf", &velocities[i].x, &velocities[i].y);
    }

    fclose(fp);
}

void synchronizing_barrier()
{
	pthread_mutex_lock(&mutex);
	counter++;
	
	if (counter == thread_count)
	{	
		counter = 0;
		pthread_cond_broadcast(&cond_var);
	}
	else
	{
		while(pthread_cond_wait(&cond_var, &mutex) != 0);
	
	}
	pthread_mutex_unlock(&mutex);

}

void* routine(void* rank)
{
	long my_rank = (long)(rank);
	int next_thread = (my_rank + 1) % thread_count;

	int local_points = bodies / thread_count;
	int start_point = my_rank * local_points;
	int end_point = start_point + local_points;
	
	double distance;
	vector current_acceleration;

	for (int i = 0; i < timeSteps; i++)
	{
		// Calculating accelerations
    		for (int u = start_point; u < end_point; u++)
    		{
        		accelerations[u].x = 0;
        		accelerations[u].y = 0;

			for (int v = 0; v < bodies; v++)
        		{	
   	    			if (u != v)
            			{
					distance = pow(mod(subtractVectors(positions[v], positions[u])), 3);
					if (distance < EPS)
					{
						distance = EPS;
					}
					current_acceleration = scaleVector(GravConstant * masses[v] / distance, subtractVectors(positions[v], positions[u]));
					accelerations[u] = addVectors(accelerations[u], current_acceleration);
				}
        		}
		}

		// Calculating positions
    		for (int u = start_point; u < end_point; u++) 
    		{	
        		new_positions[u] = addVectors(positions[u], scaleVector(DT, velocities[u]));
    		}

		// Calculating velocities
    		for (int u = start_point; u < end_point; u++)
    		{
        		velocities[u] = addVectors(velocities[u], scaleVector(DT, accelerations[u]));
    		}

		// Synchronizing operations
		// synchronizing_barrier();
		pthread_barrier_wait(&barrier);	
		
		if (my_rank == thread_count - 1)
		{
			printf("\nCycle %d\n", i + 1); // Assign the last thread for output cycle number
			for (int u = 0; u < bodies; u++)
			{	
				positions[u] = new_positions[u];
				printf("Body %d : %lf\t%lf\t%lf\t%lf\n", u + 1, positions[u].x, positions[u].y, velocities[u].x, velocities[u].y);
			}
		}

		// Synchronizing operations
		// synchronizing_barrier();
		pthread_barrier_wait(&barrier);	
		
	}

	return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage : %s <file name containing system configuration data>\n", argv[0]);
    }
    else
    {
        initiateSystem(argv[1]);
	thread_count = atoi(argv[2]);
 	
	GET_TIME(start_time);

	pthread_t* thread_handles = NULL;
	thread_handles = malloc(thread_count * sizeof(pthread_t));

	pthread_cond_init(&cond_var, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_barrier_init(&barrier, NULL, thread_count);	
	
	printf("Body   :     x              y           vx              vy   \n");
	for (long i = 0; i < thread_count; i++)
	{
		pthread_create(&thread_handles[i], NULL, routine, (void*)i);
	}

	for (long i = 0; i < thread_count; i++)
	{
		pthread_join(thread_handles[i], NULL);
	}

	free(thread_handles);
	pthread_cond_destroy(&cond_var);
	pthread_mutex_destroy(&mutex);

	GET_TIME(end_time);

	printf("\nTotal working time: %lf\n", end_time - start_time);
    }

    return 0;
}
