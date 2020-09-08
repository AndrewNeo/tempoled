#include "graphing.h"

Graphing::Graphing(int steps) {
    this->stepCount = steps;
    this->list = new LinkedList();
}

Graphing::add(int value) {
    this->list.add(step);
    
    if (this->list.size() >= this->stepCount) {
        this->list.shift();
    }
}

Graphing::asXY(int width, int height) {
    int[] points = new int[this->stepCount];

    for (int i = 0; i < this->list.size(); i++) {
        StepNode node = this->list.get(i);
    }

    return points;
}
