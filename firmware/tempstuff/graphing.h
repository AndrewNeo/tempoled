#ifndef __GRAPHING_H
#define __GRAPHING_H

#include "LinkedList.h"

struct StepNode {
    uint time;
    int value;
}

class Graphing {
    int stepCount;
    LinkedList<StepNode> list;

    public:
        Graphing(int steps);

        void add(int value);
        StepNode[][] asXY(int width, int height);
}

#endif