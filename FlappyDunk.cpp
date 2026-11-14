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

bool isInside(Vector2f mouse, float x, float y, float w, float h) {

    return mouse.x >= x && mouse.x <= x + w &&
        mouse.y >= y && mouse.y <= y + h;
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

    int screen = 0;    // 0 : main menu  ,   1 : game   , 2 : guide

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

                    if (screen == 0) {
                        // main menu
                        if (isInside(mouse, 330, 210, 200, 60))
                        {
                            screen = 1;
                        }
                        else if (isInside(mouse, 330, 280, 200, 60))
                        {
                            screen = 2;
                        }
                        else if (isInside(mouse, 330, 350, 200, 60))
                        {
                            window.close();
                        }
                    }
                    else if (screen == 2) {       // instructions

                        if (isInside(mouse, 330, 560, 200, 60))
                        {
                            screen = 0;
                        }
                    }
                }
            }

        }

        window.clear(Color(19, 27, 43));

        if (screen == 0) {

            drawText(window, font, "Flappy Dunk", 50, 310.f, 60.f, Color(255, 170, 70));
            drawText(window, font, "Bounce. Aim. Score.", 20, 370.f, 130.f);

            drawButton(window, font, "Play", 330.f, 210.f, 200.f, 60.f);
            drawButton(window, font, "Guide", 330.f, 280.f, 200.f, 60.f);
            drawButton(window, font, "Quit", 330.f, 350.f, 200.f, 60.f);

            drawText(window, font, "Best score: " + to_string(bestScore), 20, 370.f, 450.f);
        }
        else if (screen == 2) {

            drawText(window, font, "How to Play", 45, 320.f, 60.f, Color(255, 170, 70));
            drawText(window, font, "Space / Left click: bounce the ball upward.", 24, 100.f, 170.f);
            drawText(window, font, "Pass down through each hoop to score.", 24, 100.f, 225.f);
            drawText(window, font, "Basket: 1 point.", 24, 100.f, 280.f);
            drawText(window, font, "S: slow motion for 2 seconds, 3 uses per game.", 24, 100.f, 335.f);
            drawText(window, font, "Missing a hoop or hitting the floor ends the game.", 24, 100.f, 390.f);
            drawText(window, font, "R: restart. Escape: return to the menu.", 24, 100.f, 445.f);

            drawButton(window, font, "Back", 330.f, 560.f, 200.f, 60.f);
        }
        else if (screen == 1) {
            // main menu 
        }

        window.display();
    }

    return 0;
}