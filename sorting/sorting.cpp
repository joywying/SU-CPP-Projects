// mergeSort and quickSort using linked lists.

#include "listnode.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;

//this is a helper function for split which takes in two ListNode pointers/linked lists and
//rewires them so that the first node is added to the beginning of the second linked list.
//The first pointer also shifts to point to its next value
void addToBeginning(ListNode*& first, ListNode*& second){
    ListNode* temp = first;
    first = first->next;
    temp->next = second;
    second = temp;
}

//helper function for mergeSort. This function takes in a pointer "front",
//and two empty linked lists "one" and "two". The contents of front are
//split across "one" and "two". One contains the odd elements (1st, 3rd...)
//and two contains the even elements (2nd, 4th..).
void split(ListNode*& front, ListNode*& one, ListNode*& two){
    int index = 1;
    while (front != nullptr){
        if (index % 2 != 0 ){
            addToBeginning(front, one); //"adds" the current node to "one"
        }
        else{
            addToBeginning(front, two); //"adds" the current node to "two"
        }
        index++;
    }
}

//helper function for merge. Based on the values of "one" and "two", it
//points fullHelper to either the larger node or the non-null node. then,
//either pointer one or two is set to its next value
void setFullHelper(ListNode*& fullHelper, ListNode*& one, ListNode*& two){
    if (one != nullptr && two != nullptr){
        if (one->data < two->data){
            fullHelper = one;
            one = one->next;
        }
        else {
            fullHelper = two;
            two = two->next;
        }
    }
    else {
        if (one != nullptr){
            fullHelper = one;
            one = one->next;
        }
        else if (two != nullptr){ //if both are null, do nothing
            fullHelper = two;
            two = two->next;
        }
    }
}

//helper function for merge, changes the first node's next value to the second pointer's node
//then, both of the pointers are shifted to a different spots. the second pointer is shifted
//to the next value and the first pointer is shifted to where the second pointer used to be
void moveAndAdd(ListNode*& first, ListNode*& second){
    first->next = second;
    second = second->next;
    first = first->next;
}

//helper function for mergeSort
//this function takes in one pointer to the "result" list, and then two pointers, each
//pointing to sorted linked lists. these two linked lists are then merged, with "full"
//pointing to the final merged list
void merge(ListNode*& full, ListNode*& one, ListNode*& two){
    ListNode* fullHelper = nullptr;
    setFullHelper(fullHelper, one, two); //fullHelper points to the beginning of our merged list
                                         //either "one" or "two" shifts to the next node in their list
    full = fullHelper;
    while (one != nullptr || two != nullptr){
        if (one != nullptr && two!= nullptr){
            if (one->data < two->data){ //"adds" the smaller value to our full list, pointer "one" now shifts over
                moveAndAdd(fullHelper, one);
            }
            else {
                moveAndAdd(fullHelper, two); //"adds" the smaller value, pointer "two" now shifts over
            }
        }

        else { //if only one list is not null, we start adding from that list
            if (one != nullptr){
                moveAndAdd(fullHelper, one);
            }
            else{
                moveAndAdd(fullHelper, two);
            }
        }
    }
}

//given a pointer to the front of a linked list, this function uses
//recursion, as well as the helper functions "split" and "merge" in order
//to sort a linked list from least to greatest. elements are split into
//two parts, both parts are sorted recursively, and then they are joined
void mergeSort(ListNode*& front) {
    if (front == nullptr || front->next == nullptr){
        return;
    }
    ListNode* one = nullptr;
    ListNode* two = nullptr;
    split(front, one, two);

    mergeSort(one);
    mergeSort(two);

    merge(front, one, two);
}


//helper function for quickSort, given a pointer to the beginning of the full linked list,
//"full" and three null pointers, "less", "equal", and "greater", the contents of the full
//list are divided into those categories.
void partition(ListNode*& full, ListNode*& less, ListNode*& equal, ListNode*& greater){
    int partition = 0;
    if (full != nullptr){
        partition = full->data;
    }

    while (full != nullptr){
        if (full->data > partition){
            addToBeginning(full, greater); //"full" shifts over, and the data is added to "greater"
        }
        else if (full->data < partition){
            addToBeginning(full, less);
        }
        else {
            addToBeginning(full, equal);
        }
    }
}

