#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <SFML/Graphics.hpp>
#include <cassert>
#include <vector>

#include "SIR.hpp"
#include "environment.hpp"

namespace Contagion {

class Graph {
  sf::RenderWindow& window;
  sf::Font font;

 public:
  Graph(sf::RenderWindow& w)
      : window{w} {  // qui è possibile scaricare i font e le textures
    if (!font.loadFromFile("font.ttf")) {
      if (!font.loadFromFile(
              "/usr/share/fonts/truetype/msttcorefonts/arialbi.ttf")) {
        throw std::runtime_error{"cannot load font"};
      }
    }
  }

  void draw(std::vector<Contagion::State> const& states) {
    // quello che dobbiamo fare è creare tre convexshape in cui la posizione dei
    // punti è determinata in x dal numero di giorni trascorsi e in y dal valore
    // di ciascuna componente di state
    // quindi in entrambe le posioni dobbiamo trovare quella percentuale sulla
    // durata della pandemia e sul numero totale di popolazione esaminata

    sf::ConvexShape graph_S(states.size());
    // graph_S.setPointCount(states.size());

    // adesso dobbiamo segnare la posizone di ciascun punto, quindi utilizziamo
    // un ciclo che scorra tutti gli states in modo da accere al valore di S in
    // ciascun state

    const int duration = states.size();
    const int population = states[0].S + states[0].I + states[0].R;

    for (int i = 0; i != duration; ++i) {
      float const x = (i / static_cast<float>(duration)) * (window.getSize().x);
      float const y_S = (1 - states[i].S / static_cast<float>(population)) *
                        window.getSize().y;

      assert(i < (int)graph_S.getPointCount());
      graph_S.setPoint(i, sf::Vector2f{x, y_S});
    }

    // selezionamo i colori per la linea esterna e lo spessore
    graph_S.setOutlineColor(sf::Color::Blue);
    graph_S.setOutlineThickness(2.f);

    // infine disegniamo la convex sulla finestra
    window.draw(graph_S);
  }

  void draw_dots(std::vector<Contagion::State> const& states) {
    const int duration = states.size();
    const int population = states[0].S + states[0].I + states[0].R;
    const int margin = 20.f;
    
    sf::RectangleShape x_axes(sf::Vector2f(window.getSize().x ,2.f));
    x_axes.setPosition(sf::Vector2f(10, window.getSize().y-margin));
    x_axes.setFillColor(sf::Color::Black);
    
    sf::RectangleShape y_axes(sf::Vector2f(2.f , window.getSize().y));
    y_axes.setPosition(sf::Vector2f(margin , 0));
    y_axes.setFillColor(sf::Color::Black);
    
     window.draw(x_axes);
    window.draw(y_axes);
    
    sf::RectangleShape rect(sf::Vector2f(2.f, 2.f));

    for (int i = 0; i != duration; ++i) {
      float x = margin + (i / static_cast<float>(duration)) * (window.getSize().x - margin);
      rect.setFillColor(sf::Color::Black);
      float y_S = (1 - states[i].S / static_cast<float>(population)) *
                  (window.getSize().y - margin );
      rect.setPosition(sf::Vector2f(x, y_S));
      window.draw(rect);
      rect.setFillColor(sf::Color::Red);
      float y_I = (1 - states[i].I / static_cast<float>(population)) *
                  (window.getSize().y- margin );
      rect.setPosition(sf::Vector2f(x, y_I));
      window.draw(rect);
      rect.setFillColor(sf::Color::Green);
      float y_R = (1 - states[i].R / static_cast<float>(population)) *
                  (window.getSize().y - margin );
      rect.setPosition(sf::Vector2f(x, y_R));
      window.draw(rect);
    }
  }

  // funzione che disegna la legenda
  void write_leg(std::string const& legend,std::string const& nS,std::string const& nI,std::string const& nR) {
    float const x = window.getSize().x * 0.8;
    float const y = window.getSize().y * 0.3;
    
    sf::Text text;
    text.setFont(font);
    text.setString(legend);
    text.setPosition(sf::Vector2f(x, y));
    text.setFillColor(sf::Color::Black);
    text.setCharacterSize(15);

    sf::Text nStext;
    nStext.setFont(font);
    nStext.setString(nS);
    nStext.setPosition(sf::Vector2f(x, y+16));
    nStext.setFillColor(sf::Color::Blue);
    nStext.setCharacterSize(15);

    sf::Text nItext;
    
    nItext.setFont(font);
    nItext.setString(nI);
    nItext.setPosition(sf::Vector2f(x, y+32));
    nItext.setFillColor(sf::Color::Red);
    nItext.setCharacterSize(15);

    sf::Text nRtext;
    nRtext.setFont(font);
    nRtext.setString(nR);
    nRtext.setPosition(sf::Vector2f(x, y+48));
    nRtext.setFillColor(sf::Color::Green);
    nRtext.setCharacterSize(15);

    sf::RectangleShape shape(sf::Vector2f(100.f,100.f));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(3.f);
    shape.setPosition(sf::Vector2f(x-10,y-10));
    
    window.draw(text);
    window.draw(nStext);
    window.draw(nItext);
    window.draw(nRtext);
    window.draw(shape);
  }
  
  void draw(Environment const& env) {
    float const rect_dim = window.getSize().x / env.side();
    sf::RectangleShape rect(sf::Vector2f(rect_dim, rect_dim));
    rect.setOutlineThickness(1.f);
    rect.setOutlineColor(sf::Color::White);
    for (int i = 0; i != env.side(); ++i) {
      for (int j = 0; j != env.side(); ++j) {
        // setting the right fill colour
        Person P0 = env.condition(i, j);
        switch (P0) {
          case Person::Suceptible:
            rect.setFillColor(sf::Color::Green);
            break;

          case Person::Infectious:
            rect.setFillColor(sf::Color::Red);
            break;

          case Person::Removed:
            rect.setFillColor(sf::Color::Black);
            break;

          case Person::Void:
            rect.setFillColor(sf::Color::White);
            break;

          default:
            break;
        }
        // set correct position
        float const x = i * rect_dim;
        float const y = j * rect_dim;
        rect.setPosition(sf::Vector2f(x, y));
        window.draw(rect);
      }
    }
  }
};

}  // namespace Contagion

#endif
