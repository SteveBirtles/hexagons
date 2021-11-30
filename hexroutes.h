#include <cmath> 

struct HexRoutes {
  HexRoutes() {
    reduceLargest = false;
    reduceBoth = false;
    reducePositive = false;
    reduceNegative = false;
    reduceLargest_dI = 0;
    reduceBoth_dI = 0;
    reducePositive_dI = 0;
    reduceNegative_dI = 0;
    reduceLargest_dJ = 0;
    reduceBoth_dJ = 0;
    reducePositive_dJ = 0;
    reduceNegative_dJ = 0;
  }
  bool reduceLargest;
  bool reduceBoth;
  bool reducePositive;
  bool reduceNegative;

  int reduceLargest_dI;
  int reduceBoth_dI;
  int reducePositive_dI;
  int reduceNegative_dI;

  int reduceLargest_dJ;
  int reduceBoth_dJ;
  int reducePositive_dJ;
  int reduceNegative_dJ;

  static HexRoutes getRoutes(int deltaI, int deltaJ) {
    HexRoutes routes;

    if (deltaI == 0) {
      if (deltaJ < 0) {
        routes.reduceNegative = true;
      } else if (deltaJ > 0) {
        routes.reducePositive = true;
      }
    } else if (deltaJ == 0) {
      if (deltaI > 0) {
        routes.reducePositive = true;
      } else if (deltaI < 0) {
        routes.reduceNegative = true;
      }
    } else if (deltaI < 0 && deltaJ < 0) {
      if (deltaI < deltaJ || deltaJ < deltaI) {
        routes.reduceLargest = true;
        routes.reduceBoth = true;
      } else {
        routes.reduceBoth = true;
      }
    } else if (deltaI > 0 && deltaJ > 0) {
      if (deltaI > deltaJ || deltaJ > deltaI) {
        routes.reduceLargest = true;
        routes.reduceBoth = true;
      } else {
        routes.reduceBoth = true;
      }
    } else if (deltaI < 0 && deltaJ > 0 || deltaI > 0 && deltaJ < 0) {
      routes.reduceNegative = true;
      routes.reducePositive = true;
    }

    routes.reduceBoth_dI = (deltaI > 0) ? -1 : 1;
    routes.reduceBoth_dJ = (deltaJ > 0) ? -1 : 1;

    if (abs(deltaI) > abs(deltaJ)) {
      routes.reduceLargest_dI = (deltaI > 0) ? -1 : 1;
    } else {
      routes.reduceLargest_dJ = (deltaJ > 0) ? -1 : 1;
    }

    if (deltaI < 0) {
      routes.reduceNegative_dI = 1;
    } else {
      routes.reduceNegative_dJ = 1;
    }

    if (deltaI > 0) {
      routes.reducePositive_dI = -1;
    } else {
      routes.reducePositive_dJ = -1;
    }

    return routes;
  }
};