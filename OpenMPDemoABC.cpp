#include <algorithm>
#include <iostream>
#include <string>
#include <omp.h>

class SharedArray {
    private:
        char* array;
        int size, index;
        bool useMutex;
    
        void spendSomeTime() {
            for (int i = 0; i < 10000; i++) {
                for (int j = 0; j < 100; j++) {
                    // These loops shouldn't be removed by the compiler
                }
            }
        }

    public:
        SharedArray(int n, bool use) : size(n), index(0), useMutex(use) {
            array = new char[size];
            std::fill(array, array+size, '-');
        }
        
        ~SharedArray() {
            delete[] array;
        }
        
        void addChar(char c) {
            if (useMutex) {
                #pragma omp ordered
                {
                    array[index] = c;
                    spendSomeTime();
                    index++;
                }
            }
            else {
                array[index] = c;
                spendSomeTime();
                index++;
            }
        }
        
        int countOccurrences(char c) {
            return std::count(array, array + size, c);
        }
        
        std::string toString() {
            return std::string(array, size);
        }
};


class ArrayFiller {
    private:
        static const int num_threads = 4;
        static const int num_times = 20;
        SharedArray* array;

    public:
        ArrayFiller(bool useMutex) {
            array = new SharedArray(num_threads * num_times, useMutex);
        }

        void fillArrayConcurrently() {
            #pragma omp parallel for schedule(runtime) num_threads(num_threads) ordered
                for(int i = 0; i < num_threads * num_times; i++) {
                    array->addChar('A' + omp_get_thread_num());
                }
        }

        void fillArrayConcurrently(omp_sched_t schedule_type, int chunk_size) {
            omp_set_schedule(schedule_type, chunk_size);

            #pragma omp parallel for schedule(runtime) num_threads(num_threads) ordered
                for(int i = 0; i < num_threads * num_times; i++) {
                    array->addChar('A' + omp_get_thread_num());
                }
        }

        void printStats() {
            std::cout << array->toString() << std::endl;
            for (int i = 0; i < num_threads; ++i)
                std::cout << (char) ('A' + i) << "=" << array->countOccurrences('A' + i) << " ";
            std::cout << std::endl;
        }

        ~ArrayFiller() {
            delete array;
        }
};

int main() {
    std::cout << "Each thread should add its char to the array n times (n=20)" << std::endl;
    std::cout << "Correct results should total exactly nThreads * nTimes chars" << std::endl;

    std::cout << "\nNo mutex and schedule = runtime (expecting wrong results)" << std::endl;
    ArrayFiller m1(false);
    m1.fillArrayConcurrently();
    m1.printStats();
    
    std::cout << "\nSame as previous (possibly getting different results due to scheduling)" << std::endl;
    ArrayFiller m2(false);
    m2.fillArrayConcurrently();
    m2.printStats();
    
    std::cout << "\nUsing mutex and schedule = runtime (expecting correct results)" << std::endl;
    ArrayFiller m3(true);
    m3.fillArrayConcurrently();
    m3.printStats();

    omp_sched_t schedule_types[] = {
        omp_sched_static,
        omp_sched_dynamic,
        omp_sched_auto,
        omp_sched_guided
    };

    char schedule_names[][20] = {
        "omp_sched_static",
        "omp_sched_dynamic",
        "omp_sched_auto",
        "omp_sched_guided"
    };

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j <= 4; j += 4) {
            ArrayFiller array_filler(true);

            std::cout << "\nUsing mutex, schedule = " << schedule_names[i] << " and chunk_size = " << (j == 0 ? "default" : std::to_string(j)) << " (expecting correct results)" << std::endl;
            array_filler.fillArrayConcurrently(schedule_types[i], j);
            array_filler.printStats();
        }
        
    }
}
