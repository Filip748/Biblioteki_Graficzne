#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "SFML/Graphics.hpp"
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 750, 600 }), "IUiBG Lab 03", sf::Style::Titlebar | sf::Style::Close);

  
    sf::Clock clock;
    unsigned int last_frame_time = 0;

    //inicjalizacja
    std::ignore = clock.restart().asMilliseconds();
    while (window.isOpen())
    {
        //sf::Event event;
        window.clear(sf::Color::White);

        while (const auto event = window.pollEvent())
        {
            // zdarzenia
        }

        //tu wyrysować wszystko na ekran

        //tu wypisać na ekran wartość last_frame_time
        last_frame_time = (unsigned int)(clock.getElapsedTime().asMilliseconds());
        clock.restart();

        window.display();
    }
}