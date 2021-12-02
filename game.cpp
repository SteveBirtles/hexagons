#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const int MAP_SIZE = 100;

class Game : public olc::PixelGameEngine {
 public:
  Game() { sAppName = "Insert Game Name Here"; }

 private:
  float timer = 0;
  int frames = 0;
  int fps;

  olc::Sprite* hexagonSprites;
  olc::Decal* hexagonDecal;

  const float HEX_RATIO = 0.8660254;
  float hexWidth = 100;
  float hexHeight = int(HEX_RATIO * hexWidth);

  std::vector<std::tuple<float, float>> points{
      {0, 0},  // 0

      {-hexWidth / 4, -hexHeight / 2 + 1},  // 1
      {hexWidth / 4, -hexHeight / 2 + 1},   // 2
      {-hexWidth / 2 + 1, 0},               // 3
      {hexWidth / 2 - 1, 0},                // 4
      {-hexWidth / 4, hexHeight / 2 - 1},   // 5
      {hexWidth / 4, hexHeight / 2 - 1},    // 6

      {-hexWidth / 8, -hexHeight / 4},  // 7
      {hexWidth / 8, -hexHeight / 4},   // 8
      {-hexWidth / 4, 0},               // 9
      {hexWidth / 4, 0},                // 10
      {-hexWidth / 8, hexHeight / 4},   // 11
      {hexWidth / 8, hexHeight / 4},    // 12

      {0, -2 * hexHeight / 6},          // 13
      {-hexWidth / 4, -hexHeight / 6},  // 14
      {hexWidth / 4, -hexHeight / 6},   // 15
      {-hexWidth / 4, hexHeight / 6},   // 16
      {hexWidth / 4, hexHeight / 6},    // 17
      {0, 2 * hexHeight / 6},           // 18

  };

  std::vector<std::vector<int>> hexagons{

      {1, 2, 4, 6, 5, 3},

      {1, 2, 5, 3},
      {2, 4, 6, 5},
      {1, 2, 4, 3},
      {3, 4, 6, 5},
      {1, 2, 4, 6},
      {1, 6, 5, 3},

      {1, 2, 11, 3},
      {8, 4, 6, 5},
      {1, 2, 4, 9},
      {3, 10, 6, 5},
      {7, 2, 4, 6},
      {1, 12, 5, 3},

      {1, 8, 5, 3},
      {2, 4, 6, 11},
      {1, 2, 10, 3},
      {9, 4, 6, 5},
      {1, 2, 4, 12},
      {7, 6, 5, 3},

      {1, 2, 17, 5, 3},
      {14, 2, 4, 6, 5},
      {1, 2, 4, 18, 3},
      {13, 4, 6, 5, 3},
      {1, 2, 4, 6, 16},
      {1, 15, 6, 5, 3},

      {1, 2, 4, 0, 6, 5, 3},
      {1, 2, 4, 6, 5, 3, 0},
      {1, 2, 4, 6, 0, 5, 3},
      {1, 0, 2, 4, 6, 5, 3},
      {1, 2, 4, 6, 5, 0, 3},
      {1, 2, 0, 4, 6, 5, 3},

      {1, 2, 6, 3},
      {1, 4, 6, 5},
      {1, 4, 5, 3},
      {2, 4, 6, 3},
      {1, 2, 4, 5},
      {2, 6, 5, 3},

      {1, 2, 6},
      {1, 4, 6},
      {3, 4, 5},
      {3, 4, 6},
      {2, 4, 5},
      {2, 6, 5},

      {1, 6, 3},
      {1, 6, 5},
      {1, 4, 3},
      {2, 4, 3},
      {1, 2, 5},
      {3, 2, 5},

      {1, 2, 4, 0, 5, 3},
      {2, 4, 6, 5, 3, 0},
      {2, 0, 6, 5, 3, 1},
      {1, 2, 4, 6, 5, 0},
      {1, 2, 4, 6, 0, 3},
      {1, 0, 4, 6, 5, 3},

      {1, 2, 4, 5, 3},
      {2, 4, 6, 5, 3},
      {1, 2, 6, 5, 3},
      {1, 2, 4, 6, 5},
      {1, 2, 4, 6, 3},
      {1, 4, 6, 5, 3},

      {1, 2, 4, 6, 12, 5, 3},
      {1, 2, 4, 6, 5, 3, 7},
      {1, 2, 4, 10, 6, 5, 3},
      {1, 2, 4, 6, 5, 3, 9},
      {1, 2, 4, 6, 5, 11, 3},
      {1, 8, 2, 4, 6, 5, 3},

      {1, 2, 4, 12, 6, 5, 3},
      {1, 7, 2, 4, 6, 5, 3},
      {1, 2, 10, 4, 6, 5, 3},
      {1, 2, 4, 6, 5, 9, 3},
      {1, 2, 4, 6, 11, 5, 3},
      {1, 2, 8, 4, 6, 5, 3},

  };

