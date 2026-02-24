#include<iostream>
#include "cusum_calc.h"
#include "s_e_points.h"
//#include "pcm_th.h"

using namespace std;

int main()
{
	float x_temp[] = { 2, 2, 2, 2, 3, 3 };
	int length = sizeof(x_temp) / sizeof(x_temp[0]);
	float* ipc = new float[length - 1]();
	cusum_function(x_temp, length, ipc);

	for (int i = 0; i < length - 1; i++) {
		std::cout << ipc[i] << "  ";
	}
	cout << endl;
	delete[] ipc;
	
	int r[] = { 3, 6, 9, 12, 15 };	//Note we are sending e over here, in the R function e is concatenated in the end and uniqueness is checked
	int l[] = { 13, 10, 7, 4, 1 };	//Same for s
	int s = 1, e = 15;
	int len = sizeof(r) / sizeof(r[0]);			//assuming r and l have the same length
	int* right_points = new int[len+1]();
	int* left_points = new int[len+1]();
	int right_len = 0, left_len = 0;

	s_e_points(r, l, s, e, len, right_points, right_len, left_points, left_len);
	
	cout << "Right points: ";
	for (int i = 0; i < right_len-1; i++) {
		cout << right_points[i] << ", ";
	}
	cout<< right_points[right_len-1] << endl << "Left points: ";
	
	for (int i = 0; i < left_len-1; i++) {
		cout << left_points[i] << ", ";
	}
	cout << left_points[left_len-1]<<endl;

	delete[] right_points;
	delete[] left_points;

	return 0;
}