//helper function for concatenate, given three pointers to "less", "equal", and "greater"
//this function, sets what "full" is pointing at based on the values of the other three pointers
void setFull(ListNode*& full, ListNode*& less, ListNode*& equal, ListNode*& greater){
    if (less == nullptr && equal != nullptr){
        full = equal;
    }
    else if (less == nullptr && equal == nullptr){
        full = greater;
    }
    else {
        full = less;
    }
}

//helper function for quickSort
//given a pointer to the beginning of the full list, "full", and three other pointers,
//"less", "equal", and "greater", this function concatenates three linked lists together
//In the end, "full" points to the entire concatenated list
void concatenate(ListNode*& full, ListNode*& less, ListNode*& equal, ListNode*& greater){
    setFull(full, less, equal, greater);

    while (less != nullptr){ //goes through less until we find the end of the list, the concatenates
        if (less->next == nullptr && equal == nullptr){
                less->next = greater;
                break;
        }
        else if (less->next == nullptr && equal != nullptr){
                less->next = equal;
                break;
        }
        less = less->next;

    }

    while (equal != nullptr){ //another concatenation
        if (equal->next == nullptr){
            equal->next = greater;
            break;
        }
        equal = equal->next;
    }
}

//with the help of concatenate and partition, this function sorts a linked list
//from least to greatest using recursion. elements are partitioned based on the
//first element's value and then the "less" and "greater" partitions are sorted
//recursively, finally the partitioned lists are combined
void quickSort(ListNode*& front) {
    ListNode* less = nullptr;
    ListNode* equal = nullptr;
    ListNode* greater = nullptr;

    if (front == nullptr || front->next == nullptr){
        return;
    }

    partition(front, less, equal, greater);

    quickSort(less);
    quickSort(greater);

    concatenate(front, less, equal, greater);
}


/* * * * * * Test Code Below This Point * * * * * */

/*
 * We have provided this helper function as a convenience. This
 * function traverses the linked list represented by the provided
 * front pointer and prints out the data stored in the list along
 * the way in a convenient format. You may find it to be helpful to
 * use this function as a debugging aid.
 */
void printList(ListNode* front) {
   cout << "{";
   while (front != nullptr) {
       cout << front->data;
       if (front->next != nullptr){
           cout << ", ";
       }
       front = front->next;
   }
   cout << "}" << endl;
}

/*
 * This helper function is provided a pointer to the front of the list
 * and is responsible for deallocating all of the memory being used
 * to store nodes in that list.
 */
void deallocateList(ListNode* front) {
    while (front != nullptr){
            ListNode* temp = front->next;
            delete front;
            front = temp;
    }
}

/*
 * This helper function is provided a vector of integer values and
 * returns a pointer to the beginning of a linked list containing
 * those values in the specified order.
 */
ListNode* createList(Vector<int> values){
    ListNode* current = nullptr;
        ListNode* head = nullptr;
        if (values.size() != 0){
            head = new ListNode(values.get(0), nullptr);
            current = head;
        }
        for (int i = 1; i < values.size(); i++){
           current->next = new ListNode(values.get(i), nullptr);
           current = current->next;

        }
    return head;
}

/*
 * This helper function is provided a vector of integer values
 * and a pointer to the beginning of a linked list. If these two structures
 * contain the same values in the same order, the function returns
 * true, otherwise it returns false.
 */
bool checkListVectorEquality(ListNode* front, Vector<int> v){
    if (front != nullptr && v.size() == 0){
            return false;
        }
    for (int i = 0; i < v.size(); i++){
          if (front == nullptr){
              return false;
          }
          else if (front->data != v.get(i)){
              return false;
          }
          if (i == v.size() -1 && front->next != nullptr){
              return false;
          }
          front = front->next;

    }

    return true;
}

//STUDENT TESTS START HERE

STUDENT_TEST("mergeSort time trials"){

    for (int i = 10000; i <= 810000; i = i*3){
        Vector<int> values;
        for (int j = 0; j < i; j++){
            int rand = randomInteger(-10000, 10000);
            values.add(rand);
        }
        ListNode* list = createList(values);
        TIME_OPERATION(i, mergeSort(list));
        deallocateList(list);
        values = {};


    }
}

