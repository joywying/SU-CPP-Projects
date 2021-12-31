//various functions are written to do binary merge, multi merge iteratively
//and multimerge recursively
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "queue.h"
#include "testing/SimpleTest.h"
using namespace std;

//this function takes in queue and returns true if it is in order and false
//if it's not in order. This is a helper function for merge
bool isInOrder(Queue<int> queue){
    int size = queue.size();
    for(int i = 1; i < size; i++){
        int comparison = queue.dequeue();
        if(comparison > queue.peek()){
            return false;
        }
    }
    return true;
}

//merge takes in two queues of integers and merges them into one
//no elements from either queue are deleted and they return queue
//is in sorted order. if either queue is not in sorted order initially, an
//error is raised
Queue<int> merge(Queue<int> a, Queue<int> b) {
    Queue<int> result;
    if (!isInOrder(a) || !isInOrder(b)) {
        error("both queues must be sorted in increasing order");
    }

    while (!a.isEmpty() || !b.isEmpty()) {
        if (a.isEmpty()) {
            while(!b.isEmpty()){
                result.enqueue(b.dequeue());
            }
        }
        else if (b.isEmpty()) {
            while(!a.isEmpty()){
                result.enqueue(a.dequeue());
            }
        }
        else if (a.peek() < b.peek()) {
            result.enqueue(a.dequeue());
        }
        else if (a.peek() > b.peek()) {
            result.enqueue(b.dequeue());
        }
        else if (a.peek() == b.peek()) {
            result.enqueue(a.dequeue());
            result.enqueue(b.dequeue());
        }
    }

    return result;
}

/* 
 * This function assumes correct functionality of the previously
 * defined merge function and makes use of this function to
 * iteratively merge a collection of sequences, starting off with
 * an empty sequence and progressively merging in one sequence at
 * a time. This function is provided for you fully implemented – 
 * it does not need to modified at all.
 */
Queue<int> multiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;
    for (Queue<int>& q : all) {
        result = merge(q, result);
    }
    return result;
}

//this function takes in a vector of queues and merges them all into one queue recursively
//the vector is divided into two and then the merged results of both queues are then merged
//again using binary merge
Queue<int> recMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;
    if(all.size() == 0){
        return result;
    }
    else if (all.size() == 1){
        return all.get(0);
    }
    else if (all.size() == 2){ //this case isn't necessary but it reduces the amount of times the vector is split
        return merge(all.get(0), all.get(1));
    }
    else{
        Vector<Queue<int>> left = all.subList(0, all.size() / 2);
        Vector<Queue<int>> right = all.subList((all.size() / 2));
        result = merge(recMultiMerge(left), recMultiMerge(right));

    }
    return result;
}


/* * * * * * Test Cases * * * * * */

Queue<int> createSequence(int size);
void distribute(Queue<int> input, Vector<Queue<int>>& all);

PROVIDED_TEST("Test binary merge, two short sequences") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 2, 3, 3, 4, 5};
    EXPECT_EQUAL(merge(a, b), expected);
    EXPECT_EQUAL(merge(b, a), expected);
}

PROVIDED_TEST("Test multiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(multiMerge(all), expected);
}

PROVIDED_TEST("Test recMultiMerge by compare to multiMerge") {
    int n = 1000;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(n);
    distribute(input, all);
    EXPECT_EQUAL(multiMerge(all), recMultiMerge(all));
}

PROVIDED_TEST("Time binary merge operation") {
    int n = 1500000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size(), merge(a, b));
}

PROVIDED_TEST("Time multiMerge operation") {
    int n = 11000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), multiMerge(all));
}
PROVIDED_TEST("Time recMultiMerge operation") {
    int n = 90000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), recMultiMerge(all));
}

STUDENT_TEST("test isInOrder for various inputs"){
    Queue<int> a = {1, 3, 4, 0};
    EXPECT(!isInOrder(a));
    a = {7, 8, 9, 10};
    EXPECT(isInOrder(a));
    a = {3, 1, 2, 3};
    EXPECT(!isInOrder(a));
    a = {1, 2, 3, 4};
    EXPECT(isInOrder(a));

}

