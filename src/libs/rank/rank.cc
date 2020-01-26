#include "src/libs/rank/rank.h"

float BBG::absoluteValue(float A) {
    if (A < 0.0) return -A;
    return A;
}

bool BBG::compareFloat(float A, float B, float epsilon) {
    return absoluteValue(A - B) < epsilon;
}
