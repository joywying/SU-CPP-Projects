//PQSortedArray. Functions are implemented in order
//to create a priority queue using a sorted array.

#include "pqsortedarray.h"
#include "error.h"
#include "random.h"
#include "strlib.h"
#include "datapoint.h"
#include "testing/SimpleTest.h"
using namespace std;

const int INITIAL_CAPACITY = 10;

/*
 * The constructor initializes all of the member variables needed for
 * an instance of the PQSortedArray class. In particular, this
 * function initializes the number of elements stored to be 0, initializes
 * the allocated capacity to be some fixed initial value, and then allocates
 * a new array of that size.
 */
PQSortedArray::PQSortedArray() {
    numItems = 0;
    allocatedCapacity = INITIAL_CAPACITY;
    elements = new DataPoint[allocatedCapacity];
}

/* The destructor is responsible for cleaning up any memory associated
 * with an instance of the PQSortedArray class. In particular, this
 * function deallocates (frees) the array of elements.
 */
PQSortedArray::~PQSortedArray() {
    delete[] elements;
}

//this is a helper function for enqueue which allocates more space to the underlying
//array. the capacity of the array is doubled when expand() is called
void PQSortedArray::expand(){
    DataPoint* newArr = new DataPoint[allocatedCapacity * 2];

    for(int i = 0; i < numItems; i++){
        newArr[i] = elements[i];
    }

    delete[] elements;
    elements = newArr;

    allocatedCapacity *= 2;
}

//this function inserts a DataPoint, elem into a PQSortedArray and ensures that it is placed
//so that the array stays in sorted order
void PQSortedArray::enqueue(DataPoint elem) {
    if (numItems == allocatedCapacity) {
        expand();
    }

    for (int i = 0; i <= numItems; i++){
        if (elements[i].priority < elem.priority || i == numItems) { //if we find a spot, or we reach the end
            for (int j = numItems; j > i; j--){
                elements[j] = elements[j - 1]; //shifts everything over if necessary
            }
            elements[i] = elem;
            numItems++;
            break;
        }
    }

    /* Before finishing, we should validate that our array is
     * still sorted. */
    validateInternalState();
}

/*
 * The current size of the queue is stored internally as the member
 * variable numItems. This function returns that value.
 */
int PQSortedArray::size() const {
    return numItems;
}

/*
 * Since the array elements are stored in decreasing sorted order
 * by priority, the correct element to peek resides in the last slot
 * of the array. This function calculates that location and then returns
 * the element stored there.
 */
DataPoint PQSortedArray::peek() const {
    if (isEmpty()) {
        error("Cannot peek empty pqueue");
    }
    int lastOccupiedSlot = numItems - 1;
    return elements[lastOccupiedSlot];
}

/*
 * Since the array elements are stored in decreasing sorted order
 * by priority, the correct element to dequeue resides in the last slot
 * of the array. This function calculates that location, decrements the
 * number of total items stored in the array and then returns the element
 * from the end of the array.
 */
DataPoint PQSortedArray::dequeue() {
    if (isEmpty()) {
        error("Cannot deqeuue empty pqueue");
    }
    int lastOccupiedSlot = numItems - 1;
    numItems--;
    return elements[lastOccupiedSlot];
}

/*
 * Returns true if there are currently 0 elements in the queue, and
 * false otherwise.
 */
bool PQSortedArray::isEmpty() const {
    return size() == 0;
}

/*
 * Updates internal state to reflect that no elements are stored in the
 * queue any longer. No memory deallocation or clearing of previously
 * stored values is necessary, as those will eventually be overwritten
 * by new values stored in the queue.
 */
void PQSortedArray::clear() {
    numItems = 0;
}

/*
 * Prints out the state of the internal array.
 */
void PQSortedArray::printDebugInfo() {
    for (int i = 0; i < size(); i++) {
        cout << "[" << i << "] = " << elements[i] << endl;
    }
}

/*
 * Traverses the internal elements of the array and validates that the
 * elements are stored in decreasing order of priority. Throws an error if
 * any out of order elements are found. Also throws an error if the number
 * of items stored is greater than the allocated capacity.
 */
