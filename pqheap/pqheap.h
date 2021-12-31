#pragma once
#include "testing/MemoryUtils.h"
#include "datapoint.h"

/**
 * Priority queue of DataPoints implemented using a binary heap.
 */
class PQHeap {
public:
    /**
     * Creates a new, empty priority queue.
     */
    PQHeap();

    /**
     * Cleans up all memory allocated by this priorty queue.
     */
    ~PQHeap();

    /**
     * Adds a new element into the queue. This operation runs in time O(log n),
     * where n is the number of elements in the queue.
     *
     * @param element The element to add.
     */
    void enqueue(DataPoint element);

    /**
     * Removes and returns the element that is frontmost in the priority queue.
     * The frontmost element is the one with lowest priority value.
     *
     * If the priority queue is empty, this function calls error().
     *
     * This operation must run in time O(log n).
     *
     * @return The frontmost element, which is removed from queue.
     */
    DataPoint dequeue();

    /**
     * Returns, but does not remove, the element that is frontmost.
     *
     * If the priority queue is empty, this function calls error().
     *
     * This operation must run in time O(1).
     *
     * @return frontmost element
     */
    DataPoint peek() const;

    /**
     * Returns whether the priority queue is empty.
     *
     * This operation must run in time O(1).
     *
     * @return Whether the priority queue is empty.
     */
    bool isEmpty() const;

    /**
     * Returns the number of elements in this priority queue.
     *
     * This operation must run in time O(1).
     *
     * @return The number of elements in the priority queue.
     */
    int size() const;

    /**
     * Removes all elements from the priority queue.
     *
     * This operation must run in time O(1).
     */
    void clear();

    /* 
     * This function exists purely for testing purposes. You can have it do whatever you'd
     * like and we won't be invoking it when grading. In the past, students have had this
     * function print out the array representing the heap, or information about how much
     * space is allocated, etc. Feel free to use it as you see fit!
     */
    void printDebugInfo();


private:
    DataPoint* elements; //flattened representation of binary heap
    int allocatedCapacity; //space allocated
    int numItems; //how many slots are filled by DataPoints


    /* This function is responsible for traversing the internal heap array and 
     * ensuring that the heap property holds for all elements. If the heap 
     * property is violated anywhere, you should throw an error or otherwise indicate
     * that something has gone wrong.
     */
    void validateInternalState();


    /* These collection of functions are responsible for doing the math to translate
     * between an index in the array and the indices of the elements parent, left
     * child and right child, respectively.
     */
    int getParentIndex(int curIndex);
    int getLeftChildIndex(int curIndex);
    int getRightChildIndex(int curIndex);

    //this is a helper function for validateInternalState(), it insures minheap properties
    //are not violated by checking whether all of the children of a parent have priorities
    //greater than that of the parent
    bool checkLeftRightChild(int parent);

    //helper function for enqueue which doubles the size of the underlying array if space
    //runs out in the pre-existing array
    void expand();

    //given two indices, this function swaps the DataPoints at those indices
    void swap(int index1, int index2);

    //this is a helper function for dequeue which indicates whether a parent has children with priorities
    //less than the parent's priority. if both children have lower priorities, the index with the lowest
    //priority will be returned. if only one child has a lower priority, it's index will be returned.
    //otherwise, the function returns -1
    int findSwapIndex(int index1, int index2, int parentPriority);

    /* Weird C++isms: C++ loves to make copies of things, which is usually a good thing but
     * for the purposes of this assignment requires some C++ knowledge we haven't yet covered.
     * This next line disables all copy functions to make sure you don't accidentally end up
     * debugging something that isn't your fault.
     *
     * Curious what this does? Take CS106L!
     */
    DISALLOW_COPYING_OF(PQHeap);
};
