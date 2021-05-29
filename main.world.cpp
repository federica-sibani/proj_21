#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <random>

#include "environment.hpp"
#include "graph.hpp"

namespace Con = Contagion;

int main() {
  try {
    int constexpr display_limit = 150;
    std::cout << "Insert dimension of the environment: \n";
    int side;
    std::cin >> side;
    if (side < 1 || side > display_limit) {
      std::cout << "Resetting environmetal dimension to 150\n";
      side = display_limit;
    }
    Con::Environment env{side};

    // initialising the environment with 2% of void and arbitrary percentage of
    // infectious people
    double constexpr void_percent = 0.02;
    double constexpr def_inf_percent = 0.01;
    double inf_percent;
    std::cout << "Insert the percentage of infected people -between (0-1):\n";
    std::cin >> inf_percent;
    if (inf_percent <= 0 || inf_percent >= 1) {
      std::cout << "Resetting percentage of infected at 0.01\n";
      inf_percent = def_inf_percent;
    }
    int const n_void = void_percent * (side * side);
    int n_infected = inf_percent * (side * side);

    {
      std::default_random_engine gen{std::random_device{}()};
      std::uniform_int_distribution<> dist{0, side - 1};

      for (int i = 0; i != n_void; ++i) {
        int r = dist(gen);
        int c = dist(gen);
        for (; env.condition(r, c) == Con::Person::Void;
             r = dist(gen), c = dist(gen))
          ;
        env.condition(r, c) = Con::Person::Void;
      }

      for (int i = 0; i != n_infected; ++i) {
        int r = dist(gen);
        int c = dist(gen);
        for (; env.condition(r, c) == Con::Person::Infected ||
               env.condition(r, c) == Con::Person::Void;
             r = dist(gen), c = dist(gen))
          ;
        env.condition(r, c) = Con::Person::Infected;
      }
    }

    // display the evolution of env as contagion spreads out for a definite
    // number of days

    int constexpr rect_dim = 6;
    int const window_size = rect_dim * side;

    int duration = 0;
    double beta;
    double gamma;
    double mi;

    std::cout << "Insert the time span of the simulation:\n";
    std::cin >> duration;
    if (duration < 1 || duration > 120) {
      std::cout << "Resetting duration at 60 days";
      duration = 60;
    }
    std::cout << "Insert β (contagion parameter), γ (convalescence parameter) "
                 "and μ (mortality parameter):\n";
    std::cin >> beta >> gamma >> mi;
    if (beta <= 0 || beta > 1) {
      std::cerr << "invalid beta input";
    }
    if (gamma <= 0 || gamma > 1) {
      std::cerr << "invalid gamma input";
    }
    if (mi <= 0 || mi > 1) {
      std::cerr << "invalid mi input";
    }

    sf::RenderWindow window(sf::VideoMode(window_size, window_size), "pandemic",
                            sf::Style::Default);
    window.setFramerateLimit(60);
    Con::Graph draw(window);

    // game loop with the evolution of the environment

    std::default_random_engine gen{std::random_device{}()};

    using namespace std::chrono_literals;

    auto last = std::chrono::steady_clock::now();

    int i = 0;

    bool toggle = true;

    while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
          window.close();
        } else if (event.type == sf::Event::KeyPressed) {
          toggle = !toggle;
        }
      }

      window.clear(sf::Color::White);
      draw.draw(env);
      window.display();

      if (i < duration && toggle) {
        if (auto now = std::chrono::steady_clock::now(); now > last + 1s) {
          env = evolve(env, beta, gamma, mi, gen);
          last = now;
          i += 1;
        }
      }
    }

  } catch (std::exception const& e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unknown exception\n";
    return EXIT_FAILURE;
  }
}
