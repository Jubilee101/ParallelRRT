#include "rrt.h"

#define MAP "../map.pgm"

#define RESOLUTION 0.05
#define RADIUS 0.2

int main(int argc, char* argv[]) {
    Map map=Map(RESOLUTION);
    map.loadpgm(MAP);
    inflate_obstacles(map.pgm, RADIUS/RESOLUTION);
    RRT rrt=RRT(map);
    rrt.setStartPoint({8,10});
    //try
    rrt.setEndPoint({4,20});
    rrt.start_search();

    map.savepgm("output.pgm",rrt.tree);
}