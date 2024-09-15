// TSP by Elias Stihl, Elias Ram & Isak Reinholdsson

#include <iostream>
#include <unordered_set>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <random>
#include <algorithm>
#include <chrono>
#include <list>
#include <map>
#include <fstream>

#include "tsp.h"
#include "lin_kernighan.h"
#include "clarke_wright.h"
#include "opt3.h"

using namespace std;
using namespace chrono;

int main()
{
    start = high_resolution_clock::now();
    readInput();
    initializeVariables();
    vector<int> path;

    if (n < 20)
        path = bruteForce();
    else
    {
        path = clarkeWright();
        indexPath(path);
        linKernighan(path, 1800 * 1000);
        opt3(path, 1997 * 1000);
    }

    // cout << totalScore(path) << endl;
    printSolution(path);
}
