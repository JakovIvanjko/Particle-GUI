#include <entities/player.hpp>

Player::Player():
    //sprite {Sprite("smiley.png")}
    particlesystem {ParticleSystem("jump.json")}
     {
    add_component(new TransformComponent(
        this, {100,100}
    ));

    //sprite.shader_bond.set_shader("test.glsl");

    auto area_comp = new AreaComponent(this, 32, 32);
    area_comp -> set_mask_bit((int)AreaIndex::PILLAR, true);
    area_comp -> area_entered.connect([this] (Entity* entity){
        die();
    });
    add_component(area_comp);
}

void Player::process(float delta) {
    auto *trans_comp = (TransformComponent *)get_component(ComponentType::TRANSFORM);
    //sprite.update_transform(trans_comp);

    //Vector2 input_dir = InputVectorNormalized("left","right","up","down");
    //trans_comp -> interpolate_velocity(
    //    Vector2Multiply(input_dir, {100,100}),
    //    20
    //);

    if (IsPressed("up")) {
        trans_comp -> accelerate({0, -150});
        
    }

    else {
        trans_comp -> accelerate({0, 100});
    }

    if (trans_comp -> position.y <= 0 || trans_comp -> position.y >= 300) {
        queue_free();
    }
}

void Player::die(){
    queue_free();
}