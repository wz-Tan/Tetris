#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <ctime>
#include <conio.h>
#include <mutex>

const int ROWS = 10;
const int COLS = 10;
const int DROP_INTERVAL = 3;

// Grid Struct
struct GridStruct
{
    std::string grid[ROWS][COLS];

    // Constructor
    GridStruct()
    {
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                grid[i][j] = "00";
            }
        }
    }
};

struct Block;

enum class BlockCode
{
    I_BLOCK,
    L1_BLOCK,
    L2_BLOCK,
    S1_BLOCK,
    S2_BLOCK,
    T_BLOCK,
    SQUARE_BLOCK,
};

void displayGrid(std::string (&grid)[ROWS][COLS], Block activeBlock);
void clearPreviousCoordinates(std::string (&grid)[ROWS][COLS], Block activeBlock);
void checkWin(std::string (&grid)[ROWS][COLS], int &score);
Block spawnBlock(std::string (&grid)[ROWS][COLS], int score);

// Block Struct
struct Block
{
    int prevCoordinates[4][2] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
    int coordinates[4][2];
    int centerOfRotation[2];

    // Rotation (0 as default, clockwise adds 1, counter clockwise minus 1)
    int rotationCounter = 0;
    BlockCode blockCode;

    void rewriteCoordinates(int newCoordinates[4][2], Block &self)
    {
        for (int i = 0; i < 4; i++)
        {
            self.prevCoordinates[i][0] = self.coordinates[i][0];
            self.prevCoordinates[i][1] = self.coordinates[i][1];

            self.coordinates[i][0] = newCoordinates[i][0];
            self.coordinates[i][1] = newCoordinates[i][1];
        }
    }

    //Returns New Block, or Itself Again
    Block moveDown(std::string (&grid)[ROWS][COLS], Block &self, int &score)
    {
        bool hitBottom=false;
        for (auto coord : self.coordinates)
        {
            // Hit Bottom
            if (coord[1] == 0)
            {
                hitBottom = true;
                break;
            }

            // Hit Obstacle, Make Sure Not Your Own
            else
            {
                if (grid[coord[1] - 1][coord[0]] != "00")
                {
                    bool ownCoordinate = false;

                    for (auto coord2 : self.coordinates)
                    {
                        if (coord2[0] == coord[0] && coord2[1] == coord[1] - 1)
                        {
                            ownCoordinate = true;
                            break;
                        }
                    }

                    if (!ownCoordinate)
                    {
                        hitBottom = true;
                        break;
                    }
                }
            }
        }

        if (hitBottom)
        {
            checkWin(grid, score);
            return spawnBlock(grid, score);
        }

        // Move Down
        int counter = 0;

        for (auto coord : self.coordinates)
        {
            self.prevCoordinates[counter][0] = coord[0];
            self.prevCoordinates[counter][1] = coord[1];
            coord[1]--;
            counter++;
        }

        self.centerOfRotation[1]--;
        return self;
    }

    void moveRight(std::string (&grid)[ROWS][COLS], Block &self)
    {

        for (auto coord : self.coordinates)
        {
            // Hit Edge
            if (coord[0] == COLS - 1)
            {
                return;
            }

            // Hit Obstacle, Make Sure Not Your Own
            else
            {
                if (grid[coord[1]][coord[0] + 1] != "00")
                {
                    bool ownCoordinate = false;

                    for (auto coord2 : self.coordinates)
                    {
                        if (coord2[0] == coord[0] + 1 && coord2[1] == coord[1])
                        {
                            ownCoordinate = true;
                            break;
                        }
                    }

                    if (!ownCoordinate)
                    {
                        return;
                    }
                }
            }
        }

        // Move Right
        int counter = 0;

        for (auto coord : self.coordinates)
        {
            self.prevCoordinates[counter][0] = coord[0];
            self.prevCoordinates[counter][1] = coord[1];
            coord[0]++;
            counter++;
        }

        self.centerOfRotation[0]++;
    }

    void moveLeft(std::string (&grid)[ROWS][COLS], Block &self)
    {
        for (auto coord : self.coordinates)
        {
            // Hit Edge
            if (coord[0] == 0)
            {
                return;
            }

            // Hit Obstacle, Make Sure Not Your Own
            else
            {
                if (grid[coord[1]][coord[0] - 1] != "00")
                {
                    bool ownCoordinate = false;

                    for (auto coord2 : self.coordinates)
                    {
                        if (coord2[0] == coord[0] - 1 && coord2[1] == coord[1])
                        {
                            ownCoordinate = true;
                            break;
                        }
                    }

                    if (!ownCoordinate)
                    {
                        return;
                    }
                }
            }
        }

        // Move Left
        int counter = 0;

        for (auto coord : self.coordinates)
        {
            self.prevCoordinates[counter][0] = coord[0];
            self.prevCoordinates[counter][1] = coord[1];
            coord[0]--;
            counter++;
        }

        self.centerOfRotation[0]--;
    }