  int cursorI;
  int cursorJ;
  int currentHex = 12 * 6 + 1;
  bool hexPicker = false;

  int map[MAP_SIZE][MAP_SIZE];
  olc::vf2d camera{0, 0};
  olc::vf2d cameraStart;
  olc::vf2d dragStart;

  void saveMap() {
    auto mapFile = std::ofstream("map.csv");
    if (!mapFile.is_open()) return;
    for (auto y = 0; y < MAP_SIZE; y++) {
      for (auto x = 0; x < MAP_SIZE; x++) {
        mapFile << std::to_string(map[x][y]);
        if (x != MAP_SIZE - 1) {
          mapFile << ",";
        } else {
          mapFile << std::endl;
        }
      }
    }
    mapFile.close();
  }

  void loadMap() {
    auto mapFile = std::ifstream("map.csv");
    if (!mapFile.is_open()) return;
    std::string line;
    std::string token;
    int y = 0;
    while (std::getline(mapFile, line, '\n')) {
      auto stream = std::istringstream(line);
      int x = 0;
      while (std::getline(stream, token, ',')) {
        map[x][y] = std::stoi(token);
        x++;
        if (x >= MAP_SIZE) break;
      }
      y++;
      if (y >= MAP_SIZE) break;
    }
    mapFile.close();
  }

 public:
  bool OnUserCreate() override {
    /*
      Load resources here
    */

    hexagonSprites = PrepareHexagonSprite();
    // hexagonSprite = new olc::Sprite("./hexagons.png");
    hexagonDecal = new olc::Decal(hexagonSprites);

    for (auto i = 0; i < MAP_SIZE; i++) {
      for (auto j = 0; j < MAP_SIZE; j++) {
        map[i][j] = 0;
      }
    }
    loadMap();

    return true;
  }

