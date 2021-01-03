#include "Utils.h"

std::mt19937 mt;

void random_shuffle(int* start, int* end) {
    int n = end - start;
    for (int i = n - 1; i >= 0; i--) {
        int j = mt() % (i + 1);
        std::swap(start[i], start[j]);
    }
}