void PQSortedArray::validateInternalState(){
    /*
     * If the array only has zero or one elements in it, then they
     * must necessarily be in sorted order.
     */
    if (size() == 0 || size() == 1){
        return;
    }

    /*
     * If there are more elements than spots in the array, we have a problem.
     */
    if (size() > allocatedCapacity) error("Too many elements in not enough space!");


    /* This for loop walks over the elements in the array, comparing
     * neighboring pairs to one another. If the current element has larger
     * priority than the last element, then the array has not been maintained
     * in decreasing sorted order, and we should throw an error.
     */
    DataPoint lastElem = elements[0];
    for (int i = 1; i < size(); i++){
        DataPoint cur = elements[i];
        if (cur.priority > lastElem.priority){
            error("Internal state of the array is out of order!");
        }
        lastElem = cur;
    }
}

/* * * * * * Test Cases Below This Point * * * * * */

static void emptyQueue(PQSortedArray& pq, int n);
static void fillQueue(PQSortedArray& pq, int n);

//STUDENT_TEST("time enqueue and dequeue"){
//    PQSortedArray pq;


//    TIME_OPERATION(10000, fillQueue(pq, 10000));
//    TIME_OPERATION(10000, emptyQueue(pq, 10000));

//    TIME_OPERATION(20000, fillQueue(pq, 20000));
//    TIME_OPERATION(20000, emptyQueue(pq, 20000));

//    TIME_OPERATION(30000, fillQueue(pq, 30000));
//    TIME_OPERATION(30000, emptyQueue(pq, 30000));

//    TIME_OPERATION(50000, fillQueue(pq, 50000));
//    TIME_OPERATION(50000, emptyQueue(pq, 50000));

//    TIME_OPERATION(60000, fillQueue(pq, 60000));
//    TIME_OPERATION(60000, emptyQueue(pq, 60000));

//}

STUDENT_TEST("test expand by adding a new element and then dequeuing"){
    PQSortedArray pq;
    fillQueue(pq, 11);
    pq.enqueue({ "", -30});
    DataPoint expected = {"", -30};
    EXPECT_EQUAL(pq.dequeue(), expected);
}

STUDENT_TEST("test expand twice"){
    PQSortedArray pq;
    fillQueue(pq, 11);
    fillQueue(pq, 10);
    pq.enqueue({ "", -30});
    DataPoint expected = {"", -30};
    EXPECT_EQUAL(pq.dequeue(), expected);
}

STUDENT_TEST("Test if long random sequence is sorted") {
    PQSortedArray pq;

    for (int i = 0; i < 100; i++) {
        int randomValue = randomInteger(-100, 100);
        DataPoint pt = {"elem" + integerToString(randomValue), randomValue};
        pq.enqueue(pt);
    }

    DataPoint data = {"elem", -200};
    pq.enqueue(data);

    EXPECT_EQUAL(pq.dequeue(), data);

}

STUDENT_TEST("enqueue and dequeue alternating positive to negative") {
    PQSortedArray pq;

    for (int i = 0; i < 100; i++) {
        int randomValue = randomInteger(-200, -50);
        int randomValue2 = randomInteger(100, 300);
        DataPoint point = {"", randomValue};
        DataPoint point2 = {"", randomValue2};
        pq.enqueue(point);
        pq.enqueue(point2);
    }

    for (int i = 0; i < 50; i++) {
        pq.dequeue();
    }
    DataPoint data = {"elem", -200};

    pq.enqueue(data);


    EXPECT_EQUAL(pq.dequeue(), data);
}

STUDENT_TEST("enqueue two different numbers"){
    PQSortedArray pq;

    for (int i = 0; i < 20; i++){
        DataPoint point = {"", 2};
        pq.enqueue(point);
    }

    for (int i = 0; i < 20; i++){
        DataPoint point = {"", -2};
        pq.enqueue(point);
    }


    DataPoint expected = {"", -2};
    EXPECT_EQUAL(pq.dequeue(), expected);

    for (int i = 0; i < 30; i++){
        DataPoint point = pq.dequeue();
    }

    expected = {"", 2};
    EXPECT_EQUAL(pq.dequeue(), expected);

}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Provided Test: Newly-created heap is empty.") {
    PQSortedArray pq;

    EXPECT(pq.isEmpty());
    EXPECT(pq.size() == 0);
}

