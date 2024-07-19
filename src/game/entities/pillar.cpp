#include <entities/pillar.hpp>

Pillar::Pillar():
    sprite {Sprite("pillar.png")}
     {
    add_component(new TransformComponent(
        this, {300,150}
    ));

    auto area_comp = new AreaComponent(this, 32, 64);
    area_comp -> set_layer((int)AreaIndex::PILLAR, true);
    
    add_component(area_comp);
}

void Pillar::process(float delta) {
    auto *trans_comp = (TransformComponent *)get_component(ComponentType::TRANSFORM);
    sprite.update_transform(trans_comp);

    trans_comp -> velocity = {-100, 0};
    if (trans_comp -> position.x <= -50) {
        trans_comp -> position.x = 350;
    } 
}   