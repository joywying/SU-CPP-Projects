//Helper functions work together to read and solve maze files.

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "stack.h"
#include "queue.h"
#include "vector.h"
#include "set.h"
#include "maze.h"
#include "mazegraphics.h"
#include "testing/SimpleTest.h"
using namespace std;


//This function takes a Grid<bool> which is our maze and a GridLocation representing the current location
//and generates a set of valid moves directly North, South, East, and West of the current location
//Valid moves, moves that are directly N,S,E,W, only one step away, and not a wall or out of bounds
//are added to a set and returned
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;
    Set<GridLocation> possibleNeighbors;
    possibleNeighbors.add(GridLocation(cur.row, cur.col - 1)); //west
    possibleNeighbors.add(GridLocation(cur.row - 1, cur.col)); //north
    possibleNeighbors.add(GridLocation(cur.row, cur.col + 1)); //east
    possibleNeighbors.add(GridLocation(cur.row + 1, cur.col)); //south

    for (GridLocation possibleNeighbor : possibleNeighbors){ //makes sure none of them are walls or out of bounds
        if (!(possibleNeighbor.row < 0 || possibleNeighbor.col >= maze.width() || possibleNeighbor.col < 0 ||
            possibleNeighbor.row >= maze.height() || maze[possibleNeighbor] == false)) {
            neighbors.add(possibleNeighbor);

        }

    }
    return neighbors;
}


//This function checks whether a Stack<GridLocation> path is valid for a given maze. If the path
//starts at the entry, ends at the exit, contains no loops, and has locations which are within bounds
//not walls, and are one cardinal step from the next in the path, then it is valid. This function will
//not return anything if the path is valid and will raise errors when the path violates any condition
void checkSolution(Grid<bool>& maze, Stack<GridLocation> path) {
    GridLocation mazeExit = {maze.numRows()-1,  maze.numCols()-1};

    if (path.peek() != mazeExit) {
        error("Path does not end at maze exit");
    }
    int popNum = path.size();
    GridLocation previousLocation = path.pop();
    Set<GridLocation> allLocations;
    allLocations.add(previousLocation);

    for (int i = 2; i < popNum; i++){ //for all of the locations excluding the first and last, check if they are out of bounds, a wall, not a cardinal step from the previousLocation,
                                      //also checks if the path contains a loop
        GridLocation location = path.pop();
        Set<GridLocation> neighbors = generateValidMoves(maze, location);

        if (!(maze.inBounds(location.row, location.col))) {
            error("One or more location(s) in path is out of bounds");
        }
        if (!(maze[location] == true)) {
            error("One or more location(s) in path is not an open corridor");
        }
        if (!neighbors.contains(previousLocation)) {
            error("Two or more location(s) in the path are not one cardinal step from each  other");
        }
        if (allLocations.contains(location)) {
            error("This path contains loops, a location appears more than once in the path");
        }
        previousLocation = location;
        allLocations.add(previousLocation);
    }

    GridLocation mazeStart = path.pop(); //makes sure the start(last location to pop out of stack) is at 0,0 and the second step is one cardinal step from the start
    Set<GridLocation> firstNeighbors = generateValidMoves(maze, mazeStart);
    if (!firstNeighbors.contains(previousLocation)) {
        error("Two or more location(s) in the path are not one cardinal step from each  other");
    }
    if (mazeStart.col != 0 || mazeStart.row != 0) {
        error("Path does not start at entry(upper left corner) of the maze");
    }

}

//this function determines whether a stack contains a certain element. A set already has this "contains" functionality
//so we use this functionality to return a boolean, true if the stack contains the element and false if it does not
bool containsForStack(Stack<GridLocation> stack, GridLocation location){
    Set<GridLocation> stackSet;
    for (int i = 0; i < stack.size(); i++){
        stackSet.add(stack.pop());
    }
    if (stackSet.contains(location)) {
        return true;
    }
    else{
        return false;
    }
}

//this function utilizes the helper functions generateValidMoves and containsForStack
//in order to solve a maze using Breadth-first search. It takes a Grid<bool> maze and
//returns a Stack of GridLocations which is the solution and draws the path using graphics
//while the maze is solved.
Stack<GridLocation> solveMaze(Grid<bool>& maze) {
    MazeGraphics::drawGrid(maze);
    Stack<GridLocation> path;
    GridLocation end = {maze.height() - 1, maze.width() - 1};
    bool pathFound = false;

    Queue<Stack<GridLocation>> queuePaths;
    Stack<GridLocation> firstPath = {{0, 0}};
    queuePaths.enqueue(firstPath);

    while (pathFound == false){
        Stack<GridLocation> currentPath = queuePaths.dequeue();
        MazeGraphics::highlightPath(currentPath, "blue");

        if (currentPath.peek() == end) { //if this path is the correct one, we're done
            path = currentPath;
            MazeGraphics::highlightPath(currentPath, "green");
            pathFound = true;
        }
        else{ //generates new paths based on valid neighbors and enqueues them
            Set<GridLocation> neighbors = generateValidMoves(maze, currentPath.peek());

            for (GridLocation neighbor : neighbors){
                if (!containsForStack(currentPath, neighbor)) { //checks whether our path already includes the neighbor
                    Stack<GridLocation> newPath = currentPath;
                    newPath.add(neighbor);
                    queuePaths.enqueue(newPath);
                }
            }
        }
    }
    return path;
}

