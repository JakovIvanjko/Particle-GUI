#ifndef PLAYER_H
#define PLAYER_H
#include <entity.hpp>
#include <sprites.hpp>
#include <area_component.hpp>
#include <transform_component.hpp>
#include <input.hpp>
#include <particles.hpp>

class Player: public Entity {
public:
    ParticleSystem particlesystem;

    Player();
    void process(float delta);
};

#endif