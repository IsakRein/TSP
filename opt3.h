#ifndef OPT3_H
#define OPT3_H

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

void opt3(vector<int> &path, long long deadline)
{
    int i = 0;
    if (n < 6)
        return;
    while (true)
    {
        for (int indexA1 = 0; indexA1 < n - 5; indexA1++)
        {
            for (int indexB1 = indexA1 + 1; indexB1 < n - 3; indexB1++)
            {
                for (int indexC1 = indexB1 + 1; indexC1 < n - 1; indexC1++)
                {
                    i++;
                    if (i % 100000 == 0 && duration_cast<microseconds>(high_resolution_clock::now() - start).count() > deadline)
                    {
                        return;
                    }
                    int indexA2 = indexA1 + 1;
                    int indexB2 = indexB1 + 1;
                    int indexC2 = indexC1 + 1;

                    int ADel = dist(path[indexA1], path[indexA2]);
                    int BDel = dist(path[indexB1], path[indexB2]);
                    int CDel = dist(path[indexC1], path[indexC2]);

                    int bestCase = 0;
                    int bestScore = 0;

                    int score = dist(path[indexA1], path[indexC1]) + dist(path[indexA2], path[indexC2]);
                    score -= ADel + CDel;
                    if (score <= bestScore)
                    {
                        bestCase = 1;
                        bestScore = score;
                    }

                    score = dist(path[indexB2], path[indexC2]) + dist(path[indexB1], path[indexC1]);
                    score -= BDel + CDel;
                    if (score <= bestScore)
                    {
                        bestCase = 2;
                        bestScore = score;
                    }

                    score = dist(path[indexA1], path[indexB1]) + dist(path[indexA2], path[indexB2]);
                    score -= ADel + BDel;
                    if (score <= bestScore)
                    {
                        bestCase = 3;
                        bestScore = score;
                    }

                    int opt3Del = ADel + BDel + CDel;

                    score = dist(path[indexA1], path[indexB1]) + dist(path[indexA2], path[indexC1]) + dist(path[indexB2], path[indexC2]);
                    score -= opt3Del;
                    if (score <= bestScore)
                    {
                        bestCase = 4;
                        bestScore = score;
                    }

                    score = dist(path[indexA1], path[indexC1]) + dist(path[indexA2], path[indexB2]) + dist(path[indexB1], path[indexC2]);
                    score -= opt3Del;
                    if (score <= bestScore)
                    {
                        bestCase = 5;
                        bestScore = score;
                    }

                    score = dist(path[indexA1], path[indexB2]) + dist(path[indexC2], path[indexA2]) + dist(path[indexB1], path[indexC1]);
                    score -= opt3Del;
                    if (score <= bestScore)
                    {
                        bestCase = 6;
                        bestScore = score;
                    }

                    score = dist(path[indexA1], path[indexB2]) + dist(path[indexA2], path[indexC1]) + dist(path[indexB1], path[indexC2]);
                    score -= opt3Del;
                    if (score <= bestScore)
                    {
                        bestCase = 7;
                        bestScore = score;
                    }

                    switch (bestCase)
                    {
                    case 0:
                        break;
                    case 1:
                        rotate(path.begin(), path.begin() + indexC2, path.end());
                        reverse(path.begin(), path.begin() + indexA2 - indexC2);
                        break;
                    case 2:
                        reverse(path.begin() + indexB2, path.begin() + indexC2);
                        break;
                    case 3:
                        reverse(path.begin() + indexA2, path.begin() + indexB2);
                        break;
                    case 4:
                        reverse(path.begin() + indexA2, path.begin() + indexB2);
                        reverse(path.begin() + indexB2, path.begin() + indexC2);
                        break;
                    case 5:
                        reverse(path.begin() + indexA2, path.begin() + indexB2);
                        rotate(path.begin(), path.begin() + indexC2, path.end());
                        reverse(path.begin(), path.begin() + n - (indexC2 - indexA2));
                        break;
                    case 6:
                        reverse(path.begin() + indexB2, path.begin() + indexC2);
                        rotate(path.begin(), path.begin() + indexC2, path.end());
                        reverse(path.begin(), path.begin() + n - (indexC2 - indexA2));
                        break;
                    case 7:
                        reverse(path.begin() + indexA2, path.begin() + indexB2);
                        reverse(path.begin() + indexB2, path.begin() + indexC2);
                        rotate(path.begin(), path.begin() + indexC2, path.end());
                        reverse(path.begin(), path.begin() + n - (indexC2 - indexA2));
                        break;
                    }
                }
            }
        }
    }
}

#endif