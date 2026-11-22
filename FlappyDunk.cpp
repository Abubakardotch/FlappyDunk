#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <random>

using namespace std;
using namespace sf;

const int window_width = 900;
const int window_height = 700;
const float ball_size = 20.f;
const float hoop_width = 120.f;
const float hoop_height = 70.f;

const float ball_x = 250.f;
const float gravity = 1200.f;
const float jump_speed = -450.f;
const float hoop_speed = 130.f;
const float floor_y = 660.f;

struct Game {

    float ballY = 250.f;
    float velocityY = 0.f;
    float hoopX = 650.f;
    float hoopY = 420.f;
    bool hoopScored = false;
    int score = 0;
    float msgTime = 0.f;
    bool started = false;
    bool over = false;
    float slowTime = 0.f;
    int slowLeft = 3;
};

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

bool isInside(float mouseX, float mouseY, float x, float y, float w, float h) {

    return mouseX >= x && mouseX <= x + w &&
        mouseY >= y && mouseY <= y + h;
}

void ball(RenderWindow& window, float x, float y){

    CircleShape ball(ball_size);
    ball.setOrigin({ ball_size, ball_size });
    ball.setPosition({ x, y });
    ball.setFillColor(Color(255, 150, 45));
    window.draw(ball);
}

void hoop(RenderWindow& window, float x, float y){

    Color color(255, 150, 45);

    RectangleShape left({ 5.f, hoop_height });
    left.setPosition({ x, y });
    left.setFillColor(color);
    window.draw(left);

    RectangleShape right({ 5.f, hoop_height });
    right.setPosition({ x + hoop_width - 5.f, y });
    right.setFillColor(color);
    window.draw(right);

    CircleShape rim(hoop_width / 2.f, 80);
    rim.setOrigin({ hoop_width / 2.f, hoop_width / 2.f });
    rim.setPosition({ x + hoop_width / 2.f, y });
    rim.setScale({ 1.f, 0.14f });
    rim.setFillColor(Color::Transparent);
    rim.setOutlineThickness(5.f);
    rim.setOutlineColor(color);
    window.draw(rim);
}

void resetGame(Game& game){

    game.ballY = 250.f;
    game.velocityY = 0.f;

    game.hoopX = 650.f;
    game.hoopY = 420.f;
    game.hoopScored = false;

    game.score = 0;
    game.msgTime = 0.f;

    game.started = false;
    game.over = false;

    game.slowTime = 0.f;
    game.slowLeft = 3;
}

void flap(Game& game) {

    if (game.over) {
        return;
    }

    game.started = true;
    game.velocityY = jump_speed;
}

void endGame(Game& game, int& bestScore){

    game.over = true;

    if (game.score > bestScore) {
        bestScore = game.score;
        saveBestScore(bestScore);
    }
}

float hoopPosition() {

    static mt19937 generator(random_device{}());
    uniform_int_distribution<int> range(250, 500);
    return (float)range(generator);
}

