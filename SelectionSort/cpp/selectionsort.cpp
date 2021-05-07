/*****************
 Rich Kisic
 http://github.com/richkisic
 richard.kisic@gmail.com

https://en.wikipedia.org/wiki/Selection_sort:
In computer science, selection sort is an in-place comparison sorting algorithm. It has an O(n2) time complexity, 
which makes it inefficient on large lists, and generally performs worse than the similar insertion sort. Selection 
sort is noted for its simplicity and has performance advantages over more complicated algorithms in certain situations, 
particularly where auxiliary memory is limited.

The algorithm divides the input list into two parts: a sorted sublist of items which is built up from left to right at 
the front (left) of the list and a sublist of the remaining unsorted items that occupy the rest of the list. Initially, 
the sorted sublist is empty and the unsorted sublist is the entire input list. The algorithm proceeds by finding the smallest 
(or largest, depending on sorting order) element in the unsorted sublist, exchanging (swapping) it with the leftmost unsorted 
element (putting it in sorted order), and moving the sublist boundaries one element to the right.
 ******************/

#include <cstdlib>
#include <iostream>
#include <array>
#include <vector>
#include <chrono>
#include <thread>
#include <future>        

using namespace std;

template <size_t N>
void charSelectionSort(std::array<char, N>& chars)
{
    int newMinIndex;
    char leftChar;

    for(int i=0; i<chars.size()-1; i++)
    {
        newMinIndex=i;
        for(int j=i+1; j<chars.size(); j++)
        {
            if(chars[j]<chars[newMinIndex])
            {
                newMinIndex = j;
            }
        } 

        if(i != newMinIndex)
        {
            // swap
            leftChar = chars[i];
            chars[i] = chars[newMinIndex];
            chars[newMinIndex] = leftChar;
        }
    }
}

int main()
{
    // one megabyte of characters
    std::array<char, 4096> alphabet; // caution: increasing the size will cause some sort algorithms to exponentially grow in time
    std::chrono::steady_clock::time_point begin; // used for profiling each completed sort
    std::chrono::steady_clock::time_point end;
    std::vector<double> timesPerElementInNanoseconds; // used to store sort time per element for each completed sort, which will allow us to calc average 

    int ctrOfProfilingIterations=0;
    do
    {    
        // use promises/futures to check if our current threaded sort is still running
        std::packaged_task<void()> sortTask([&alphabet, &begin, &end] {
            begin = std::chrono::steady_clock::now();
            charSelectionSort(alphabet);
            end = std::chrono::steady_clock::now();
        });
        auto future = sortTask.get_future();

        // fill the character array
        std::srand(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count()); // use current time as seed for random generator
        for(int i=0;i<alphabet.size(); i++)
        {
            alphabet[i] = 'a' + std::rand()%26; // a-z
        }
        
        // perform the selection sort
        std::thread t(std::move(sortTask));
        do
        {
            cout << ".";
        } while (future.wait_for(100ms) != std::future_status::ready);
        
        t.join(); // we must join the thread or we will abort for losing track of a running thread

        timesPerElementInNanoseconds.push_back(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() / double(alphabet.size()));
    } while(++ctrOfProfilingIterations<10);

    cout << "done";
}

