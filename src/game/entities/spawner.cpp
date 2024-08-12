#include <entities/spawner.hpp>


Spawner::Spawner():

    counter{3}

    {}

void Spawner::process(float delta){
    if (live<=0){
       SceneManager::scene_on->add_entity(new Obstacle());

        live=counter;


    }
    live-=delta;

}

