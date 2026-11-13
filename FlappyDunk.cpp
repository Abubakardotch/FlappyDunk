#include <SFML/Graphics.hpp>

const unsigned int WINDOW_WIDTH = 900;
const unsigned int WINDOW_HEIGHT = 700;

int main(){

    sf::RenderWindow window(
        sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }),
        "Flappy Dunk",
        sf::Style::Titlebar | sf::Style::Close
    );

    window.setFramerateLimit(60);

    while (window.isOpen()){

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()){
                window.close();
            }
        }

        window.clear(sf::Color(19, 27, 43));

        window.display();
    }

    return 0;
}