void updateGame(Game& game, float dt, int& bestScore){

    if (!game.started || game.over) {
        return;
    }

    game.msgTime -= dt;
    if (game.msgTime < 0.f) {
        game.msgTime = 0.f;
    }

    float moveDt = dt;

    if (game.slowTime > 0.f) {
        moveDt = dt * 0.5f;
        game.slowTime -= dt;
        if (game.slowTime < 0.f) {
            game.slowTime = 0.f;
        }
    }

    float oldBallY = game.ballY;

    game.velocityY += gravity * moveDt;
    game.ballY += game.velocityY * moveDt;
    game.hoopX -= hoop_speed * moveDt;

    // ball can not move out from top
    if (game.ballY < ball_size) {
        game.ballY = ball_size;
        game.velocityY = 0.f;
    }
    
    if (!game.hoopScored && game.velocityY > 0.f &&
        oldBallY < game.hoopY && game.ballY >= game.hoopY)
    {
        if (ball_x - ball_size >= game.hoopX + 8.f &&
            ball_x + ball_size <= game.hoopX + hoop_width - 8.f)
        {
            game.hoopScored = true;
            game.score++;
            game.msgTime = 1.f;
        }
    }

    // hoop missed
    if (!game.hoopScored && game.hoopX + hoop_width - 8.f < ball_x + ball_size) {
        endGame(game, bestScore);
        return;
    }

    // ball touched the floor
    if (game.ballY + ball_size >= floor_y) {
        endGame(game, bestScore);
        return;
    }

    // new hoop 
    if (game.hoopScored && game.hoopX + hoop_width < ball_x - 60.f) {
        game.hoopX = window_width + 40.f;
        game.hoopY = hoopPosition();
        game.hoopScored = false;
    }
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

    Game game;

    while (window.isOpen()) {

        float dt = 1.f / 60.f;

        while (const auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>()) {
                window.close();
            }

            if (const auto* key = event->getIf<Event::KeyPressed>())
            {
                if (key->code == Keyboard::Key::Escape)
                {
                    screen = 0;
                }
                else if (screen == 1)
                {
                    if (key->code == Keyboard::Key::Space) {
                        flap(game);
                    }
                    else if (key->code == Keyboard::Key::R) {
                        resetGame(game);
                    }
                    else if (key->code == Keyboard::Key::S) {
                        if (game.started && !game.over && game.slowLeft > 0 && game.slowTime <= 0.f) {
                            game.slowLeft--;
                            game.slowTime = 2.f;
                        }
                    }
                }
            }

            if (const auto* click = event->getIf<Event::MouseButtonPressed>())
            {
                if (click->button == Mouse::Button::Left) {

                    float mouseX = click->position.x;
                    float mouseY = click->position.y;

                    if (screen == 0){
                        // main menu
                        if (isInside(mouseX, mouseY, 330, 210, 200, 60)){

                            screen = 1;
                            resetGame(game);
                        }
                        else if (isInside(mouseX, mouseY, 330, 280, 200, 60))
                        {
                            screen = 2;
                        }
                        else if (isInside(mouseX,mouseY, 330, 350, 200, 60))
                        {
                            window.close();
                        }
                    }
                    else if (screen == 1) {
                        flap(game);
                    }
                    else if (screen == 2){       // instructions

                        if (isInside(mouseX,mouseY, 330, 560, 200, 60))
                        {
                            screen = 0;
                        }
                    }
                }
            }

        }

        if (screen == 1) {
            updateGame(game, dt, bestScore);
        }

        window.clear(Color(19, 27, 43));

        if (screen == 0){

            drawText(window, font, "Flappy Dunk", 50, 310.f, 60.f, Color(255, 170, 70));
            drawText(window, font, "Bounce. Aim. Score.", 20, 370.f, 130.f);

            drawButton(window, font, "Play", 330.f, 210.f, 200.f, 60.f);
            drawButton(window, font, "Guide", 330.f, 280.f, 200.f, 60.f);
            drawButton(window, font, "Quit", 330.f, 350.f, 200.f, 60.f);

            drawText(window, font, "Best score: " + to_string(bestScore), 20, 370.f, 450.f);
        }
        else if (screen == 2){

            drawText(window, font, "How to Play", 45, 320.f, 60.f, Color(255, 170, 70));
            drawText(window, font, "Space / Left click: bounce the ball upward.", 24, 100.f, 170.f);
            drawText(window, font, "Pass down through each hoop to score.", 24, 100.f, 225.f);
            drawText(window, font, "Basket: 1 point.", 24, 100.f, 280.f);
            drawText(window, font, "S: slow motion for 2 seconds, 3 uses per game.", 24, 100.f, 335.f);
            drawText(window, font, "Missing a hoop or hitting the floor ends the game.", 24, 100.f, 390.f);
            drawText(window, font, "R: restart. Escape: return to the menu.", 24, 100.f, 445.f);

            drawButton(window, font, "Back", 330.f, 560.f, 200.f, 60.f);
        }
        else if (screen == 1){

            RectangleShape floor({ 900.f, 40.f });
            floor.setPosition({ 0.f, floor_y });
            floor.setFillColor(Color(40, 65, 80));
            window.draw(floor);

            hoop(window, game.hoopX, game.hoopY);
            ball(window, ball_x, game.ballY);

            drawText(window, font, "Score: " + to_string(game.score), 28, 20.f, 20.f);
            drawText(window, font, "Best: " + to_string(bestScore), 28, 720.f, 20.f);
            drawText(window, font, "Slow (S): " + to_string(game.slowLeft), 24, 20.f, 60.f);

            if (game.slowTime > 0.f) {
                drawText(window, font, "SLOW MOTION", 30, 350.f, 60.f, Color(100, 200, 255));
            }

            if (!game.started) {
                drawText(window, font, "Press SPACE or click to start", 27, 280.f, 140.f);
            }

            if (game.msgTime > 0.f) {
                drawText(window, font, "+1", 45, 425.f, 100.f, Color(255, 170, 70));
            }

            if (game.over) {
                drawText(window, font, "GAME OVER", 46, 320.f, 215.f, Color(255, 150, 80));
                drawText(window, font, "Score: " + to_string(game.score) + "  Best: " + to_string(bestScore), 29, 350.f, 300.f);
                drawText(window, font, "Press R to restart, ESC for menu", 24, 290.f, 370.f);
            }
        }
        window.display();
    }
    return 0;
}