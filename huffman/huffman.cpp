
//a variety of functions are written to compress and decompress
//files using Huffman coding

#include "bits.h"
#include "treenode.h"
#include "huffman.h"
#include "map.h"
#include "vector.h"
#include "priorityqueue.h"
#include "strlib.h"
#include "testing/SimpleTest.h"
using namespace std;

/**
 * Given a Queue<Bit> containing the compressed message bits and the encoding tree
 * used to encode those bits, decode the bits back to the original message text.
 *
 * You can assume that tree is a well-formed non-empty encoding tree and
 * bits queue contains a valid sequence of encoded bits.
*/
string decodeText(EncodingTreeNode* tree, Queue<Bit>& messageBits) {
    string message;
    EncodingTreeNode* helper = tree;

    while (!messageBits.isEmpty()){
        Bit currentBit = messageBits.dequeue();
        if (currentBit == 0){
            helper = helper->zero;
        }
        else {
            helper = helper->one;
        }

        if (helper->zero == nullptr){ //search the tree until we find a leaf node, then we go back to the top
            message += helper->ch;
            helper = tree;
        }

    }
    return message;
}

/**
 * Reconstruct encoding tree from flattened form Queue<Bit> and Queue<char>.
 *
 * You can assume that the input Queues are well-formed and represent
 * a valid encoding tree.
 */

//to reconstruct the trees, leaf nodes are represented by the bit 0 and interior
//nodes are represented by the bit 1 followed by the encoding of its left then right
//subtree
EncodingTreeNode* unflattenTree(Queue<Bit>& treeBits, Queue<char>& treeLeaves) {
    EncodingTreeNode* node;

    if (treeBits.isEmpty()){ //base case
        return nullptr;
    }

    else {
        Bit currentBit = treeBits.dequeue();
        if (currentBit == 0){ //it's a leaf
            return new EncodingTreeNode(treeLeaves.dequeue());
        }
        else { //it's an interior node, construct its left and right subtrees
            return new EncodingTreeNode(unflattenTree(treeBits, treeLeaves), unflattenTree(treeBits, treeLeaves));
        }
    }

    return node;
}

/**
 * Decompress the given EncodedData and return the original text.
 *
 * You can assume the input data is well-formed and was created by a correct
 * implementation of compress.
 *
 * Your implementation may change the data parameter however you like. There
 * are no requirements about what it should look like after this function
 * returns.
 */

//this function uses unflattenTree to generate a tree and then decodeText to
//generate the message. the generated tree is deallocated within the function
string decompress(EncodedData& data) {
    EncodingTreeNode* unflattenedTree = unflattenTree(data.treeBits, data.treeLeaves);
    string decodedText = decodeText(unflattenedTree, data.messageBits);
    deallocateTree(unflattenedTree);

    return decodedText;
}

//this is a helper function for buildHuffmanTree which takes in a text string
//and builds a frequency map of the characters, if the text doesn't contain at
//least two distinct characters, an error is raised. At the end, a priority queue of
//leaf nodes is returned
PriorityQueue<EncodingTreeNode*> buildPriorityQueue(string text){
    PriorityQueue<EncodingTreeNode*> pq;
    Map<char, int> frequencyMap;

    for (char character : text){
        frequencyMap[character]++;
    }
    if (frequencyMap.size() < 2){
        error("the input must contain at least two distinct characters");
    }
    for (char character : frequencyMap){
        pq.enqueue(new EncodingTreeNode(character), frequencyMap[character]);
    }

    return pq;
}

/**
 * Constructs an optimal Huffman coding tree for the given text, using
 * the algorithm described in lecture.
 *
 * Reports an error if the input text does not contain at least
 * two distinct characters.
 *
 * When assembling larger trees out of smaller ones, make sure to set the first
 * tree dequeued from the queue to be the zero subtree of the new tree and the
 * second tree as the one subtree.
 */

//this function utilizes buildPriorityQueue
EncodingTreeNode* buildHuffmanTree(string text) {
    PriorityQueue<EncodingTreeNode*> pq = buildPriorityQueue(text); //initial priority queue is built

    while (pq.size() != 1){
        double priorityOne = pq.peekPriority();
        EncodingTreeNode* one = pq.dequeue();
        double priorityTwo = pq.peekPriority();
        EncodingTreeNode* two = pq.dequeue();

        EncodingTreeNode* combine = new EncodingTreeNode(one, two);
        pq.enqueue(combine, priorityOne + priorityTwo);
    }

    return pq.dequeue(); //the entire tree is returned
}

