#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <thread>
#include <deque>
#include <ctime>

using namespace std::literals::chrono_literals; //pour pouvoir ecrire Xs (x secondes)

float snakeSpeed = 1.0f;
bool moveOnCooldown = false;

const int CELL_SIZE = 60;
const int CELL_COUNT = 12;

int score = 0;
int bestScore = 0;

sf::SoundBuffer loopBuffer;
sf::Sound musicLoop;

sf::SoundBuffer snakeMoveBuffer;
sf::Sound soundSnakeMove;

sf::SoundBuffer collectBuffer;
sf::Sound soundCollect;

sf::SoundBuffer gameOverBuffer;
sf::Sound musicGameOver;

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
    if (!fruitTexture.loadFromFile("assets/sprites/Bolt.png"))
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


void loadMusics()
{

    if (!loopBuffer.loadFromFile("assets/musics/GameLoop.wav")) {
        std::cout << "Couldn't load game loop music" << std::endl;
    }
    musicLoop.setBuffer(loopBuffer);
    musicLoop.setLoop(true);

    if (!gameOverBuffer.loadFromFile("assets/musics/GameOver.wav")) {
        std::cout << "Couldn't load game over music" << std::endl;
    }
    musicGameOver.setBuffer(gameOverBuffer);

    if (!snakeMoveBuffer.loadFromFile("assets/musics/SnakeMove.wav")) {
        std::cout << "Couldn't load snake sound" << std::endl;
    }
    soundSnakeMove.setBuffer(snakeMoveBuffer);

    if (!collectBuffer.loadFromFile("assets/musics/Collect.wav")) {
        std::cout << "Couldn't load collect sound" << std::endl;
    }
    soundCollect.setBuffer(collectBuffer);
}

sf::Vector2f multiplyVectors(sf::Vector2f first, sf::Vector2f second)
{
    return { first.x * second.x , first.y * second.y };
}

sf::Text* createText() {
    sf::Font* font = new sf::Font;
    sf::Text* text = new sf::Text;
    if (!font->loadFromFile("assets/fonts/Minecraft.ttf")) {
        std::cout << "Font not loaded\n";
    }

    text->setFont(*font);
    return text;
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
                headSprite.setPosition(multiplyVectors(body[i], { CELL_SIZE, CELL_SIZE }) + sf::Vector2f{ CELL_SIZE / 2, CELL_SIZE / 2 });
                headSprite.setOrigin((sf::Vector2f)headTexture.getSize() / 2.f);
                headSprite.setRotation(rotation[i]);
                window.draw(headSprite);
            }
            else if (i % 2 == 0) {
                tailSprite_2.setPosition(multiplyVectors(body[i], { CELL_SIZE, CELL_SIZE }) + sf::Vector2f{ CELL_SIZE / 2, CELL_SIZE / 2 });
                tailSprite_2.setOrigin((sf::Vector2f)tailTexture_2.getSize() / 2.f);
                tailSprite_2.setRotation(rotation[i]);
                window.draw(tailSprite_2);
            }
            else {
                tailSprite.setPosition(multiplyVectors(body[i], { CELL_SIZE, CELL_SIZE }) + sf::Vector2f{ CELL_SIZE / 2, CELL_SIZE / 2 });
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
        fruitSprite.setPosition(multiplyVectors(position, { CELL_SIZE, CELL_SIZE }) + sf::Vector2f{ CELL_SIZE / 2, CELL_SIZE / 2 });
        fruitSprite.setOrigin((sf::Vector2f)fruitTexture.getSize() / 2.f);
        window.draw(fruitSprite);
    }
};

class Game {
public:
    Snake snake;
    Fruit fruit;
    bool gameOver;
    bool onMenu;

    sf::Text* playButton;
    sf::Text* titleText;
    sf::Text* scoreText;
    sf::Text* bestScoreText;
    sf::Text* gameOverText;

    Game() : gameOver(false), onMenu(true), playButton(nullptr), titleText(nullptr), scoreText(nullptr), bestScoreText(nullptr), gameOverText(nullptr)
    {
        fruit.position = { 5, 5 };
    }

    void writeScore(sf::RenderWindow& window) {

        if (scoreText) {
            scoreText->setString("Score: " + std::to_string(score));
            scoreText->setPosition({ static_cast<float>((window.getSize().x - scoreText->getLocalBounds().width) / 4), 0 });

            scoreText->setFillColor(sf::Color::White);
        }
        else {
            sf::Text* text = createText();

            // text->setOrigin(text->getLocalBounds().width / 2, text->getLocalBounds().height / 2);

            text->setString("Score: " + std::to_string(score));
            text->setCharacterSize(24);
            text->setFillColor(sf::Color::White);
            text->setOutlineColor(sf::Color::Black);
            text->setOutlineThickness(2);
            text->setPosition({ static_cast<float>((window.getSize().x - text->getLocalBounds().width) / 4), 0 });

            scoreText = text;
        }

        if (bestScoreText) {
            bestScoreText->setString("Best Score: " + std::to_string(bestScore));
            bestScoreText->setPosition({ static_cast<float>((window.getSize().x - bestScoreText->getLocalBounds().width) / 4) * 3, 0 });
        }
        else {
            sf::Text* bestText = createText();

            //bestText->setOrigin(bestText->getLocalBounds().width / 2, bestText->getLocalBounds().height / 2);

            bestText->setString("Best Score: " + std::to_string(bestScore));
            bestText->setCharacterSize(24);
            bestText->setFillColor(sf::Color::White);
            bestText->setOutlineColor(sf::Color::Black);
            bestText->setOutlineThickness(2);
            bestText->setPosition({ static_cast<float>((window.getSize().x - bestText->getLocalBounds().width) / 4) * 3, 0 });

            bestScoreText = bestText;
        }

        window.draw(*scoreText);
        window.draw(*bestScoreText);
    }

