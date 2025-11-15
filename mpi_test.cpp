#include <vector>
#include <iostream>
#include <mpi.h>
#include <cstdint>

int main(int argc, char **argv){
    MPI_Init(&argc,&argv);
    const size_t N=1'000'000;
    int rank;
    int num_procs;
    MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if (rank == 0){
        std::vector<uint64_t> data(N);
        for (size_t i = 0; i < N; i++){
            data[i] = i;
        }
        MPI_Bcast(data.data(),N,MPI_UINT64_T,0,MPI_COMM_WORLD);
    }
    MPI_Status status;
    std::vector<uint64_t> received_data;
    received_data.reserve(N);

    MPI_Recv(received_data.data(),N,MPI_UINT64_T,0,0,MPI_COMM_WORLD,&status);
    std::cout<<received_data[10]<<std::endl;
    MPI_Finalize();
}