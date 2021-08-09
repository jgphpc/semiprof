#include <iostream>
#include <vector>

#include <omp.h>
#include <mpi.h>   

#include <semiprof/semiprof.hpp>

// CC -DSEMIPROF -I../semiprof.gitjg/include mpi+omp.cpp ./libsemiprof.a -fopenmp
int main(int argc, char *argv[]) {
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::cout << "-----------------------\n"
              << "OpenMP semiprof example\n"
              << "Using " << omp_get_max_threads() << " threads\n"
              << "-----------------------\n\n";


    const size_t n = 1<<26;
    // const size_t n = 1<<28;

    std::vector<double> a(n);
    std::vector<double> b(n);
    std::vector<double> c(n);
    #pragma omp parallel for
    for (auto i=0lu; i<n; ++i) {
        a[i] = 1;
        b[i] = 2;
        c[i] = 3;
    }
    a[n/2] = 0;
    a[n/4] = 2;

    double sum = 0;
    #pragma omp parallel
    {
        PE(add);
        #pragma omp for
        for (auto i=0lu; i<n; ++i) {
            c[i] += a[i] + b[i];
        }
        PL();

        PE(reduce);
        #pragma omp for reduction(+:sum)
        for (auto i=0lu; i<n; ++i) {
            sum += c[i];
        }
        PL();
    }

    std::cout << semiprof::profiler_summary() << "\n\n";
    // std::cout << semiprof::profiler_summary_sphexa() << "\n\n";
    semiprof::profiler_summary_sphexa();

    std::cout << "result : " << sum << "\n";

    MPI_Finalize();
    return 0;
}