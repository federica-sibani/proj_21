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

  // draw continous line
  void draw_continuous(std::vector<Contagion::State> const& states) {
    int const duration = states.size();
    const int population = states[0].S + states[0].I + states[0].R;

    constexpr int margin = 20.f;

    sf::RectangleShape x_axes(sf::Vector2f(window.getSize().x - margin, 2.f));
    x_axes.setPosition(sf::Vector2f(10, window.getSize().y - margin));
    x_axes.setFillColor(sf::Color::Black);

    sf::RectangleShape y_axes(sf::Vector2f(2.f, window.getSize().y));
    y_axes.setPosition(sf::Vector2f(margin, 0));
    y_axes.setFillColor(sf::Color::Black);

    window.draw(x_axes);
    window.draw(y_axes);

    sf::VertexArray S_curve(sf::LineStrip, duration);
    sf::VertexArray I_curve(sf::LineStrip, duration);
    sf::VertexArray R_curve(sf::LineStrip, duration);

    for (int i = 0; i != duration; ++i) {
      float x = margin + ((i / static_cast<float>(duration)) *
                          (window.getSize().x - 8 * margin));
      float y_S = (1 - states[i].S / static_cast<float>(population)) *
                  (window.getSize().y - margin);
      S_curve[i].position = sf::Vector2f(x, y_S);
      S_curve[i].color = sf::Color::Blue;
      float y_I = (1 - states[i].I / static_cast<float>(population)) *
                  (window.getSize().y - margin);
      I_curve[i].position = sf::Vector2f(x, y_I);
      I_curve[i].color = sf::Color::Red;
      float y_R = (1 - states[i].R / static_cast<float>(population)) *
                  (window.getSize().y - margin);
      R_curve[i].position = sf::Vector2f(x, y_R);
      R_curve[i].color = sf::Color::Green;
    }
    window.draw(S_curve);
    window.draw(I_curve);
    window.draw(R_curve);
  }

  // draw discrete graph
  void draw_dots(std::vector<Contagion::State> const& states) {
    const int duration = states.size();
    const int population = states[0].S + states[0].I + states[0].R;

    constexpr int margin = 20.f;

    sf::RectangleShape x_axes(sf::Vector2f(window.getSize().x - margin, 2.f));
    x_axes.setPosition(sf::Vector2f(10, window.getSize().y - margin));
    x_axes.setFillColor(sf::Color::Black);

    sf::RectangleShape y_axes(sf::Vector2f(2.f, window.getSize().y));
    y_axes.setPosition(sf::Vector2f(margin, 0));
    y_axes.setFillColor(sf::Color::Black);

    window.draw(x_axes);
    window.draw(y_axes);

    sf::RectangleShape rect(sf::Vector2f(2.f, 2.f));

    for (int i = 0; i != duration; ++i) {
      float x = margin + ((i / static_cast<float>(duration)) *
                          (window.getSize().x - 8 * margin));
      rect.setFillColor(sf::Color::Black);
      float y_S = (1 - states[i].S / static_cast<float>(population)) *
                  (window.getSize().y - margin);
      rect.setPosition(sf::Vector2f(x, y_S));
      window.draw(rect);
      rect.setFillColor(sf::Color::Red);
      float y_I = (1 - states[i].I / static_cast<float>(population)) *
                  (window.getSize().y - margin);
      rect.setPosition(sf::Vector2f(x, y_I));
      window.draw(rect);
      rect.setFillColor(sf::Color::Green);
      float y_R = (1 - states[i].R / static_cast<float>(population)) *
                  (window.getSize().y - margin);
      rect.setPosition(sf::Vector2f(x, y_R));
      window.draw(rect);
    }
  }

  // write legend
  void write_leg(std::string const& leg_title, std::string const& nS,
                 std::string const& nI, std::string const& nR) {
    constexpr float x_leg = 0.8;
    constexpr float y_leg = 0.3;
    constexpr float line_space = 16.;
    float const x = window.getSize().x * x_leg;
    float const y = window.getSize().y * y_leg;

    sf::Text text;
    text.setFont(font);

    text.setString(leg_title);
    text.setPosition(sf::Vector2f(x, y));
    text.setFillColor(sf::Color::Black);
    text.setCharacterSize(15);
    window.draw(text);

    text.setString(nS);
    text.setPosition(sf::Vector2f(x, y + line_space));
    text.setFillColor(sf::Color::Blue);
    window.draw(text);

    text.setString(nI);
    text.setPosition(sf::Vector2f(x, y + 2 * line_space));
    text.setFillColor(sf::Color::Red);
    window.draw(text);

    text.setString(nR);
    text.setPosition(sf::Vector2f(x, y + 3 * line_space));
    text.setFillColor(sf::Color::Green);
    window.draw(text);

    sf::RectangleShape shape(sf::Vector2f(100.f, 100.f));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(3.f);
    shape.setPosition(sf::Vector2f(x - 10, y - 10));
    window.draw(shape);
  }

  // draw 2D environment
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

          case Person::Infected:
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
