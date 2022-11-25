#include "rrt.h"
#include <iostream>
#include <cstring>

#define MAP "./map.pgm"

#define RESOLUTION 0.05
#define RADIUS 0.2

int main(int argc, char* argv[]) {
    int seed = time(NULL);
    srand(seed);

    Map map=Map(RESOLUTION);
    map.loadpgm(MAP);
    PGM *pgm=load_pgm(MAP);
    // std::cout<<"load map successfully!"<<std::endl;
    inflate_obstacles(map.pgm, RADIUS/RESOLUTION);
    // std::cout<<"inflate map successfully!"<<std::endl;
    nodeList tree=nodeList();
    RRT rrt=RRT(map,tree);
    // std::cout<<" start rrt successfully!"<<std::endl;

    rrt.setStartPoint({8,10});
    //try
    rrt.setEndPoint({15,3});
    // std::cout<<" end set point!"<<std::endl;

    std::cout<<" start search!"<<std::endl;

    rrt.start_search();
    
    std::cout<<" end rrt start search!"<<std::endl;


    // std::cout<<"start creating the tree!"<<std::endl;

    
    for(int i=1;i<rrt.tree->getSize();i++){
        int currX = (int)(rrt.tree->coordinates[i].first / RESOLUTION);
        int currY = (int)(rrt.tree->coordinates[i].second / RESOLUTION);
        int parentX=(int)(rrt.tree->parents[i].first / RESOLUTION);
        int parentY=(int)(rrt.tree->parents[i].second/ RESOLUTION);

        draw_line(pgm,currX,currY,parentX,parentY,0);
    }
    if(pgm!=nullptr){
        strcpy(pgm->file,"ouput.pgm");
        // std::cout<<" start saving!"<<std::endl;

        save_pgm(pgm);
        // std::cout<<" end saving search!"<<std::endl;
    }

}