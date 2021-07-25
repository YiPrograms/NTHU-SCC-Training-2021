#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

unsigned long long ans = 0;
// pthread_mutex_t mutex;
unsigned long long r, k, part, sqr, top, r2;

pthread_t threads[12];
int ID[12];
unsigned long long res[12];

void *worker(void *arg) {
	const int id = *(int* )arg;
	unsigned long long pixels = 0;
	unsigned long long x = part * id;
	unsigned long long nowy = ceil(sqrtl(r * r - x * x));
	for (; x < part * (id + 1) &&  x < sqr; x++) {
		if (r2 - x * x <= (nowy - 1) * (nowy - 1))
			nowy--;
		pixels += nowy;
		// printf("x: %03llu, y: %llu, topy: %llu\n", x, nowy, nowy - sqr);
	}

	res[id] = pixels + pixels;

	pthread_exit(NULL);
}

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "must provide exactly 2 arguments!\n");
		return 1;
	}

	r = atoll(argv[1]);
	k = atoll(argv[2]);
	cpu_set_t cpuset;
	sched_getaffinity(0, sizeof(cpuset), &cpuset);
	unsigned long long ncpus = CPU_COUNT(&cpuset);
	unsigned long long num_threads = r > 1e5? ncpus: 4;


	r2 = r * r;

	sqr = ceil(r / 1.4142135623730951);
	//printf("atan(1): %lf, cos(atan(1)): %lf, sqr: %llu\n", acos(1), cos(atan(1)), sqr);
	// ans = sqr * sqr;

	part = sqr / num_threads + 1;

	for (int i = 0; i < num_threads; i++) {
		ID[i] = i;
		pthread_create(&threads[i], NULL, worker, (void *) &ID[i]);
	}


	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		ans += res[i];
	}

	ans -= sqr * sqr;
	ans %= k;

	printf("%llu\n", (4 * ans) % k);
}
