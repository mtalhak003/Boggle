#include <iostream>

#include <vector>


#include <random>
#include <ctime>

// Constants for board size
const int BOARD_SIZE = 5;

class Board
{
private:
    char grid[BOARD_SIZE][BOARD_SIZE];
    bool visited[BOARD_SIZE][BOARD_SIZE];

    // Arrays of letters for the dice faces
    const std::vector<std::vector<char>> dice = {
        {'A', 'A', 'A', 'F', 'R', 'S'},
        {'A', 'E', 'E', 'G', 'M', 'U'},
        {'C', 'E', 'I', 'L', 'T', 'T'},
        {'D', 'H', 'H', 'N', 'O', 'T'},
        {'F', 'I', 'P', 'R', 'S', 'Y'},
        {'A', 'O', 'O', 'T', 'T', 'W'},
        {'C', 'I', 'M', 'O', 'T', 'U'},
        {'D', 'E', 'I', 'L', 'R', 'X'},
        {'E', 'I', 'I', 'L', 'S', 'T'},
        {'E', 'L', 'P', 'S', 'T', 'U'},
        {'H', 'A', 'S', 'P', 'C', 'O'},
        {'H', 'I', 'M', 'N', 'Q', 'U'}, // 'Qu' changed to 'Q'
        {'A', 'B', 'B', 'J', 'O', 'O'},
        {'E', 'E', 'G', 'H', 'N', 'W'},
        {'E', 'E', 'I', 'N', 'S', 'U'},
        {'G', 'O', 'R', 'R', 'V', 'W'}};

    // Random number generator
    std::mt19937 rng{std::random_device{}()};

public:
    Board()
    {
        shuffleBoard();
    }

    void shuffleBoard()
    {
        std::vector<int> indices(BOARD_SIZE * BOARD_SIZE);
        for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
        {
            indices[i] = i;
        }
        std::shuffle(indices.begin(), indices.end(), rng);

        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            for (int j = 0; j < BOARD_SIZE; ++j)
            {
                int index = indices[i * BOARD_SIZE + j];
                int diceIndex = index % dice.size();
                int faceIndex = index / dice.size();
                grid[i][j] = dice[diceIndex][faceIndex];
                visited[i][j] = false;
            }
        }
    }

    void displayBoard()
    {
        std::cout << "\n\tBOGGLE BOARD:\n";
        std::cout << "\t-------------------\n";
        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            std::cout << "\t| ";
            for (int j = 0; j < BOARD_SIZE; ++j)
            {
                std::cout << grid[i][j] << " ";
            }
            std::cout << "|\n";
        }
        std::cout << "\t-------------------\n";
    }

    char getLetter(int row, int col)
    {
        return grid[row][col];
    }

    bool isValidPosition(int row, int col)
    {
        return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && !visited[row][col];
    }

    void markVisited(int row, int col)
    {
        visited[row][col] = true;
    }

    void unmarkVisited(int row, int col)
    {
        visited[row][col] = false;
    }
};

class Dictionary
{
private:
    std::unordered_set<std::string> wordSet;

public:
    Dictionary(const std::string &dictFile)
    {
        std::ifstream fin(dictFile);
        if (!fin.is_open())
        {
            throw std::runtime_error("Failed to open dictionary file");
        }

        std::string word;
        while (std::getline(fin, word))
        {
            wordSet.insert(word);
        }
        fin.close();
    }

    bool isValidWord(const std::string &word) const
    {
        return wordSet.count(word) > 0;
    }
};

struct Point
{
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
};

// Direction arrays for moving in 8 possible directions (horizontal, vertical, diagonal)
const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

bool isValidCell(int x, int y, Board &board)
{
    return board.isValidPosition(x, y);
}

void dfs(Board &board, int x, int y, std::string &currentWord, Dictionary &dictionary, std::vector<std::string> &foundWords)
{
    // Mark current cell as visited
    board.markVisited(x, y);

    // Append current letter to the current word
    currentWord += board.getLetter(x, y);

    // Check if the current word is valid and in dictionary
    if (currentWord.size() >= 3 && dictionary.isValidWord(currentWord))
    {
        foundWords.push_back(currentWord);
    }

    // Explore all 8 neighbors
    for (int dir = 0; dir < 8; ++dir)
    {
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        if (isValidCell(nx, ny, board) && !board.isValidPosition(nx, ny))
        {
            dfs(board, nx, ny, currentWord, dictionary, foundWords);
        }
    }

    // Backtrack: Unmark the current cell as visited and remove its letter from current word
    board.unmarkVisited(x, y);
    currentWord.pop_back();
}

int main()
{
    std::cout << "\n\tGAME INSTRUCTIONS\n";
    std::cout << "1. You can make words using the letters available on the 5X5 board\n";
    std::cout << "2. The letters must be adjoining in a 'chain'\n";
    std::cout << "   (Letters in the chain may be adjacent horizontally, vertically, or diagonally)\n";
    std::cout << "3. Each word should be of at least three letters\n";
    std::cout << "4. Words that have the exact spelling but different meanings will be counted only once\n";
    std::cout << "5. You cannot repeat any words\n";
    std::cout << "6. You can use both singular and plural forms of the same word\n";
    std::cout << "7. Time Limit = 120 seconds\n";
    std::cout << "8. Press 0 to end the game\n";

    Board board;
    board.displayBoard();

    Dictionary dictionary("GameDict.txt");

    std::vector<std::string> foundWords;
    std::string word;
    time_t startTime = std::time(nullptr);
    time_t endTime = startTime + 120; // 120 seconds limit

    while (std::time(nullptr) <= endTime)
    {
        std::cout << "\nEnter a word (or 0 to end): ";
        std::cin >> word;

        if (word == "0")
        {
            break;
        }

        std::transform(word.begin(), word.end(), word.begin(), ::toupper);

        if (word.size() < 3)
        {
            std::cout << "Word must be at least 3 letters long.\n";
            continue;
        }

        if (std::find(foundWords.begin(), foundWords.end(), word) != foundWords.end())
        {
            std::cout << "Word already entered.\n";
            continue;
        }

        bool foundOnBoard = false;
        // Perform DFS to check if the word can be formed on the board
        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            for (int j = 0; j < BOARD_SIZE; ++j)
            {
                if (board.getLetter(i, j) == word[0])
                {
                    std::string currentWord = "";
                    dfs(board, i, j, currentWord, dictionary, foundWords);
                    if (std::find(foundWords.begin(), foundWords.end(), word) != foundWords.end())
                    {
                        foundOnBoard = true;
                        break;
                    }
                }
            }
            if (foundOnBoard)
                break;
        }

        if (foundOnBoard)
        {
            std::cout << "Valid word!\n";
        }
        else
        {
            std::cout << "Invalid word or not on the board.\n";
        }
    }

    // Display found words and calculate score
    std::cout << "\nValid words entered by you are: ";
    for (const auto &w : foundWords)
    {
        std::cout << w << ", ";
    }
    std::cout << "\n";

    int score = foundWords.size(); // Simple scoring based on number of valid words
    std::cout << "\nYou scored " << score << " points!\n";

    return 0;
}