    void menuScreen(sf::RenderWindow& window) {

        if (titleText) {
            titleText->setPosition({ static_cast<float>((window.getSize().x - titleText->getLocalBounds().width) / 2)
                    ,static_cast<float>((window.getSize().x - titleText->getLocalBounds().height) / 3) });
        }
        else {
            sf::Text* title = createText();
            title->setString("ROBO-SNAKE");
            title->setCharacterSize(64);
            title->setFillColor(sf::Color(125, 125, 125));

            title->setPosition({ static_cast<float>((window.getSize().x - title->getLocalBounds().width) / 2)
                                ,static_cast<float>((window.getSize().x - title->getLocalBounds().height) / 3) });

            titleText = title;
        }


        if (playButton)
        {
            playButton->setPosition({ static_cast<float>((window.getSize().x - playButton->getLocalBounds().width) / 2)
                    ,static_cast<float>((window.getSize().x - playButton->getLocalBounds().height) / 1.5) });
        }
        else
        {
            sf::Text* button = createText();
            button->setString("PLAY");
            button->setCharacterSize(64);
            button->setFillColor(sf::Color::Green);

            button->setPosition({ static_cast<float>((window.getSize().x - button->getLocalBounds().width) / 2)
                                ,static_cast<float>((window.getSize().x - button->getLocalBounds().height) / 1.5) });

            playButton = button;
        }

        window.draw(*playButton);
        window.draw(*titleText);
        window.display();
    }

    void draw(sf::RenderWindow& window) {
        snake.draw(window);
        fruit.draw(window);
        writeScore(window);

        window.display();
    }

    void updateScore() {
        score++;
    }

    void update(sf::Vector2f direction, int orientation) {

        sf::Vector2f newBodyPosition = { snake.body[0].x + (1 * direction.x), snake.body[0].y + (1 * direction.y) };
        snake.updatePosition(newBodyPosition, orientation);

        if ((snake.body[0].x < 0 || snake.body[0].x >= CELL_COUNT) || (snake.body[0].y < 0 || snake.body[0].y >= CELL_COUNT))
        {
            std::cout << "game over!" << std::endl;
            gameOver = true;
            snake.gameOver = true;
            snake.body.pop_front();
            return;
        }

        if (checkCollision(fruit)) {
            snake.grow();
            spawnFruit();
            updateScore();
            soundCollect.play();
            std::cout << "Collided" << std::endl;
            //snakeSpeed += .05f;
        }
        else if (checkCollision(snake)) {
            std::cout << "game over!" << std::endl;
            gameOver = true;
            snake.gameOver = true;
            snake.body.pop_front();
        }
    }

    void death(sf::RenderWindow& window) {

        int bodySize = snake.body.size();
        for (unsigned int i = 0; i < bodySize; i++)
        {
            window.clear();
            drawGrid(window);
            std::cout << "BOOM!" << std::endl;

            boomSprite.setPosition(multiplyVectors(snake.body[i], { CELL_SIZE, CELL_SIZE }) + sf::Vector2f{ CELL_SIZE / 2 , CELL_SIZE / 2 });
            boomSprite.setOrigin((sf::Vector2f)boomTexture.getSize() / 2.f);
            window.draw(boomSprite);

            snake.body[i] = { -999 * CELL_SIZE,-999 * CELL_SIZE }; //put out of bound

            //snake.body.pop_front();
            snake.draw(window);

            window.display();
            std::this_thread::sleep_for(.75s / bodySize);
        }
    }