  olc::Sprite* PrepareHexagonSprite() {
    auto sprite = new olc::Sprite(hexWidth * 13, hexHeight * 6);

    SetDrawTarget(sprite);

    Clear(olc::BLANK);

    for (auto hex = 1; hex <= hexagons.size(); hex++) {
      int x = int((hex - 1) / 6) * (hexWidth * 1.0);
      int y = ((hex - 1) % 6) * (hexHeight * 1.0);

      auto centre =
          olc::vf2d{float(hexWidth * 0.5 + x), float(hexHeight * 0.5 + y)};

      auto h = (hex < hexagons.size()) ? hex : 0;

      for (auto i = 0; i < hexagons[h].size(); i++) {
        auto j = (i + 1) % hexagons[h].size();
        auto start = hexagons[h][i];
        auto end = hexagons[h][j];

        auto first =
            olc::vf2d{std::get<0>(points[start]), std::get<1>(points[start])} +
            centre;
        auto second =
            olc::vf2d{std::get<0>(points[end]), std::get<1>(points[end])} +
            centre;

        DrawLine(first, second);
      }
    }

    SetDrawTarget(nullptr);
    return sprite;
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

    hexPicker = GetKey(olc::Key::TAB).bHeld;

    if (hexPicker) {
      auto x = GetMouseX() - (WINDOW_WIDTH / 2 - hexWidth * 6);
      auto y = GetMouseY() - (WINDOW_HEIGHT / 2 - hexHeight * 3);
      if (x < 0 || y < 0 || x >= hexWidth * 12 || y >= hexHeight * 6) {
        currentHex = 12 * 6 + 1;
      } else {
        currentHex = 1 + int(y / hexHeight) + 6 * int(x / hexWidth);
        if (currentHex < 1 || currentHex > 12 * 6 + 1) currentHex = 12 * 6 + 1;
      }
    }

    if (!hexPicker) {
      if (GetKey(olc::Key::DOWN).bPressed) {
        currentHex++;
        if (currentHex > 12 * 6 + 1) currentHex = 1;
      }

      if (GetKey(olc::Key::UP).bPressed) {
        currentHex--;
        if (currentHex < 1) currentHex = 12 * 6 + 1;
      }

      auto i = cursorI + MAP_SIZE / 2;
      auto j = cursorJ  + MAP_SIZE / 2;
      if (i >= 0 && j >= 0 && i < MAP_SIZE && j < MAP_SIZE) {
        if (GetMouse(0).bHeld) {
          map[i][j] = currentHex;
        } else if (GetMouse(1).bHeld) {
          map[i][j] = 0;
        }
      }
    }

    if (GetMouse(2).bPressed) {
      dragStart.x = GetMouseX();
      dragStart.y = GetMouseY();
      cameraStart.x = camera.x;
      cameraStart.y = camera.y;
    }
    if (GetMouse(2).bHeld) {
      camera.x = cameraStart.x + dragStart.x - GetMouseX();
      camera.y = cameraStart.y + dragStart.y - GetMouseY();
    }
  }

  void processes() {
    /*
      Game logic goes here
    */
  }

  olc::vi2d vecFromIJ(int i, int j) {
    float x = WINDOW_WIDTH / 2 + i * 3 * (hexWidth / 4);
    float y = WINDOW_HEIGHT / 2 - j * hexHeight + i * hexHeight / 2;
    return olc::vi2d{int(x), int(y)};
  }

