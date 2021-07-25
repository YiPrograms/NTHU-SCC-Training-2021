#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "must provide exactly 2 arguments!\n");
		return 1;
	}
	unsigned long long r = atoll(argv[1]);
	unsigned long long k = atoll(argv[2]);

	MPI_Init(NULL, NULL);
    
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    const unsigned long long part = r / world_size + 1;
	const unsigned long long sqr = ceil(r / 1.4142135623730951);
	const unsigned long long r2 = r * r;

	unsigned long long pixels = 0;
	unsigned long long x = part * world_rank;
	unsigned long long nowy = ceil(sqrtl(r * r - x * x));
	for (; x < part * (world_rank + 1) &&  x < sqr; x++) {
		if (r2 - x * x <= (nowy - 1) * (nowy - 1))
			nowy--;
		pixels += nowy;
	}

	// res[id] = pixels + pixels;

    //printf("Rank %d: %llu\n", world_rank, pixels);
    unsigned long long ans;
    MPI_Reduce(&pixels, &ans, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if (world_rank == 0)
	    printf("%llu\n", (4 * ((ans * 2 - sqr * sqr) % k)) % k);
    MPI_Finalize();
}
