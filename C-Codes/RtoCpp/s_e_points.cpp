#include"s_e_points.h"
#include<math.h>

//sort in asc/desc
//convert to integer
//define new arrays which have r pts and no repition

static void SortAsc(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

static void SortDesc(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] < arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

static int removeDuplicates(int arr[], int n) {
    int w = 1;
    for (int i = 1; i < n; i++) {
        if (arr[i] != arr[i - 1]) {
            arr[w++] = arr[i];
        }
    }
    return w;
}

static int keepPositive(int arr[], int n) {
    int w = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] >= 0) {         
            arr[w++] = arr[i];
        }
    }
    return w;
}

void s_e_points(int r[], int l[], int s, int e, int len, int e_points[], int &e_len, int s_points[], int &s_len)
{
	int posr = 0, posl = 0;
	int unique_r = 0, unique_l = 0;

	for (int i = 0; i < len; i++) {
        if (r[i] < s || r[i] > e) {
            r[i] = -1;
        }
        if (l[i] < s || l[i] > e) {
            l[i] = -1;
        }
    }
	posr = keepPositive(r, len);
    posl = keepPositive(l, len);

    SortAsc(r, posr);
    SortDesc(l, posl);

    e_len = removeDuplicates(r, posr);
    s_len = removeDuplicates(l, posl);

    for (int i = 0; i < e_len; i++) {
        e_points[i] = r[i];
	}
    //e_points[e_len] = e;

    for (int i = 0; i < s_len; i++) {
        s_points[i] = l[i];
    }
    //s_points[s_len] = s;
}