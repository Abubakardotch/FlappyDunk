#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;
using namespace sf;

const int window_width = 900;
const int window_height = 700;

void drawText(RenderWindow& window, Font& font,
    string str, int size,
    float x, float y, Color color = Color::White)
{
    Text text(font, str, size);
    text.setFillColor(color);
    text.setPosition({ x, y });
    window.draw(text);
}

void drawButton(RenderWindow& window, Font& font,
    string label, float x, float y, float width, float height)
{
    RectangleShape button({ width, height });
    button.setPosition({ x, y });
    button.setFillColor(Color(40, 65, 100));

    window.draw(button);

    drawText(window, font, label, 30, x + 70, y + 10);
}

int loadBestScore() {

    ifstream file("best_score.txt");

    int score = 0;

    if (!(file >> score)) {
        return 0;
    }

    return score;
}

bool saveBestScore(int score) {

    ofstream file("best_score.txt");

    if (!file) {
        return false;
    }

    file << score << '\n';
    file.close();

    return !file.fail();
}


int main() {

    Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        return 1;
    }

    int bestScore = loadBestScore();

    RenderWindow window(
        VideoMode({ window_width, window_height }),
        "Flappy Dunk",
        Style::Titlebar | Style::Close
    );

    window.setFramerateLimit(60);


    while (window.isOpen()) {

        while (const auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>()) {
                window.close();
            }

            if (const auto* click = event->getIf<Event::MouseButtonPressed>())
            {
                if (click->button == Mouse::Button::Left) {

                    Vector2f mouse = window.mapPixelToCoords(click->position);

                    if (mouse.x >= 330.f && mouse.x <= 530.f &&
                        mouse.y >= 210.f && mouse.y <= 270.f)
                    {
                        cout << "Play clicked" << endl;
                    }

                    if (mouse.x >= 330.f && mouse.x <= 530.f &&
                        mouse.y >= 280.f && mouse.y <= 340.f)
                    {
                        cout << "Guide clicked" << endl;
                    }

                    if (mouse.x >= 330.f && mouse.x <= 530.f &&
                        mouse.y >= 350.f && mouse.y <= 410.f)
                    {
                        window.close();

                    }
                }
            }

        }

        window.clear(Color(19, 27, 43));

        drawText(window, font, "Flappy Dunk", 50, 310.f, 60.f, Color(255, 170, 70));
        drawText(window, font, "Bounce. Aim. Score.", 20, 370.f, 130.f);

        drawButton(window, font, "Play", 330.f, 210.f, 200.f, 60.f);
        drawButton(window, font, "Guide", 330.f, 280.f, 200.f, 60.f);
        drawButton(window, font, "Quit", 330.f, 350.f, 200.f, 60.f);

        drawText(window, font, "Best score: " + to_string(bestScore), 20, 370.f, 450.f);

        window.display();
    }

    return 0;
}