PROVIDED_TEST("Provided Test: Enqueue / dequeue single element (two cycles)") {
    PQSortedArray pq;
    DataPoint point = { "Programming Abstractions", 106 };
    pq.enqueue(point);
    EXPECT_EQUAL(pq.size(), 1);
    EXPECT_EQUAL(pq.isEmpty(), false);

    EXPECT_EQUAL(pq.peek(), point);
    EXPECT_EQUAL(pq.dequeue(), point);
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);

    pq.enqueue(point);
    EXPECT_EQUAL(pq.size(), 1);
    EXPECT_EQUAL(pq.isEmpty(), false);

    EXPECT_EQUAL(pq.peek(), point);
    EXPECT_EQUAL(pq.dequeue(), point);
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Provided Test: Test clear operation works with single element."){
    PQSortedArray pq;
    DataPoint point = { "Programming Abstractions", 106 };

    pq.enqueue(point);
    pq.clear();
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Provided Test: Dequeue / peek on empty priority queue throws error") {
    PQSortedArray pq;

    EXPECT(pq.isEmpty());
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());
}

PROVIDED_TEST("Provided Test: Dequeue / peek on recently-cleared priority queue throws error") {
    PQSortedArray pq;
    DataPoint point = { "Programming Abstractions", 106 };

    pq.enqueue(point);
    pq.clear();
    EXPECT(pq.isEmpty());
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());
}

