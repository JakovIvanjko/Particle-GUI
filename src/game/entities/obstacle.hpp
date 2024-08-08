#ifndef OBSTACLE_H
#define OBSTACLE_H
#include <entity.hpp>
#include <sprites.hpp>
#include <transform_component.hpp>
#include <area_component.hpp>




class Obstacle: public Entity{

    public:
        
        
        Sprite sprite;
        


        Obstacle();
        void process(float delta);



};







#endif