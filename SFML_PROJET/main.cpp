
#include <SFML/Graphics.hpp>

int main() {
    // Création de la fenêtre
    sf::RenderWindow window(sf::VideoMode(800, 600), "Fenêtre SFML");

    // Création d'un cercle
    sf::CircleShape shape(50); // Rayon de 50 pixels
    shape.setFillColor(sf::Color::Green);

    sf::CircleShape shape2(70); // Rayon de 50 pixels
    shape2.setFillColor(sf::Color::Yellow);

    // Boucle principale
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // Fermer la fenêtre
        }

        // Effacer la fenêtre
        window.clear();
        // Dessiner la forme
        window.draw(shape2);
        window.draw(shape);
        // Afficher le contenu
        window.display();
    }

    return 0;
}