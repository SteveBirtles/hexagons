#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "hexroutes.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const float HEX_RATIO = 0.8660254;

class Game : public olc::PixelGameEngine {
 public:
  Game() { sAppName = "Insert Game Name Here"; }

 private:
  float timer = 0;
  int frames = 0;
  int fps;
  int cursorI = 0;
  int cursorJ = 0;
  int targetI = 0;
  int targetJ = 0;
  int currentI = 0;
  int currentJ = 0;
  int nextI = 0;
  int nextJ = 0;
  float progress = 0;
  float speed = 2;
  float hexWidth = 50;
  float hexHeight = HEX_RATIO * hexWidth;
  float hexSide = hexWidth / 2;

 public:
  bool OnUserCreate() override {
    /*
      Load resources here
    */
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

    if (GetMouse(0).bHeld) {
      targetI = cursorI;
      targetJ = cursorJ;
    }
  }

  void processes() {
    if (currentI != targetI || currentJ != targetJ) {
      if (progress == 0) {
        auto routes =
            HexRoutes::getRoutes(currentI - targetI, currentJ - targetJ);

        if (routes.reduceBoth && routes.reduceLargest ||
            routes.reducePositive && routes.reduceNegative) {
          speed = 2;
        } else {
          speed = 4;
        }

        nextI = currentI;
        nextJ = currentJ;

        if (routes.reduceBoth) {
          nextI += routes.reduceBoth_dI;
          nextJ += routes.reduceBoth_dJ;
        }

        if (routes.reduceLargest) {
          nextI += routes.reduceLargest_dI;
          nextJ += routes.reduceLargest_dJ;
        }

        if (routes.reduceNegative) {
          nextI += routes.reduceNegative_dI;
          nextJ += routes.reduceNegative_dJ;
        }

        if (routes.reducePositive) {
          nextI += routes.reducePositive_dI;
          nextJ += routes.reducePositive_dJ;
        }
      }
      progress += GetElapsedTime() * speed;

      if (progress > 1) {
        progress = 0;
        currentI = nextI;
        currentJ = nextJ;
      }
    }
  }

  olc::vi2d vecFromIJ(int i, int j) {
    float x = WINDOW_WIDTH / 2 + i * (hexWidth + hexSide) / 2;
    float y = WINDOW_HEIGHT / 2 - j * hexHeight + i * hexHeight / 2;
    return olc::vi2d{int(x), int(y)};
  }

  void outputs() {
    SetPixelMode(olc::Pixel::NORMAL);

    Clear(olc::BLACK);

    /*
      Game graphics drawn here
    */

    int iRange = (WINDOW_WIDTH / ((hexWidth + hexSide) / 2)) / 2 + 1;
    int jRange = (WINDOW_HEIGHT / hexHeight) / 2 + 1;
    int stagger = iRange % 2;
    for (int u = -iRange; u <= iRange; u++) {
      for (int v = -jRange; v <= jRange; v++) {
        int i = u;
        int j = -v + floor((u - (u < 0 ? 1 : 0)) / 2);

        auto hexVec = vecFromIJ(i, j);
        if (pow(hexVec.x - GetMouseX(), 2) + pow(hexVec.y - GetMouseY(), 2) <
            pow(hexWidth / 2, 2)) {
          cursorI = i;
          cursorJ = j;
        }

        int deltaI = currentI - i;
        int deltaJ = currentJ - j;

        auto distance = std::max(abs(deltaI), abs(deltaJ));
        if (deltaI < 0 != deltaJ < 0) distance = abs(deltaI) + abs(deltaJ);

        uint8_t colour = 255.0 * (1.0 - distance / 16.0);
        if (distance > 16) colour = 0;

        DrawStringDecal(hexVec, std::to_string(distance),
                        olc::Pixel{0, colour, 0});

        DrawLine(hexVec.x + hexSide / 2, hexVec.y - hexHeight / 2,
                 hexVec.x + hexWidth / 2, hexVec.y, olc::DARK_GREY);
        DrawLine(hexVec.x + hexSide / 2, hexVec.y + hexHeight / 2,
                 hexVec.x + hexWidth / 2, hexVec.y, olc::DARK_GREY);
        DrawLine(hexVec.x + hexSide / 2, hexVec.y - hexHeight / 2,
                 hexVec.x - hexSide / 2, hexVec.y - hexHeight / 2,
                 olc::DARK_GREY);
      }
      stagger = (stagger + 1) % 2;
    }

    DrawCircle(vecFromIJ(cursorI, cursorJ), hexSide / 2, olc::GREEN);

    DrawCircle(vecFromIJ(nextI, nextJ), hexSide / 2, olc::RED);

    FillCircle(vecFromIJ(currentI, currentJ) +
                   progress * (vecFromIJ(nextI, nextJ) -
                               vecFromIJ(currentI, currentJ)),
               hexSide / 2, olc::GREEN);

    auto xyPosition = olc::vi2d(GetMouseX(), GetMouseY() - 20);
    DrawStringDecal(xyPosition,
                    std::to_string(cursorI) + ", " + std::to_string(cursorJ));

    if (fps > 0) {
      auto fpsPosition = olc::vi2d(WINDOW_WIDTH - 70, WINDOW_HEIGHT - 70);
      DrawStringDecal(fpsPosition, "FPS " + std::to_string(fps));
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
