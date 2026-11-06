// benchmark.cpp - experimental timing for section  5.2
// place in the same folder  as the project  headers.
// compile: g++ -std=c++17 -o2 benchmark.cpp -o  benchmark
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <cstdlib>
#include "floodreport.h"
#include "dynamicarray.h"
#include "queue.h"
#include "stack.h"
#include "mergesort.h"
#include "binarysearch.h"
#include "linearsearch.h"

using Clock   =  std::chrono::steady_clock;

const int RUNS = 5;                       // repetitions per input size
const int SIZES[] = {100, 1000, 10000, 100000};
const int NUM_SIZES  =   4;
const int NUM_STREETS = 50;               // distinct street names in test data

bool higherSeverityFirst(const FloodReport& a, const FloodReport& b) {
    return a.severity > b.severity;
}
int getId(const FloodReport& r) { return r.id; }
std::string getStreetLower(const FloodReport& r) { return toLowerStr(r.street); }

double elapsedMs(Clock::time_point start, Clock::time_point end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// same  data for every algorithm: fixed seed, ids go up by 1  like the  real system
void fillData(DynamicArray<FloodReport>& archive, int n) {
    std::srand(12345);
    for (int i = 0; i < n; i++) {
        FloodReport r;
        r.id = 101+i;
        r.street = "Street " + std::to_string(std::rand() % NUM_STREETS);
        r.severity = 1 + std::rand() % 4; 
        r.dateTime = "2026-10-27 08:00";
        archive.push_back(r);
    }
}

// same logic as  floodsystem::removefromqueue in main.cpp (queue operations  only)
void removeFromQueue(qNode<int>*& front, qNode<int>*& back, int id) {
    int n = 0;
    for (qNode<int>* c = front; c != nullptr; c = c->next) n++;
    for (int i = 0; i < n; i++) {
        int x = 0;
        dequeue(&front, &back, x);
        if (x != id) enqueue(&front, &back, x);
    }
}

int main() {
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Average time per operation, " << RUNS << " runs per size (ms)\n\n";
    std::cout << std::left << std::setw(10) << "n"
              << std::setw(15) << "BinarySearch"
              << std::setw(15) << "LinearSearch"
              << std::setw(15) << "MergeSort"
              << std::setw(18) << "Enqueue(n total)"
              << std::setw(16) << "Push(n total)"
              << std::setw(16) << "Pop(n total)"
              << std::setw(15) << "MarkFixed(1)" << "\n";

    volatile int sink = 0;   // stops the compiler from removing the work

    for (int s = 0; s < NUM_SIZES; s++) {
        int n = SIZES[s];
        DynamicArray<FloodReport> archive;
        fillData(archive, n);

        double binTotal = 0, linTotal = 0, sortTotal = 0, enqTotal = 0;
        double pushTotal = 0, popTotal = 0, markTotal = 0;

        for (int run = 0; run < RUNS; run++) {  
            // 1. binary search: many lookups, then divide (one lookup is too fast to time)
            const int BIN_REPS  =   100000;
            auto t0 = Clock::now();
            for (int k = 0; k < BIN_REPS; k++) {
                int id =  101 + (k * 7919) % n;
                sink = sink + binarySearch(archive.data(), n, id, getId);
            }
            auto t1   =  Clock::now();
            binTotal += elapsedMs(t0, t1) / BIN_REPS;

            // 2. linear search by street (all  matches)
            const int LIN_REPS = (n >= 10000) ? 20 : 500;
            int* matches   = new int[n];
            std::string key = "street 7";
            t0 = Clock::now();
            for (int k = 0; k < LIN_REPS; k++) {
                sink = sink + linearSearchAll(archive.data(), n, key, getStreetLower, matches);
            }
            t1 = Clock::now();
            linTotal += elapsedMs(t0, t1) / LIN_REPS;
            delete[] matches;

            // 3. merge sort by  severity (copy first, time  only  the  sort)
            FloodReport* copy = new FloodReport[n];
            for (int i = 0; i < n; i++) copy[i] = archive[i];
            t0 =  Clock::now();
            mergeSort(copy, n, higherSeverityFirst);
            t1 = Clock::now();
            sortTotal += elapsedMs(t0, t1);
            sink = sink + copy[0].severity;
            delete[] copy;

            // 4. enqueue n report  ids (total time  for all n)
            qNode<int>* front = nullptr; 
            qNode<int>* back = nullptr;
            t0 = Clock::now();
            for (int i = 0; i < n; i++) enqueue(&front, &back, 101 + i);
            t1 = Clock::now();
            enqTotal += elapsedMs(t0, t1);

            // 5. mark  as  fixed: removefromqueue  on  a queue of  n ids (one call, middle id)
            //    the removed id is put back  afterwards (not timed) so  the  queue stays size n.
            const int MARK_REPS = (n >= 10000) ? 5 : 50; 
            double markSum = 0;
            for (int k = 0; k < MARK_REPS; k++) {  
                int id = 101 + n / 2;
                t0 = Clock::now();
                removeFromQueue(front, back, id);
                t1 = Clock::now();
                markSum += elapsedMs(t0, t1);
                enqueue(&front, &back, id);
            }
            markTotal += markSum / MARK_REPS;
            clearQueue(&front, &back);

            // 6. stack: push n  report ids, then pop them  all (total  time for each)
            sNode<int>* top = nullptr;
            t0 = Clock::now();
            for (int i = 0; i < n; i++) push(&top, 101 + i);
            t1 = Clock::now();
            pushTotal += elapsedMs(t0, t1);

            int popped = 0;
            t0 = Clock::now();
            for (int i = 0; i < n; i++) { pop(&top, popped); sink = sink + popped;  }
            t1 = Clock::now();
            popTotal += elapsedMs(t0, t1);
            clearStack(&top);
        }

        std::cout << std::left << std::setw(10) << n
                  << std::setw(15) << binTotal / RUNS
                  << std::setw(15) << linTotal / RUNS
                  << std::setw(15) << sortTotal / RUNS
                  << std::setw(18) << enqTotal / RUNS
                  << std::setw(16) << pushTotal / RUNS
                  << std::setw(16) << popTotal/RUNS
                  << std::setw(15) << markTotal / RUNS << "\n";
    }
    return 0;
}