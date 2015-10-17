#include <iostream>
#include <stack>
#include <deque>
#include <vector>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <functional>
#include <climits>

#include <piet.hpp>

struct Diff {
  int lightness;
  int hue;
  static Diff colorDiff(Color src, Color dst) {
    if (src == Black || src == White || dst == Black || dst == White) return { -1, -1};
    return {((dst - src)/6 + 3)%3, ((dst - src)%6 + 6)%6};
  }
};

struct Point {
  int x;
  int y;

  bool operator== (Point const p) {
    return x == p.x && y == p.y;
  }
};

struct Piet {
  Color* piet;
  size_t width;
  size_t height;
  Color at(Point p) {
    // std::cout << "x: " << p.x << ", y: " << p.y << std::endl;
    return piet[p.y * width + p.x];
  }
};

struct Env {
  std::stack<int> stack;
  int area;
  Point point;
  bool cc;
  int dp;
  Env() : stack(), area(0), point({0,0}), cc(false), dp(0) {}
};

enum class Op {
  Nop,
  Push,
  Pop,
  Add,
  Sub,
  Mul,
  Div,
  Mod,
  Not,
  Greater,
  Pointer,
  Switch,
  Dup,
  Roll,
  InN,
  InC,
  OutN,
  OutC,
};

char const* show(Op op) {
  switch (op) {
  default:
  case Op::Nop: return "Nop";
  case Op::Push: return "Push";
  case Op::Pop: return "Pop";
  case Op::Add: return "Add";
  case Op::Sub: return "Sub";
  case Op::Mul: return "Mul";
  case Op::Div: return "Div";
  case Op::Mod: return "Mod";
  case Op::Not: return "Not";
  case Op::Greater: return "Greater";
  case Op::Pointer: return "Pointer";
  case Op::Switch: return "Switch";
  case Op::Dup: return "Dup";
  case Op::Roll: return "Roll";
  case Op::InN: return "InN";
  case Op::InC: return "InC";
  case Op::OutN: return "OutN";
  case Op::OutC: return "OutC";
  }
}

bool outside(Piet piet, Point point) {
  return (0 > point.x ||
          0 > point.y ||
          static_cast<unsigned>(point.x) >= piet.width ||
          static_cast<unsigned>(point.y) >= piet.height);
}

bool unmovable(Piet piet, Point point) {
  if (outside(piet, point)) return true;
  return piet.at(point) == Black;
}

int mod(int n, int m) {
  if (n == 0 || m == 0 || n%m == 0) return 0;
  if ((n > 0 && m > 0) || (n < 0 && m < 0)) return n % m;
  return n % m + m;
}

Op detectCmd(Color current, Color next) {
  if (current == White || next == White) return Op::Nop;
  Diff d = Diff::colorDiff(current, next);

  // std::cout << "hue: " << d.hue << ", lightness: " << d.lightness << std::endl;

  switch (d.hue) {
  case 0:
    if (d.lightness == 0) return Op::Nop;
    if (d.lightness == 1) return Op::Push;
    if (d.lightness == 2) return Op::Pop;
  case 1:
    if (d.lightness == 0) return Op::Add;
    if (d.lightness == 1) return Op::Sub;
    if (d.lightness == 2) return Op::Mul;
  case 2:
    if (d.lightness == 0) return Op::Div;
    if (d.lightness == 1) return Op::Mod;
    if (d.lightness == 2) return Op::Not;
  case 3:
    if (d.lightness == 0) return Op::Greater;
    if (d.lightness == 1) return Op::Pointer;
    if (d.lightness == 2) return Op::Switch;
  case 4:
    if (d.lightness == 0) return Op::Dup;
    if (d.lightness == 1) return Op::Roll;
    if (d.lightness == 2) return Op::InN;
  case 5:
    if (d.lightness == 0) return Op::InC;
    if (d.lightness == 1) return Op::OutN;
    if (d.lightness == 2) return Op::OutC;
  default:
    throw("never come");
    return Op::Nop;
  }
}

