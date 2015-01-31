#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define REPETITIONS 500
#define SIZE 124

int a[SIZE][SIZE] = {{0}};
int b[SIZE][SIZE] = {{0}};
int res[SIZE][SIZE] = {{0}};

#define MEASURE(FUNC) {\
    uint64_t beg = RDTSC(); \
    FUNC;\
    uint64_t end = RDTSC(); \
    printf("Spent %lld\n", end - beg); \
}

#define MEASURE_ASSIGN(B, FUNC) {\
    uint64_t beg = RDTSC(); \
    FUNC;\
    uint64_t end = RDTSC(); \
    B = (end - beg); \
}

static inline uint64_t RDTSC() 
{
    unsigned hi, lo; 
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

void fill_random(int a[][SIZE], int h, int w) {
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; j++) {
            int value = rand() % 1000; // make sure we do not overflow later
            a[i][j] = value;
        }
    }
}

void mmul1(int* a, int *b, int* res, int h, int w) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int k = 0; k < w; k++) {
                (res + i * w)[j] += (a + i * w)[k] * (b + k * w)[j];
            }
        }
    }
}

void mmul2(int* a, int *b, int* res, int h, int w) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int k = 0; k < w; k+=2) {
                (res + i * w)[j] += (a + i * w)[k] * (b + k * w)[j];
                (res + i * w)[j] += (a + i * w)[k+1] * (b + (k+1) * w)[j];
            }
        }
    }
}

void mmul3(int* a, int *b, int* res, int h, int w) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int k = 0; k < w; k+=4) {
                (res + i * w)[j] += (a + i * w)[k] * (b + k * w)[j];
                (res + i * w)[j] += (a + i * w)[k+1] * (b + (k+1) * w)[j];
                (res + i * w)[j] += (a + i * w)[k+2] * (b + (k+2) * w)[j];
                (res + i * w)[j] += (a + i * w)[k+3] * (b + (k+3) * w)[j];
            }
        }
    }
}

void mmul4(int* a, int *b, int* res, int h, int w) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int k = 0; k < w; k+=8) {
                (res + i * w)[j] += (a + i * w)[k] * (b + k * w)[j];
                (res + i * w)[j] += (a + i * w)[k+1] * (b + (k+1) * w)[j];
                (res + i * w)[j] += (a + i * w)[k+2] * (b + (k+2) * w)[j];
                (res + i * w)[j] += (a + i * w)[k+3] * (b + (k+3) * w)[j];
                (res + i * w)[j] += (a + i * w)[k+4] * (b + (k+4) * w)[j];
                (res + i * w)[j] += (a + i * w)[k+5] * (b + (k+5) * w)[j];
                (res + i * w)[j] += (a + i * w)[k+6] * (b + (k+6) * w)[j];
                (res + i * w)[j] += (a + i * w)[k+7] * (b + (k+7) * w)[j];
            }
        }
    }
}

void mmul5(int* a, int *b, int* res, int h, int w) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int k = 0; k < w; k+=4) {
                int* line = (a + i * w);
                int* res_pos = &(res + i * w)[j];
                *res_pos += (line)[k] * (b + k * w)[j];
                *res_pos += (line)[k+1] * (b + (k+1) * w)[j];
                *res_pos += (line)[k+2] * (b + (k+2) * w)[j];
                *res_pos += (line)[k+3] * (b + (k+3) * w)[j];
            }
        }
    }
}

/* Wrong*/
void mmul6(int* a, int *b, int* res, int h, int w) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int k = 0; k < w; k+=4) {
                int* line = (a + i * w);
                int* res_pos = &(res + i * w)[j];
                *res_pos += (line)[k] * (b + j * w)[k];
                *res_pos += (line)[k+1] * (b + (j+1) * w)[k];
                *res_pos += (line)[k+2] * (b + (j+2) * w)[k];
                *res_pos += (line)[k+3] * (b + (j+3) * w)[k];
            }
        }
    }
}

//
// A * B = [ A11 A12 ] * [ B11 B12 ]
//         [ A21 A22 ]   [ B21 B22 ] 

// Blocked version
void mmul7(int* a, int *b, int* res, int h, int w) {
    int stepw = 8, steph = 8; // 8*8 squares
    for (int line = 0; line < h; line+=steph) {
        for (int col = 0; col < w; col+=stepw) {

            // b block chosen
            // now multiply a * b

            for (int i = line; i < line + steph; i++) {
                for (int j = col; j < col + stepw; j++) {
                    for (int k = 0; k < w; k++) {

                        (res + k * w)[i] += (a + k * w)[j]*(b + j * w)[i];

                        //(res + i * w)[l + j] += (a + i * w)[col+k] * (b + j * w)[l + i];
                    }
                }
            }
        }
    }
}

typedef void (*func)(int*, int*, int*, int, int);

func pointers[] = {
    mmul1,
    mmul2,
    mmul3,
    mmul4,
    mmul5,
    mmul7,
    mmul6,
};

int** result_matrix[sizeof(pointers) / sizeof(func)];

uint64_t average_cycles[sizeof(pointers) / sizeof(func)];

int is_equal(int* a, int* b, int h, int w) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int a_val = *(a + i * w + j);
            int b_val = *(b + i * w + j);

            if (a_val != b_val)
                return 0;
        }
    }
    return 1;
}

int main() {
    fill_random(a, SIZE, SIZE);
    fill_random(b, SIZE, SIZE);


    for (int j = 0; j < sizeof(pointers) / sizeof(func); ++j) {
        uint64_t time, total_time = 0;
        for (int i = 0; i < REPETITIONS; ++i) {

            int** new_matrix = (int**) malloc(sizeof(int) * SIZE * SIZE);
            assert(new_matrix);
            result_matrix[j] = new_matrix;

            MEASURE_ASSIGN(time,
                    pointers[j]((int*)a, (int*)b, (int*)new_matrix, SIZE, SIZE);
                    )
                total_time += time;
        }
        total_time /= REPETITIONS;
        average_cycles[j] = total_time;
    }

    for (int i = 0; i <  sizeof(pointers) / sizeof(func); ++i) {
        int correct = 0;
        if (i > 0) {
            correct = is_equal((int*)result_matrix[i], (int*)result_matrix[i-1], SIZE, SIZE);
        }

        printf("Version %d. Average cycles: %lld. Correct?: %s\n", i, average_cycles[i], 
                (i == 0 ? "N/A" : (correct == 0 ? "FALSE" : "TRUE")));
    }

    return 0;
}


