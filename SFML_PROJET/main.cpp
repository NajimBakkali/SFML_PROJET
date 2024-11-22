#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <vector>
using namespace std::literals::chrono_literals; //pour pouvoir ecrire Xs (x secondes)

const int SNAKE_SPEED = 2;

const int SNAKE_SIZE = 20;


struct TailInstance {
    sf::Vector2f position;
    TailInstance* next;

    TailInstance(sf::Vector2f position): position(position), next(nullptr) {}
};

class Snake {
private:
    TailInstance* head;
    std::vector<TailInstance*> instances;
public:
    Snake(TailInstance* head): head(head) {}
    ~Snake() {
        for (auto instance : instances) {
            delete instance;
        }
        instances.clear();
    }

    sf::Vector2f getHeadPosition() const {
        return head->position;
    }

    TailInstance* getHead() const {
        return head;
    }

    TailInstance* addTailInstance(sf::Vector2f position) {
        TailInstance* newTail = new TailInstance(position);
        instances.push_back(newTail);
        
        int nextIndex = instances.size() - 1;
        std::cout << "size: " << instances.size() - 1 << std::endl;
        if (nextIndex <= 0) {
            newTail->next = head;
        }else {
            newTail->next = instances[nextIndex];
        }
        newTail->position = newTail->next->position;

        return newTail;
    }

    void drawTail(sf::RenderWindow& window) {
        sf::RectangleShape headRectangle({ SNAKE_SIZE, SNAKE_SIZE });
        headRectangle.setFillColor(sf::Color::Green);
        headRectangle.move(head->position);

        for (auto instance : instances) {
            instance->position = instance->next->position;
            sf::RectangleShape tail({ SNAKE_SIZE, SNAKE_SIZE });
            tail.setFillColor(sf::Color::Green);
            tail.move(instance->position);
            window.draw(tail);
            
        }
    }
};

int main() {
    // Création de la fenêtre
    sf::RenderWindow window(sf::VideoMode(800, 600), "Fenêtre SFML");

    sf::Vector2f direction = { 1, 0 };


    // Création du serpent
    Snake snake(new TailInstance({20, 20}));

    TailInstance* newTail = snake.addTailInstance(snake.getHeadPosition());
    TailInstance* newTail2 = snake.addTailInstance(newTail->position);

    // Boucle principale
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // Fermer la fenêtre
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case(sf::Keyboard::Up):
                    direction = { 0, -1 };
                    break;
                case(sf::Keyboard::Down):
                    direction = { 0, 1 };
                    break;
                case(sf::Keyboard::Left):
                    direction = { -1, 0 };
                    break;
                case(sf::Keyboard::Right):
                    direction = { 1, 0 };
                    break;
                }
            }
        }
        std::cout << snake.getHeadPosition().x << " " << snake.getHeadPosition().y << std::endl;
        snake.getHead()->position = { snake.getHeadPosition().x + direction.x * SNAKE_SIZE
                            , snake.getHeadPosition().y + direction.y * SNAKE_SIZE };

        snake.drawTail(window);

        window.display();
        std::this_thread::sleep_for(.25s / SNAKE_SPEED);
        window.clear();
    }

    return 0;
}