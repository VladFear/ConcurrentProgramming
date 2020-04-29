#include <iostream>
#include <mpi.h>
#include <fstream>
#include <cassert>
#include <string>
#include <ctime>

void merge(int *a, int *b, const int l, const int m, const int r)
{
	int h, i, j, k;
	h = l;
	i = l;
	j = m + 1;

	while ((h <= m) && (j <= r))
	{
		if (a[h] <= a[j])
		{
			b[i] = a[h];
			h++;
		}
		else
		{
			b[i] = a[j];
			j++;
		}
		i++;
	}
	if (m < h)
	{
		for (k = j; k <= r; k++)
		{
			b[i] = a[k];
			i++;
		}
	}
	else
	{
		for (k = h; k <= m; k++)
		{
			b[i] = a[k];
			i++;
		}
	}

	for (k = l; k <= r; k++)
		a[k] = b[k];
}

void mergeSort(int *a, int *b, const int l, const int r)
{
	int m;

	if (l < r)
	{
		m = (l + r) / 2;

		mergeSort(a, b, l, m);
		mergeSort(a, b, (m + 1), r);
		merge(a, b, l, m, r);
	}
}

void heapify(int * arr, const int size, const int i)
{
	int largest = i;
	int l = 2 * i + 1;
	int r = 2 * i + 2;

	if (l < size && arr[l] > arr[largest])
		largest = l;

	if (r < size && arr[r] > arr[largest])
		largest = r;

	if (largest != i)
	{
		std::swap(arr[i], arr[largest]);
		heapify(arr, size, largest);
	}
}

void heapSort(int * arr, const int size)
{
	for (int i = size / 2 - 1; i >= 0; --i)
		heapify(arr, size, i);

	for (int i = size - 1; i >= 0; --i)
	{
		std::swap(arr[0], arr[i]);
		heapify(arr, i, 0);
	}
}

void printArrayInRow(const int * arr, const int size)
{
	for (int i = 0; i < size; ++i)
		std::cout << arr[i] << " ";

	std::cout << std::endl;
	std::cout << std::endl;
}

void initializeArray(int * arr, const int size)
{
	const unsigned int boundary = (4 * size) + 1;

	for (int i = 0; i < size; ++i)
		arr[i] = rand() % boundary - (boundary / 2);
}

void printArrayInFile(const std::string & filename, const int * arr, const int size)
{
	std::ofstream file(filename);

	if (!file.is_open())
	{
		std::cerr << "Error during opening " << filename;
		return;
	}

	for (int i = 0; i < size; ++i)
		file << arr[i] << " ";

	file.close();
}

const bool DEBUG = false;

int main(int argc, char** argv)
{
	srand(time(NULL));
	const int MASTER_PROC = 0;

	if (argc < 2)
	{
		std::cerr << "Argument hasn't been specified (values per process).\n";
		return EXIT_FAILURE;
	}

	int size_per_process = atoi(argv[1]);
	if (size_per_process < 1)
	{
		std::cerr << "Values per process must be greater than 0. Fatal error.\n";
		return EXIT_FAILURE;
	}

	int proc = 0;
	int process_count = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	int original_size = process_count * size_per_process;
	int * original_array = nullptr;

	if (proc == MASTER_PROC)
	{
		original_array = new int[original_size];
		assert(original_array != nullptr);
		std::cout << "The array was allocated with " << original_size << " elements.\n\n";

		initializeArray(original_array, original_size);

		if (DEBUG || original_size <= 50)
		{
			std::cout << "Initial(unsorted) array: " << std::endl;
			printArrayInRow(original_array, original_size);
			std::cout << std::endl;

			std::string filename = "original";
			printArrayInFile(filename, original_array, original_size);
		}
	}

	double start_time = MPI_Wtime();

	int * sub_array = new int[size_per_process];
	assert(sub_array != nullptr);

	MPI_Scatter(original_array,
	            size_per_process,
	            MPI_INT,
	            sub_array,
	            size_per_process,
	            MPI_INT,
	            MASTER_PROC,
	            MPI_COMM_WORLD);

	heapSort(sub_array, size_per_process);

	if (DEBUG)
	{
		std::string filename = "subarray_" + std::to_string(proc);
		printArrayInFile(filename, sub_array, size_per_process);
	}

	int * tmp_array = new int[size_per_process];
	assert(tmp_array != nullptr);

	mergeSort(sub_array, tmp_array, 0, (size_per_process - 1));

	int * sorted = nullptr;

	if (proc == MASTER_PROC)
	{
		sorted = new int[original_size];
		assert(sorted != nullptr);
	}

	MPI_Gather(sub_array,
	           size_per_process,
	           MPI_INT,
	           sorted,
	           size_per_process,
	           MPI_INT,
	           MASTER_PROC,
	           MPI_COMM_WORLD);

	if (proc == MASTER_PROC)
	{
		int * other_array = new int[size_per_process];
		assert(other_array != nullptr);

		mergeSort(sorted, other_array, 0, (size_per_process - 1));
		double end_time = MPI_Wtime();

		if (DEBUG || original_size <= 50)
		{
			std::cout << "Sorted array: " << std::endl;
			printArrayInRow(original_array, original_size);

			std::string filename = "sorted";
			printArrayInFile(filename, sorted, original_size);
		}

		std::cout << "Array was successfully sorted. Elapsed time: " << end_time - start_time << std::endl;

		delete [] sorted;
		delete [] other_array;
		delete [] original_array;
	}

	delete [] sub_array;
	delete [] tmp_array;

	MPI_Finalize();

	return EXIT_SUCCESS;
}
