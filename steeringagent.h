#ifndef GENSEPERATIONAGENT_H_INCLUDED
#define GENSEPERATIONAGENT_H_INCLUDED

#include <vector>

struct Point
{
    float x;
    float y;

    void Normalize(int n);
    Point();
    Point(float ix, float iy): x(ix), y(iy) {}

};

struct SteeringAgent
{
    bool operator!=(SteeringAgent const &rhs) const noexcept
    {
        return lhs != rhs.lhs;
    }
    int lhs;

    float x3;
    float y3;

    Point velocity;

    int DistanceFromAgent(SteeringAgent a);
    void Normalize(int n);

};


#endif // GENSEPERATIONAGENT_H_INCLUDED
