#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <deque>
#include <ctime>

using namespace std::literals::chrono_literals; //pour pouvoir ecrire Xs (x secondes)

float snakeSpeed = 1.0f;

const int CELL_SIZE = 60;
const int CELL_COUNT = 12;

sf::Sprite headSprite;
sf::Texture headTexture;

sf::Sprite tailSprite;
sf::Texture tailTexture;

sf::Sprite tailSprite_2;
sf::Texture tailTexture_2;

sf::Sprite fruitSprite;
sf::Texture fruitTexture;

sf::Sprite boomSprite;
sf::Texture boomTexture;

void loadSprites() {
    
    //load head
    if (!headTexture.loadFromFile("assets/sprites/RobotHead.png"))
    {
        std::cout << "couldn't load head texture" << std::endl;
    }
    headSprite.setTexture(headTexture);   
    headSprite.setScale({ static_cast<float>(CELL_SIZE / headTexture.getSize().x), static_cast<float>(CELL_SIZE / headTexture.getSize().y) });

    //load tail
    if (!tailTexture.loadFromFile("assets/sprites/RobotTail4_2.png"))
    {
        std::cout << "couldn't load tail texture" << std::endl;
    }
    tailSprite.setTexture(tailTexture);
    tailSprite.setScale({ static_cast<float>(CELL_SIZE / tailTexture.getSize().x), static_cast<float>(CELL_SIZE / tailTexture.getSize().y) });

    //load end
    if (!tailTexture_2.loadFromFile("assets/sprites/RobotTail5.png"))
    {
        std::cout << "couldn't load tail_2 texture" << std::endl;
    }
    tailSprite_2.setTexture(tailTexture_2);
    tailSprite_2.setScale({ static_cast<float>(CELL_SIZE / tailTexture_2.getSize().x), static_cast<float>(CELL_SIZE / tailTexture_2.getSize().y) });

    //load fruit
    if (!fruitTexture.loadFromFile("assets/sprites/Fruit.png"))
    {
        std::cout << "couldn't load fruit texture" << std::endl;
    }
    fruitSprite.setTexture(fruitTexture);
    fruitSprite.setScale({ static_cast<float>(CELL_SIZE / fruitTexture.getSize().x), static_cast<float>(CELL_SIZE / fruitTexture.getSize().y) });

    //load explosion
    if (!boomTexture.loadFromFile("assets/sprites/Explosion.png"))
    {
        std::cout << "couldn't load explosion texture" << std::endl;
    }
    boomSprite.setTexture(boomTexture);
    boomSprite.setScale({ static_cast<float>(CELL_SIZE / boomTexture.getSize().x), static_cast<float>(CELL_SIZE / boomTexture.getSize().y) });
}

sf::Vector2f multiplyVectors(sf::Vector2f first, sf::Vector2f second)
{
    return { first.x * second.x , first.y * second.y };
}

class Snake {
public:
    std::deque<sf::Vector2f> body = { {2, 0}, {1, 0}, {0, 0} };
    std::deque<int> rotation = { 90, 90, 90 };
    bool gameOver = false;

    void draw(sf::RenderWindow& window) 
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            if (i == 0 && !gameOver) {
                headSprite.setPosition(multiplyVectors(body[i], { CELL_SIZE, CELL_SIZE }));
                headSprite.setOrigin((sf::Vector2f)headTexture.getSize() / 2.f);
                headSprite.setRotation(rotation[i]);
                window.draw(headSprite);
            }
           else if (i % 2 == 0) {
                tailSprite_2.setPosition(multiplyVectors(body[i], { CELL_SIZE, CELL_SIZE }));
                tailSprite_2.setOrigin((sf::Vector2f)tailTexture_2.getSize() / 2.f);
                tailSprite_2.setRotation(rotation[i]);
                window.draw(tailSprite_2);
            }
            else {
                tailSprite.setPosition(multiplyVectors(body[i], { CELL_SIZE, CELL_SIZE }));
                tailSprite.setOrigin((sf::Vector2f)tailTexture.getSize() / 2.f);
                tailSprite.setRotation(rotation[i]);
                window.draw(tailSprite);
            }
        }
    }

    void updatePosition(sf::Vector2f direction, int orientation) {
        body.pop_back();
        body.push_front(direction);

        rotation.pop_back();
        rotation.push_front(orientation);
    }

    void grow() 
    {
        body.push_back(body[body.size() - 1]);
    }
};

class Fruit {
public:
    sf::Vector2f position;

    void draw(sf::RenderWindow& window)
    {
        fruitSprite.setPosition(multiplyVectors(position, { CELL_SIZE, CELL_SIZE }));
        fruitSprite.setOrigin((sf::Vector2f)fruitTexture.getSize() / 2.f);
        window.draw(fruitSprite);
    }
};

class Game {
public:
    Snake snake;
    Fruit fruit;
    bool gameOver;