//this is a helper function for encodeText which generates a Map<char, Queue<Bit>> where each
//character is a key and the path to each character is represented as Queue<Bit>
void fillMap(EncodingTreeNode* tree, Map<char, Queue<Bit>>& map, Vector<Bit> currentVector){
     Bit zero = Bit(0);
     Bit one = Bit(1);

     if (tree->zero == nullptr){
         for (Bit bit : currentVector){
             map[tree->ch].enqueue(bit);
         }
     }
     else {
         currentVector.add(zero);
         fillMap(tree->zero, map, currentVector); //go to the left branch and search for leaf nodes
         currentVector.remove(currentVector.size() - 1);

         currentVector.add(one);
         fillMap(tree->one, map, currentVector); //go to the right branch and search for leaf nodes
         currentVector.remove(currentVector.size() - 1);
     }
}

/**
 * Given a string and an encoding tree, encode the text using the tree
 * and return a Queue<Bit> of the encoded bit sequence.
 *
 * You can assume tree is a valid non-empty encoding tree and contains an
 * encoding for every character in the text.
 */

//this function uses fillMap to encode text into a Queue<Bit>, each character in the string text is
//associated with a path, all of those paths are combined into one Queue<Bit>
Queue<Bit> encodeText(EncodingTreeNode* tree, string text) {
    Map<char, Queue<Bit>> allChars;
    Queue<Bit> returnValue;
    Vector<Bit> helperVector;
    Queue<Bit> helperQueue;

    fillMap(tree, allChars, helperVector); //each character in the tree has a path

    for (int i = 0; i < int(text.length()); i++){
        helperQueue = allChars[text[i]];

        while (!helperQueue.isEmpty()){
            returnValue.enqueue(helperQueue.dequeue()); //add the character's path to the returnValue
        }
    }

    return returnValue;
}

/**
 * Flatten the given tree into a Queue<Bit> and Queue<char> in the manner
 * specified in the assignment writeup.
 *
 * You can assume the input Queues are empty on entry to this function.
 *
 * You can assume tree is a valid well-formed encoding tree.
 */

//to flatten the tree, leaf nodes are represented by the Bit 0 and interior nodes with the
//Bit 1
void flattenTree(EncodingTreeNode* tree, Queue<Bit>& treeBits, Queue<char>& treeLeaves) {
    Bit zero = Bit(0);
    Bit one = Bit(1);

    if (tree->zero == nullptr){ //found a leaf node, add zero to treeBits and add the character to treeLeaves
        treeBits.add(zero);
        treeLeaves.add(tree->ch);
    }
    else {
        treeBits.add(one); //found interior node, represent with the Bit 1, and then with encoding of left/right subtrees
        flattenTree(tree->zero, treeBits, treeLeaves);
        flattenTree(tree->one, treeBits, treeLeaves);
    }
}

/**
 * Compress the input text using Huffman coding, producing as output
 * an EncodedData containing the encoded message and encoding tree used.
 *
 * Reports an error if the message text does not contain at least
 * two distinct characters.
 */
EncodedData compress(string messageText) {
    EncodedData returnValue;
    Queue<Bit> treeBits;
    Queue<char> treeLeaves;

    EncodingTreeNode* huffmanTree = buildHuffmanTree(messageText); //buildHuffmanTree does the error handling
    flattenTree(huffmanTree, treeBits, treeLeaves); //treeBits and treeLeaves are filled

    Queue<Bit> messageBits = encodeText(huffmanTree, messageText);
    deallocateTree(huffmanTree); //encoded the message and deallocated the tree we generated

    returnValue.treeBits = treeBits;
    returnValue.messageBits = messageBits;
    returnValue.treeLeaves = treeLeaves;

    return returnValue;
}

/* * * * * * Testing Helper Functions Below This Point * * * * * */

