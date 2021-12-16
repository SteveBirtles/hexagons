#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class Game : public olc::PixelGameEngine {
 public:
  Game() { sAppName = "Insert Game Name Here"; }

 private:
  float timer = 0;
  int frames = 0;
  int fps;
  bool segment[24]{false, false, false, false, false, false, false, false,
                   false, false, false, false, false, false, false, false,
                   false, false, false, false, false, false, false, false};
  float angle;
  bool bigAngle = false;

  olc::vi2d mouseStart;
  bool mouseDown = false;

  olc::Sprite* hexagonSprites;
  olc::Decal* hexagonDecal;

  const float HEX_RATIO = 0.8660254;
  float hexWidth = 500;
  float hexHeight = int(HEX_RATIO * hexWidth);
  float zoom = 1;

  std::vector<olc::vf2d> points{
      {-hexWidth / 4, -hexHeight / 2},
      {hexWidth / 4, -hexHeight / 2},
      {hexWidth / 2, 0},
      {hexWidth / 4, hexHeight / 2},
      {-hexWidth / 4, hexHeight / 2},
      {-hexWidth / 2, 0},
      {0, 0},
      {0, 0},
      {0, 0},
      {0, 0},
      {0, 0},
      {0, 0},
      {0, 0},
  };

  std::vector<std::tuple<int, int, int>> segPoints{

      {0, 1, 11},  {1, 2, 6},  {2, 3, 7},  {3, 4, 8},  {4, 5, 9},   {5, 0, 10},
      {0, 1, 6},   {1, 2, 7},  {2, 3, 8},  {3, 4, 9},  {4, 5, 10},  {5, 0, 11},
      {12, 1, 11}, {12, 2, 6}, {12, 3, 7}, {12, 4, 8}, {12, 5, 9},  {12, 0, 10},
      {12, 0, 6},  {12, 1, 7}, {12, 2, 8}, {12, 3, 9}, {12, 4, 10}, {12, 5, 11},

  };

  olc::vf2d zero{0, 0};

  olc::vf2d lineIntersection(olc::vf2d start1, olc::vf2d end1, olc::vf2d start2,
                             olc::vf2d end2) {
    auto& x1 = start1.x;
    auto& y1 = start1.y;
    auto& x2 = end1.x;
    auto& y2 = end1.y;
    auto& x3 = start2.x;
    auto& y3 = start2.y;
    auto& x4 = end2.x;
    auto& y4 = end2.y;
    auto wallD = std::sqrt(std::pow(x4 - x3, 2) + std::pow(y4 - y3, 2));
    if (wallD == 0) return {-1, -1};
    auto d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (d == 0) return {-1, -1};
    auto p = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / d;
    auto q = ((x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2)) / d;
    return {p, q};
  }

 public:
  bool OnUserCreate() override {
    /*
      Load resources here
    */

    for (int i = 0; i < 6; i++) {
      points[i + 6].x = (points[i].x + points[(i + 2) % 6].x) / 2;
      points[i + 6].y = (points[i].y + points[(i + 2) % 6].y) / 2;
    }

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    timer += fElapsedTime;
    frames++;
    if (timer > 1.0) {
      fps = frames;
      frames = 0;
      timer -= 1;
    }

    inputs();
    processes();
    outputs();

    if (GetKey(olc::Key::ESCAPE).bPressed) {
      return false;
    } else {
      return true;
    }
  }

  void inputs() {
    /*
      Game controls goes here
    */

    if (GetMouse(0).bPressed) {
      mouseStart = GetMousePos();
    }

    if (GetMouse(0).bHeld) {
      mouseDown = true;
    } else {
      mouseDown = false;
    }
  }

  void processes() {
    /*
      Game logic goes here
    */

    angle = 90 + atan2(GetMouseY() - mouseStart.y, GetMouseX() - mouseStart.x) *
                     (180 / 3.14159265);
    if (angle < 0) angle += 360;

    if (abs(angle - 30) <= 15 || abs(angle - 90) <= 15 ||
        abs(angle - 150) <= 15 || abs(angle - 210) <= 15 ||
        abs(angle - 270) <= 15 || abs(angle - 330) <= 15) {
      bigAngle = true;
    } else {
      bigAngle = false;
    }

    for (int i = 0; i < 24; i++) {
      segment[i] = true;
    }

    if (mouseDown) {
      // if (bigAngle) {
      olc::vf2d centre(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

      for (int j = 0; j < 24; j++) {
        auto first =
            lineIntersection(points[std::get<0>(segPoints[j])] + centre,
                             points[std::get<1>(segPoints[j])] + centre,
                             mouseStart, GetMousePos());

        auto second =
            lineIntersection(points[std::get<1>(segPoints[j])] + centre,
                             points[std::get<2>(segPoints[j])] + centre,
                             mouseStart, GetMousePos());

        auto third =
            lineIntersection(points[std::get<2>(segPoints[j])] + centre,
                             points[std::get<0>(segPoints[j])] + centre,
                             mouseStart, GetMousePos());

        if (first.x >= 0 && first.x <= 1 && first.y >= 0 && first.y <= 1 ||
            second.x >= 0 && second.x <= 1 && second.y >= 0 && second.y <= 1 ||
            third.x >= 0 && third.x <= 1 && third.y >= 0 && third.y <= 1)
          segment[j] = false;
      }
      //}
    }

    for (int i = 0; i < 6; i++) {
      if (segment[i] && segment[i + 6] &&
          (!segment[i + 18] && !segment[i + 12]) &&
          (!segment[(i + 1) % 6] || !segment[(i + 5) % 6 + 6])) {
        segment[i] = false;
        segment[i + 6] = false;
      }

      // // 6 23 !0 !17
      // if (segment[i + 6] && segment[(i + 5) % 6 + 18] && !segment[i] &&
      //     !segment[(i + 5) % 6 + 12]) {
      //   segment[i + 6] = false;
      //   segment[(i + 5) % 6 + 18] = false;
      // }

      // // 0 13 !6 !19
      // if (segment[i] && segment[(i + 1) % 6 + 12] && !segment[i + 6] &&
      //     !segment[(i + 1) % 6 + 18]) {
      //   segment[i] = false;
      //   segment[(i + 1) % 6 + 12] = false;
      // }

      // // 13 23 !12 !18
      // if (segment[(i + 1) % 6 + 12] && segment[(i + 5) % 6 + 18] &&
      //     !segment[i + 12] && !segment[i + 18]) {
      //   segment[(i + 1) % 6 + 12] = false;
      //   segment[(i + 5) % 6 + 18] = false;
      // }

      // // // 0 //!11 //!12 //!18
      // // if (segment[i] && segment[(i + 5) % 6 + 6] && !segment[i + 12] &&
      // //     !segment[i + 18]) {
      // //   segment[i] = false;
      // // }

      // // // 6 //!1 //!12 //!18
      // // if (segment[i + 6] && segment[(i + 1) % 6] && !segment[i + 12] &&
      // //     !segment[i + 18]) {
      // //   segment[i + 6] = false;
      // // }
    }
  }

  olc::vi2d vecFromIJ(int i, int j) {
    float x = WINDOW_WIDTH / 2 + i * 3 * (hexWidth * zoom / 4);
    float y =
        WINDOW_HEIGHT / 2 - j * hexHeight * zoom + i * hexHeight * zoom / 2;
    return olc::vi2d{int(x), int(y)};
  }

  void outputs() {
    Clear(olc::BLACK);

    olc::vf2d centre(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    for (int j = 0; j < 24; j++) {
      if (!segment[j]) continue;

      olc::Pixel colour = olc::BLUE;

      // if (!segment[j % 6] || !segment[j % 6 + 6] || !segment[j % 6 + 12] ||
      //     !segment[j % 6 + 18])
      //   colour = olc::DARK_BLUE;

      FillTriangle(points[std::get<0>(segPoints[j])] + centre,
                   points[std::get<1>(segPoints[j])] + centre,
                   points[std::get<2>(segPoints[j])] + centre, colour);
    }

    for (int j = 0; j < 24; j++) {
      auto pos = (points[std::get<0>(segPoints[j])] +
                  points[std::get<1>(segPoints[j])] +
                  points[std::get<2>(segPoints[j])]) /
                 3;
      DrawString(pos + centre, std::to_string(j),
                 segment[j] ? olc::GREEN : olc::RED);
    }

    if (mouseDown) {
      DrawString(olc::vi2d(10, 10), std::to_string(angle));

      // if (bigAngle) {
      DrawLine(mouseStart, GetMousePos(), olc::RED);
      //}
    }
  }

  bool OnUserDestroy() override {
    std::cout << "Closing game" << std::endl;
    return true;
  }
};

int main() {
  Game game;
  if (game.Construct(WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1)) game.Start();
  return 0;
}