    //if clockwise is false then its anticlockwise
    void rotate(std::string (&grid)[ROWS][COLS], Block &self, bool clockwise)
    {

        int counter = 0;
        int newCoordinates[4][2];

        //(x,y) becomes (y,-x) -> In our case since y goes in negative, so we dont need to make it negative x
        for (auto coord : self.coordinates)
        {
            int yDiff = coord[1] - self.centerOfRotation[1];
            int xDiff = coord[0] - self.centerOfRotation[0];
            
            if (clockwise){
               newCoordinates[counter][0] = self.centerOfRotation[0] + yDiff;
                newCoordinates[counter][1] = self.centerOfRotation[1] - xDiff; 
            }

            else{
                newCoordinates[counter][0] = self.centerOfRotation[0] - yDiff;
                newCoordinates[counter][1] = self.centerOfRotation[1] + xDiff; 
            }

            counter++;
        }

        // Make Sure Enough Space
        for (auto coord : newCoordinates)
        {
            // Hit Edge
            if ((coord[1] < 0 || coord[1] >= ROWS) || (coord[0] < 0 || coord[0] >= COLS))
            {
                return;
            }

            // Hit Obstacle, Make Sure Not Your Own
            else
            {
                if (grid[coord[1]][coord[0]] != "00")
                {
                    bool ownCoordinate = false;

                    for (auto coord2 : self.coordinates)
                    {
                        if (coord2[0] == coord[0] && coord2[1] == coord[1])
                        {
                            ownCoordinate = true;
                            break;
                        }
                    }

                    if (!ownCoordinate)
                    {
                        return;
                    }
                }
            }
        }

        // Assign Previous Coordinates for the Block
        self.rewriteCoordinates(newCoordinates, self);
        self.rotationCounter = (self.rotationCounter + 1) % 4;
        
    }
};

// Just Check Wins and Rewrite Score
void checkWin(std::string (&grid)[ROWS][COLS], int &score)
{
    int currRow = ROWS - 1;
    std::string temp;
    int counter = 0;

    while (currRow >= 0)
    {
        counter = 0;
        // Check for Full Rows
        for (int i = 0; i < COLS; i++)
        {
            if (grid[currRow][i] != "00")
            {
                counter++;
            }
        }

        // Entire Row Is Filled
        if (counter == COLS)
        {
            score += 100;

            std::cout << std::endl << "Cleared A Row! Current Score is " << score << std::endl;

            // Rewrite Row
            for (int i = 0; i < COLS; i++)
            {
                grid[currRow][i] = "00";
            }

            // Move Things Down (Leave Top 2 Rows Alone - For Spawning)
            for (int i = currRow+1; i < ROWS-2; i++)
            {
                for (int j = 0; j < COLS; j++)
                {
                    temp = grid[i][j];
                    grid[i][j] = "00";
                    grid[i - 1][j] = temp;
                }
            }
        }

        // Move To Next Row
        else
        {
            currRow--;
        }
    }
}

void clearPreviousCoordinates(std::string (&grid)[ROWS][COLS], Block activeBlock)
{
    for (auto coord : activeBlock.prevCoordinates)
    {
        // At The Start - No Need to Rewrite
        if (coord[0] != -1)
        {
            grid[coord[1]][coord[0]] = "00";
        }
    }
}

