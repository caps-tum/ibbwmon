#include <iostream>
#include <string>

#include <cassert>
#include <cstring>

#include <mpi.h>

#include "time_measure.hpp"

/*** config vars **/
static size_t iterations = 10000;
static size_t per_iterations = 100;
static size_t comm_size = 50000000;

[[noreturn]] static void print_help(const char *argv) {
	std::cout << argv << " supports the following flags:\n";
	std::cout << "\t --iter \t Number of iterations. \t\t\t Default: " << iterations << "\n";
	std::cout << "\t --per-iter \t Communications per iteration. \t\t\t Default: " << per_iterations << "\n";
	std::cout << "\t --size \t Message size in bytes. \t\t\t Default: " << comm_size << "\n";
	exit(0);
}

static void parse_options(size_t argc, const char **argv) {
	if (argc == 1) {
		print_help(argv[0]);
	}

	for (size_t i = 1; i < argc; ++i) {
		std::string arg(argv[i]);

#if 0
		if (arg == "--server") {
			if (i + 1 >= argc) {
				print_help(argv[0]);
			}
			// server = std::string(argv[i + 1]);
			++i;
			continue;
		}
#endif
		if (arg == "--iter") {
			if (i + 1 >= argc) {
				print_help(argv[0]);
			}
			iterations = std::stoul(std::string(argv[i + 1]));
			++i;
			continue;
		}
		if (arg == "--size") {
			if (i + 1 >= argc) {
				print_help(argv[0]);
			}
			comm_size = std::stoul(std::string(argv[i + 1]));
			++i;
			continue;
		}
		if (arg == "--per-iter") {
			if (i + 1 >= argc) {
				print_help(argv[0]);
			}
			per_iterations = std::stoul(std::string(argv[i + 1]));
			++i;
			continue;
		}
	}

	// if (server == "") print_help(argv[0]);
}

static void bench(char *send, char *recv) {
	for (size_t i = 0; i < per_iterations; ++i) {
		if (MPI_Alltoall(send, comm_size, MPI_CHAR, recv, comm_size, MPI_CHAR, MPI_COMM_WORLD) != MPI_SUCCESS)
			assert(false);
	}
}

static void print_result(int miliseconds, int size) {
	// send comm_size many chars to everyone else + receive the same amount
	double data_per_iteration = sizeof(char) * comm_size * (size - 1) * 2;
	// multiple times per iteration
	data_per_iteration *= per_iterations;

	std::cout << "mili-sec: " << miliseconds << " - bw: " << data_per_iteration / miliseconds * 1000 / 1000 / 1000
			  << "\n";
}

int main(int argc, char *argv[]) {
	parse_options(static_cast<size_t>(argc), const_cast<const char **>(argv));

	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) assert(false);

	int rank = -1, size = -1;
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank) != MPI_SUCCESS) assert(false);
	if (MPI_Comm_size(MPI_COMM_WORLD, &size) != MPI_SUCCESS) assert(false);

	if (rank == 0) {
		std::cout << "Running with " << size << " processes.\n";
	}

	if (size < 2) {
		std::cout << "You must use >= 2 processes. You are using " << size << " processes." << std::endl;
		exit(0);
	}

	char *send_buf = new char[comm_size * size];
	std::fill_n(send_buf, comm_size * size, 23);
	char *recv_buf = new char[comm_size * size];
	std::fill_n(recv_buf, comm_size * size, 42);

	if (rank == 0) std::cout << "Starting initialization ...\n";

	size_t miliseconds = time_measure<>::execute(bench, send_buf, recv_buf);

	if (rank == 0) print_result(miliseconds, size);

	for (int i = 0; i < iterations; ++i) {
		size_t miliseconds = time_measure<>::execute(bench, send_buf, recv_buf);
		if (rank == 0) print_result(miliseconds, size);
	}
}
