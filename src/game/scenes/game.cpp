#include <scenes/game.hpp>

GameScene::GameScene(): Scene("game_scene") {}

void GameScene::restart() {
    add_entity(new Player());
}