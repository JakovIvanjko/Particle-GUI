#include <entities/player.hpp>

Player::Player():
    particlesystem {ParticleSystem("jump.json")}
     {
    particlesystem.position = {260, 90};
    particlesystem.set_left(-1);
    set_name("player");
}

void Player::process(float delta) {}