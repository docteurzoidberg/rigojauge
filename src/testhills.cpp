
#define OLC_PGE_APPLICATION

using namespace std;

#include "../lib/olcPixelGameEngine.h"

#define SCREEN_W 1000
#define SCREEN_H 500
#define SCREEN_PIXELSIZE 1

#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>

class Landscape {

public:
  std::vector<std::vector<std::pair<int, int>>> layers;
  int width;
  int height;

public:
  Landscape(int width, int height) : width(width), height(height) {}

  void midpointDisplacement(std::pair<int, int> start, std::pair<int, int> end, double roughness, double vertical_displacement, int num_of_iterations) {
    std::vector<std::pair<int, int>> points;
    points.push_back(start);
    points.push_back(end);

    for (int iteration = 0; iteration < num_of_iterations; ++iteration) {
      std::vector<std::pair<int, int>> new_points;
      for (size_t i = 0; i < points.size() - 1; ++i) {
        double midpoint_x = (points[i].first + points[i + 1].first) / 2.0;
        double midpoint_y = (points[i].second + points[i + 1].second) / 2.0;
        double random_disp = (double)(rand() % 100) / 100.0 * 2.0 - 1.0;
        double disp = random_disp * vertical_displacement;
        midpoint_y += disp;
        new_points.push_back(points[i]);
        new_points.push_back(std::make_pair(midpoint_x, midpoint_y));
      }
      new_points.push_back(points.back());
      points = new_points;
      vertical_displacement *= pow(2.0, -roughness);
    }
    layers.push_back(points);
  }
};

class TestRenderer : public olc::PixelGameEngine
{
public:
  TestRenderer()
  {
    sAppName = "Hills";
  }

private:  
  Landscape landscape = Landscape(SCREEN_W, SCREEN_H);
  double seed = 0;
   // Define color palette
  std::vector<olc::Pixel> colorPalette = {
    olc::Pixel(195, 157, 224),
    olc::Pixel(158, 98, 204),
    olc::Pixel(130, 79, 138),
    olc::Pixel(68, 28, 99),
    olc::Pixel(49, 7, 82),
    olc::Pixel(23, 3, 38),
    olc::Pixel(240, 203, 163)
  };

  void drawLayers(std::vector<olc::Pixel> colour_palette) {
    // Draw the sun
    //DrawEllipse(50, 25, 100, 75, 255, 255, 255);
    DrawCircle(olc::vi2d(50, 25), 0.08f*SCREEN_W, olc::Pixel(255, 255, 255));
    for (size_t i = 0; i < landscape.layers.size(); ++i) {
      drawLayer(landscape.layers[i], colour_palette[i]);
    }
  }

  void drawLayer(std::vector<std::pair<int, int>>& layer, olc::Pixel color) {
    // Sample the y values of all x in image
    for (size_t i = 0; i < layer.size() - 1; ++i) {
      //DrawLine(layer[i].first, landscape.height - layer[i].second, layer[i + 1].first, landscape.height - layer[i + 1].second, color.r, color.g, color.b);
      DrawLine(olc::vi2d(layer[i].first, landscape.height - layer[i].second), olc::vi2d(layer[i + 1].first, landscape.height - layer[i + 1].second), color);
    }
  }
  
  virtual bool OnUserCreate()
  {
    seed = time(nullptr);
    srand(static_cast<unsigned int>(seed));
    landscape.midpointDisplacement(std::make_pair(250, 0), std::make_pair(landscape.width, 200), 1.4, 20, 12);
    landscape.midpointDisplacement(std::make_pair(0, 180), std::make_pair(landscape.width, 80), 1.2, 30, 12);
    landscape.midpointDisplacement(std::make_pair(0, 270), std::make_pair(landscape.width, 190), 1, 120, 9);
    landscape.midpointDisplacement(std::make_pair(0, 350), std::make_pair(landscape.width, 320), 0.9, 250, 8);
    return true;
  }

  virtual bool OnUserUpdate(float fElapsedTime)
  {
    Clear(olc::BLACK);
    drawLayers(colorPalette);
    return true;
  }
};

int main() {
    TestRenderer renderer;
    if (renderer.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
      renderer.Start();
    return 0;
}

