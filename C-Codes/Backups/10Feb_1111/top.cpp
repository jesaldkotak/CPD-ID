#include<iostream>
#include "cusum_calc.h"

int main()
{
	float x_temp[] = { 2, 2, 2, 2, 3, 3 };
	int length = sizeof(x_temp) / sizeof(x_temp[0]);
	float* ipc = new float[length - 1]();
	cusum_function(x_temp, length, ipc);

	for (int i = 0; i < length - 1; i++) {
		std::cout << ipc[i] << std::endl;
	}
	delete[] ipc;
	
	return 0;
}