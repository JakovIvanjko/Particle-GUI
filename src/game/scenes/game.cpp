#include <scenes/game.hpp>

GameScene::GameScene(): Scene("game_scene") {}

void GameScene::restart() {
    add_entity(new Player());
    add_entity(new Tilemap({16,16},"particle_test.png"));
}