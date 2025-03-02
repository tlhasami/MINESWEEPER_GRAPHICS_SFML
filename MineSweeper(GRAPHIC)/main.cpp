#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace sf;

//---------------> Screen resolution
const int screen_resolution_x = 605;
const int screen_resolution_y = 550;
const int limit = 10;
//---------------> Game-Over
bool game_over = false;
Font font;
//--------------> CLASS for Main Mine
class MINE
{
public:
    bool hidden;
    char value;

private:
    // Use char for value
    RectangleShape mine;
    Text hidden_number;

    void after_effect()
    {
        if (!hidden)
        {
            hidden_number.setString(" ");
        }
        else
        {
            hidden_number.setString(value == 'X' ? "X" : "" + to_string(value - '0'));
            if (value == 'X')
            {
                hidden_number.setFillColor(Color::Red);
            }
            else if (value == '1')
            {
                hidden_number.setFillColor(Color::Green);
            }
            else if (value == '2')
            {
                hidden_number.setFillColor(Color::Yellow);
            }
            else if (value == '3')
            {
                hidden_number.setFillColor(Color::Cyan);
            }
            else if (value == '4')
            {
                hidden_number.setFillColor(Color::Magenta);
            }
            else if (value == 'X')
            {
                hidden_number.setString(to_string(value));
                hidden_number.setFillColor(Color::White);
            }
            else
            {
                mine.setFillColor(Color::White);
                hidden_number.setString(" ");
            }
        }
    }

public:
    void values(char number, bool stat, Vector2f size, Vector2f position, Font &font)
    {
        value = number;
        hidden = stat;
        mine.setSize(size);
        mine.setPosition(position);
        mine.setOutlineThickness(2.f);
        mine.setFillColor(Color::Black);
        hidden_number.setFont(font);
        hidden_number.setCharacterSize(20);                                              // Set a suitable character size
        hidden_number.setPosition(position.x + size.x / 4 + 5, position.y + size.y / 4); // Center text within the rectangle
        after_effect();
    }

    void status(bool showed)
    {
        hidden = showed;
        after_effect();
    }

    void draw(RenderWindow &window)
    {
        window.draw(mine);

        if (hidden)
            window.draw(hidden_number);
    }

    bool contains(Vector2i mousePos)
    {
        return mine.getGlobalBounds().contains(Vector2f(mousePos));
    }
};

void screen(RectangleShape &);
void tracker(char box[limit][limit], MINE mines[limit][limit], int _x, int _y);
void font_register(Font &);
void bomb_generator(char bomb[][limit], int n);
void mines_dealer(MINE mines[][limit], char backhand[limit][limit]);
void reconstruction_of_game(MINE mines[][limit], char back_hand[limit][limit]);