// Clears Previous Location and Redraws New One -> Other Functions Just Need to Change the Coordinates
void displayGrid(std::string (&grid)[ROWS][COLS], Block activeBlock)
{
    clearPreviousCoordinates(grid, activeBlock);

    for (auto coord : activeBlock.coordinates)
    {
        grid[coord[1]][coord[0]] = "11";
    }

    std::cout << std::endl;
    for (int i = ROWS - 1; i >= 0; i--)
    {
        for (int j = 0; j < COLS; j++)
        {
            std::cout << grid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// Spawning Blocks
Block spawnBlock(std::string (&grid)[ROWS][COLS], int score)
{
    Block newBlock;

    srand(time(NULL));
    int seed = (rand() % 7);

    // Spawn Different Blocks and Assign Their Start Points
    switch (seed)
    {
    case 0:
        newBlock.blockCode = BlockCode::I_BLOCK;

        for (int i = 0; i < 4; i++)
        {
            newBlock.coordinates[i][0] = 3 + i;
            newBlock.coordinates[i][1] = ROWS - 2;
        }

        newBlock.centerOfRotation[0] = newBlock.coordinates[1][0];
        newBlock.centerOfRotation[1] = newBlock.coordinates[1][1];

        break;
    case 1:
        newBlock.blockCode = BlockCode::L1_BLOCK;

        newBlock.coordinates[0][0] = 3;
        newBlock.coordinates[0][1] = ROWS - 1;

        for (int i = 1; i < 4; i++)
        {
            newBlock.coordinates[i][0] = 2 + i;
            newBlock.coordinates[i][1] = ROWS - 2;
        }

        newBlock.centerOfRotation[0] = newBlock.coordinates[2][0];
        newBlock.centerOfRotation[1] = newBlock.coordinates[2][1];

        break;

    case 2:
        newBlock.blockCode = BlockCode::L2_BLOCK;

        for (int i = 0; i < 3; i++)
        {
            newBlock.coordinates[i][0] = 3 + i;
            newBlock.coordinates[i][1] = ROWS - 2;
        }

        newBlock.coordinates[3][0] = 5;
        newBlock.coordinates[3][1] = ROWS - 1;

        newBlock.centerOfRotation[0] = newBlock.coordinates[1][0];
        newBlock.centerOfRotation[1] = newBlock.coordinates[1][1];

        break;

    case 3:
        newBlock.blockCode = BlockCode::S1_BLOCK;

        newBlock.coordinates[0][0] = 3;
        newBlock.coordinates[0][1] = ROWS - 2;

        newBlock.coordinates[1][0] = 4;
        newBlock.coordinates[1][1] = ROWS - 2;

        newBlock.coordinates[2][0] = 4;
        newBlock.coordinates[2][1] = ROWS - 1;

        newBlock.coordinates[3][0] = 5;
        newBlock.coordinates[3][1] = ROWS - 1;

        newBlock.centerOfRotation[0] = newBlock.coordinates[1][0];
        newBlock.centerOfRotation[1] = newBlock.coordinates[1][1];

        break;

    case 4:
        newBlock.blockCode = BlockCode::S2_BLOCK;

        newBlock.coordinates[0][0] = 3;
        newBlock.coordinates[0][1] = ROWS - 1;

        newBlock.coordinates[1][0] = 4;
        newBlock.coordinates[1][1] = ROWS - 1;

        newBlock.coordinates[2][0] = 4;
        newBlock.coordinates[2][1] = ROWS - 2;

        newBlock.coordinates[3][0] = 5;
        newBlock.coordinates[3][1] = ROWS - 2;

        newBlock.centerOfRotation[0] = newBlock.coordinates[2][0];
        newBlock.centerOfRotation[1] = newBlock.coordinates[2][1];

        break;

    case 5:

        newBlock.blockCode = BlockCode::T_BLOCK;

        newBlock.coordinates[0][0] = 4;
        newBlock.coordinates[0][1] = ROWS - 1;

        for (int i = 1; i < 4; i++)
        {
            newBlock.coordinates[i][0] = 2 + i;
            newBlock.coordinates[i][1] = ROWS - 2;
        }

        newBlock.centerOfRotation[0] = newBlock.coordinates[2][0];
        newBlock.centerOfRotation[1] = newBlock.coordinates[2][1];

        break;

    case 6:
        newBlock.blockCode = BlockCode::SQUARE_BLOCK;

        newBlock.coordinates[0][0] = 4;
        newBlock.coordinates[0][1] = ROWS - 1;

        newBlock.coordinates[1][0] = 5;
        newBlock.coordinates[1][1] = ROWS - 1;

        newBlock.coordinates[2][0] = 4;
        newBlock.coordinates[2][1] = ROWS - 2;

        newBlock.coordinates[3][0] = 5;
        newBlock.coordinates[3][1] = ROWS - 2;

        newBlock.centerOfRotation[0] = newBlock.coordinates[2][0];
        newBlock.centerOfRotation[1] = newBlock.coordinates[2][1];

        break;
    }

    for (auto coordinate: newBlock.coordinates){
        if (grid[coordinate[1]][coordinate[0]]!="00"){
            std::cout << std::endl << "You Have Lost!";
            std::cout << std::endl << "Your Total Score Is: " << score;
            exit(0);
        }
    }
    return newBlock;
}

void takeInput(std::string (&grid)[ROWS][COLS], Block &currentBlock, int &score){

    char response=' ';
    //Detect Keyboard Hit, If Yes Then Proceed Based Off Response, If Not We will go down per usual
    if (_kbhit()){
        response=_getch();
        if (response!=' '){
            if (response == 's')
                {
                    currentBlock=currentBlock.moveDown(grid,currentBlock,score);
                }
                else if (response == 'd')
                {
                    currentBlock.moveRight(grid, currentBlock);
                }
                else if (response == 'a')
                {
                    currentBlock.moveLeft(grid, currentBlock);
                }
                else if (response == 'r')
                {
                    currentBlock.rotate(grid, currentBlock, true);
                }

                else if (response == 'R')
                {
                    currentBlock.rotate(grid, currentBlock, false);
                }

                else if (response == 'q')
                {
                    exit(0);
                }
            displayGrid(grid,currentBlock);
            response=' ';
            }
    }

}


int main()
{
    GridStruct gridStruct = GridStruct();
    std::string(&grid)[ROWS][COLS] = gridStruct.grid;
    int score=0;
    Block currentBlock = spawnBlock(grid, score);
    auto lastDrop=std::chrono::steady_clock::now();
    displayGrid(grid, currentBlock);

    while (true)
    {
        takeInput(grid,currentBlock,score);
        //2 Seconds has Passed Since Last Drop
        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now()-lastDrop).count() >= DROP_INTERVAL){
            currentBlock=currentBlock.moveDown(grid, currentBlock, score);
            displayGrid(grid,currentBlock);
            lastDrop=std::chrono::steady_clock::now();
        }
        
    }

    return 0;
}