#include <cstdio>
#include <mpi.h>
#include <algorithm>

#define DEBUG 0

bool merge_front(float* ans, float* a, float* b, int la, int lb, int lans) {
	int i, j, k = 0;
	for(i = j = 0; i < la && j < lb && k < lans; k++) ans[k] = a[i] <= b[j] ? a[i++] : b[j++];
	if(i == la) return false;
	while(k < lans && i < la) ans[k++] = a[i++];
	while(k < lans && j < lb) ans[k++] = b[j++];
	return true;
}

bool merge_back(float* ans, float* a, float* b, int la, int lb, int lans) {
	int i, j, k = lans - 1;
	for(i = la - 1, j = lb - 1; i >= 0 && j >= 0 && k >= 0; k--) ans[k] = a[i] >= b[j] ? a[i--] : b[j--];
	if(i == -1) return false;
	while(k >= 0 && i >= 0) ans[k--] = a[i--];
	while(k >= 0 && j >= 0) ans[k--] = b[j--];
	return true;
}

int main(int argc, char** argv) {
	MPI_Init(&argc,&argv);
	int wrank, wsize;
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
	MPI_Comm_size(MPI_COMM_WORLD, &wsize);
	
	long long rank = wrank,
	     size = wsize,
	     N = atoll(argv[1]), 
	     chunk = (N + size - 1) / size,
	     l = std::min(N, chunk * rank),
	     r = std::min(N, chunk * (rank + 1)),
	     pre = std::min(N, chunk * (rank - 1)),
	     nex = std::min(N, chunk * (rank + 2)),
	     pre_len = l - pre,
	     nex_len = nex - r,
	     remote_len = std::max(pre_len, nex_len),
	     remote_rank;
	if(DEBUG) printf("[%lld] [%lld,%lld)\n", rank, l, r);
	
	MPI_File in, out;
	MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_RDONLY, MPI_INFO_NULL, &in);	
	MPI_File_open(MPI_COMM_WORLD, argv[3], MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &out);

	float *local = (float*)malloc(sizeof(float) * (r - l)),
	      *remote = (float*)malloc(sizeof(float) * remote_len),
	      *ans = (float*)malloc(sizeof(float) * (r - l));
	
	MPI_File_read_at(in, sizeof(float) * l, local, (int)(r - l), MPI_FLOAT, MPI_STATUS_IGNORE);	
	std::sort(local, local + (r - l));

	bool has_modify, 
	     glo_modify = true, 
	     phase = false, 
	     type = (rank % 2 == 0);

	while(glo_modify) {
		if(phase == false) has_modify = false;
		remote_rank = rank + (phase ^ type ? 1 : -1);
		remote_len = (phase ^ type ? nex_len : pre_len);
		if(DEBUG) printf("[%lld] remote -> %lld, remote size -> %lld\n", rank, remote_rank, remote_len);
		if(0 <= remote_rank && remote_rank < size && r - l > 0 && remote_len > 0) {	
			if(DEBUG) printf("[%lld] Start\n", rank);
			if(phase ^ type) {
				MPI_Send(local, r - l, MPI_FLOAT, remote_rank, 0, MPI_COMM_WORLD);	
				MPI_Recv(remote, remote_len, MPI_FLOAT, remote_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			} else {
				MPI_Recv(remote, remote_len, MPI_FLOAT, remote_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(local, r - l, MPI_FLOAT, remote_rank, 0, MPI_COMM_WORLD);
			}
			has_modify |= (
					phase ^ type ? 
					merge_front(ans, local, remote, r - l, remote_len, r - l) :
					merge_back(ans, local, remote, r - l, remote_len, r - l)
				      );

			std::swap(ans, local);
		}
		MPI_Barrier(MPI_COMM_WORLD);	
		phase = !phase;	
		if(phase == false) MPI_Allreduce(&has_modify, &glo_modify, 1, MPI::BOOL, MPI_LOR, MPI_COMM_WORLD);
	}

	MPI_File_write_at(out, sizeof(float) * l, local, (int)(r - l), MPI_FLOAT, MPI_STATUS_IGNORE);
	MPI_Finalize();
	
	free(local);
	free(remote);
	free(ans);
}
