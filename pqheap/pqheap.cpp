
//functions are implemented in order to create a priority queue
//which is stored as a binary heap.

//**Important Note** when the comments reference "smaller" or "larger" priorities,
//this is referring to the *numerical* priority values, not the actual priority of
//the DataPoint, which we can infer by looking at its numerical priority

#include "pqheap.h"
#include "error.h"
#include "random.h"
#include "strlib.h"
#include "datapoint.h"
#include "testing/SimpleTest.h"
using namespace std;

const int INITIAL_CAPACITY = 10;

//this constructor initializes the private variables
//an empty array with an allocated capacity of 10 is created
PQHeap::PQHeap() {
    numItems = 0;
    allocatedCapacity = INITIAL_CAPACITY;
    elements = new DataPoint[allocatedCapacity];
}

//this destructor deallocates the memory allocated to the elements
//array in the constructor
PQHeap::~PQHeap() {
    delete[] elements;
}

//this is a helper function for enqueue which allocates more space to
//the elements array. the allocated capacity is doubled and nothing is
//returned
void PQHeap::expand(){
    DataPoint* newArr = new DataPoint[allocatedCapacity * 2];

    for (int i = 0; i < numItems; i++){
        newArr[i] = elements[i];
    }

    delete[] elements;
    elements = newArr;

    allocatedCapacity *= 2;
}

//this function swaps DataPoints at two indices. It is given two indices
//and does not return anything
void PQHeap::swap(int index1, int index2){
    DataPoint data = elements[index2];
    DataPoint helper = elements[index1];
    elements[index1] = data;
    elements[index2] = helper;
}

//this function places an element in a binary heap given the DataPoint elem
//if the array is already "full" more space is allocated via expand()
//otherwise, the element is placed into the binary heap and it bubbles up
//until minheap heap properties are satisfied
void PQHeap::enqueue(DataPoint elem) {
    if (numItems == allocatedCapacity) {
        expand();
    }
    int index = numItems;

    elements[numItems] = elem; //element is added to the end
    numItems++;

    int parentIndex = getParentIndex(index);
    while (getParentIndex(index) != -1) { //there are still parents to check
        parentIndex = getParentIndex(index);
        if (elements[parentIndex].priority > elem.priority) { //if heap properties are violated, swap
            swap(parentIndex, index);

        }
        else{
            break;
        }
        index = parentIndex;
    }


    validateInternalState(); //make sure minheap properties are preserved


}

//this function returns but does not remove the top element in the binary heap
//this should be the element with the highest priority
DataPoint PQHeap::peek() const {
    if (isEmpty()) {
        error("Cannot peek empty pqheap");
    }
    return elements[0];
}

//helper function for dequeue(). given two indices, if both hold datapoints
//with smaller priority values compared to the parent's priority, the
//index of the smallest child is returned. if one of the two children is
//smaller, it returns that index, if none of the children have smaller priorites
//it returns -1
int PQHeap::findSwapIndex(int index1, int index2, int parentPriority){
    if (index1 != -1 && index2 != -1) { //both children exist
        int priority1 = elements[index1].priority;
        int priority2 = elements[index2].priority;

        if (priority1 < parentPriority && (priority1 <= priority2)) { //if the first child has a smaller(or equal) priority than the second and parent
                return index1;
        }
        else if (priority2 < parentPriority && (priority2 < priority1)) { //if the second child has a smaller priority than the first and parent
            return index2;
        }
    }
    else if (index1 != -1 && index2 == -1) { //only one child exists
        int priority1 = elements[index1].priority;
        if (priority1 < parentPriority) {
            return index1;
        }
    }
    return -1; //none of the existing children have smaller priorities

}

//this function removes and returns an element from the top of the binary heap
//the last element in the heap then replaces the top element and then bubbles
//down until minheap properties are satisfied. this function is made simpler
//with the helper function findSwapIndex as well as swap
DataPoint PQHeap::dequeue() {
    if (numItems == 0) {
        error("cannot dequeue from empty PQHeap");
    }

    DataPoint returnValue = elements[0]; //get the top element, last element is moved to top
    elements[0] = elements[numItems - 1];
    int priorityVal = elements[0].priority;
    numItems--;

    int index = 0;

    while (getLeftChildIndex(index) != -1) { //while there are still children to check
        int swapIndex = findSwapIndex(getLeftChildIndex(index), getRightChildIndex(index), priorityVal);

        if (swapIndex != -1) { //swapIndex will tell us which child needs to swap places with the current parent
            swap(swapIndex, index);
            index = swapIndex;
        }
        else { //if swapIndex is -1, we know that there is no more swapping to do
            break;
        }
    }

    validateInternalState(); //added this to make sure minheap properties are preserved

    return returnValue;
}

//this function returns true if a heap is empty(0 elements) and false
//if it is not
bool PQHeap::isEmpty() const {
    return (numItems == 0);
}

//this function returns the number of elements in a heap using the private
//variable numItems
int PQHeap::size() const {
    return numItems;
}

//this function sets the numItems to 0, meaning the heap is now empty. there
//is no deallocation of memory written in the function
void PQHeap::clear() {
    numItems = 0;
}

//this function prints out the elements of the array which represents the
//binary heap. this is the same printDebugInfo() function found in pqsortedarray.cpp
void PQHeap::printDebugInfo() {
    for (int i = 0; i < size(); i++) {
        cout << "[" << i << "] = " << elements[i] << endl;
    }
}