int main()
{
    // Create a window
    srand(time(0));
    RenderWindow window(VideoMode(screen_resolution_x, screen_resolution_y), "Hover Effect Example");
    Music kill;
    if (!kill.openFromFile("Music/kill.ogg"))
    {
        cout << "Error loading kill file!" << endl;
        return -1;
    }
    kill.setVolume(200);
    // Create a rectangle shape
    RectangleShape rectangle;
    Text again;
    again.setString("O");
    again.setPosition(565.f, 5.f);
    again.setFont(font);
    again.setFillColor(Color(100, 44, 56));
    screen(rectangle);
    font_register(font);
    char back_hand[limit][limit];
    for (int i = 0; i < limit; i++)
    {
        for (int j = 0; j < limit; j++)
        {
            back_hand[i][j] = ' ';
        }
    }
    bomb_generator(back_hand, limit);
    MINE mines[limit][limit];
    mines_dealer(mines, back_hand);
    Text MINESWEEPER;
    MINESWEEPER.setString("MINE-SWEEPER");
    MINESWEEPER.setPosition(165.f, 5.f);
    MINESWEEPER.setFont(font);
    MINESWEEPER.setFillColor(Color::White);
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event ::Closed)
            {
                window.close();
            }
            if (event.type == Event::MouseButtonPressed &&
                event.mouseButton.button == Mouse::Left)
            {
                Vector2i mousePosition = Mouse::getPosition(window);
                if (again.getGlobalBounds().contains(Vector2f(mousePosition)))
                {
                    reconstruction_of_game(mines, back_hand);
                }
                if (!game_over)
                {
                    for (int i = 0; i < limit; i++)
                    {
                        for (int j = 0; j < limit; j++)
                        {

                            if (mines[i][j].contains(mousePosition))
                            { // Add a contain` method in `MINE`
                                if (back_hand[i][j] == ' ')
                                {
                                    cout << "tracking " << endl;
                                    tracker(back_hand, mines, i, j);
                                }
                                else
                                {

                                    cout << " not tracking " << endl;
                                    mines[i][j].status(true); // Reveal the mine
                                    if (mines[i][j].value == 'X')
                                    {
                                        kill.play();
                                        game_over = true;
                                        for (int k = 0; k < limit; k++)
                                        {
                                            for (int l = 0; l < limit; l++)
                                            {
                                                mines[k][l].status(true);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // Get the mouse position relative to the window
        // Check if the mouse is over the rectangle
        window.clear();
        window.draw(rectangle); // Draw the rectangle
        for (int i = 0; i < limit; i++)
        {
            for (int j = 0; j < limit; j++)
            {
                mines[i][j].draw(window);
            }
        }
        window.draw(again);
        window.draw(MINESWEEPER);
        window.display();
    }

    return 0;
}

void screen(RectangleShape &rectangle)
{
    rectangle.setSize(Vector2f(600.f, 500.f)); // Width: 200, Height: 100
    rectangle.setFillColor(Color(100, 100, 100));
    rectangle.setPosition(1.f, 100.f); // Position on the screen
}

void font_register(Font &font)
{
    if (!font.loadFromFile("Fonts/Super_Comic.ttf"))
    {
        cerr << "Error loading font file!" << endl;
        exit(EXIT_FAILURE); // Exit if the font fails to load
    }
}
// Offsets for the 8 neighboring cells
const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

void calculateNeighborCounts(char bomb[][limit], int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            // Skip cells with bombs
            if (bomb[i][j] == 'X')
                continue;
            int count = 0;
            // Check all 8 neighboring cells
            for (int k = 0; k < 8; k++)
            {
                int ni = i + dx[k];
                int nj = j + dy[k];
                // Ensure the neighbor is within bounds and contains a bomb
                if (ni >= 0 && ni < n && nj >= 0 && nj < n && bomb[ni][nj] == 'X')
                {
                    count++;
                }
            }
            // Update the cell with the bomb count (convert to char)
            if (count > 0)
            {
                bomb[i][j] = count + '0';
            }
        }
    }
}

void bomb_generator(char bomb[][limit], int n = 10)
{
    int bombs = 5;

    // Place bombs randomly
    for (int i = 0; i < bombs; i++)
    {
        int bombX = rand() % n;
        int bombY = rand() % n;

        while (bomb[bombX][bombY] == 'X')
        {
            bombX = rand() % n;
            bombY = rand() % n;
        }

        bomb[bombX][bombY] = 'X';
    }

    // Calculate neighbor counts
    calculateNeighborCounts(bomb, n);
}

void mines_dealer(MINE mines[][limit], char backhand[limit][limit])
{
    int x = 600 / limit, y = 500 / limit;
    for (int i = 0; i < limit; i++)
    {
        for (int j = 0; j < limit; j++)
        {
            mines[i][j].values(int(backhand[i][j]), false, Vector2f(x - 5, y - 5), Vector2f(j * x + 5, i * y + 50), font);
        }
    }
}
void tracker(char box[limit][limit], MINE mines[limit][limit], int _x, int _y)
{
    cout << box[_x][_y] << " status" << mines[_x][_y].hidden << endl;
    // Boundary and base conditions
    if (_x < 0 || _y < 0 || _x >= 10 || _y >= 10 || (mines[_x][_y].hidden != 0))
    {
        cout << "return" << endl;
        return;
    }
    if (box[_x][_y] == ' ')
    {
        cout << "Cleaning: " << _x << ", " << _y << endl;
        mines[_x][_y].status(true);
        // Only recurse if the cell is empty
        for (int i = 0; i < 8; ++i)
        {
            int nx = _x + dx[i], ny = _y + dy[i];
            tracker(box, mines, nx, ny);
        }
    }

    return;
}

void reconstruction_of_game(MINE mines[][limit], char back_hand[limit][limit])
{
    game_over = false;
    for (int i = 0; i < limit; i++)
    {
        for (int j = 0; j < limit; j++)
        {
            back_hand[i][j] = ' ';
        }
    }
    bomb_generator(back_hand);
    mines_dealer(mines, back_hand);
}
