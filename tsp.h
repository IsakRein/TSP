// TSP by Elias Stihl, Elias Ram & Isak Reinholdsson
#ifndef TSP_H
#define TSP_H

#include <iostream>
#include <unordered_set>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <random>
#include <algorithm>
#include <chrono>
#include <list>
#include <array>

using namespace std;
using namespace chrono;

auto randomGenerator = default_random_engine();
high_resolution_clock::time_point start;

struct Coord
{
    float x;
    float y;
};

struct Edge
{
    int a, b;

    Edge(int a, int b) : a(a), b(b) {}

    // Equality comparison
    bool operator==(const Edge &other) const
    {
        return (a == other.a && b == other.b) || (a == other.b && b == other.a);
    }
};

// Globabl variables
int n;
int maxY;
int distanceMatrix[1000][1000];
vector<int> neighbors[1000];
Coord coords[1000];
int indexedPath[1000];
int adjacent[1000][2];

int euclidianDistance(Coord &a, Coord &b)
{
    return (int)(sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2)) + 0.5);
}

int dist(int a, int b)
{
    return distanceMatrix[a][b];
}

int dist(vector<int> &path, Edge e)
{
    return dist(path[e.a], path[e.b]);
}

struct hashEdge
{
public:
    size_t operator()(const Edge &x) const
    {
        return hash<int>()(min(x.a, x.b)) ^ hash<int>()(max(x.a, x.b));
    }
};

int boundIndex(int index)
{
    return (index + n) % n;
}

bool adjacentIndices(int a, int b)
{
    return abs(a - b) <= 1 || abs(a - b) == n - 1;
}

class BreakException : public std::exception
{
    const char *what() const throw()
    {
        return "Breaking out of recursion";
    }
};

array<int, 2> getIndicesAround(int index)
{
    return {boundIndex(index - 1), boundIndex(index + 1)};
}

void indexPath(vector<int> &path)
{
    for (int i = 0; i < n; i++)
    {
        indexedPath[path[i]] = i;
    }
}

int indexOf(int value)
{
    return indexedPath[value];
}

bool contains(unordered_set<Edge, hashEdge> set, Edge element)
{
    return set.find(element) != set.end();
}

struct CompareNodeDistance
{
    int node;

    CompareNodeDistance(int node) : node(node) {}

    int operator()(int a, int b)
    {
        return dist(node, a) < dist(node, b);
    }
};

void readInput()
{
    // Initialize variables
    cin >> n;

    for (int i = 0; i < n; i++)
        cin >> coords[i].x >> coords[i].y;

    // ifstream file("/Users/isak/KTH/AvAlg/TSP/input.txt");
    // file >> n;

    // for (int i = 0; i < n; i++)
    //     file >> coords[i].x >> coords[i].y;

    // file.close();
}

template <typename RandomAccessIterator, typename RNG>
void pseudoRandomShuffle(RandomAccessIterator first, RandomAccessIterator last, RNG &&rng)
{
    if (first == last)
        return;

    for (auto i = (last - first) - 1; i > 0; --i)
    {
        std::uniform_int_distribution<decltype(i)> distribution(0, i);
        auto j = distribution(rng);
        std::swap(first[i], first[j]);
    }
}

void initializeVariables()
{
    // Create distance matrix
    int currentDistance;
    for (int i = 0; i < n; i++)
    {
        neighbors[i] = vector<int>();
        neighbors[i].reserve(n - 1);
        for (int j = 0; j < n; j++)
        {
            currentDistance = euclidianDistance(coords[i], coords[j]);

            distanceMatrix[i][j] = currentDistance;
            distanceMatrix[j][i] = currentDistance;

            // For neighbor list
            if (j != i)
                neighbors[i].emplace_back(j);
        }
    }

    // Create list of closest neighbors
    for (int i = 0; i < n; i++)
    {
        CompareNodeDistance comp(i);
        sort(neighbors[i].begin(), neighbors[i].end(), comp);
    }
}

void printSolution(vector<int> &path)
{
    for (auto &x : path)
    {
        cout << x << "\n";
    }
}

int totalScore(vector<int> &path)
{
    int score = 0;
    for (int i = 1; i < n; i++)
    {
        score += dist(path[i - 1], path[i]);
    }
    score += dist(path[n - 1], path[0]);
    return score;
}

vector<int> bruteForce()
{
    vector<int> path, bestPath;
    int score, bestScore;
    for (int i = 0; i < n; i++)
        path.emplace_back(i);

    bestScore = totalScore(path);
    bestPath = path;

    while (next_permutation(path.begin(), path.end()))
    {
        if (totalScore(path) < bestScore)
        {
            bestScore = totalScore(path);
            bestPath = path;
        }
    }
    return bestPath;
}

vector<int> defaultPath()
{
    vector<int> path(n);
    for (int i = 0; i < n; i++)
        path[i] = i;
    return path;
}

#endif