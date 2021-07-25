#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <mpi.h>

#include <mimalloc.h>
// #include <mimalloc-new-delete.h>
#include <boost/sort/sort.hpp>

int main(int argc, char** argv) {
	int n = atoll(argv[1]);

	MPI_Init(NULL, NULL);
    
    int world_size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Processes to use
    int processes = n < world_size? n: world_size;
    // if (rank == 0)
    //     printf("Using %d processes\n", processes);

    MPI_Comm comm = MPI_COMM_WORLD;

    if (processes < world_size) {
        MPI_Group orig_group, new_group;
        MPI_Comm_group(MPI_COMM_WORLD, &orig_group);

        int ranges[1][3] = {{ processes, world_size - 1, 1 }};
        MPI_Group_range_excl(orig_group, 1, ranges, &new_group);
        
        MPI_Comm_create(MPI_COMM_WORLD, new_group, &comm);

        if (comm == MPI_COMM_NULL) {
            // Close unwanted processes
            MPI_Finalize();
            exit(0);
        }
    }

    const bool parity = rank & 1;

    const int item_per_process = ceil((double)n / processes);
    const int item_cnt = (rank == processes - 1)? n - item_per_process * rank: item_per_process;
    // float *items = new float[item_per_process * 4];
    float *items = (float *)mi_malloc(item_per_process * 4 * sizeof(float));
    float *buffer = items + (item_per_process * 2);

	MPI_File fin;
	MPI_File_open(comm, argv[2], MPI_MODE_RDONLY, MPI_INFO_NULL, &fin);
    MPI_File_set_view(fin, rank * item_per_process * sizeof(float), MPI_FLOAT, MPI_FLOAT, "native", MPI_INFO_NULL);
    MPI_File_read(fin, items, item_cnt, MPI_FLOAT, MPI_STATUS_IGNORE);
    
    // Local sort
    // std::sort(items, items + item_cnt);
    boost::sort::spreadsort::spreadsort(items, items + item_cnt);

    const int dir = parity? 1: -1;

    // Exchange targets
    int targets[2] = {rank + dir, rank - dir};
    int targets_item_count[2] = { item_per_process, item_per_process };

    if (targets[0] < 0 || targets[0] >= processes)
        targets_item_count[0] = -1;
    if (targets[1] < 0 || targets[1] >= processes)
        targets_item_count[1] = -1;
    if (targets[0] == processes - 1)
        targets_item_count[0] = n - item_per_process * (processes - 1);
    if (targets[1] == processes - 1)
        targets_item_count[1] = n - item_per_process * (processes - 1);

    float ex_sample;
    

    // True: EVEN, False: ODD
    bool phase = true;

    bool changed;
    bool round_change = true;

    int rounds = processes;

    // Odd Even loop
    do {
        changed = false;

        const int ex = targets[phase];
        const int ex_item_count = targets_item_count[phase];

        // printf("Rank %d Round %d: %s phase, ex: %d, ex_item_count: %d\n", rank, round, phase? "EVEN": "ODD", ex, ex_item_count); 

        if (ex_item_count != -1) {
            if (parity ^ phase) { // Left: Receive, process data and send

                // Send & Recv a sample
                MPI_Sendrecv(&items[item_cnt - 1], 1, MPI_FLOAT, ex, 0,
                             &ex_sample, 1, MPI_FLOAT, ex, 0,
                             comm, MPI_STATUS_IGNORE);

                // printf("Rank %d Round %d: I'm LEFT, My sample: %f, %d's sample: %f\n", rank, round, items[item_cnt - 1], ex, ex_sample);
                
                // Needs change
                if (items[item_cnt - 1] > ex_sample) {
                    // printf("Rank %d Round %d: Changing with %d\n", rank, round, ex);
                    
                    MPI_Recv(items+item_cnt, ex_item_count, MPI_FLOAT, ex, 1, comm, MPI_STATUS_IGNORE);

                    std::merge(items, items + item_cnt,
                            items+item_cnt, items+item_cnt + ex_item_count,
                            buffer);

                    std::swap(items, buffer);

                    MPI_Send(items+item_cnt, ex_item_count, MPI_FLOAT, ex, 1, comm);

                    changed = true;
                }
                
            } else { // Right: Send and receive

                // Send & Recv a sample
                MPI_Sendrecv(items, 1, MPI_FLOAT, ex, 0,
                             &ex_sample, 1, MPI_FLOAT, ex, 0,
                             comm, MPI_STATUS_IGNORE);

                // printf("Rank %d Round %d: I'm RIGHT, My sample: %f, %d's sample: %f\n", rank, round, items[0], ex, ex_sample);

                // Needs change
                if (ex_sample > items[0]) {
                    // printf("Rank %d Round %d: Changing with %d\n", rank, round, ex);
                    MPI_Sendrecv_replace(items, item_cnt, MPI_FLOAT, ex, 1, ex, 1, comm, MPI_STATUS_IGNORE);
                }
            }
        }

        phase = !phase;

        // Termination check
        // printf("Rank %d Round %d: Stopped at barrier\n", rank, round);
        // MPI_Barrier(comm);
        // if (phase)
        //     MPI_Allreduce(&changed, &round_change, 1, MPI_CXX_BOOL, MPI_LOR, comm);
    } while (rounds--);

    MPI_File fout;
    MPI_File_open(comm, argv[3], MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fout);
    MPI_File_write_at(fout, rank * item_per_process * sizeof(float), items, item_cnt, MPI_FLOAT, MPI_STATUS_IGNORE);

    MPI_Finalize();
}