STUDENT_TEST("quickSort time trials"){

    for (int i = 10000; i <= 810000; i = i*3){
        Vector<int> values;

        for (int j = 0; j < i; j++){
            int rand = randomInteger(-10000, 10000);
            values.add(rand);
        }

        ListNode* list = createList(values);
        TIME_OPERATION(i, quickSort(list));
        deallocateList(list);
        values = {};
    }
}

STUDENT_TEST("mergeSort empty list"){
    Vector<int> values = {};

    ListNode* list = createList(values);

    mergeSort(list);
    EXPECT(checkListVectorEquality(list, values));
}

STUDENT_TEST("mergeSort list with one element"){
    Vector<int> values = {25};

    ListNode* list = createList(values);

    mergeSort(list);
    EXPECT(checkListVectorEquality(list, values));

    deallocateList(list);
}

STUDENT_TEST("mergeSort list in sorted order"){
    Vector<int> values;
    for (int i = 0; i < 200; i++){
        values.add(i);
    }

    ListNode* list = createList(values);

    mergeSort(list);
    EXPECT(checkListVectorEquality(list, values));

    deallocateList(list);
}

STUDENT_TEST("mergeSort list in reverse sorted order"){
    Vector<int> values;
    for (int i = 100; i <= 0; i++){
        values.add(i);
    }

    ListNode* list = createList(values);

    values.reverse();
    mergeSort(list);
    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);

}

STUDENT_TEST("mergeSort with duplicate elements"){
    Vector<int> values;
    for (int i = 0; i < 100; i++){
        values.add(i);
    }

    ListNode* list = createList(values);
    mergeSort(list); //sort with all duplicate elemenets
    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);

    for (int i = 0; i < 100; i++){
        int rand = randomInteger(-100, 100);
        values.add(rand);
    }

    list = createList(values);
    mergeSort(list); //sort with some duplicate elements
    values.sort();

    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);
}

STUDENT_TEST("mergeSort with all positive elements"){
    Vector<int> values;
    for (int i = 0; i < 200; i++){
        int rand = randomInteger(100, 300);
        values.add(rand);
    }

    ListNode* list = createList(values);
    mergeSort(list);
    values.sort();

    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);
}

STUDENT_TEST("mergeSort will all negative elements"){
    Vector<int> values;
    for (int i = 0; i < 200; i++){
        int rand = randomInteger(-1000, -100);
        values.add(rand);
    }

    ListNode* list = createList(values);
    mergeSort(list);
    values.sort();

    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);
}

STUDENT_TEST("mergeSort long list of randomized elements"){
    Vector<int> values;
    for (int i = 0; i < 100000; i++){
        int rand = randomInteger(-1000, 1000);
        values.add(rand);
    }

    ListNode* list = createList(values);
    mergeSort(list);
    values.sort();

    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);
}

STUDENT_TEST("quickSort empty list"){
    Vector<int> values = {};

    ListNode* list = createList(values);

    quickSort(list);
    EXPECT(checkListVectorEquality(list, values));
}

STUDENT_TEST("quickSort list with one element"){
    Vector<int> values = {25};

    ListNode* list = createList(values);

    quickSort(list);
    EXPECT(checkListVectorEquality(list, values));

    deallocateList(list);
}

STUDENT_TEST("quickSort list in sorted order"){
    Vector<int> values;
    for (int i = 0; i < 200; i++){
        values.add(i);
    }

    ListNode* list = createList(values);

    quickSort(list);
    EXPECT(checkListVectorEquality(list, values));

    deallocateList(list);
}

STUDENT_TEST("quickSort list in reverse sorted order"){
    Vector<int> values;
    for (int i = 100; i <= 0; i++){
        values.add(i);
    }

    ListNode* list = createList(values);

    values.reverse();
    quickSort(list);
    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);

}

STUDENT_TEST("quickSort with duplicate elements"){
    Vector<int> values;
    for (int i = 0; i < 100; i++){
        values.add(i);
    }

    ListNode* list = createList(values);
    quickSort(list); //sort with all duplicate elemenets
    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);

    for (int i = 0; i < 100; i++){
        int rand = randomInteger(-100, 100);
        values.add(rand);
    }

    list = createList(values);
    quickSort(list); //sort with some duplicate elements
    values.sort();

    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);
}

