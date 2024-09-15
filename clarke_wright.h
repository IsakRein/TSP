#ifndef CLARKE_WRIGHT_H
#define CLARKE_WRIGHT_H
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
#include "tsp.h"

using namespace std;
using namespace chrono;

struct WeightedEdge
{
    int x;
    int y;
    int distance;
};

bool compareDist(const WeightedEdge &a, const WeightedEdge &b)
{
    return a.distance > b.distance;
}

void insertEdge(WeightedEdge &saving, vector<int> *neighbors, vector<list<int>> &subTours, unordered_set<int> &unvisited)
{

    bool inserted = false;
    list<int> *firstTour = nullptr;
    list<int> *secondTour = nullptr;

    for (list<int> &tour : subTours)
    {
        int start = tour.front();
        int end = tour.back();
        // potential tours
        if (start == saving.x || start == saving.y || end == saving.x || end == saving.y)
        {
            if (firstTour == nullptr)
            {
                firstTour = &tour;
            }
            else
            {
                secondTour = &tour;
            }
        }
    }

    // No existing tour to add to
    if ((firstTour == nullptr && secondTour == nullptr) && (unvisited.count(saving.x) && unvisited.count(saving.y)))
    {
        subTours.push_back({saving.x, saving.y});
        neighbors[saving.y].push_back(saving.x);
        neighbors[saving.x].push_back(saving.y);
    }

    // We have found a single tour which the edge can be connected to
    else if (firstTour != nullptr && secondTour == nullptr)
    {
        int start = firstTour->front();
        int end = firstTour->back();
        // saving.x is only connected to hub
        if (neighbors[saving.x].size() == 1)
        {
            if (start == saving.y)
            {
                firstTour->push_front(saving.x);
                neighbors[saving.x].push_back(start);
                neighbors[start].push_back(saving.x);
                // remove hub edge
                neighbors[start].erase(remove(neighbors[start].begin(), neighbors[start].end(), 0), neighbors[start].end());
                neighbors[0].erase(remove(neighbors[0].begin(), neighbors[0].end(), start), neighbors[0].end());
                unvisited.erase(start);
            }
            else if (end == saving.y)
            {
                firstTour->push_back(saving.x);
                neighbors[saving.x].push_back(end);
                neighbors[end].push_back(saving.x);
                // remove hub edge
                neighbors[end].erase(remove(neighbors[end].begin(), neighbors[end].end(), 0), neighbors[end].end());
                neighbors[0].erase(remove(neighbors[0].begin(), neighbors[0].end(), end), neighbors[0].end());
                unvisited.erase(end);
            }
        }

        // saving.y is only connected to hub
        if (neighbors[saving.y].size() == 1)
        {
            if (start == saving.x)
            {
                firstTour->push_front(saving.y);
                neighbors[saving.y].push_back(start);
                neighbors[start].push_back(saving.y);
                // remove hub edge
                neighbors[start].erase(remove(neighbors[start].begin(), neighbors[start].end(), 0), neighbors[start].end());
                neighbors[0].erase(remove(neighbors[0].begin(), neighbors[0].end(), start), neighbors[0].end());
                unvisited.erase(start);
            }
            else if (end == saving.x)
            {

                firstTour->push_back(saving.y);
                neighbors[saving.y].push_back(end);
                neighbors[end].push_back(saving.y);
                // remove hub edge
                neighbors[end].erase(remove(neighbors[end].begin(), neighbors[end].end(), 0), neighbors[end].end());
                neighbors[0].erase(remove(neighbors[0].begin(), neighbors[0].end(), end), neighbors[0].end());
                unvisited.erase(end);
            }
        }
    }
    // We have found two tours which should connect
    else if (firstTour != nullptr && secondTour != nullptr)
    {
        if ((saving.x == firstTour->front() && saving.y == secondTour->front()) || (saving.y == firstTour->front() && saving.x == secondTour->front()))
        {
            secondTour->reverse();
            secondTour->splice(secondTour->end(), *firstTour);
        }
        else if ((saving.x == firstTour->back() && saving.y == secondTour->back()) || (saving.y == firstTour->back() && saving.x == secondTour->back()))
        {
            secondTour->reverse();
            firstTour->splice(firstTour->end(), *secondTour);
        }
        else if ((saving.x == firstTour->back() && saving.y == secondTour->front()) || (saving.y == firstTour->back() && saving.x == secondTour->front()))
        {
            firstTour->splice(firstTour->end(), *secondTour);
        }
        else if ((saving.x == firstTour->front() && saving.y == secondTour->back()) || (saving.y == firstTour->front() && saving.x == secondTour->back()))
        {
            secondTour->splice(secondTour->end(), *firstTour);
        }

        neighbors[saving.y].push_back(saving.x);
        neighbors[saving.x].push_back(saving.y);
        neighbors[saving.x].erase(remove(neighbors[saving.x].begin(), neighbors[saving.x].end(), 0), neighbors[saving.x].end());
        neighbors[saving.y].erase(remove(neighbors[saving.y].begin(), neighbors[saving.y].end(), 0), neighbors[saving.y].end());
        neighbors[0].erase(remove(neighbors[0].begin(), neighbors[0].end(), saving.y), neighbors[0].end());
        neighbors[0].erase(remove(neighbors[0].begin(), neighbors[0].end(), saving.x), neighbors[0].end());
        unvisited.erase(saving.x);
        unvisited.erase(saving.y);
    }
}

vector<int> clarkeWright()
{
    vector<int> path;
    // initialize set, all nodes except hub
    unordered_set<int> unvisited;
    for (int i = 1; i < n; ++i)
    {
        unvisited.insert(i);
    }
    // Set first coord as hub
    mt19937 generator(0);
    uniform_int_distribution<> distribution(0, n - 1);
    int hub = distribution(generator);

    // Compute starting costs
    int startingCost[n];
    startingCost[0] = 0;
    for (size_t i = 1; i < n; ++i)
    {
        startingCost[i] = dist(hub, i);
    }

    // Compute savings
    vector<WeightedEdge> savings;
    for (int i = 1; i < n; ++i)
    {
        for (int j = i; j < n; ++j)
        {
            if (i != j)
            {
                savings.push_back({i, j, startingCost[i] + startingCost[j] - dist(i, j)});
            }
        }
    }

    // sort savings in decreasing order
    sort(savings.begin(), savings.end(), compareDist);

    vector<list<int>> subTours;
    vector<int> neighbors[n];

    // every vertex is neihgbour with hub from beginning
    for (size_t i = 1; i < n; ++i)
    {
        neighbors[i].push_back(0);
    }
    for (size_t i = 1; i < n; ++i)
    {
        neighbors[0].push_back(i);
    }

    if (unvisited.size() <= 2)
    {
        for (size_t i = 0; i < n; i++)
        {
            path.push_back(i);
        }
    }

    while (unvisited.size() > 2)
    {
        for (WeightedEdge &saving : savings)
        {
            insertEdge(saving, neighbors, subTours, unvisited);
        }
    }

    for (list<int> &subTour : subTours)
    {
        if (subTour.size() != 0)
        {
            subTour.push_front(0);
            for (int i : subTour)
            {
                path.push_back(i);
            }
        }
    }
    return path;
}

#endif
