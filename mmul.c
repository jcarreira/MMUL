#include <stdlib.h>

int a[40][40] = {0}
int b[40][40] = {0}

void fill_random(int a[][40], int h, int w) {
    for (int i = 0; i < h ++i) {
        for (int j = 0 j < w j++) {
            int value = rand();
            a[h][j] = value;
        }
    }
}

void mmul(int** a, int **b, int h, int w) {

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {

        }
    }

    return 0;
}

int main() {
    fill_random(a, 40, 40);
    fill_random(b, 40, 40);

    mmul(a, b, 40, 40);

    return;
}
