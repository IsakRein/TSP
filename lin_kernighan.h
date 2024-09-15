#ifndef LKH_H
#define LKH_H

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

long long linKernighanDeadline;

void linKernighan(vector<int> &path, long long deadline);
bool linKernighanImprove(vector<int> &path, int &total_iterations);
bool chooseX(vector<int> &path,
             unordered_set<Edge, hashEdge> &X,
             unordered_set<Edge, hashEdge> &Y,
             int t_1,
             int t_2i_minus_1,
             int gain_i);
bool chooseY(vector<int> &path,
             unordered_set<Edge, hashEdge> &X,
             unordered_set<Edge, hashEdge> &Y,
             int t_1,
             int t_2i,
             int gain_i);
bool createPath(vector<int> &path,
                vector<int> &newPath,
                unordered_set<Edge, hashEdge> &X,
                unordered_set<Edge, hashEdge> &Y);

// Main loop of algorithm
void linKernighan(vector<int> &path, long long deadline)
{
    linKernighanDeadline = deadline;
    int total_iterations = 0;
    try
    {
        // Continue while there are improvements
        while (linKernighanImprove(path, total_iterations))
        {
        }
    }

    // Timeout for kattis reached
    catch (const BreakException &e)
    {
    }
}

// Checks that time is before deadline. Throws an exception otherwise.
int checkTimeIndex = 0;
void checkTime()
{
    if (
        checkTimeIndex++ % 500 == 0 &&
        duration_cast<microseconds>(high_resolution_clock::now() - start).count() > linKernighanDeadline)
    {
        throw BreakException();
    }
}

// Find an improvement
bool linKernighanImprove(vector<int> &path, int &total_iterations)
{
    bool improvement = false;

    for (int t_1 = 0; t_1 < n; t_1++)
    {
        checkTime();
        array<int, 2> around = getIndicesAround(t_1);

        // Always choose the option with the largest gain first
        int pred = around[0];
        int succ = around[1];
        if (dist(path[pred], path[t_1]) < dist(path[succ], path[t_1]))
        {
            swap(around[0], around[1]);
        }

        for (int t_2 : around)
        {

            Edge x_1 = {t_1, t_2};
            int gain_1 = dist(path, x_1);

            // Try to create y_1 from the first 60 neighbors of t_2
            int checkIndex = 60;
            
            bool currentImprovement = false;
            for (int t_3_val : neighbors[path[t_2]])
            {
                if (currentImprovement)
                    break;
                // Stop if 60 neighbors have been checked
                if (checkIndex <= 0)
                    break;
                checkIndex--;

                int t_3 = indexOf(t_3_val);

                // Skip if t_2, t_3 already in path
                if (adjacentIndices(t_2, t_3))
                    continue;

                Edge y_i = {t_2, t_3};
                int gain_2 = gain_1 - dist(path, y_i);

                // Skip if no gain
                if (gain_2 <= 0)
                    break;

                // Found good candidates for x_1 and y_1, start with recursion to find more edges.
                unordered_set<Edge, hashEdge> X;
                unordered_set<Edge, hashEdge> Y;
                X.emplace(x_1);
                Y.emplace(y_i);

                if (chooseX(path, X, Y, t_1, t_3, gain_2))
                {
                    currentImprovement = true;
                    improvement = true;
                    break;
                }
            }
        }
    }

    return improvement;
}

// Choose x_i based on previos choices and edges
bool chooseX(vector<int> &path,
             unordered_set<Edge, hashEdge> &X,
             unordered_set<Edge, hashEdge> &Y,
             int t_1,
             int t_2i_minus_1,
             int gain_i)
{
    array<int, 2> around = getIndicesAround(t_2i_minus_1);

    // Always choose the option with the largest gain first
    int pred = around[0];
    int succ = around[1];
    if (dist(path[pred], path[t_2i_minus_1]) < dist(path[succ], path[t_2i_minus_1]))
    {
        swap(around[0], around[1]);
    }

    for (int t_2i : around)
    {
        Edge x_i = {t_2i_minus_1, t_2i};

        // Skip if x_i already removed or already added
        if (contains(X, x_i))
            continue;

        int gain_j = gain_i + dist(path, x_i);

        // Try to connect back to y_1
        Edge y_i = {t_2i, t_1};
        int gain_k = gain_j - dist(path, y_i);
        X.emplace(x_i);

        // If y_i is a valid candidate to connect back
        if (gain_k > 0 && !contains(Y, y_i))
        {
            Y.emplace(y_i);

            // Try to create path based on X and Y
            vector<int> newPath;
            if (createPath(path, newPath, X, Y))
            {
                path = newPath;
                indexPath(path);
                return true;
            }

            // Could not create path. Remove y_i
            Y.erase(y_i);
        }

        // If too deep in recursion, stop searching.
        if (Y.size() > 3)
        {
            X.erase(x_i);
            continue;
        }

        // Could not reconnect to y_1. Continue searching for another y.
        bool foundY = chooseY(path, X, Y, t_1, t_2i, gain_j);
        X.erase(x_i);
        return foundY;
    }
    return false;
}

