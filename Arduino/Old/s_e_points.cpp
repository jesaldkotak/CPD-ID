#include "s_e_points.h"

static void SortAsc(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = temp;
            }
        }
    }
}

static void SortDesc(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] < arr[j + 1]) {
                int temp = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = temp;
            }
        }
    }
}

static int removeDuplicates(int arr[], int n) {
    if (n == 0) return 0;
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
        if (arr[i] >= 0) arr[w++] = arr[i];
    }
    return w;
}

void s_e_points(int r[], int l[], int s, int e, int len,
                int e_points[], int& e_len,
                int s_points[], int& s_len)
{
    // Collect interior right points (strictly between s and e)
    int tmp_r[len], cnt_r = 0;
    for (int i = 0; i < len; i++) {
        if (r[i] > s && r[i] < e) tmp_r[cnt_r++] = r[i];
    }

    // Collect interior left points (strictly between s and e)
    int tmp_l[len], cnt_l = 0;
    for (int i = 0; i < len; i++) {
        if (l[i] > s && l[i] < e) tmp_l[cnt_l++] = l[i];
    }

    // Sort ascending, descending
    SortAsc(tmp_r, cnt_r);
    SortDesc(tmp_l, cnt_l);

    // Deduplicate
    int ur = removeDuplicates(tmp_r, cnt_r);
    int ul = removeDuplicates(tmp_l, cnt_l);

    // Copy interior points then append the boundary
    for (int i = 0; i < ur; i++) e_points[i] = tmp_r[i];
    e_points[ur] = e;           // always append e at the end
    e_len = ur + 1;

    for (int i = 0; i < ul; i++) s_points[i] = tmp_l[i];
    s_points[ul] = s;           // always append s at the end
    s_len = ul + 1;
}
