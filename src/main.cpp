#include "feature_utils.h"
#include <iostream>

int main() {
    double eval = eval_trade({"71484026"}, {"20573078", "20573078"}, 0, 0);

    std::cout << eval << "\n";

    return 0;
}