//readMazeFile reads a file into a Grid<bool> maze
//In order to make this function more robust,I added error checking functionality that insures no row is
//longer or shorter than the others and that the maze does not contain characters other than '@' and '-'
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following lines read in the data from the file into a Vector
     * of strings representing the lines of the file. We haven't talked
     * in class about what ifstreams are, so don't worry if you don't fully
     * understand what is going on here.
     */
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line

    for (int i = 1; i < numRows; i++){ //makes sure all rows have the same length
        if (lines[i].length() != numCols) {
            error("A maze row is longer or shorter than the other rows");
        }
    }

    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            }
            else{
                error("The maze contains a character that is not '@' or '-'");
            }
        }
    }
}

/* This provided functions opens and read the contents of files ending
 * in a .soln extension and interprets the contents as a Stack of
 * GridLocations, populating the provided soln data structure.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Stack<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    Vector<string> lines;
    readEntireFile(in, lines);

    if (lines.size() != 1){
        error("File contained too many or too few lines.");
    }

    istringstream istr(lines[0]); // Stack read does its own error-checking
    if (!(istr >> soln)) {// if not successfully read
        error("Solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateNeighbors on location in the center of 3x3 grid with no walls"){
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> neighbors = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, center));
}

PROVIDED_TEST("generateNeighbors on location on the side of 3x3 grid with no walls"){
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> neighbors = {{0,0}, {0,2}, {1, 1}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, side));
}

PROVIDED_TEST("generateNeighbors on corner of 2x2 grid with walls"){
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> neighbors = {{1, 0}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, corner));
}


PROVIDED_TEST("checkSolution on correct path") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(checkSolution(maze, soln));
}

PROVIDED_TEST("checkSolution on correct path loaded from file for medium maze"){
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(checkSolution(maze, soln));
}

PROVIDED_TEST("checkSolution on correct path loaded from file for large maze"){
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/25x33.maze", maze);
    readSolutionFile("res/25x33.soln", soln);

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(checkSolution(maze, soln));
}

PROVIDED_TEST("checkSolution on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> not_end_at_exit = { {1, 0}, {0, 0} };
    Stack<GridLocation> not_begin_at_entry = { {1, 0}, {1, 1} };
    Stack<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {1, 1} };
    Stack<GridLocation> teleport = { {0 ,0}, {1, 1} };

    EXPECT_ERROR(checkSolution(maze, not_end_at_exit));
    EXPECT_ERROR(checkSolution(maze, not_begin_at_entry));
    EXPECT_ERROR(checkSolution(maze, go_through_wall));
    EXPECT_ERROR(checkSolution(maze, teleport));
}

PROVIDED_TEST("solveMaze on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(checkSolution(maze, soln));
}

PROVIDED_TEST("solveMaze on file 21x35") {
    Grid<bool> maze;
    readMazeFile("res/21x35.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(checkSolution(maze, soln));
}

PROVIDED_TEST("Test readMazeFile on valid file 2x2.maze") {
    Grid<bool> maze;

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(readMazeFile("res/2x2.maze", maze));
}

PROVIDED_TEST("Test readMazeFile on valid file 5x7.maze") {
    Grid<bool> maze;

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(readMazeFile("res/5x7.maze", maze));
}

PROVIDED_TEST("readMazeFile on nonexistent file should raise an error") {
    Grid<bool> g;

    EXPECT_ERROR(readMazeFile("res/nonexistent_file", g));
}

PROVIDED_TEST("readMazeFile on malformed file should raise an error") {
    Grid<bool> g;

    EXPECT_ERROR(readMazeFile("res/malformed.maze", g));
}

//STUDENT TESTS
STUDENT_TEST("generateNeighbors on location in the center of 3x3 grid with walls"){
    Grid<bool> maze = {{true, true, true},
                       {true, true, false},
                       {true, false, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> neighbors = {{1, 0}, {0, 1}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, center));
}

STUDENT_TEST("generateNeighbors on location on the side of 3x3 grid with walls"){
    Grid<bool> maze = {{false, true, true},
                       {true, false, true},
                       {true, true, true}};
    GridLocation side = {1, 0};
    Set<GridLocation> neighbors = {{2,0}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, side));
}

STUDENT_TEST("generateNeighbors on location on the side of 2x2 grid with walls"){
    Grid<bool> maze = {{false, false},
                       {true, false}};
    GridLocation side = {1, 0};
    Set<GridLocation> neighbors = {};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, side));
}

STUDENT_TEST("generateNeighbors on location on the corner of 4x4 grid with walls"){
    Grid<bool> maze = {{false, true, true, true},
                       {true, false, true, true},
                       {true, true, true, false},
                       {true, false, true, false}};
    GridLocation corner = {3, 0};
    Set<GridLocation> neighbors = {{2,0}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, corner));
}

STUDENT_TEST("generateNeighbors on location on 4x4 grid with walls"){
    Grid<bool> maze = {{false, true, true, true},
                       {true, false, true, true},
                       {true, true, true, false},
                       {true, false, true, false}};
    GridLocation location = {2, 2};
    Set<GridLocation> neighbors = {{1,2}, {3, 2}, {2, 1}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, location));
}

STUDENT_TEST("checkSolution on correct path loaded from file for medium/large maze"){
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/19x35.maze", maze);
    readSolutionFile("res/19x35.soln", soln);

    EXPECT_NO_ERROR(checkSolution(maze, soln));
}


STUDENT_TEST("checkSolution on invalid path should raise error") {
    Grid<bool> maze = {{true, false, false},
                       {true, true, false},
                       {true, true, true}};

    Stack<GridLocation> teleport = { {0, 0}, {1, 1}, {2, 2}};
    Stack<GridLocation> outOfBounds = { {0, 0}, {5, 1}, {2, 2}};
    Stack<GridLocation> loops = { {0 ,0}, {1, 0}, {0, 0}, {1, 0}, {1, 1}, {2, 1}, {2, 2}};

    EXPECT_ERROR(checkSolution(maze, teleport));
    EXPECT_ERROR(checkSolution(maze, outOfBounds));
    EXPECT_ERROR(checkSolution(maze, loops));
}

STUDENT_TEST("checkSolution on invalid path should raise error") {
    Grid<bool> maze = {{true, false, true},
                       {true, false, false},
                       {false, true, true}};

    Stack<GridLocation> wrongStart = { {2, 0}, {1, 0}, {1, 1}, {2, 1}, {2, 2}};
    Stack<GridLocation> inWall = { {0, 0}, {1, 0}, {3, 1}, {2, 1}, {2, 2}};

    EXPECT_ERROR(checkSolution(maze, wrongStart));
    EXPECT_ERROR(checkSolution(maze, inWall));
}

STUDENT_TEST("checkSolution on invalid path should raise error") {
    Grid<bool> maze = {{true, false, true},
                       {true, true, false},
                       {false, true, true}};

    Stack<GridLocation> wrongExit = { {2, 1}, {1, 0}, {1, 1}, {2, 1}, {1, 2}};
    Stack<GridLocation> teleport = { {0, 0}, {1, 1}, {2, 1}, {2, 2}};
    Stack<GridLocation> loop= {{0, 0}, {1, 0}, {1, 1}, {2, 1}, {2, 2}, {2, 1}, {2,2}};

    EXPECT_ERROR(checkSolution(maze, wrongExit));
    EXPECT_ERROR(checkSolution(maze, teleport));
    EXPECT_ERROR(checkSolution(maze, loop));

}

STUDENT_TEST("solveMaze on file 33x41") {
    Grid<bool> maze;
    readMazeFile("res/33x41.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(checkSolution(maze, soln));
}

STUDENT_TEST("solveMaze and check solution for 25x33") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/25x33.maze", maze);
    readSolutionFile("res/25x33.soln", soln);
    Stack<GridLocation> mySolution = solveMaze(maze);

    EXPECT_EQUAL(mySolution, soln);
}

STUDENT_TEST("solveMaze and check solution for 17x37") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/17x37.maze", maze);
    readSolutionFile("res/17x37.soln", soln);
    Stack<GridLocation> mySolution = solveMaze(maze);

    EXPECT_EQUAL(mySolution, soln);
}

STUDENT_TEST("checking extra helper function, containsForStack"){
    Stack<GridLocation> a;
    a.push(GridLocation(1, 4));
    a.push(GridLocation(4, 5));
    a.push(GridLocation(2,2));

    EXPECT_EQUAL(containsForStack(a, GridLocation(4, 5)), true);
    EXPECT_EQUAL(containsForStack(a, GridLocation(7, 5)), false);

}

STUDENT_TEST("readMazeFile on file with some rows longer than others should raise an error") {
    Grid<bool> g;

    EXPECT_ERROR(readMazeFile("res/malformed_2.maze", g)); //this malformed maze file that I created
                                                           //has some rows that are either longer or shorter than the others
}

STUDENT_TEST("readMazeFile on file with some rows longer than others and non characters should raise an error") {
    Grid<bool> g;

    EXPECT_ERROR(readMazeFile("res/malformed_3.maze", g)); //this malformed maze file has incongruent rows as well as non-accepted characters
}

