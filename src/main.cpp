#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>


enum EstadoJuego {
    MENU_INICIO,
    JUGANDO,
    GAME_OVER
};

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    sf::RenderWindow window(sf::VideoMode(800, 600), "Super Shooter Espacial");
    window.setFramerateLimit(60);

    
    sf::Texture texNave, texRoca, texFondo;
    if (!texNave.loadFromFile("assets/nave.png")) return -1;
    if (!texRoca.loadFromFile("assets/roca.png")) return -1;
    bool hayFondo = texFondo.loadFromFile("assets/fondo.jpg"); 

    sf::Font fuente;
    if (!fuente.loadFromFile("assets/fuente.ttf")) {
        std::cout << "ERROR: No se encontro fuente.ttf" << std::endl;
    }

    sf::Music musica;
    if (musica.openFromFile("assets/musica.ogg")) {
        musica.setLoop(true);   
        musica.setVolume(50);   
       
    }

    sf::SoundBuffer bufferDisparo;
    sf::Sound sonidoDisparo;
    if (bufferDisparo.loadFromFile("assets/disparo.ogg")) {
        sonidoDisparo.setBuffer(bufferDisparo);
    }

    
    sf::Sprite spriteFondo(texFondo);
    sf::Vector2u tamañoImagen = texFondo.getSize();
    float escalaX = 800.0f / tamañoImagen.x;
    float escalaY = 600.0f / tamañoImagen.y;
    spriteFondo.setScale(escalaX, escalaY);
    
    sf::Sprite avion(texNave);
    avion.setScale(.5f, .5f);
    avion.setPosition(375, 500);

   
    sf::Text textoPuntaje;
    textoPuntaje.setFont(fuente);
    textoPuntaje.setCharacterSize(25);
    textoPuntaje.setFillColor(sf::Color::White);
    textoPuntaje.setPosition(10, 10);

    sf::Text textoVidas;
    textoVidas.setFont(fuente);
    textoVidas.setCharacterSize(25);
    textoVidas.setFillColor(sf::Color::Red);
    textoVidas.setPosition(10, 40);

    
    std::vector<sf::CircleShape> balas;
    std::vector<sf::Sprite> meteoritos;
    sf::Clock relojMeteoritos;

    int puntaje = 0;
    int vidas = 3;

    
    EstadoJuego estado = MENU_INICIO; 

    sf::Text titulo, btnIniciar, btnSalir;
    
   
    titulo.setFont(fuente);
    titulo.setString(" SPACE WAR");
    titulo.setCharacterSize(60);
    titulo.setFillColor(sf::Color::Yellow); 
    titulo.setPosition(130, 100);

    btnIniciar.setFont(fuente);
    btnIniciar.setString("INICIAR");
    btnIniciar.setCharacterSize(40);
    btnIniciar.setPosition(320, 300);

    
    btnSalir.setFont(fuente);
    btnSalir.setString("SALIR");
    btnSalir.setCharacterSize(40);
    btnSalir.setPosition(340, 400);

    
    while (window.isOpen()) {
        sf::Event event;
        
        
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            
            if (estado == MENU_INICIO) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                
                    if (btnIniciar.getGlobalBounds().contains(mousePosF)) {
                        estado = JUGANDO; 
                        musica.play();    
                    }
                    
                    if (btnSalir.getGlobalBounds().contains(mousePosF)) {
                        window.close();
                    }
                }
            }
            
            
            if (estado == JUGANDO) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                    sf::CircleShape nuevaBala(5);
                    nuevaBala.setFillColor(sf::Color::Red);
                    nuevaBala.setPosition(avion.getPosition().x + 30, avion.getPosition().y);
                    balas.push_back(nuevaBala);
                    sonidoDisparo.play();
                }
            }
        }

    

        if (estado == MENU_INICIO) {
            
            if (btnIniciar.getGlobalBounds().contains(mousePosF)) btnIniciar.setFillColor(sf::Color::Green);
            else btnIniciar.setFillColor(sf::Color::White);

            if (btnSalir.getGlobalBounds().contains(mousePosF)) btnSalir.setFillColor(sf::Color::Red);
            else btnSalir.setFillColor(sf::Color::White);
        }
        else if (estado == JUGANDO) {
            
            
            
            if (relojMeteoritos.getElapsedTime().asSeconds() > 0.5f) {
                sf::Sprite nuevoMeteorito(texRoca);
                nuevoMeteorito.setScale(.5f, .5f);
                float posX = static_cast<float>(std::rand() % 700);
                nuevoMeteorito.setPosition(posX, -100);
                meteoritos.push_back(nuevoMeteorito);
                relojMeteoritos.restart();
            }

            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && avion.getPosition().x > 0) 
                avion.move(-5.0f, 0.0f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && avion.getPosition().x < 700) 
                avion.move(5.0f, 0.0f);

            
            for (size_t i = 0; i < balas.size(); i++) balas[i].move(0, -10);
            for (size_t i = 0; i < meteoritos.size(); i++) meteoritos[i].move(0, 3);

            
            for (int i = balas.size() - 1; i >= 0; i--) {
                bool balaBorrada = false;
                for (int j = meteoritos.size() - 1; j >= 0; j--) {
                    if (balas[i].getGlobalBounds().intersects(meteoritos[j].getGlobalBounds())) {
                        puntaje += 10; 
                        meteoritos.erase(meteoritos.begin() + j);
                        balas.erase(balas.begin() + i);
                        balaBorrada = true;
                        break; 
                    }
                }
                if (!balaBorrada && balas[i].getPosition().y < 0) balas.erase(balas.begin() + i);
            }

            
            for (int i = meteoritos.size() - 1; i >= 0; i--) {
                if (avion.getGlobalBounds().intersects(meteoritos[i].getGlobalBounds())) {
                    vidas--; 
                    meteoritos.erase(meteoritos.begin() + i);
                    
                    if (vidas <= 0) {
                        estado = GAME_OVER; 
                        musica.stop(); 
                    }
                }
                else if (meteoritos[i].getPosition().y > 600) {
                    meteoritos.erase(meteoritos.begin() + i);
                }
            }

            textoPuntaje.setString("PUNTOS: " + std::to_string(puntaje));
            textoVidas.setString("VIDAS: " + std::to_string(vidas));
        
        } else if (estado == GAME_OVER) {
            
            textoPuntaje.setString("GAME OVER");
            textoPuntaje.setCharacterSize(50);
            textoPuntaje.setPosition(250, 250);
            
            textoVidas.setString("Puntaje Final: " + std::to_string(puntaje));
            textoVidas.setPosition(280, 320);
        }

        
        window.clear();
    
        
        if (hayFondo) window.draw(spriteFondo);

        if (estado == MENU_INICIO) {
            
            window.draw(titulo);
            window.draw(btnIniciar);
            window.draw(btnSalir);
        }
        else if (estado == JUGANDO) {
            
            if (estado != GAME_OVER) {
                window.draw(avion);
                for (auto& bala : balas) window.draw(bala);
                for (auto& meteoro : meteoritos) window.draw(meteoro);
            }
            window.draw(textoPuntaje);
            window.draw(textoVidas);
        }
        else if (estado == GAME_OVER) {
    
            window.draw(textoPuntaje);
            window.draw(textoVidas);
        }

        window.display();
    }
    return 0;
}