    Game(): gameOver(false) { fruit.position = { 5, 5 }; }

    void draw(sf::RenderWindow& window) {
        snake.draw(window);
        fruit.draw(window);
        window.display();
    }

    void update(sf::Vector2f direction, int orientation) {
        snake.updatePosition({ snake.body[0].x + (1 * direction.x)
                    , snake.body[0].y + (1 * direction.y) }, orientation);

        if (checkCollision(fruit)) {
            snake.grow();
            spawnFruit();
            std::cout << "Collided" << std::endl;
            //snakeSpeed += .05f;
        }
        else if (checkCollision(snake)) {
            std::cout << "game over!" << std::endl;
            gameOver = true;
            snake.gameOver = true;
        }
    }

    void death(sf::RenderWindow& window) {
        
        int bodySize = snake.body.size();
        for (unsigned int i = 0; i < bodySize; i++)
        {
          window.clear();
          std::cout << "BOOM!" << std::endl;
          
          boomSprite.setPosition(multiplyVectors(snake.body[i], { CELL_SIZE, CELL_SIZE }));
          boomSprite.setOrigin((sf::Vector2f)boomTexture.getSize() / 2.f);
          window.draw(boomSprite);

          snake.body[i] = { -100*CELL_SIZE,-100 * CELL_SIZE }; //put out of bound
          //snake.body.pop_front();
          snake.draw(window);

          window.display();
          std::this_thread::sleep_for(.75s / bodySize);
        }
    }

    void spawnFruit() {
        int x = rand() % CELL_COUNT;
        int y = rand() % CELL_COUNT;

        //check if it is not occupied by snake
        for (unsigned int i = 0; i < snake.body.size(); i++) {
            if (snake.body[i].x == x && snake.body[i].y == y) {
                std::cout << "respawn" << std::endl;
                spawnFruit();
                return;
            }
        }

        fruit.position.x = x; fruit.position.y = y;
    }

    bool checkCollision(Fruit& fruit) //check fruit collision
    {
        return snake.body[0] == fruit.position;
    }

    bool checkCollision(Snake& snake) //check tail collision
    {
        if (snake.body.size() <= 0) return false;
        for (unsigned int i = 1; i < snake.body.size(); i++) {
            //std::cout << snake.body[0].x << " " << snake.body[0].y << " || " << snake.body[i].y << " " << snake.body[i].y << std::endl;
            if (snake.body[0] == snake.body[i]) {
                return true;
            }
        }
        return false;
    }

    void drawGrid(sf::RenderWindow& window) {
        int count = 0;
        for (int x = 0; x < CELL_COUNT; x++)
        {
            for (int y = 0; y < CELL_COUNT; y++)
            {
                sf::RectangleShape tile({ CELL_SIZE, CELL_SIZE });
                tile.setPosition({ static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE) });
                tile.setOrigin(sf::Vector2f{ CELL_SIZE, CELL_SIZE } / 2.f);

                if (count % 2 == 0) {
                    tile.setFillColor(sf::Color(0x525065));
                }
                else {
                    tile.setFillColor(sf::Color(0x424052));
                }
                window.draw(tile);
                count++;
            }
            count++;
        }
    }

};

int main() {
    srand(time(NULL)); //set seed

    // Création de la fenêtre
    sf::RenderWindow window(sf::VideoMode(CELL_SIZE * CELL_COUNT, CELL_SIZE * CELL_COUNT), "Fenêtre SFML");

    sf::Vector2f direction = { 1, 0 };
    int orientation = 90;
    sf::Vector2f negativeDirection = { -1, -1 };

    Game game;
    loadSprites();

    // Boucle principale
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // Fermer la fenêtre
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case(sf::Keyboard::Up):
                    if (multiplyVectors(direction, negativeDirection) != sf::Vector2f{ 0, -1 }) { direction = { 0, -1 }; orientation = 0;}
                    break;
                case(sf::Keyboard::Down):
                    if (multiplyVectors(direction, negativeDirection) != sf::Vector2f{ 0, 1 }) { direction = { 0, 1 }; orientation = 180;}
                    break;
                case(sf::Keyboard::Left):
                    if (multiplyVectors(direction, negativeDirection) != sf::Vector2f{ -1, 0 }) { direction = { -1, 0 }; orientation = -90;}
                    break;
                case(sf::Keyboard::Right):
                    if (multiplyVectors(direction, negativeDirection) != sf::Vector2f{ 1, 0 }) { direction = { 1, 0 }; orientation = 90;}
                    break;
                }
            }
        }
       
        game.drawGrid(window);

        game.update(direction, orientation);
        game.draw(window);

        if (game.gameOver) { game.death(window);  window.clear(); window.display(); std::this_thread::sleep_for(1s); break; }
        std::this_thread::sleep_for(.25s / snakeSpeed);
        window.clear();
    }

    return 0;
}