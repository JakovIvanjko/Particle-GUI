#include <entities/player.hpp>

Player::Player():
    particlesystems {},
    sys_open {0}
     {
    set_name("player");
}

void Player::process(float delta) {
    for (int i = 0; i < particlesystems.size(); i++) {
        particlesystems[i]->set_left(-1);
        particlesystems[i]->visible = i == sys_open;
    }
}