#ifndef VISUALIZER_H
#define VISUALIZER_H

#ifndef LTSGRAPH_H
  #include "gltsgraph.h"
#else
  class GLTSGraph;
#endif
#include "state.h"
#include "mcrl2/utilities/font_renderer.h"
class Visualizer
{
  public:
    Visualizer(GLTSGraph* app);
    ~Visualizer();
    
    void visualize(double width, double height, double pixelSize, 
                   bool inSelectMode);

    void initFontRenderer();

    // Setters
    void setWidth(double width); 
    void setHeight(double height); 
    void setPixelSize(double size);
    // Getters
  private:
    GLTSGraph* owner;
    double pixelSize; // Pixel size in world coordinates.
    double width; // Canvas width
    double height; // Canvas height


    void drawStates(bool inSelectMode);
    // Draws a single state s
    void drawState(State* s);

    // Draws a single transition, from its in to its out transition
    void drawTransition(Transition* t, size_t trid);

    // Draws an arrow head
    void drawArrowHead(double baseLength);


    // Draws a self-loop, a transition from a state to itself
    void drawSelfLoop(Transition* t, size_t trid);

    mcrl2::utilities::font_renderer* fr;
};

#endif //VISUALIZER_/H
