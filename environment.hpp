#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <cassert>
#include <cmath>
#include <limits>
#include <random>
#include <vector>

#include "SIR.hpp"

namespace Contagion {

enum class Person { Suceptible, Infectious, Removed, Void };

class Environment {
 private:
  using Row = std::vector<Person>;
  using Grid = std::vector<Row>;
  int m_side;
  Grid my_env;

 public:
  Environment(int side)
      : m_side(side), my_env(side, Row(side, Person::Suceptible)) {
    if (side < 0 || side > std::numeric_limits<int>::max()) {
      throw std::runtime_error("Invalid dimension of the environment");
    }
  };
  Environment() : Environment{0} {};

  // return a const reference to the condition of a Person
  Person const& condition(int r, int c) const {
    if (r == -1) {
      r = m_side - 1;
    }
    if (r == m_side) {
      r = 0;
    }
    if (c == -1) {
      c = m_side - 1;
    }
    if (c == m_side) {
      c = 0;
    }
    assert(r >= 0 && r < m_side);
    assert(c >= 0 && c < m_side);
    return my_env[r][c];
  }
  // return a modifiable reference to the condition of a Person
  Person& condition(int r, int c) {
    if (r == -1) {
      r = m_side - 1;
    }
    if (r == m_side) {
      r = 0;
    }
    if (c == -1) {
      c = m_side - 1;
    }
    if (c == m_side) {
      c = 0;
    }
    assert(r >= 0 && r < m_side);
    assert(c >= 0 && c < m_side);
    return my_env[r][c];
  }

  // return the side dimension of the environment
  int side() const { return m_side; }
};

// count the number of infectious people in the neighbouring cells
inline int N_Inf(Environment const& my_env, int r, int c) {
  int counter = 0;
  for (int i : {-1, 0, -1}) {
    for (int j : {-1, 0, 1}) {
      counter = my_env.condition(r + i, c + j) == Person::Infectious
                    ? (counter + 1)
                    : counter;
    }
  }
  if (my_env.condition(r, c) == Person::Infectious) counter += -1;
  return counter;
}

// produce a novel environment according to the state of the previous one
inline Environment evolve(Environment const& current, double beta, double gamma,
                          double mi, std::default_random_engine& gen) {
  int const side = current.side();
  Environment next{side};
  std::uniform_real_distribution<> dist(0, 1);

  for (int i = 0; i != side; ++i) {
    for (int j = 0; j != side; ++j) {
      Person P0 = current.condition(i, j);
      // decide which branch to execute according to person's state
      switch (P0) {
        case Person::Suceptible: {
          auto const p_inf = 1 - pow(1 - beta, N_Inf(current, i, j));
          assert(p_inf >= 0 && p_inf <= 1);
          if (dist(gen) <= p_inf) {
            P0 = Person::Infectious;
          }
        } break;
        // the bigger the gamma, the longer the individual is infectious
        case Person::Infectious: {
          if (dist(gen) >= gamma) {
            if (dist(gen) >= mi) {
              P0 = Person::Suceptible;
            } else {
              P0 = Person::Removed;
            }
          }
        } break;
        default:
          break;
      }
      next.condition(i, j) = P0;
    }
  }
  return next;
}

// count the number of susceptible, infectious and removed people
// in the environment
inline Contagion::State count_SIR(Environment const& current) {
  int const side = current.side();
  int n_sus = 0;
  int n_inf = 0;
  int n_rem = 0;
  for (int i = 0; i != side; ++i) {
    for (int j = 0; j != side; ++j) {
      Person P0 = current.condition(i, j);
      switch (P0) {
        case Person::Suceptible:
          n_sus += 1;
          break;
        case Person::Infectious:
          n_inf += 1;
          break;
        case Person::Removed:
          n_rem += 1;
          break;
        default:
          break;
      }
    }
  }
  State state{n_sus, n_inf, n_rem};
  return state;
}

}  // namespace Contagion

#endif