STUDENT_TEST("quickSort with all positive elements"){
    Vector<int> values;
    for (int i = 0; i < 200; i++){
        int rand = randomInteger(100, 300);
        values.add(rand);
    }

    ListNode* list = createList(values);
    quickSort(list);
    values.sort();

    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);
}

STUDENT_TEST("quickSort will all negative elements"){
    Vector<int> values;
    for (int i = 0; i < 200; i++){
        int rand = randomInteger(-1000, -100);
        values.add(rand);
    }

    ListNode* list = createList(values);
    quickSort(list);
    values.sort();

    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);
}

STUDENT_TEST("quickSort long list of randomized elements"){
    Vector<int> values;
    for (int i = 0; i < 100000; i++){
        int rand = randomInteger(-1000, 1000);
        values.add(rand);
    }

    ListNode* list = createList(values);
    quickSort(list);
    values.sort();

    EXPECT(checkListVectorEquality(list, values));
    deallocateList(list);
}

STUDENT_TEST("concatenate lists together, 9 elems"){
    Vector<int> allItems = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Vector<int> a = {1, 2, 3};
    Vector<int> b = {4, 5, 6};
    Vector<int> c = {7, 8, 9};

    ListNode* one = createList(a);
    ListNode* two = createList(b);
    ListNode* three = createList(c);
    ListNode* fullList = nullptr;

    concatenate(fullList, one, two, three);

    EXPECT(checkListVectorEquality(fullList, allItems));
    deallocateList(fullList);

}

STUDENT_TEST("concatenate lists together, empty second list"){
    Vector<int> allItems = {5, 6, 7, 8, 9, 20};
    Vector<int> a = {5, 6, 7, 8};
    Vector<int> b = {};
    Vector<int> c = {9, 20};

    ListNode* one = createList(a);
    ListNode* two = createList(b);
    ListNode* three = createList(c);
    ListNode* fullList = nullptr;

    concatenate(fullList, one, two, three);

    EXPECT(checkListVectorEquality(fullList, allItems));
    deallocateList(fullList);

}

STUDENT_TEST("concatenate lists together, empty lists one and two"){
    Vector<int> allItems = {7, 8, 9, 10, 11};
    Vector<int> a = {};
    Vector<int> b = {};
    Vector<int> c = {7, 8, 9, 10, 11};

    ListNode* one = createList(a);
    ListNode* two = createList(b);
    ListNode* three = createList(c);
    ListNode* fullList = nullptr;

    concatenate(fullList, one, two, three);

    EXPECT(checkListVectorEquality(fullList, allItems));
    deallocateList(fullList);

}

STUDENT_TEST("concatenate lists together, empty lists one and three"){
    Vector<int> allItems = {7, 8, 9, 10, 11};
    Vector<int> a = {};
    Vector<int> b = {7, 8, 9, 10, 11};
    Vector<int> c = {};

    ListNode* one = createList(a);
    ListNode* two = createList(b);
    ListNode* three = createList(c);
    ListNode* fullList = nullptr;

    concatenate(fullList, one, two, three);

    EXPECT(checkListVectorEquality(fullList, allItems));
    deallocateList(fullList);

}

STUDENT_TEST("concatenate lists together, empty lists two and three"){
    Vector<int> allItems = {7, 8, 9, 10, 11};
    Vector<int> a = {7, 8, 9, 10, 11};
    Vector<int> b = {};
    Vector<int> c = {};

    ListNode* one = createList(a);
    ListNode* two = createList(b);
    ListNode* three = createList(c);
    ListNode* fullList = nullptr;

    concatenate(fullList, one, two, three);

    EXPECT(checkListVectorEquality(fullList, allItems));
    deallocateList(fullList);

}

STUDENT_TEST("concatenate lists together, all empty"){
    Vector<int> allItems = {};
    Vector<int> a = {};
    Vector<int> b = {};
    Vector<int> c = {};

    ListNode* one = createList(a);
    ListNode* two = createList(b);
    ListNode* three = createList(c);
    ListNode* fullList = nullptr;

    concatenate(fullList, one, two, three);

    EXPECT(checkListVectorEquality(fullList, allItems));
    deallocateList(fullList);
}

