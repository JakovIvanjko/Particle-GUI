#ifndef SPAWNER_H
#define SPAWNER_H
#include <entity.hpp>
#include <entities/obstacle.hpp>
#include <scene.hpp>


class Spawner: public Entity{

public:

    double counter=3;
    double live=counter;

    Spawner();
    void process(float delta);
    




};




#endif