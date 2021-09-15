#ifndef REPLAY_MANAGER_H
#define REPLAY_MANAGER_H

#include "Base.h"

class ReplayManager{
public:
    static ReplayManager Instance();
    void clear();
    std::vector<Point3d> getPoints();
    void add(Point3d position);
private:
    std::vector<Point3d> points;
};

#endif
