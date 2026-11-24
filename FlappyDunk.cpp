#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <random>

using namespace std;
using namespace sf;

const int window_width = 900;
const int window_height = 700;
const float ball_size = 30.f;
const float hoop_width = 120.f;
const float hoop_height = 80.f;

const float ball_x = 250.f;
const float gravity = 1200.f;
const float jump_speed = -450.f;
const float start_speed = 130.f;
const float max_speed = 300.f;
const float floor_y = 665.f;

struct Game {

    float ballY = 250.f;
    float velocityY = 0.f;
    float hoopX = 650.f;
    float hoopY = 420.f;
    bool hoopScored = false;
    float hoopSpeed = start_speed;
    int score = 0;
    float msgTime = 0.f;
    bool started = false;
    bool over = false;
    float slowTime = 0.f;
    int slowLeft = 3;
    bool paused = false;
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
    button.setFillColor(Color(83, 34, 22));

    window.draw(button);
    drawText(window, font, label, 30, x + 70, y + 10, Color(238, 223, 199));
}

void stepRow(RenderWindow& window, Font& font,
    string number, string text, float y)
{
    CircleShape circle(20.f);
    circle.setPosition({ 90.f, y });
    circle.setFillColor(Color(255, 170, 70));
    window.draw(circle);

    drawText(window, font, number, 24, 103.f, y + 4.f, Color(19, 27, 43));
    drawText(window, font, text, 24, 150.f, y + 4.f);
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

void ball(RenderWindow& window, Sprite& sprite, float x, float y) {

    sprite.setPosition({ x, y });
    window.draw(sprite);
}

void hoop(RenderWindow& window, Sprite& sprite, float x, float y) {

    sprite.setPosition({ x, y });
    window.draw(sprite);
}

void resetGame(Game& game) {

    game.ballY = 250.f;
    game.velocityY = 0.f;

    game.hoopX = 650.f;
    game.hoopY = 420.f;
    game.hoopScored = false;

    game.score = 0;
    game.msgTime = 0.f;

    game.started = false;
    game.over = false;

    game.hoopSpeed = start_speed;

    game.slowTime = 0.f;
    game.slowLeft = 3;

    game.paused = false;
}

void flap(Game& game) {

    if (game.over || game.paused) {
        return;
    }

    game.started = true;
    game.velocityY = jump_speed;
}

void endGame(Game& game, int& bestScore) {

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

void updateGame(Game& game, float dt, int& bestScore) {

    if (!game.started || game.over || game.paused) {
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
    game.hoopX -= game.hoopSpeed * moveDt;

    // ball can not move out from top
    if (game.ballY < ball_size) {
        game.ballY = ball_size;
        game.velocityY = 0.f;
    }

    if (!game.hoopScored && game.velocityY > 0.f &&
        oldBallY < game.hoopY && game.ballY >= game.hoopY)
    {
        if (ball_x >= game.hoopX && ball_x <= game.hoopX + hoop_width)
        {
            game.hoopScored = true;
            game.score++;
            game.msgTime = 1.f;

            if (game.score % 5 == 0 && game.hoopSpeed < max_speed) {
                game.hoopSpeed += 15.f;
            }
        }
    }

    // hoop missed
    if (!game.hoopScored && game.hoopX + hoop_width < ball_x - ball_size) {
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

    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("background.jpg")) {
        return 1;
    }

    // Ball Creation
    Texture ballTexture;
    if (!ballTexture.loadFromFile("ball.png")) {
        return 1;
    }

    Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale({
        (float)window_width / backgroundTexture.getSize().x,
        (float)window_height / backgroundTexture.getSize().y
        });

    Sprite ballSprite(ballTexture);
    ballSprite.setOrigin({ ballTexture.getSize().x / 2.f, ballTexture.getSize().y / 2.f });
    ballSprite.setScale({
        ball_size * 2 / ballTexture.getSize().x,
        ball_size * 2 / ballTexture.getSize().y
        });

    // Hoop Creation
    Texture hoopTexture;
    if (!hoopTexture.loadFromFile("hoop.png")) {
        return 1;
    }

    Sprite hoopSprite(hoopTexture);
    hoopSprite.setScale({
        hoop_width / hoopTexture.getSize().x,
        hoop_height / hoopTexture.getSize().y
        });

    SoundBuffer scoreBuffer;
    if (!scoreBuffer.loadFromFile("score.mp3")) {
        return 1;
    }

    SoundBuffer overBuffer;
    if (!overBuffer.loadFromFile("gameover.mp3")) {
        return 1;
    }

    Sound scoreSound(scoreBuffer);
    Sound overSound(overBuffer);

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
                    else if (key->code == Keyboard::Key::P) {
                        if (game.started && !game.over) {
                            game.paused = !game.paused;
                        }
                    }
                    else if (key->code == Keyboard::Key::S) {
                        if (game.started && !game.over && !game.paused && game.slowLeft > 0 && game.slowTime <= 0.f) {
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

                    if (screen == 0) {
                        // main menu
                        if (isInside(mouseX, mouseY, 330, 210, 200, 60)) {

                            screen = 1;
                            resetGame(game);
                        }
                        else if (isInside(mouseX, mouseY, 330, 280, 200, 60))
                        {
                            screen = 2;
                        }
                        else if (isInside(mouseX, mouseY, 330, 350, 200, 60))
                        {
                            window.close();
                        }
                    }
                    else if (screen == 1) {
                        flap(game);
                    }
                    else if (screen == 2) {       // instructions

                        if (isInside(mouseX, mouseY, 330, 560, 200, 60))
                        {
                            screen = 0;
                        }
                    }
                }
            }

        }

        if (screen == 1) {

            int oldScore = game.score;
            bool wasOver = game.over;

            updateGame(game, dt, bestScore);

            if (game.score > oldScore) {
                scoreSound.play();
            }

            if (game.over && !wasOver) {
                overSound.play();
            }
        }

        window.clear(Color(19, 27, 43));
        window.draw(backgroundSprite);

        if (screen == 0) {

            RectangleShape panel({ 500.f, 480.f });
            panel.setPosition({ 200.f, 30.f });
            panel.setFillColor(Color(19, 27, 43, 220));
            panel.setOutlineThickness(3.f);
            panel.setOutlineColor(Color(255, 170, 70));
            window.draw(panel);

            drawText(window, font, "Flappy Dunk", 50, 290.f, 60.f, Color(255, 160, 46));
            drawText(window, font, "Bounce. Aim. Score.", 20, 340.f, 130.f);

            drawButton(window, font, "Play", 330.f, 210.f, 200.f, 60.f);
            drawButton(window, font, "Guide", 330.f, 280.f, 200.f, 60.f);
            drawButton(window, font, "Quit", 330.f, 350.f, 200.f, 60.f);

            drawText(window, font, "Best Score: " + to_string(bestScore), 20, 370.f, 450.f);
        }
        else if (screen == 2) {

            RectangleShape panel({ 780.f, 610.f });
            panel.setPosition({ 60.f, 30.f });
            panel.setFillColor(Color(19, 27, 43, 220));
            panel.setOutlineThickness(3.f);
            panel.setOutlineColor(Color(255, 170, 70));
            window.draw(panel);

            drawText(window, font, "HOW TO PLAY", 45, 280.f, 45.f, Color(255, 170, 70));

            RectangleShape line({ 300.f, 4.f });
            line.setPosition({ 280.f, 110.f });
            line.setFillColor(Color(255, 170, 70));
            window.draw(line);

            stepRow(window, font, "1", "Press SPACE or click to bounce the ball up.", 160.f);
            stepRow(window, font, "2", "Drop the ball through a hoop to get 1 point.", 230.f);
            stepRow(window, font, "3", "Press S for slow motion (2 seconds, 3 uses).", 300.f);
            stepRow(window, font, "4", "Missing a hoop or touching the floor ends the game.", 370.f);
            stepRow(window, font, "5", "Press P to pause, Press R to restart and ESC for menu.", 440.f);

            drawButton(window, font, "Back", 330.f, 560.f, 200.f, 60.f);
        }
        else if (screen == 1) {

            RectangleShape floor({ 900.f, 35.f });
            floor.setPosition({ 0.f, floor_y });
            floor.setFillColor(Color(197, 193, 193));
            window.draw(floor);

            hoop(window, hoopSprite, game.hoopX, game.hoopY);
            ball(window, ballSprite, ball_x, game.ballY);

            drawText(window, font, "Score: " + to_string(game.score), 28, 20.f, 20.f);
            drawText(window, font, "Best: " + to_string(bestScore), 28, 720.f, 20.f);
            drawText(window, font, "Slow (S): " + to_string(game.slowLeft), 28, 20.f, 60.f);
            drawText(window, font, "Pause (P)", 28, 720.f, 60.f);

            if (game.slowTime > 0.f) {
                drawText(window, font, "SLOW MOTION", 30, 350.f, 60.f, Color(100, 200, 255));
            }

            if (!game.started) {
                drawText(window, font, "Press SPACE or click to start", 27, 280.f, 140.f);
            }

            if (game.msgTime > 0.f) {
                drawText(window, font, "+1", 45, 425.f, 100.f, Color(255, 170, 70));
            }

            if (game.paused) {

                RectangleShape panel({ 460.f, 200.f });
                panel.setPosition({ 220.f, 230.f });
                panel.setFillColor(Color(19, 27, 43, 220));
                panel.setOutlineThickness(3.f);
                panel.setOutlineColor(Color(255, 170, 70));
                window.draw(panel);

                drawText(window, font, "PAUSED", 46, 355.f, 255.f, Color(255, 150, 80));
                drawText(window, font, "Press P to continue", 24, 345.f, 345.f);
            }

            if (game.over) {

                RectangleShape panel({ 460.f, 260.f });
                panel.setPosition({ 220.f, 180.f });
                panel.setFillColor(Color(19, 27, 43, 220));
                panel.setOutlineThickness(3.f);
                panel.setOutlineColor(Color(255, 170, 70));
                window.draw(panel);

                drawText(window, font, "GAME OVER", 46, 320.f, 215.f, Color(255, 150, 80));
                drawText(window, font, "Score: " + to_string(game.score) + "  Best: " + to_string(bestScore), 29, 340.f, 300.f);
                drawText(window, font, "Press R to restart, ESC for menu", 24, 290.f, 370.f);
            }
        }
        window.display();
    }
    return 0;
}