    void gameOverScreen(sf::RenderWindow& window) {
        window.clear();


        if (gameOverText)
        {
            gameOverText->setString("GAME OVER");

            gameOverText->setPosition({ static_cast<float>((window.getSize().x - gameOverText->getLocalBounds().width) / 2)
                                       ,static_cast<float>((window.getSize().x - gameOverText->getLocalBounds().height) / 3) });
            //gameOverText->setPosition({ static_cast<float>((window.getSize().x - gameOverText->getLocalBounds().width) / 2),
            //static_cast<float>((window.getSize().y - gameOverText->getLocalBounds().height) / 2) });
        }
        else
        {
            sf::Text* text = createText();

            text->setOrigin(text->getLocalBounds().width / 2, text->getLocalBounds().height / 2);

            text->setOrigin(text->getLocalBounds().width / 2, text->getLocalBounds().height / 2);
            text->setString("GAME OVER");
            text->setCharacterSize(64);
            text->setFillColor(sf::Color::Red);

            text->setPosition({ static_cast<float>((window.getSize().x - text->getLocalBounds().width) / 2)
                           ,static_cast<float>((window.getSize().x - text->getLocalBounds().height) / 3) });

            // text->setPosition({ static_cast<float>((window.getSize().x + text->getLocalBounds().width) / 6),
            //     static_cast<float>((window.getSize().y - text->getLocalBounds().height) / 2) });

            gameOverText = text;

        }

        // scoreText->setPosition({ gameOverText->getLocalBounds().getPosition().x + window.getSize().x / 2.8f
          //                       ,gameOverText->getPosition().y + window.getSize().y / 4 });



        if (score > bestScore)
        {
            scoreText->setString("NEW BEST SCORE: " + std::to_string(score));
        }
        else
        {
            scoreText->setString("FINAL SCORE: " + std::to_string(score));
        }

        scoreText->setPosition({ static_cast<float>((window.getSize().x - scoreText->getLocalBounds().width) / 2)
            ,static_cast<float>((window.getSize().x - scoreText->getLocalBounds().height) / 1.5) });

        scoreText->setFillColor(sf::Color::Red);

        window.draw(*gameOverText);
        window.draw(*scoreText);
        //window.draw(*bestScoreText);
        window.display();
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
                tile.setPosition({ static_cast<float>(x * CELL_SIZE + CELL_SIZE / 2), static_cast<float>(y * CELL_SIZE + CELL_SIZE / 2) });
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

    // Cr?ation de la fen?tre
    sf::RenderWindow window(sf::VideoMode(CELL_SIZE * CELL_COUNT, CELL_SIZE * CELL_COUNT), "Fen?tre SFML");

    Game game;
    loadSprites();
    loadMusics();

    sf::Vector2f direction = { 1, 0 };
    int orientation = 90;
    sf::Vector2f negativeDirection = { -1, -1 };

    game.menuScreen(window);

    // Boucle principale
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // Fermer la fen?tre

            if (event.type == sf::Event::KeyPressed && !game.onMenu && !moveOnCooldown) {
                switch (event.key.code) {
                case(sf::Keyboard::Up):
                    if (multiplyVectors(direction, negativeDirection) != sf::Vector2f{ 0, -1 } && orientation != 0)
                    {
                        direction = { 0, -1 }; orientation = 0; soundSnakeMove.play(); moveOnCooldown = true;
                    }
                    break;
                case(sf::Keyboard::Down):
                    if (multiplyVectors(direction, negativeDirection) != sf::Vector2f{ 0, 1 } && orientation != 180)
                    {
                        direction = { 0, 1 }; orientation = 180; soundSnakeMove.play(); moveOnCooldown = true;
                    }
                    break;
                case(sf::Keyboard::Left):
                    if (multiplyVectors(direction, negativeDirection) != sf::Vector2f{ -1, 0 } && orientation != -90)
                    {
                        direction = { -1, 0 }; orientation = -90; soundSnakeMove.play(); moveOnCooldown = true;
                    }
                    break;
                case(sf::Keyboard::Right):
                    if (multiplyVectors(direction, negativeDirection) != sf::Vector2f{ 1, 0 } && orientation != 90)
                    {
                        direction = { 1, 0 }; orientation = 90; soundSnakeMove.play(); moveOnCooldown = true;
                    }
                    break;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.key.code == sf::Mouse::Left)
                {
                    if (game.playButton != nullptr)
                    {
                        if (game.playButton->getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) //check if mouse is on text
                        {
                            musicLoop.play();
                            game.onMenu = false;
                        }
                    }
                }
            }
        }

        window.clear();

        if (game.onMenu) {
            if (game.playButton->getGlobalBounds().contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))) {
                // Appliquer un effet (changer la couleur du texte par exemple)
                game.playButton->setFillColor(sf::Color::Green);
            }
            else {
                // R?initialiser la couleur du texte
                game.playButton->setFillColor(sf::Color::Yellow);
            }
            game.menuScreen(window);
        }
        else
        {
            game.drawGrid(window);
            game.update(direction, orientation);

            //game over check
            if (game.gameOver) {
                musicLoop.stop();
                musicGameOver.play();
                game.death(window);   window.clear(); window.display();
                game.gameOverScreen(window);
                std::this_thread::sleep_for(2s);

                //reset everything
                game.onMenu = true;
                game.gameOver = false;
                game.snake.gameOver = false;

                if (score > bestScore) {
                    bestScore = score;
                }
                score = 0;

                direction = { 1, 0 };
                orientation = 90;
                game.snake.body = { { 2, 0 }, { 1, 0 }, { 0, 0 } };
                game.snake.rotation = { 90, 90, 90 };

                game.fruit.position = { 5, 5 };
            }
            else {
                game.draw(window);
                std::this_thread::sleep_for(.25s / snakeSpeed);
                moveOnCooldown = false;
            }
        }
    }

    return 0;
}