STUDENT_TEST("partition a small list"){
    Vector<int> values = {7, 6, 2, 3, 4, 5, 9};
    Vector<int> lessThan = {5, 4, 3, 2, 6};
    Vector<int> equalTo = {7};
    Vector<int> greaterThan = {9};

    ListNode* full = createList(values);
    ListNode* less = nullptr;
    ListNode* greater = nullptr;
    ListNode* equal = nullptr;

    partition(full, less, equal, greater);
    EXPECT(checkListVectorEquality(less, lessThan));
    EXPECT(checkListVectorEquality(equal, equalTo));
    EXPECT(checkListVectorEquality(greater, greaterThan));

    deallocateList(less);
    deallocateList(equal);
    deallocateList(greater);
}

STUDENT_TEST("partition an empty list"){
    Vector<int> values = {};
    ListNode* full = createList(values);
    ListNode* less = nullptr;
    ListNode* greater = nullptr;
    ListNode* equal = nullptr;
    partition(full, less, equal, greater);

    EXPECT(checkListVectorEquality(less, values));
    EXPECT(checkListVectorEquality(equal, values));
    EXPECT(checkListVectorEquality(greater, values));

}

STUDENT_TEST("partition list with one element"){
    Vector<int> values = {20};
    Vector<int> empty = {};
    ListNode* full = createList(values);
    ListNode* less = nullptr;
    ListNode* greater = nullptr;
    ListNode* equal = nullptr;
    partition(full, less, equal, greater);

    EXPECT(checkListVectorEquality(less, empty));
    EXPECT(checkListVectorEquality(equal, values));
    EXPECT(checkListVectorEquality(greater, empty));


    deallocateList(equal);//contains the 20 because it's equal
}

STUDENT_TEST("partition a larger list"){
    Vector<int> values = {};
    Vector<int> lessThan = {};
    Vector<int> equalTo = {1};
    Vector<int> greaterThan = {};

    for (int i = 1; i < 100; i++){
        values.add(i);
    }
    for (int i = 99; i >= 2; i--){
        greaterThan.add(i);
    }

    ListNode* full = createList(values);
    ListNode* less = nullptr;
    ListNode* greater = nullptr;
    ListNode* equal = nullptr;
    partition(full, less, equal, greater);

    EXPECT(checkListVectorEquality(less, lessThan));
    EXPECT(checkListVectorEquality(equal, equalTo));
    EXPECT(checkListVectorEquality(greater, greaterThan));

    deallocateList(less);
    deallocateList(equal);
    deallocateList(greater);

}

STUDENT_TEST("partition a list with varying values, positive and negative"){
    Vector<int> values = {-15, 37, -88, 45, 8};
    Vector<int> lessThan = {-88};
    Vector<int> equalTo = {-15};
    Vector<int> greaterThan = {8, 45, 37};

    ListNode* full = createList(values);
    ListNode* less = nullptr;
    ListNode* greater = nullptr;
    ListNode* equal = nullptr;
    partition(full, less, equal, greater);

    EXPECT(checkListVectorEquality(less, lessThan));
    EXPECT(checkListVectorEquality(equal, equalTo));
    EXPECT(checkListVectorEquality(greater, greaterThan));

    deallocateList(less);
    deallocateList(equal);
    deallocateList(greater);
}

STUDENT_TEST("split, a small list"){
    Vector<int> vec1 = {5, 3, 1};
    Vector<int> vec2 = {6, 4, 2};

    Vector<int> combined = {1, 2, 3, 4, 5, 6};
    ListNode* combinedList = createList(combined);
    ListNode* firstHalf = nullptr;
    ListNode* secondHalf = nullptr;
    split(combinedList, firstHalf, secondHalf);

    EXPECT(checkListVectorEquality(firstHalf, vec1));
    EXPECT(checkListVectorEquality(secondHalf, vec2));

    deallocateList(firstHalf);
    deallocateList(secondHalf);
}

STUDENT_TEST("split, empty list"){
    Vector<int> vec1 = {};
    Vector<int> vec2 = {};


    Vector<int> combined = {};
    ListNode* combinedList = createList(combined);
    ListNode* firstHalf = nullptr;
    ListNode* secondHalf = nullptr;
    split(combinedList, firstHalf, secondHalf);

    EXPECT(checkListVectorEquality(firstHalf, vec1));
    EXPECT(checkListVectorEquality(secondHalf, vec2));

    deallocateList(firstHalf);
    deallocateList(secondHalf);
}

