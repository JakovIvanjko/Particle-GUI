#include <entities/player.hpp>

Player::Player():
    particlesystems {new ParticleSystem("assets/particles/jump.json"), new ParticleSystem("assets/particles/test.json")},
    sys_open {0}
     {

    for (auto& particlesystem: particlesystems) {
        particlesystem->position = {260, 90};
        particlesystem->set_left(-1);
        particlesystem->visible = false;
    }
    set_name("player");
}

void Player::process(float delta) {
    for (int i = 0; i < particlesystems.size(); i++) {
        particlesystems[i]->position = {260, 90};
        particlesystems[i]->set_left(-1);
        particlesystems[i]->visible = i == sys_open;
    }
}