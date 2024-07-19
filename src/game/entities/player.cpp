#include <entities/player.hpp>

Player::Player():
    particlesystem {ParticleSystem("jump.json")}
     {
    particlesystem.position = {260, 90};
    set_name("player");
}

void Player::process(float delta) {}