STUDENT_TEST("split, list with one element"){
    Vector<int> vec1 = {1};
    Vector<int> vec2 = {};

    Vector<int> combined = {1};
    ListNode* combinedList = createList(combined);
    ListNode* firstHalf = nullptr;
    ListNode* secondHalf = nullptr;
    split(combinedList, firstHalf, secondHalf);

    EXPECT(checkListVectorEquality(firstHalf, vec1));
    EXPECT(checkListVectorEquality(secondHalf, vec2));


    deallocateList(firstHalf);
    deallocateList(secondHalf);

}

STUDENT_TEST("merge, two small lists"){
    Vector<int> vec1 = {1, 2, 4};
    Vector<int> vec2 = {3, 6, 9, 20};

    Vector<int> combined = {1, 2, 3, 4, 6, 9, 20};
    ListNode* one = createList(vec1);
    ListNode* two = createList(vec2);
    ListNode* fullList;

    merge(fullList, one, two);

    EXPECT(checkListVectorEquality(fullList, combined));

    deallocateList(fullList);
}

STUDENT_TEST("merge, empty lists"){
    Vector<int> vec1 = {};
    Vector<int> vec2 = {};

    ListNode* one = createList(vec1);
    ListNode* two = createList(vec2);
    ListNode* fullList;

    merge(fullList, one, two);
    EXPECT(checkListVectorEquality(fullList, vec1));

    deallocateList(fullList);
}

STUDENT_TEST("merge, one empty one not"){
    Vector<int> vec1 = {1, 4, 6};
    Vector<int> vec2 = {};

    ListNode* one = createList(vec1);
    ListNode* two = createList(vec2);
    ListNode* fullList;

    merge(fullList, one, two);
    EXPECT(checkListVectorEquality(fullList, vec1));

    deallocateList(fullList);
}


STUDENT_TEST("This is a skeleton for what your overall student tests should look like."){
    /* Initialize the values and list for this test case. */
    Vector<int> values = {1, 2, 3, 4};
    ListNode* list = createList(values);

    /* Sort the linked list. */
    mergeSort(list);

    /* Sort the vector. */
    values.sort();

    /* Check to make sure that the list is now in the correct order. */
    EXPECT(checkListVectorEquality(list, values));

    /* Avoid memory leaks by deallocating list. */
    deallocateList(list);
}

PROVIDED_TEST("This tests some of the testing helper functions you are recommended to implement."){
    /* Creates a small test list containing the values 1->2->3. */
    ListNode* testList = nullptr;
    testList = new ListNode(3, testList);
    testList = new ListNode(2, testList);
    testList = new ListNode(1, testList);

    /* Test equality comparison function. */
    EXPECT(checkListVectorEquality(testList, {1, 2, 3}));
    EXPECT(!checkListVectorEquality(testList, {1, 2}));
    EXPECT(!checkListVectorEquality(testList, {1, 2, 3, 4}));
    EXPECT(!checkListVectorEquality(testList, {1, 3, 2}));

    /* Test create list function. */
    ListNode* studentList = createList({1, 2, 3, 4});
    printList(studentList);
    ListNode* cur = studentList;
    for (int i = 1; i <= 4; i++){
        EXPECT(cur != nullptr);
        EXPECT_EQUAL(cur->data, i);
        cur = cur->next;
    }

    /* Test deallocate list functions. There should be no memory leaks from this test. */
    deallocateList(studentList);
    deallocateList(testList);
}

/*
 * The provided test case demonstrates a comparsion between the same
 * algorithm (quicksort) operating on a linked list versus a vector.
 */
PROVIDED_TEST("Time linked list quicksort vs vector quicksort") {
    int startSize = 50000;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v;
        ListNode* list = nullptr;

        /* Insert the same random values into linked list and vector. */
        for (int i = 0; i < n; i++) {
            int val = randomInteger(-10000, 10000);
            v.add(val);
            list = new ListNode(val, list);
        }

        /* The vector values are currently in the reverse order than the
         * linked list values – let's fix that so we have a fair comparison. */
        v.reverse();

        /* NOTE: This test does not check correctness of the linked list sort
         * function. It only times the two operations to generate a comparison. */
        TIME_OPERATION(n, quickSort(list));
        TIME_OPERATION(n, v.sort());        /* Standard vector sort operation is backed
                                               with quicksort algorithm. */

        deallocateList(list);
    }
}



