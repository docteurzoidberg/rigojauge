
#include <iostream>
#define OLC_PGE_APPLICATION

using namespace std;

#include "../lib/olcPixelGameEngine.h"

#define SCREEN_W 500
#define SCREEN_H 240
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
    FillCircle(olc::vi2d(50, 25), 0.08f*SCREEN_W, olc::Pixel(255, 255, 255));
    
    
    std::vector<std::vector<std::pair<int, int>>> final_layers = {};


    for (size_t i = 0; i < landscape.layers.size(); ++i) {
      
      olc::Pixel colour = colour_palette[i];
      std::vector<std::pair<int, int>> layer = landscape.layers[i];
      std::vector<std::pair<int, int>> sampled_layer;

      for (size_t j = 0; j < layer.size() - 1; ++j) {
        
        sampled_layer.push_back(layer[j]);
        if (layer[j + 1].first - layer[j].first > 1) {
          double m = (double)(layer[j + 1].second - layer[j].second) / (layer[j + 1].first - layer[j].first);
          double n = layer[j].second - m * layer[j].first;
          auto r = [m, n](int x) { return m * x + n; };
          for (int k = layer[j].first + 1; k < layer[j + 1].first; ++k) {
            sampled_layer.push_back(std::make_pair(k, r(k)));
          }
        }
      }
      //DOUTE: faut il pas boucler et remplir un tableau 1d?
      sampled_layer.push_back(layer[layer.size() - 1]);
      final_layers.push_back(sampled_layer);
    }

    for (int i = 0; i < final_layers.size(); ++i) {
      auto& layer = final_layers[i];
      //console layer size
      std::cout << "Layer size: " << layer.size() << std::endl;
      for(int x = 0; x < layer.size(); ++x) {
        DrawLine(olc::vi2d{layer[x].first, landscape.height - layer[x].second}, olc::vi2d{layer[x].first, landscape.height}, colour_palette[i]);
      }
    } 

    /* from python:
    final_layers = []
    for layer in layers:
        sampled_layer = []
        for i in range(len(layer) - 1):
            sampled_layer += [layer[i]]
            # If x difference is greater than 1
            if layer[i + 1][0] - layer[i][0] > 1:
                # Linearly sample the y values in the range x_[i+1]-x_[i]
                # This is done by obtaining the equation of the straight
                # line (in the form of y=m*x+n) that connects two consecutive
                # points
                m = float(layer[i + 1][1] - layer[i][1]) / (
                    layer[i + 1][0] - layer[i][0]
                )
                n = layer[i][1] - m * layer[i][0]
                r = lambda x: m * x + n  # straight line
                for j in range(
                    int(layer[i][0] + 1), int(layer[i + 1][0])
                ):  # for all missing x
                    sampled_layer += [[j, r(j)]]  # Sample points
        final_layers += [sampled_layer]

    final_layers_enum = enumerate(final_layers)
    for final_layer in final_layers_enum:
        # traverse all x values in the layer
        for x in range(len(final_layer[1]) - 1):
            # for each x value draw a line from its y value to the bottom
            landscape_draw.line(
                (
                    final_layer[1][x][0],
                    height - final_layer[1][x][1],
                    final_layer[1][x][0],
                    height,
                ),
                colour_dict[str(final_layer[0])],
            )
    */
  }

  virtual bool OnUserCreate()
  {
    seed = time(nullptr);
    srand(static_cast<unsigned int>(seed));
    landscape.midpointDisplacement(std::make_pair(250, 0), std::make_pair(landscape.width, 200), 1.4, .04*landscape.height, 12);
    landscape.midpointDisplacement(std::make_pair(0, .70*landscape.height), std::make_pair(landscape.width, .64*landscape.height), 0.9, .50*landscape.height, 8); 
    landscape.midpointDisplacement(std::make_pair(0, .54*landscape.height), std::make_pair(landscape.width, .38*landscape.height), 1, .24*landscape.height, 9);
    landscape.midpointDisplacement(std::make_pair(0, .36*landscape.height), std::make_pair(landscape.width, .16*landscape.height), 1.2, .06*landscape.height, 12);
    return true;
  }

  virtual bool OnUserUpdate(float fElapsedTime)
  {
    Clear(colorPalette[colorPalette.size() - 1]);
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