//explicitly creates the example tree with seven nodes and returns the root
EncodingTreeNode* createExampleTree() {
    /* Example encoding tree used in multiple test cases:
     *                *
     *              /   \
     *             T     *
     *                  / \
     *                 *   E
     *                / \
     *               R   S
     */
    EncodingTreeNode* node = new EncodingTreeNode(new EncodingTreeNode('R'), new EncodingTreeNode('S'));
    EncodingTreeNode* nodeTwo = new EncodingTreeNode(node, new EncodingTreeNode('E'));
    EncodingTreeNode* root = new EncodingTreeNode(new EncodingTreeNode('T'), nodeTwo);
    return root;
}

//deallocates memory associated with a tree, given the root
void deallocateTree(EncodingTreeNode* root) {
    if (root->zero == nullptr && root->one == nullptr){
        delete root;
    }
    else {
        deallocateTree(root->zero);
        deallocateTree(root->one);
        delete root;
    }
}

//checks whether two trees are equal, given two root nodes
bool areEqual(EncodingTreeNode* a, EncodingTreeNode* b) {
    if (a == nullptr || b == nullptr){ //if either is null, see if both are null
        return a == b;
    }
    else if (a->zero == nullptr || b->zero == nullptr){ //if either is a leaf node, make sure both are leaf nodes and are equal
        return a->ch == b->ch;
    }
    else {
        return areEqual(a->zero, b->zero) && areEqual(a->one, b->one);
    }
}

/* * * * * * Test Cases Below This Point * * * * * */

STUDENT_TEST("createExampleTree and deallocate"){
    EncodingTreeNode* example = createExampleTree();
    deallocateTree(example);
}

STUDENT_TEST("areEqual test with identical trees"){
    EncodingTreeNode* treeOne = createExampleTree();
    EncodingTreeNode* treeTwo = createExampleTree();
    EXPECT(areEqual(treeOne, treeTwo));
    EXPECT(areEqual(treeOne->zero, treeTwo->zero));
    EXPECT(areEqual(treeOne->one, treeTwo->one));

    deallocateTree(treeOne);
    deallocateTree(treeTwo);
}

STUDENT_TEST("areEqual with non-identical trees"){
    EncodingTreeNode* one = nullptr;
    EncodingTreeNode* two = new EncodingTreeNode('C');
    EXPECT(!areEqual(one, two));

    one = new EncodingTreeNode('A');
    EXPECT(!areEqual(one, two));

    deallocateTree(one);
    deallocateTree(two);
}

STUDENT_TEST("areEqual, one is interior node, other is leaf node"){
    EncodingTreeNode* one = new EncodingTreeNode(new EncodingTreeNode('A'), new EncodingTreeNode('B'));
    EncodingTreeNode* two = new EncodingTreeNode('C');
    EXPECT(!areEqual(one, two));
    EXPECT(!areEqual(one->zero, two->zero));
    EXPECT(!areEqual(one->one, two->one));

    deallocateTree(one);
    deallocateTree(two);

}

STUDENT_TEST("decodeText, more messageBits"){
    EncodingTreeNode* tree = createExampleTree();
    Queue<Bit> messageBits = {0};
    EXPECT_EQUAL(decodeText(tree, messageBits), "T");

    messageBits = {1, 0, 0, 1, 0, 1};
    EXPECT_EQUAL(decodeText(tree, messageBits), "RS");

    deallocateTree(tree);
}

STUDENT_TEST("unflattenTree, smaller tree than example"){
    EncodingTreeNode* expected = new EncodingTreeNode(new EncodingTreeNode('A'), new EncodingTreeNode('B'));
    Queue<Bit> treeBits = {1, 0, 0};
    Queue<char> treeLeaves = {'A', 'B'};

    EncodingTreeNode* tree = unflattenTree(treeBits, treeLeaves);

    EXPECT(areEqual(tree, expected));

    deallocateTree(tree);
    deallocateTree(expected);

}

STUDENT_TEST("decompress, smaler input") {
    EncodedData data = {
        { 1, 0, 0}, // treeBits
        {'A', 'B'},  // treeLeaves
        { 0, 1, 0, 1, 0, 1} // messageBits
    };

    EXPECT_EQUAL(decompress(data), "ABABAB");
}

STUDENT_TEST("encodeText, small, two leaf tree") {
    EncodingTreeNode* expected = new EncodingTreeNode(new EncodingTreeNode('E'), new EncodingTreeNode('?'));

    Queue<Bit> messageBits = {0, 1, 1, 0 };
    EXPECT_EQUAL(encodeText(expected, "E??E"), messageBits);

    messageBits = { 1, 1, 1, 1};
    EXPECT_EQUAL(encodeText(expected, "????"), messageBits);


    deallocateTree(expected);
}

