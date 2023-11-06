#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

#define DT 0.05
#define EPS 1e-6

typedef struct
{
    double x, y;
    int current_time_step; // Current time step for exchanging F(k, q) and -F(q, k). This is Newton's 3rd law
} vector;

pthread_cond_t cond_var; // For synchronize barrier
pthread_mutex_t mutex; // For synchronize barrier
int counter = 0; // For synchornize barrier

pthread_barrier_t barrier; // For synchronize barrier using built-in function

int thread_count; // Number of threads used

int bodies, timeSteps;
double *masses, GravConstant;
vector *positions, *velocities, *accelerations;
vector *new_positions;
vector **common_accelerations; // For Newton's 3rd law

FILE *fp_out;
double start_time;
double end_time;

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
    printf("Points: %d\n", bodies);
    printf("Time steps: %d\n", timeSteps);
    printf("Threads: %d\n", thread_count);

    masses = (double *)malloc(bodies * sizeof(double));
    positions = (vector *)malloc(bodies * sizeof(vector));
    velocities = (vector *)malloc(bodies * sizeof(vector));
    accelerations = (vector *)malloc(bodies * sizeof(vector));
    
    common_accelerations = (vector **)malloc(bodies * sizeof(vector*));
    new_positions = (vector *)malloc(bodies * sizeof(vector));
    
    for (i = 0; i < bodies; i++)
    {
        fscanf(fp, "%lf", &masses[i]);
        fscanf(fp, "%lf%lf", &positions[i].x, &positions[i].y);
        fscanf(fp, "%lf%lf", &velocities[i].x, &velocities[i].y);
    }

    fclose(fp);
}

void synchronizing_output_barrier(int current_iter)
{
	pthread_mutex_lock(&mutex);
	counter++;
	
	if (counter == thread_count)
	{	
		fprintf(fp_out, "\nCycle %d\n", current_iter + 1); 
		for (int u = 0; u < bodies; u++)
		{	
			positions[u] = new_positions[u];
			fprintf(fp_out, "Body %d : %lf\t%lf\t%lf\t%lf\n", u + 1, new_positions[u].x, new_positions[u].y, velocities[u].x, velocities[u].y);
		}

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

	int local_points = bodies / thread_count;
	int start_point = my_rank * local_points;
	int end_point = start_point + local_points;
	
	double distance;
	vector current_acceleration;

	// Fix if bodies mod thread_count > 0
	if (my_rank == thread_count - 1)
	{
		end_point = bodies;
	}

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
					// Newton's 3rd law
					if (common_accelerations[v][u].current_time_step != i)
					{
						distance = pow(mod(subtractVectors(positions[v], positions[u])), 3);
						if (distance < EPS)
						{
							distance = EPS;
						}
						current_acceleration = scaleVector(GravConstant * masses[v] / distance, subtractVectors(positions[v], positions[u]));
						
						// Order is important
						common_accelerations[u][v] = current_acceleration; // Firstly, save current_acceleration
						common_accelerations[u][v].current_time_step = i; // Secondly, save current_time_step
					}

					else
					{
						current_acceleration.x = (-1) * common_accelerations[v][u].x;
						current_acceleration.y = (-1) * common_accelerations[v][u].y;
					}

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

		// This lines need if you want to use only 1 syncronizing barrier with built-in output.
		// But sometimes it's slowly than output between 2 pthread syncronizing barriers using pthread_barrier_wait().
		// synchronizing_output_barrier(i);
	
		pthread_barrier_wait(&barrier); // Synchronizing operation
		
		// Assign the last thread for output cycle number				       
		if (my_rank == thread_count - 1)
		{
			fprintf(fp_out, "\nCycle %d\n", i + 1); 
			for (int u = 0; u < bodies; u++)
			{
				positions[u] = new_positions[u];
				fprintf(fp_out, "Body %d : %lf\t%lf\t%lf\t%lf\n", u + 1, new_positions[u].x, new_positions[u].y, velocities[u].x, velocities[u].y);
			}
		}

		pthread_barrier_wait(&barrier);	// Synchronizing operation
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
	thread_count = atoi(argv[2]);
	initiateSystem(argv[1]);
 	fp_out = fopen("output", "w");

	GET_TIME(start_time);

	pthread_t* thread_handles = NULL;
	thread_handles = malloc(thread_count * sizeof(pthread_t));

	pthread_cond_init(&cond_var, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_barrier_init(&barrier, NULL, thread_count);	
	

	for (int i = 0; i < bodies; i++)
	{
		common_accelerations[i] = (vector *)malloc(bodies * sizeof(vector));
		for (int j = 0; j < bodies; j++)
		{
			common_accelerations[i][j].current_time_step = -1;
		}
	}

	fprintf(fp_out, "Body   :     x              y           vx              vy   \n");
	for (long i = 0; i < thread_count; i++)
	{
		pthread_create(&thread_handles[i], NULL, routine, (void*)i);
	}

	for (long i = 0; i < thread_count; i++)
	{
		pthread_join(thread_handles[i], NULL);
	}

	fclose(fp_out);
	free(thread_handles);
	pthread_cond_destroy(&cond_var);
	pthread_mutex_destroy(&mutex);

	GET_TIME(end_time);

	printf("\nTotal working time: %lf\n", end_time - start_time);
    }

    return 0;
}
