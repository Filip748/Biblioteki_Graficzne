#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <string>
#include <optional>
#include <cstdint>

sf::Color HSLtoRGB(float H, float S, float L) {
    float C = (1.0f - std::abs(2.0f * L - 1.0f)) * S;
    float H_prime = H / 60.0f;
    float X = C * (1.0f - std::abs(fmod(H_prime, 2.0f) - 1.0f));
    float m = L - C / 2.0f;
    float R = 0, G = 0, B = 0;

    if (0 <= H_prime && H_prime < 1) { 
        R = C; G = X; B = 0; 
    }
    else if (1 <= H_prime && H_prime < 2) {
        R = X; G = C; B = 0; 
    }
    else if (2 <= H_prime && H_prime < 3) {
         R = 0; G = C; B = X;
    }
    else if (3 <= H_prime && H_prime < 4) {
         R = 0; G = X; B = C;
    }
    else if (4 <= H_prime && H_prime < 5) { 
        R = X; G = 0; B = C; 
    }
    else if (5 <= H_prime && H_prime < 6) { 
        R = C; G = 0; B = X; 
    }

    return sf::Color(static_cast<std::uint8_t>((R + m) * 255), static_cast<std::uint8_t>((G + m) * 255),  static_cast<std::uint8_t>((B + m) * 255));
}

sf::Color HSVtoRGB(float H, float S, float V) {
    float C = V * S;
    float H_prime = H / 60.0f;
    float X = C * (1.0f - std::abs(fmod(H_prime, 2.0f) - 1.0f));
    float m = V - C;
    float R = 0, G = 0, B = 0;

    if (0 <= H_prime && H_prime < 1) { 
        R = C; G = X; B = 0;
    }
    else if (1 <= H_prime && H_prime < 2) { 
        R = X; G = C; B = 0; 
    }
    else if (2 <= H_prime && H_prime < 3) { 
        R = 0; G = C; B = X; 
    }
    else if (3 <= H_prime && H_prime < 4) {
        R = 0; G = X; B = C; 
    }
    else if (4 <= H_prime && H_prime < 5) { 
        R = X; G = 0; B = C; 
    }
    else if (5 <= H_prime && H_prime < 6) { 
        R = C; G = 0; B = X; 
    }

    return sf::Color(static_cast<std::uint8_t>((R + m) * 255), static_cast<std::uint8_t>((G + m) * 255), static_cast<std::uint8_t>((B + m) * 255));
}

sf::Color CMYtoRGB(float C, float M, float Y) {
    return sf::Color(static_cast<std::uint8_t>((1.0f - C) * 255), static_cast<std::uint8_t>((1.0f - M) * 255),  static_cast<std::uint8_t>((1.0f - Y) * 255));
}

void updateWheels(sf::Image& img, float sliderValue, int radius) {

    for (unsigned int y = 0; y < img.getSize().y; ++y) {
        for (unsigned int x = 0; x < img.getSize().x; ++x) {
            img.setPixel({x, y}, sf::Color::White);
        }
    }
    
    int centers[4][2] = {
        {150, 150},
        {450, 150},
        {150, 450},
        {450, 450} 
    };

    for (int wheel = 0; wheel < 4; ++wheel) {
        int cx = centers[wheel][0];
        int cy = centers[wheel][1];

        for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
                float dist = std::sqrt(x * x + y * y);
                if (dist <= radius) {
                    float r = dist / radius; 
                    float alpha = std::atan2(-y, x) * 180.0f / M_PI; 
                    if (alpha < 0) alpha += 360.0f;

                    float alphaNorm = alpha / 360.0f;

                    sf::Color pixelColor;
                    if (wheel == 0) { 
                        pixelColor = HSLtoRGB(alpha, r, sliderValue);
                    } else if (wheel == 1) { 
                        pixelColor = HSVtoRGB(alpha, r, sliderValue);
                    } else if (wheel == 2) { 
                        pixelColor = CMYtoRGB(r, alphaNorm, sliderValue); 
                    } else if (wheel == 3) { 
                        pixelColor = sf::Color(static_cast<std::uint8_t>(r * 255), 
                                               static_cast<std::uint8_t>(alphaNorm * 255), 
                                               static_cast<std::uint8_t>(sliderValue * 255));
                    }
                    img.setPixel(sf::Vector2u(cx + x, cy + y), pixelColor);
                }
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 750, 600 }), "IUiBG Lab 03", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("/System/Library/Fonts/Helvetica.ttc")) {
        std::cerr << "Nie udalo sie zaladowac czcionki!" << std::endl;
    }
    sf::Text timeText(font);
    timeText.setCharacterSize(16);
    timeText.setFillColor(sf::Color::Black);
    timeText.setPosition({620, 300});

    sf::Clock clock;
    unsigned int last_frame_time = 0;

    int radius = 120;
    float sliderValue = 0.537f;
    bool needsUpdate = true;
    bool isDragging = false;

    sf::Image wheelsImage;
    wheelsImage.resize({600, 600}); 
    
    sf::Texture wheelsTexture;
    sf::Sprite wheelsSprite(wheelsTexture);

    sf::RectangleShape sliderBar(sf::Vector2f(20.f, 200.f));
    sliderBar.setPosition({650.f, 50.f});
    sliderBar.setFillColor(sf::Color(200, 200, 200));
    sliderBar.setOutlineColor(sf::Color::Black);
    sliderBar.setOutlineThickness(1.f);

    sf::RectangleShape sliderHandle(sf::Vector2f(30.f, 10.f));
    sliderHandle.setFillColor(sf::Color(100, 100, 100));

    std::ignore = clock.restart().asMilliseconds();

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    if (sliderBar.getGlobalBounds().contains(sf::Vector2f(mousePressed->position))) {
                        isDragging = true;
                    }
                }
            }
            
            if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseReleased->button == sf::Mouse::Button::Left) {
                    isDragging = false;
                }
            }
            
            if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                if (isDragging) {
                    float newY = std::max(50.0f, std::min(250.0f, static_cast<float>(mouseMoved->position.y)));
                    sliderValue = 1.0f - ((newY - 50.0f) / 200.0f);
                    needsUpdate = true;
                }
            }
        }

        if (needsUpdate) {
            updateWheels(wheelsImage, sliderValue, radius);
            if (wheelsTexture.loadFromImage(wheelsImage)) {
                wheelsSprite.setTexture(wheelsTexture, true);
            }
            needsUpdate = false;
        }

        sliderHandle.setPosition({645.f, 50.f + (1.0f - sliderValue) * 200.f - 5.f});

        window.clear(sf::Color::White);
        window.draw(wheelsSprite);
        window.draw(sliderBar);
        window.draw(sliderHandle);

        timeText.setString("Refresh time:\n" + std::to_string(last_frame_time) + " ms\nValue: " + std::to_string(sliderValue));
        window.draw(timeText);

        last_frame_time = static_cast<unsigned int>(clock.getElapsedTime().asMilliseconds());
        clock.restart();
        window.display();
    }
    return 0;
}