STUDENT_TEST("flattenTree and unflatten example tree") {
    EncodingTreeNode* reference = createExampleTree();
    Queue<Bit>  treeBits;
    Queue<char> treeLeaves;
    flattenTree(reference, treeBits, treeLeaves);
    EncodingTreeNode* tree = unflattenTree(treeBits, treeLeaves);

    EXPECT(areEqual(tree, reference)); //flattens then unflattens, checks equality

    deallocateTree(reference);
    deallocateTree(tree);
}

STUDENT_TEST("buildHuffmanTree, small tree and invalid tree") {
    EncodingTreeNode* reference = new EncodingTreeNode(new EncodingTreeNode('E'), new EncodingTreeNode('?'));
    EncodingTreeNode* tree = buildHuffmanTree("E??");
    EXPECT(areEqual(tree, reference));

    deallocateTree(reference);
    deallocateTree(tree);

    EXPECT_ERROR(buildHuffmanTree("a"));
}

STUDENT_TEST("compress, error should be raised for strings without at least two unique characters"){
    string text = "aa";
    EXPECT_ERROR(compress(text));

    text = "";
    EXPECT_ERROR(compress(text));

    text = "ccccccc";
    EXPECT_ERROR(compress(text));
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("decodeText, small example encoding tree") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(decodeText(tree, messageBits), "E");

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(decodeText(tree, messageBits), "SET");

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1}; // STREETS
    EXPECT_EQUAL(decodeText(tree, messageBits), "STREETS");

    deallocateTree(tree);
}

PROVIDED_TEST("unflattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  treeBits   = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeLeaves = { 'T', 'R', 'S', 'E' };
    EncodingTreeNode* tree = unflattenTree(treeBits, treeLeaves);

    EXPECT(areEqual(tree, reference));

    deallocateTree(tree);
    deallocateTree(reference);
}

PROVIDED_TEST("decompress, small example input") {
    EncodedData data = {
        { 1, 0, 1, 1, 0, 0, 0 }, // treeBits
        { 'T', 'R', 'S', 'E' },  // treeLeaves
        { 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "TRESS");
}

PROVIDED_TEST("buildHuffmanTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    EncodingTreeNode* tree = buildHuffmanTree("STREETTEST");
    EXPECT(areEqual(tree, reference));

    deallocateTree(reference);
    deallocateTree(tree);
}

PROVIDED_TEST("encodeText, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(encodeText(reference, "E"), messageBits);

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(encodeText(reference, "SET"), messageBits);

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1 }; // STREETS
    EXPECT_EQUAL(encodeText(reference, "STREETS"), messageBits);

    deallocateTree(reference);
}

PROVIDED_TEST("flattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  expectedBits   = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> expectedLeaves = { 'T', 'R', 'S', 'E' };

    Queue<Bit>  treeBits;
    Queue<char> treeLeaves;
    flattenTree(reference, treeBits, treeLeaves);

    EXPECT_EQUAL(treeBits, expectedBits);
    EXPECT_EQUAL(treeLeaves, expectedLeaves);

    deallocateTree(reference);
}

PROVIDED_TEST("compress, small example input") {
    EncodedData data = compress("STREETTEST");
    Queue<Bit>  treeBits    = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeChars   = { 'T', 'R', 'S', 'E' };
    Queue<Bit>  messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0 };

    EXPECT_EQUAL(data.treeBits, treeBits);
    EXPECT_EQUAL(data.treeLeaves, treeChars);
    EXPECT_EQUAL(data.messageBits, messageBits);
}

PROVIDED_TEST("Test end-to-end compress -> decompress") {
    Vector<string> inputs = {
        "HAPPY HIP HOP",
        "The job requires extra pluck and zeal from every young wage earner.",
        ":-) :-D XD <(^_^)>",
    };

    for (string input: inputs) {
        EncodedData data = compress(input);
        string output = decompress(data);

        EXPECT_EQUAL(output.size(), input.size());

        /* Don't clobber the output with a huge string if there's a mismatch. */
        EXPECT(input == output);
    }
}
