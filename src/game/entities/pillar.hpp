#ifndef PILLAR_H
#define PILLAR_H
#include <entity.hpp>
#include <sprites.hpp>
#include <area_component.hpp>
#include <transform_component.hpp>


class Pillar: public Entity {
public:
    Sprite sprite;
    

    Pillar();

    void process(float delta);
};

#endif