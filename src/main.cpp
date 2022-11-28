#include "rrt.h"
#include <iostream>
#include <cstring>
#include <time.h>
#include <fstream>
#define MAP "./map.pgm"
// #define IS_OBSTACLE(pgm,i,j) (i<0 || j<0 || i>=pgm->height || j>=pgm->width || pgm->raster[(i)*pgm->width+(j)]<250)
//[8,10]==>[15,3]
#define OFFICE "./office.pgm"
//[8,10]==>[34.5,14]

#define RESOLUTION 0.05
#define RADIUS 0.2
bool is_obstacle(PGM* pgm, int i, int j) {
    // printf("height: %d, width: %d\n", pgm->height, pgm->width);
    // printf("i: %d, j: %d, land on: %d\n", i, j, pgm->raster[(i)*pgm->width+(j)]);
    return i<0 || j<0 || i>=pgm->height || j>=pgm->width || pgm->raster[(i)*pgm->width+(j)]<250;
}
int main(int argc, char* argv[]) {
    int selection=0;
    int iteration=0;
    int threadNum=0;;

    if (argc==1) {
		printf("\nFormat: %s [a] [r] [c]\n",argv[0]);
		printf("\n[a] is the algorithm index\n");
		printf("\t1 = serialized RRT\n");
        printf("\t2 = parallel RRT\n");
		printf("\n[r] iteration(only used in bash)\n");
        printf("\n[c] is the threads number (only used in bash)\n");
		return 0;
	}
		
	if (argc>1) {
		selection = atoi(argv[1])-1;	
	}
	
	if (argc>2) {
	    iteration = atoi(argv[2]);
	}

	if (argc>3) {
		threadNum = atoi(argv[3]);
	}
    Config config=Config(selection,iteration,threadNum);
    int seed = time(NULL);
    srand(seed);

    Map map=Map(RESOLUTION);

    map.loadpgm(MAP);
    PGM *pgm=load_pgm(MAP);

    Timer timer=Timer();
    Timer inflatetimer=Timer();

    timer.start();
    inflatetimer.start();
    // std::cout<<"load map successfully!"<<std::endl;
    inflate_obstacles(map.pgm, RADIUS/RESOLUTION);
    inflatetimer.end();
    double end_x = 15;
    double end_y = 3;
    // if (is_obstacle(map.pgm,end_x/RESOLUTION,end_y/RESOLUTION)) {
    //     printf("End point lands on obstacle, exit now\n");
    //     return -1;
    // }
    inflatetimer.print_total_duration("inflate obstacles");
    inflatetimer.writeTofile("inflate obstacles",config);
    // std::cout<<"inflate map successfully!"<<std::endl;
    nodeList tree=nodeList();
    RRT rrt=RRT(map,tree);
    // std::cout<<" start rrt successfully!"<<std::endl;

    rrt.setStartPoint({8,10});
    //try
    rrt.setEndPoint({end_x,end_y});
    // std::cout<<" end set point!"<<std::endl;

    // std::cout<<" start search!"<<std::endl;

    rrt.start_search(config);
    // clock_gettime(CLOCK_MONOTONIC, &cur_timer);
    // uint64_t end_time = getTime(&cur_timer);
    // uint64_t duration = end_time - start_time;
    // printf("Time spent: %ld ms\n", duration);
    timer.end();
    timer.print_duration("total");
    timer.writeTofile("total",config);
    // std::cout<<" end rrt start search!"<<std::endl;
    std::ofstream myfile;
    myfile.open ("summary.csv",ios::app);
    myfile<<threadNum<<","<<selection<<","<<iteration<<","<<timer.get_duration()<<std::endl;
    myfile.close();

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