//this is a helper function for validateInternalState(), given a parent index
//it makes sure that its children do not validate minheap properties. the heap
//must be filled from left to right, and the children cannot have priorities smaller
//than the parent
bool PQHeap::checkLeftRightChild(int parent){
    int parentPriority = elements[parent].priority;
    int left = getLeftChildIndex(parent);
    int right = getRightChildIndex(parent);

    if (left != -1 && right == -1) { //if only left child exists, its priority must be >= the parent's priority
        return (elements[left].priority >= parentPriority);
    }
    else if (left == -1 && right != -1) { //if only right child exists, this violates minheap properties
        return false;
    }
    else if (left == -1 && right == -1) { //if both children do not exist, no properties are violated
        return true;
    }
    else { //here, both children exist. they both must have larger priorities than the parent
        int priorityLeft = elements[left].priority;
        int priorityRight = elements[right].priority;
        return (priorityLeft >= parentPriority && priorityRight >= parentPriority);
    }


}

//this function traverses the elements array and ensures that minheap properites
//are not violated
void PQHeap::validateInternalState(){
    bool isValid = true;
    for (int i = 0; i < numItems; i++){
        isValid = checkLeftRightChild(i);
        if (!isValid) {
            error("minHeap properities are violated in this PQHeap");
        }
    }

}

//this function returns the parent index(int) given the child index.
//if there is no parent index, it returns -1
int PQHeap::getParentIndex(int curIndex){
    if (curIndex - 1 < 0) {
        return -1;
    }

    int calculatedIndex = (curIndex - 1) / 2;
    if (calculatedIndex >= 0) {
        return calculatedIndex;
    }
    else{
        return -1;
    }
}

//this function returns the leftChildIndex(int) given a parent index
//if there is no left child, -1 is returned
int PQHeap::getLeftChildIndex(int curIndex){
    int calculatedIndex = curIndex*2 + 1;
    if (calculatedIndex < numItems) {
        return calculatedIndex;
    }
    else{
        return -1;
    }
}

//this function returns the rightChildIndex(int) given a parent index
//if there is no right child, -1 is returned
int PQHeap::getRightChildIndex(int curIndex){
    int calculatedIndex = curIndex*2 + 2;
    if (calculatedIndex < numItems) {
        return calculatedIndex;
    }
    else{
        return -1;
    }
}

/* * * * * * Test Cases Below This Point * * * * * */
STUDENT_TEST("enqueue ascending positive values and descending negative values"){
    PQHeap pq;
    for (int i = 100; i < 200; i++){
        DataPoint point = {"", i};
        pq.enqueue(point);//enqueue validates
    }
    for (int i = -100; i > -200; i--){
        DataPoint point = {"", i};
        pq.enqueue(point); //enqueue validates
    }
    EXPECT_EQUAL(pq.size(), 200);
}

STUDENT_TEST("enqueue/dequeue random positive and negative values"){
    PQHeap pq;
    for (int i = 0; i < 300; i++){
        int randomNum = randomInteger(-200, 100);
        DataPoint point = {"", randomNum};
        pq.enqueue(point); //enqueue validates
    }
    EXPECT_EQUAL(pq.size(), 300);
    EXPECT(!pq.isEmpty());

    DataPoint expected = pq.peek();
    EXPECT_EQUAL(expected, pq.dequeue());

    for (int i = 0; i < 299; i++){
        DataPoint point = pq.dequeue(); //dequeue also validates min heap
    }
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Provided Test: Newly-created heap is empty.") {
    PQHeap pq;

    EXPECT(pq.isEmpty());
    EXPECT(pq.size() == 0);
}

PROVIDED_TEST("Provided Test: Enqueue / dequeue single element (two cycles)") {
    PQHeap pq;
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
    PQHeap pq;
    DataPoint point = { "Programming Abstractions", 106 };

    pq.enqueue(point);
    pq.clear();
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Provided Test: Dequeue / peek on empty priority queue throws error") {
    PQHeap pq;

    EXPECT(pq.isEmpty());
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());
}

PROVIDED_TEST("Provided Test: Dequeue / peek on recently-cleared priority queue throws error") {
    PQHeap pq;
    DataPoint point = { "Programming Abstractions", 106 };

    pq.enqueue(point);
    pq.clear();
    EXPECT(pq.isEmpty());
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());
}

PROVIDED_TEST("Provided Test: Enqueue elements in sorted order.") {
    PQHeap pq;
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
    PQHeap pq;
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
    PQHeap pq;
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

    PQHeap pq;
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
    PQHeap pq;
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
    PQHeap pq;
    for (int i = 0; i < 10; i++) {
        pq.enqueue({ "" , i });
    }
    EXPECT_EQUAL(pq.size(), 10);
}

PROVIDED_TEST("Test enqueue/dequeue of longer random sequence") {
    PQHeap pq;

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
    PQHeap pq;
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
    PQHeap pq;
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

//static void fillAndEmpty(int n) {
//    PQHeap pq;
//    DataPoint max = {"max", 106106106};
//    DataPoint min = {"min", -106106106};

//    pq.enqueue(min);
//    pq.enqueue(max);
//    for (int i = 0; i < n; i++) {
//        int randomPriority = randomInteger(-10000, 10000);
//        pq.enqueue({ "", randomPriority });
//    }
//    EXPECT_EQUAL(pq.size(), n + 2);

//    EXPECT_EQUAL(pq.dequeue(), min);
//    for (int i = 0; i < n; i++) {
//        pq.dequeue();
//    }
//    EXPECT_EQUAL(pq.dequeue(), max);
//    EXPECT_EQUAL(pq.size(), 0);
//}

//PROVIDED_TEST("Provided Test: Stress Test. Time the amount of time it takes to cycle many elements in and out. Should take at most about 5-10 seconds.") {
//    TIME_OPERATION(20000, fillAndEmpty(20000));
//}
