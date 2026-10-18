#include "imgui.h"
#include "imgui-SFML.h"

#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <cmath>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1800, 900 }), "My Paint :D");
    sf::Clock deltaClock;

    bool toolsOpt = false;
    bool aboutWin = false;
    float borderCol[3] = { 0.0f, 0.0f, 0.0f }; 
    float fillCol[3] = { 0.0f, 1.0f, 1.0f };
    float borderThick = 1.0f;
    bool gradientLine = false;
    bool save = false;
    bool openAs = false;
    bool exitbut = false;
    char filename[256] = "pic.png";

    window.setFramerateLimit(60);
    std::ignore = ImGui::SFML::Init(window);

    enum class Tool {
        Line,
        Rectangle,
        FullRectangle,
        Circle
    };

    Tool tool = Tool::Line;

    bool isDrawing = false;
    sf::Vector2f start;
    sf::Vector2f current;
    sf::RenderTexture paper;
    if(!paper.resize( {1800, 900} )) {
        return -1;
    }
    paper.clear(sf::Color::White);
    paper.display();

    //funkcja do rysowania wybranego ksztaltu
    auto drawCurrentShape = [&](sf::RenderTarget& target) {
        sf::Color bc(
            static_cast<std::uint8_t>(borderCol[0] * 255),
            static_cast<std::uint8_t>(borderCol[1] * 255),
            static_cast<std::uint8_t>(borderCol[2] * 255)
        );
        sf::Color fc(
            static_cast<std::uint8_t>(fillCol[0] * 255),
            static_cast<std::uint8_t>(fillCol[1] * 255),
            static_cast<std::uint8_t>(fillCol[2] * 255)
        );

        if (tool == Tool::Line) {
            sf::Color endColor = gradientLine ? fc : bc;
            sf::Vertex line[] = {
                sf::Vertex{start, bc},
                sf::Vertex{current, endColor}
            };
            target.draw(line, 2, sf::PrimitiveType::Lines);
        } 
        else if (tool == Tool::Rectangle || tool == Tool::FullRectangle) {
            sf::RectangleShape rect;
            // obliczanie lewego gornego rogu i rozmiaru
            rect.setPosition({std::min(start.x, current.x), std::min(start.y, current.y)});
            rect.setSize({std::abs(current.x - start.x), std::abs(current.y - start.y)});
            
            if (tool == Tool::FullRectangle) {
                rect.setFillColor(fc);
            } else {
                rect.setFillColor(sf::Color::Transparent);
            }
            rect.setOutlineColor(bc);
            rect.setOutlineThickness(borderThick);
            target.draw(rect);
        } 
        else if (tool == Tool::Circle) {
            // r to odległosc od punktu srodka do obecnej pozycji kursora
            float r = std::sqrt(std::pow(current.x - start.x, 2) + std::pow(current.y - start.y, 2));
            sf::CircleShape circle(r);
            circle.setPosition({start.x - r, start.y - r});
            
            circle.setFillColor(fc);
            circle.setOutlineColor(bc);
            circle.setOutlineThickness(borderThick);
            target.draw(circle);
        }
    };

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()) window.close();

            // gdy klikniemy myszka wyliczamy pozycje 
            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Left && !ImGui::GetIO().WantCaptureMouse) {
                    isDrawing = true;
                    start = window.mapPixelToCoords(mouse->position);
                    current = start;
                }
            }

            // ruch myszki
            if (const auto* moved = event->getIf<sf::Event::MouseMoved>()) {
                if (isDrawing) {
                    current = window.mapPixelToCoords(moved->position);
                }
            }

            // odklikniecie myszki
            if (const auto* released = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (released->button == sf::Mouse::Button::Left && isDrawing) {
                    isDrawing = false;
                    drawCurrentShape(paper);
                    paper.display();
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Menu
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New")) { 
                    paper.clear(sf::Color::White); 
                    paper.display(); // Wymuszenie aktualizacji po czyszczeniu
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Open")) { openAs = true; }
                if (ImGui::MenuItem("Save As...")) { save = true; }
                if (ImGui::MenuItem("Exit")) { window.close(); }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Show Tool Options", NULL, &toolsOpt)) {  }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About")) { aboutWin = true; }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Tools
        ImGui::SetNextWindowPos({1790.0f, 30.0f}, ImGuiCond_FirstUseEver, {1.0f, 0.0f});
        ImGui::Begin("Tools");
        if (ImGui::RadioButton("Line", tool == Tool::Line)) { tool = Tool::Line; }
        ImGui::SameLine();
        if (ImGui::RadioButton("Rectangle", tool == Tool::Rectangle)) { tool = Tool::Rectangle; }
        if (ImGui::RadioButton("FilledRectangle", tool == Tool::FullRectangle)) { tool = Tool::FullRectangle; }
        ImGui::SameLine();
        if (ImGui::RadioButton("Circle", tool == Tool::Circle)) { tool = Tool::Circle; }
        if (toolsOpt) {
            ImGui::Separator();
            ImGui::Text("Tool options");
            if (tool == Tool::Line) {
                ImGui::Checkbox("Gradient line", &gradientLine);
            } else {
                ImGui::SliderFloat("Border Thick", &borderThick, 1.0f, 20.0f);
            }
        }
        ImGui::Separator();
        ImGui::ColorEdit3("Border Color", borderCol);
        ImGui::ColorEdit3("Fill Color", fillCol);

        ImGui::End(); 



        // okno zapisu 
        if (save) {
            ImGui::Begin("Save as ...", &save);
            ImGui::InputText("File name .png", filename, sizeof(filename));
            if (ImGui::Button("Save")) {
                sf::Image img = paper.getTexture().copyToImage();
                img.saveToFile(filename);
                save = false;
            }
            ImGui::End();
        }

        // wczytanie pliku
        if (openAs) {
            ImGui::Begin("Open file", &openAs);
            ImGui::InputText("File name .png", filename, sizeof(filename));
            if (ImGui::Button("Open")) {
                sf::Texture loadedTexture;
                if (loadedTexture.loadFromFile(filename)) {
                    sf::Sprite loadedSprite(loadedTexture);
                    paper.clear(sf::Color::White);
                    paper.draw(loadedSprite);
                    paper.display();
                }
                openAs = false;
            }
            ImGui::End();
        }

        // sekcja about
        if (aboutWin) {
            ImGui::Begin("About", &aboutWin);
            ImGui::Text("Author: Filip Sobieraj\n Hi i am 3rd year computer science student :D");
            if (ImGui::Button("Ok")) {
                aboutWin = false;
            }
            ImGui::End();
        }



        window.clear(sf::Color(100, 100, 100));

        paper.display();

        // rysowanie bufora 
        sf::Sprite canvasSprite(paper.getTexture());
        window.draw(canvasSprite);

        // rysowanie ksztaltu
        if (isDrawing) {
            drawCurrentShape(window);
        }
        

        // rysowanie interfejsu
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}