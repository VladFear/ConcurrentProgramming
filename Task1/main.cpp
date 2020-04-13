#include <iostream>
#include <cstdlib>
#include <chrono>

void oddEvenSort(int * arr, const size_t size)
{
	bool is_sorted = false;

	while (!is_sorted)
	{
		is_sorted = true;

		for (int i = 1; i < size - 1; i += 2)
		{
			if (arr[i] > arr[i + 1])
			{
				std::swap(arr[i], arr[i + 1]);
				is_sorted = false;
			}
		}

		for (int i = 0; i < size - 1; i += 2)
		{
			if (arr[i] > arr[i + 1])
			{
				std::swap(arr[i], arr[i + 1]);
				is_sorted = false;
			}
		}
	}
}

void printArray(const int * arr, const size_t size)
{
	for (size_t i = 0; i < size; ++i)
		std::cout << arr[i] << " ";
	std::cout << std::endl;
}

void initializeArray(int * arr, const size_t size)
{
	for (size_t i = 0; i < size; ++i)
		arr[i] = rand() % 201 - 100;
}

int main(int argc, char ** argv)
{
	srand(time(NULL));
	int * array = nullptr;
	size_t size = 0;

	std::cout << "Enter array size: ";
	std::cin >> size;

	array = new int[size];
	initializeArray(array, size);

	// std::cout << "Initial array:\n";
	// printArray(array, size);
	std::chrono::time_point<std::chrono::system_clock> before = std::chrono::system_clock::now();
	oddEvenSort(array, size);
	std::chrono::time_point<std::chrono::system_clock> after = std::chrono::system_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
	// std::cout << std::endl;
	// std::cout << "Sorted array:\n";
	// printArray(array, size);

	std::cout << std::endl;
	std::cout << "Execution time(milliseconds): " << ms << std::endl;
	delete [] array;
	return 0;
}