void execCmd(Env& env, Color current, Color next) {
  Op op = detectCmd(current, next);

  auto biOp = [&](std::function<int(int, int)> biop) {
    if (env.stack.size() >= 2) {
      int x1 = env.stack.top();
      env.stack.pop();
      int x2 = env.stack.top();
      env.stack.pop();

      env.stack.push(biop(x2, x1));
    }
  };

  std::cout << show(op) << std::endl;

  switch (op) {
  default:
  case Op::Nop:
    /* do nothing */
    break;
  case Op::Push:
    env.stack.push(env.area);
    break;
  case Op::Pop:
    env.stack.pop();
    break;
  case Op::Add:
    biOp(std::plus<int>());
    break;
  case Op::Sub:
    biOp(std::minus<int>());
    break;
  case Op::Mul:
    biOp(std::multiplies<int>());
    break;
  case Op::Div:
    biOp(std::divides<int>()); // when stack top is 0, it will clash. but this is defined "implementation-dependent error".
    // "though simply ignoring the command is recommended." ### FIXME ###
    break;
  case Op::Mod:
    biOp(mod);
    break;
  case Op::Not:
    if(env.stack.size() != 0) {
      if(env.stack.top() == 0) {
        env.stack.pop();
        env.stack.push(1);
      } else {
        env.stack.pop();
        env.stack.push(0);
      }
    }
    break;
  case Op::Greater:
    biOp(std::greater<int>());
    break;
  case Op::Pointer:
    if(env.stack.size() != 0) {
      int tmp = env.stack.top();
      env.stack.pop();
      env.dp += tmp;
    }
    break;
  case Op::Switch:
    if(env.stack.size() != 0) {
      int tmp = env.stack.top();
      env.stack.pop();
      env.cc += tmp;
    }
    break;
  case Op::Dup:
    if(env.stack.size() != 0) {
      int tmp = env.stack.top();
      env.stack.push(tmp);
    }
    break;
  case Op::Roll:
    if(env.stack.size() >= 2) {
      int const x1 = env.stack.top();
      env.stack.pop();
      int const x2 = env.stack.top();
      if (x2 < 0 || env.stack.size() < static_cast<unsigned>(x2 - 1)) {
        env.stack.push(x1);
      } else {
        env.stack.pop();
        std::vector<int> view(x2);
        for (int i(0); i < x2; ++i) {
          view.push_back(env.stack.top());
          env.stack.pop();
        }
        for (int i(0); i < x2; ++i) {
          env.stack.push(view[(x2 - 1) - (i + x1) % x2]);
        }
      }
    }
    break;
  case Op::InN:
  {
    int n;
    std::cin >> n;
    env.stack.push(n);
  }
  break;
  case Op::InC:
  {
    char n;
    std::cin >> n;
    env.stack.push(n);
  }
  break;
  case Op::OutN:
  {
    std::cout << env.stack.top();
    env.stack.pop();
  }
  break;
  case Op::OutC:
  {
    std::cout << static_cast<char>(env.stack.top());
    env.stack.pop();
  }
  break;
  }
}

std::tuple<Point, int> findNextCodelImp(Env const& env, Piet piet) {
  using std::begin;
  using std::end;
  std::deque<Point> que;
  que.push_back(env.point);
  std::vector<Point> done, same;
  Color color = piet.at(env.point);

  while (que.size() > 0) {
    Point p = que.front();
    que.pop_front();
    done.push_back(p);
    if(piet.at(p) != color) continue;
    same.push_back(p);
    Point newp{-1, -1};
    auto adder = [&](Point po) {
      if (std::find(begin(done), end(done), po) == end(done)) {
        if (std::find(begin(que), end(que), po) == end(que)) {
          que.push_back(po);
        }
      }
    };
    if (p.x != 0) {
      newp = {p.x - 1, p.y};
      adder(newp);
    }
    if (p.y != 0) {
      newp = {p.x, p.y - 1};
      adder(newp);
    }
    if (static_cast<unsigned>(p.x) != width - 1) {
      newp = {p.x + 1, p.y};
      adder(newp);
    }
    if (static_cast<unsigned>(p.y) != height - 1) {
      newp = {p.x, p.y + 1};
      adder(newp);
    }
  }
  int area = same.size();
  Point next;
  int max = -1;
  int min = INT_MAX;
  auto lessX = [](Point p, Point q) { return p.x < q.x; };
  auto lessY = [](Point p, Point q) { return p.y < q.y; };
  switch (env.dp % 4) {
  case 0:
  {
    max = std::accumulate(begin(same), end(same), max, [](int m, Point p) { return std::max(m, p.x); }); // max x
    auto dend = std::remove_if(begin(same), end(same), [max](Point p) { return p.x != max; });
    if (std::distance(begin(same), dend) != 1) {
      if (env.cc %2 == 0) {
        next = *std::min_element(begin(same), dend, lessY);
      } else {
        next = *std::max_element(begin(same), dend, lessY);
      }
    } else {
      next = same[0];
    }
    next.x += 1;
  }
  break;
  case 1:
  {
    max = std::accumulate(begin(same), end(same), max, [](int m, Point p) { return std::max(m, p.y); }); // max y
    auto dend = std::remove_if(begin(same), end(same), [max](Point p) { return p.y != max; });
    if (std::distance(begin(same), dend) != 1) {
      if (env.cc %2 == 0) {
        next = *std::min_element(begin(same), dend, lessX);
      } else {
        next = *std::max_element(begin(same), dend, lessX);
      }
    } else {
      next = same[0];
    }
    next.y += 1;
  }
  break;
  case 2:
  {
    min = std::accumulate(begin(same), end(same), min, [](int m, Point p) { return std::min(m, p.x); }); // min x
    auto dend = std::remove_if(begin(same), end(same), [min](Point p) { return p.x != min; });
    if (std::distance(begin(same), dend) != 1) {
      if (env.cc %2 == 0) {
        next = *std::max_element(begin(same), dend, lessY);
      } else {
        next = *std::min_element(begin(same), dend, lessY);
      }
    } else {
      next = same[0];
    }
    next.x -= 1;
  }
  break;
  case 3:
  {
    min = std::accumulate(begin(same), end(same), min, [](int m, Point p) { return std::min(m, p.y); }); // min y
    auto dend = std::remove_if(begin(same), end(same), [min](Point p) { return p.y != min; });
    if (std::distance(begin(same), dend) != 1) {
      if (env.cc %2 == 0) {
        next = *std::max_element(begin(same), dend, lessX);
      } else {
        next = *std::min_element(begin(same), dend, lessX);
      }
    } else {
      next = same[0];
    }
    next.x -= 1;
  }
  break;
}

  return std::make_tuple(next, area);
}