  void outputs() {
    Clear(olc::BLACK);
    SetPixelMode(olc::Pixel::MASK);

    /*
      Game graphics drawn here
    */

    auto topLeftCorner = olc::vf2d{hexWidth / 2, hexHeight / 2};

    int uRange = (WINDOW_WIDTH / hexWidth) / 2 + 1;
    int vRange = (WINDOW_HEIGHT / hexHeight) / 2 + 1;
    for (int u = -uRange + camera.x / hexWidth;
         u <= uRange + camera.x / hexWidth; u++) {
      for (int v = -vRange + camera.y / hexHeight;
           v <= vRange + camera.y / hexHeight; v++) {
        int i = u;
        int j = -v + floor((u - (u < 0 ? 1 : 0)) / 2);

        auto position = vecFromIJ(i, j) - camera;

        if (pow(position.x - GetMouseX(), 2) +
                pow(position.y - GetMouseY(), 2) <
            pow(hexWidth / 2, 2)) {
          cursorI = i;
          cursorJ = j;
        }

        if (i  + MAP_SIZE / 2 >= 0 && j  + MAP_SIZE / 2 >= 0 && i  + MAP_SIZE / 2 < MAP_SIZE && j  + MAP_SIZE / 2 < MAP_SIZE) {
          int hex = map[i  + MAP_SIZE / 2][j  + MAP_SIZE / 2];
          auto size = olc::vf2d{hexWidth, hexHeight};

          if (hex > 0 && hex <= 12 * 6 + 1) {
            auto offset = olc::vi2d{int(((hex - 1) / 6) * hexWidth),
                                    int(((hex - 1) % 6) * hexHeight)};
            DrawPartialDecal(position - topLeftCorner, size, hexagonDecal,
                             offset, size);
          } else {
            auto offset = olc::vi2d{int(((12 * 6) / 6) * hexWidth),
                                    int(((12 * 6) % 6) * hexHeight)};
            DrawPartialDecal(position - topLeftCorner, size, hexagonDecal,
                             offset, size, olc::VERY_DARK_BLUE);
          }
        }
      }
    }

    if (hexPicker) {
      FillRectDecal(olc::vf2d{WINDOW_WIDTH / 2 - hexWidth * 6,
                              WINDOW_HEIGHT / 2 - hexHeight * 3},
                    olc::vf2d{hexWidth * 12 + 2, hexHeight * 6 + 2},
                    olc::Pixel(0, 0, 0, 192));

      DrawLineDecal(olc::vf2d{WINDOW_WIDTH / 2 - hexWidth * 6,
                              WINDOW_HEIGHT / 2 - hexHeight * 3},
                    olc::vf2d{WINDOW_WIDTH / 2 + hexWidth * 6 + 2,
                              WINDOW_HEIGHT / 2 - hexHeight * 3},
                    olc::GREEN);

      DrawLineDecal(olc::vf2d{WINDOW_WIDTH / 2 + hexWidth * 6 + 2,
                              WINDOW_HEIGHT / 2 - hexHeight * 3},
                    olc::vf2d{WINDOW_WIDTH / 2 + hexWidth * 6 + 2,
                              WINDOW_HEIGHT / 2 + hexHeight * 3 + 2},
                    olc::GREEN);

      DrawLineDecal(olc::vf2d{WINDOW_WIDTH / 2 + hexWidth * 6 + 2,
                              WINDOW_HEIGHT / 2 + hexHeight * 3 + 2},
                    olc::vf2d{WINDOW_WIDTH / 2 - hexWidth * 6,
                              WINDOW_HEIGHT / 2 + hexHeight * 3 + 2},
                    olc::GREEN);

      DrawLineDecal(olc::vf2d{WINDOW_WIDTH / 2 - hexWidth * 6,
                              WINDOW_HEIGHT / 2 + hexHeight * 3 + 2},
                    olc::vf2d{WINDOW_WIDTH / 2 - hexWidth * 6,
                              WINDOW_HEIGHT / 2 - hexHeight * 3},
                    olc::GREEN);

      DrawPartialDecal(olc::vf2d{WINDOW_WIDTH / 2 - hexWidth * 6,
                                 WINDOW_HEIGHT / 2 - hexHeight * 3},
                       olc::vf2d{hexWidth * 12, hexHeight * 6}, hexagonDecal,
                       olc::vf2d{0, 0}, olc::vf2d{hexWidth * 12, hexHeight * 6},
                       olc::GREEN);
    }

    // DrawCircle(vecFromIJ(cursorI, cursorJ), hexHeight / 2, olc::GREEN);

    if (!hexPicker && currentHex > 0 && currentHex <= 12 * 6 + 1) {
      auto offset = olc::vi2d{int(((currentHex - 1) / 6) * hexWidth),
                              int(((currentHex - 1) % 6) * hexHeight)};
      auto size = olc::vf2d{hexWidth, hexHeight};
      DrawPartialDecal(vecFromIJ(cursorI, cursorJ) - camera - topLeftCorner,
                       size, hexagonDecal, offset, size, olc::GREEN);
    }

    auto xyPosition = olc::vi2d(WINDOW_WIDTH - 70, WINDOW_HEIGHT - 50);
    DrawStringDecal(xyPosition,
                    std::to_string(cursorI) + ", " + std::to_string(cursorJ));

    if (fps > 0) {
      auto fpsPosition = olc::vi2d(WINDOW_WIDTH - 70, WINDOW_HEIGHT - 70);
      DrawStringDecal(fpsPosition, "FPS " + std::to_string(fps));
    }
  }

  bool OnUserDestroy() override {
    std::cout << "Closing game" << std::endl;
    saveMap();
    return true;
  }
};

int main() {
  Game game;
  if (game.Construct(WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1)) game.Start();
  return 0;
}