STUDENT_TEST("test binary merge where one or all queues are not in sorted order"){
    Queue<int> a = {1, 3, 7, 3};
    Queue<int> b = {9, 10, 14};
    EXPECT_ERROR(merge(a, b));
    a = {196, 2, 5};
    b = {20, 10, 14};
    EXPECT_ERROR(merge(a, b));
}

STUDENT_TEST("test binary merge where one queue is entire empty and the other is not"){
    Queue<int> a = {};
    Queue<int> b = {9, 10, 14};
    EXPECT_EQUAL(merge(a, b), b);
    a = {2, 8, 15};
    b = {};
    EXPECT_EQUAL(merge(a, b), a);
}

STUDENT_TEST("test binary merge when both queues are empty"){
    Queue<int> a = {};
    Queue<int> b = {};
    Queue<int> result = merge(a, b);
    EXPECT(result.isEmpty());
}

STUDENT_TEST("test binary merge where the queues are vastly different sizes"){
    Queue<int> a = {0, 1, 4, 5};
    Queue<int> b = {9, 10, 14, 17, 22, 30, 50, 123, 213, 552, 553, 554, 555, 556};
    Queue<int> result = {0, 1, 4, 5, 9, 10, 14, 17, 22, 30, 50, 123, 213, 552, 553, 554, 555, 556};
    EXPECT_EQUAL(merge(a, b), result);
}

STUDENT_TEST("using timing operation to measure time for binary merge"){
    int n = 100000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), merge(a, b));
    n = 500000;
    a = createSequence(n);
    b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), merge(a, b));
    n = 1000000;
    a = createSequence(n);
    b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), merge(a, b));
    n = 10000000;
    a = createSequence(n);
    b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), merge(a, b));


}

STUDENT_TEST("Test multiMerge with 0 queues, should return empty queue") {
    Vector<Queue<int>> all = {};
    Queue<int> expected = {};
    EXPECT_EQUAL(multiMerge(all), expected);
}

STUDENT_TEST("Test multiMerge with many empty queues, should return one empty queue") {
    Vector<Queue<int>> all = {{}, {}, {}, {}};
    Queue<int> expected = {};
    EXPECT_EQUAL(multiMerge(all), expected);
}

STUDENT_TEST("using timing operation to measure time for multimerge, keeping k constant"){
    Vector<Queue<int>> all = {{}, {}, {}, {}};
    Queue<int> queue = createSequence(10000);
    distribute(queue, all);
    TIME_OPERATION(queue.size(), multiMerge(all));

    all = {{}, {}, {}, {}};
    queue = createSequence(100000);
    distribute(queue, all);
    TIME_OPERATION(queue.size(), multiMerge(all));

    all = {{}, {}, {}, {}};
    queue = createSequence(1000000);
    distribute(queue, all);
    TIME_OPERATION(queue.size(), multiMerge(all));

    all = {{}, {}, {}, {}};
    queue = createSequence(10000000);
    distribute(queue, all);
    TIME_OPERATION(queue.size(), multiMerge(all));

    all = {{}, {}, {}, {}};
    queue = createSequence(50000000);
    distribute(queue, all);
    TIME_OPERATION(queue.size(), multiMerge(all));

}

STUDENT_TEST("using timing operation to measure time for multimerge, keeping n constant"){
    Queue<int> queue = createSequence(25000);
    Vector<Queue<int>> a(100);
    distribute(queue, a);
    TIME_OPERATION(a.size(), multiMerge(a));

    a = {};
    for (int i = 0; i < 1000; i++){
        Queue<int> q;
        a.add(q);
    }
    distribute(queue, a);
    TIME_OPERATION(a.size(), multiMerge(a));

    a = {};
    for (int i = 0; i < 5000; i++){
        Queue<int> q;
        a.add(q);
    }
    distribute(queue, a);
    TIME_OPERATION(a.size(), multiMerge(a));

    a = {};
    for (int i = 0; i < 10000; i++){
        Queue<int> q;
        a.add(q);
    }
    distribute(queue, a);
    TIME_OPERATION(a.size(), multiMerge(a));

    a = {};
    for (int i = 0; i < 15000; i++){
        Queue<int> q;
        a.add(q);
    }
    distribute(queue, a);
    TIME_OPERATION(a.size(), multiMerge(a));

    a = {};
    for (int i = 0; i < 25000; i++){
        Queue<int> q;
        a.add(q);
    }
    distribute(queue, a);
    TIME_OPERATION(a.size(), multiMerge(a));

}