std::tuple<Point, int, bool> findNextCodel(Env const& env, Piet piet) {
  std::tuple<Point, int> point = findNextCodelImp(env, piet);
  Point p = std::get<0>(point);
  if (outside(piet, p)) {
    return std::make_tuple(p, std::get<1>(point), false);
  }

  Color color = piet.at(std::get<0>(point));
  bool exec = true;
  if (color == White) {
    exec = false;
    while (!outside(piet, p) && piet.at(p) == White) {
      switch (env.dp % 4) {
      case 0:
        p.x++;
        break;
      case 1:
        p.y++;
        break;
      case 2:
        p.x--;
        break;
      case 3:
        p.y--;
        break;
      }
    }
  }

  return std::make_tuple(p, std::get<1>(point), exec);
}

bool next(Env& env, Piet piet) {
  std::tuple<Point, int, bool> nextCodel = findNextCodel(env, piet);

  if (unmovable(piet, std::get<0>(nextCodel))) {
    env.cc = !env.cc;
    nextCodel = findNextCodel(env, piet);
    if (unmovable(piet, std::get<0>(nextCodel))) {
      env.dp++;
      nextCodel = findNextCodel(env, piet);
      if (unmovable(piet, std::get<0>(nextCodel))) {
        env.cc = !env.cc;
        nextCodel = findNextCodel(env, piet);
        if (unmovable(piet, std::get<0>(nextCodel))) {
          env.dp++;
          nextCodel = findNextCodel(env, piet);
          if (unmovable(piet, std::get<0>(nextCodel))) {
            env.cc = !env.cc;
            nextCodel = findNextCodel(env, piet);
            if (unmovable(piet, std::get<0>(nextCodel))) {
              env.dp++;
              nextCodel = findNextCodel(env, piet);
              if (unmovable(piet, std::get<0>(nextCodel))) {
                env.cc = !env.cc;
                nextCodel = findNextCodel(env, piet);
                if (unmovable(piet, std::get<0>(nextCodel))) {
                  env.dp++;
                  nextCodel = findNextCodel(env, piet);
                  if (unmovable(piet, std::get<0>(nextCodel))) {
                    return false;
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  Color current = piet.at(env.point);
  Color next = piet.at(std::get<0>(nextCodel));
  env.area = std::get<1>(nextCodel);
  if (std::get<2>(nextCodel)) {
    execCmd(env, current, next);
  }
  env.point = std::get<0>(nextCodel);
  return true;
}

void runPiet(Piet piet) {
  Env env;
  bool status(true);
  while(status) {
    status = next(env, piet);
  }
  return;
}

int main(int, char**) {
  runPiet({piet, width, height});
  return 0;
}
