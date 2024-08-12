#include <entities/obstacle.hpp>

Obstacle::Obstacle():
    sprite {Sprite("Trihorn.png")}
    
    

    {
        
        
        add_component(new TransformComponent(
            this, {res.x,(res.y*RandF())}

        ));


        auto area_comp=new AreaComponent(this,32,32);
        add_component(area_comp);
        area_comp->set_layer((int)AreaIndex::OBSTACLE,true);
        
    }

    void Obstacle::process(float delta){
        auto *trans_comp = (TransformComponent*)get_component(ComponentType::TRANSFORM);
        sprite.update_transform(trans_comp);

        trans_comp -> velocity=Vector2Scale({-1,0},20);
    }

    