PROVIDED_TEST("Provided Test: Enqueue elements in sorted order.") {
    PQSortedArray pq;
    for (int i = 0; i < 25; i++) {
        pq.enqueue({ "elem" + integerToString(i), i });
    }

    EXPECT_EQUAL(pq.size(), 25);
    for (int i = 0; i < 25; i++) {
        DataPoint removed = pq.dequeue();
        DataPoint expected = {
            "elem" + integerToString(i), i
        };
        EXPECT_EQUAL(removed, expected);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Provided Test: Enqueue elements in reverse-sorted order.") {
    PQSortedArray pq;
    for (int i = 25; i >= 0; i--) {
        pq.enqueue({ "elem" + integerToString(i), i });
    }

    EXPECT_EQUAL(pq.size(), 26);
    for (int i = 0; i <= 25; i++) {
        DataPoint removed = pq.dequeue();
        DataPoint expected = {
            "elem" + integerToString(i), i
        };
        EXPECT_EQUAL(removed, expected);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Provided Test: Insert ascending and descending sequences.") {
    PQSortedArray pq;
    for (int i = 0; i < 20; i++) {
        pq.enqueue({ "a" + integerToString(i), 2 * i });
    }
    for (int i = 19; i >= 0; i--) {
        pq.enqueue({ "b" + integerToString(i), 2 * i + 1 });
    }

    EXPECT_EQUAL(pq.size(), 40);
    for (int i = 0; i < 40; i++) {
        DataPoint removed = pq.dequeue();
        EXPECT_EQUAL(removed.priority, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Provided Test: Insert random sequence of elements.") {
    Vector<DataPoint> sequence = {
        { "A", 0 },
        { "D", 3 },
        { "F", 5 },
        { "G", 6 },
        { "C", 2 },
        { "H", 7 },
        { "I", 8 },
        { "B", 1 },
        { "E", 4 },
        { "J", 9 },
    };

    PQSortedArray pq;
    for (DataPoint elem: sequence) {
        pq.enqueue(elem);
    }

    EXPECT_EQUAL(pq.size(), sequence.size());

    for (int i = 0; i < 10; i++) {
        DataPoint removed = pq.dequeue();
        DataPoint expected = {
            charToString('A' + i), i
        };
        EXPECT_EQUAL(removed, expected);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}


PROVIDED_TEST("Provided Test: Insert duplicate elements.") {
    PQSortedArray pq;
    for (int i = 0; i < 20; i++) {
        pq.enqueue({ "a" + integerToString(i), i });
    }
    for (int i = 19; i >= 0; i--) {
        pq.enqueue({ "b" + integerToString(i), i });
    }

    EXPECT_EQUAL(pq.size(), 40);
    for (int i = 0; i < 20; i++) {
        DataPoint one = pq.dequeue();
        DataPoint two = pq.dequeue();

        EXPECT_EQUAL(one.priority, i);
        EXPECT_EQUAL(two.priority, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Provided Test: Handles data points with empty string name.") {
    PQSortedArray pq;
    for (int i = 0; i < 10; i++) {
        pq.enqueue({ "" , i });
    }
    EXPECT_EQUAL(pq.size(), 10);
}

PROVIDED_TEST("Test enqueue/dequeue of longer random sequence") {
    PQSortedArray pq;

    for (int i = 0; i < 100; i++) {
        int randomValue = randomInteger(0, 100);
        DataPoint pt = {"elem" + integerToString(randomValue), randomValue};
        pq.enqueue(pt);
    }
    EXPECT_EQUAL(pq.size(), 100);
    DataPoint last = {"", -1};
    for (int i = 0; i < 100; i++) {
        DataPoint cur = pq.dequeue();
        EXPECT(cur.priority >= 0 && cur.priority <= 100 && cur.priority >= last.priority);
        last = cur;
    }
    EXPECT_EQUAL(pq.size(), 0);
}


PROVIDED_TEST("Provided Test: Handles data points with negative weights.") {
    PQSortedArray pq;
    for (int i = -10; i < 10; i++) {
        pq.enqueue({ "" , i });
    }
    EXPECT_EQUAL(pq.size(), 20);
    for (int i = -10; i < 10; i++) {
        DataPoint removed = pq.dequeue();
        EXPECT_EQUAL(removed.priority, i);
    }
}

PROVIDED_TEST("Provided Test: Interleave enqueues and dequeues.") {
    PQSortedArray pq;
    int n = 100;
    for (int i = n / 2; i < n; i++) {
        pq.enqueue({"", i});
    }
    EXPECT_EQUAL(pq.size(), 50);
    for (int i = n / 2; i < n; i++) {
        EXPECT_EQUAL(pq.dequeue().priority, i);
    }
    EXPECT_EQUAL(pq.size(), 0);

    for (int i = 0; i < n / 2; i++) {
        pq.enqueue({"", i});
    }
    EXPECT_EQUAL(pq.size(), 50);
    for (int i = 0; i < n / 2; i++) {
        EXPECT_EQUAL(pq.dequeue().priority, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
}

static void fillAndEmpty(int n) {
    PQSortedArray pq;
    DataPoint max = {"max", 106106106};
    DataPoint min = {"min", -106106106};

    pq.enqueue(min);
    pq.enqueue(max);
    for (int i = 0; i < n; i++) {
        int randomPriority = randomInteger(-10000, 10000);
        pq.enqueue({ "", randomPriority });
    }
    EXPECT_EQUAL(pq.size(), n + 2);

    EXPECT_EQUAL(pq.dequeue(), min);
    for (int i = 0; i < n; i++) {
        pq.dequeue();
    }
    EXPECT_EQUAL(pq.dequeue(), max);
    EXPECT_EQUAL(pq.size(), 0);
}

//PROVIDED_TEST("Provided Test: Stress Test. Time the amount of time it takes to cycle many elements in and out. Should take at most a couple seconds.") {
//    TIME_OPERATION(10000, fillAndEmpty(10000));
//}

static void fillQueue(PQSortedArray& pq, int n){
    for (int i = 0; i < n; i++){
        pq.enqueue({ "", i });
    }
}

static void emptyQueue(PQSortedArray& pq, int n){
    for (int i = 0; i < n; i++){
        pq.dequeue();
    }
}

//PROVIDED_TEST("Provided Test: Introductory test for timing analysis. Uses both fillQueue and emptyQueue functions."){
//    PQSortedArray pq;
//    TIME_OPERATION(10000, fillQueue(pq, 10000));
//    TIME_OPERATION(10000, emptyQueue(pq, 10000));
//}