STUDENT_TEST("compare recmultiMerge to multiMerge, empty queues"){
    Vector<Queue<int>> all = {};
    EXPECT_EQUAL(multiMerge(all), recMultiMerge(all));

    all = {{}, {}, {}, {}, {}, {}, {}};
    EXPECT_EQUAL(multiMerge(all), recMultiMerge(all));

}

STUDENT_TEST("compare recMultiMerge to multiMerge, short sequences from provided test") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    EXPECT_EQUAL(multiMerge(all), recMultiMerge(all));
}

STUDENT_TEST("compare recMultiMerge to multiMerge, very long sequences"){
    Vector<Queue<int>> all(5000);
    Queue<int> queue = createSequence(5000);
    distribute(queue, all);

    EXPECT_EQUAL(multiMerge(all), recMultiMerge(all));
}

STUDENT_TEST("using timing operation to measure time for recMultiMerge, keeping n constant"){
    Queue<int> queue = createSequence(1000000);
    Vector<Queue<int>> a(50000);
    distribute(queue, a);
    TIME_OPERATION(a.size(), recMultiMerge(a));

    Vector<Queue<int>> b(100000);
    distribute(queue, b);
    TIME_OPERATION(b.size(), recMultiMerge(b));

    Vector<Queue<int>> c(200000);
    distribute(queue, c);
    TIME_OPERATION(c.size(), recMultiMerge(c));

    Vector<Queue<int>> d(300000);
    distribute(queue, d);
    TIME_OPERATION(d.size(), recMultiMerge(d));

    Vector<Queue<int>> e(500000);
    distribute(queue, e);
    TIME_OPERATION(e.size(), recMultiMerge(e));

    Vector<Queue<int>> f(800000);
    distribute(queue, f);
    TIME_OPERATION(f.size(), recMultiMerge(f));

}

STUDENT_TEST("using timing operation to measure time for recMultiMerge, keeping k constant"){
    Vector<Queue<int>> all = {{}, {}, {}, {}};
    Queue<int> queue = createSequence(10000);
    distribute(queue, all);
    TIME_OPERATION(queue.size(), recMultiMerge(all));

    all = {{}, {}, {}, {}};
    queue = createSequence(100000);
    distribute(queue, all);
    TIME_OPERATION(queue.size(), recMultiMerge(all));

    all = {{}, {}, {}, {}};
    queue = createSequence(1000000);
    distribute(queue, all);
    TIME_OPERATION(queue.size(), recMultiMerge(all));

    all = {{}, {}, {}, {}};
    queue = createSequence(10000000);
    distribute(queue, all);
    TIME_OPERATION(queue.size(), recMultiMerge(all));

    all = {{}, {}, {}, {}};
    queue = createSequence(50000000);
    distribute(queue, all);
    TIME_OPERATION(queue.size(), recMultiMerge(all));

}

/* Test helper to fill queue with sorted sequence */
Queue<int> createSequence(int size) {
    Queue<int> q;
    for (int i = 0; i < size; i++) {
        q.enqueue(i);
    }
    return q;
}

/* Test helper to distribute elements of sorted sequence across k sequences,
   k is size of Vector */
void distribute(Queue<int> input, Vector<Queue<int>>& all) {
    while (!input.isEmpty()) {
        all[randomInteger(0, all.size()-1)].add(input.dequeue());
    }
}