// Choose y_i based on previos choices and edges
bool chooseY(vector<int> &path,
             unordered_set<Edge, hashEdge> &X,
             unordered_set<Edge, hashEdge> &Y,
             int t_1,
             int t_2i,
             int gain_i)
{
    // How many neighbors to look, depends on size of current solution.
    // Y will always be less than or equal to three here because of a check in chooseX.

    int checkIndex = 3;
    if (Y.size() == 1)
        checkIndex = 20;
    if (Y.size() == 2)
        checkIndex = 5;

    for (int i = 0; i < neighbors[t_2i].size() && checkIndex > 0; i++)
    {
        checkTime();

        int t_2i_plus_1 = indexOf(neighbors[t_2i][i]);

        // Skip if y_i already is in solution
        if (adjacentIndices(t_2i, t_2i_plus_1))
            continue;

        Edge y_i = {t_2i, t_2i_plus_1};
        int gain_j = gain_i - dist(path, y_i);

        // Skip if no gain
        if (!adjacentIndices(t_2i, t_1) && gain_j <= 0 || contains(X, y_i) || contains(Y, y_i))
            continue;

        // Only consider option as "looked at" if all above criteria are met
        checkIndex--;

        // Continue recursion with current y_i
        Y.emplace(y_i);
        if (chooseX(path, X, Y, t_1, t_2i_plus_1, gain_j))
            return true;

        // Could not find solution with this y_i
        Y.erase(y_i);
    }
    return false;
}

// Create a path based on added and removed edges
bool createPath(vector<int> &path,
                vector<int> &newPath,
                unordered_set<Edge, hashEdge> &X,
                unordered_set<Edge, hashEdge> &Y)
{
    // In the beginning, i-1 and i+1 are adjacent to index i.
    for (int i = 0; i < n; i++)
    {
        int a = boundIndex(i - 1);
        int b = boundIndex(i + 1);
        adjacent[i][0] = a;
        adjacent[i][1] = b;
    }

    // Remove all adjacencies from X
    for (Edge edge : X)
    {
        // If the predecesor to a is b: remove predecessor of a and successor of b.
        if (edge.a > edge.b)
        {
            adjacent[edge.a][0] = -1;
            adjacent[edge.b][1] = -1;
        }

        // If the successor to a is b: remove successor of a and predecessor of b.
        else
        {
            adjacent[edge.a][1] = -1;
            adjacent[edge.b][0] = -1;
        }
    }

    // Add all adjecencies from Y
    for (Edge edge : Y)
    {
        // If a has no predecessor, add b as its predecessor
        if (adjacent[edge.a][0] == -1)
            adjacent[edge.a][0] = edge.b;

        // If a has no successor, add b as its successor
        else
            adjacent[edge.a][1] = edge.b;

        // If b has no predecessor, add a as its predecessor
        if (adjacent[edge.b][0] == -1)
            adjacent[edge.b][0] = edge.a;

        // If b has no successor, add a as its successor
        else
            adjacent[edge.b][1] = edge.a;
    }

    // Construct path from edges by walking through the neighbors
    vector<bool> visited(n, false);
    newPath.reserve(n);
    int index = 0; // Start path from arbitrary node

    while (true)
    {
        newPath.emplace_back(path[index]); // Add to path
        visited[index] = true;             // Mark as visited

        // Find next step in path
        int a = adjacent[index][0];
        int b = adjacent[index][1];

        // If predecessor is not visited, go there
        if (!visited[a])
            index = a;
        // If successor is not visited, go there
        else if (!visited[b])
            index = b;
        // If both are visited, break. We have found a loop.
        else
            break;
    }

    // Found a loop before visiting all paths
    if (newPath.size() < n)
        return false;

    return true;
}
#endif