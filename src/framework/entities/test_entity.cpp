#include <test_entity.hpp>

TestEntity::TestEntity():
    sprite {Sprite("test.png")},
    particle_sys {ParticleSystem("test.json")}
{
    trail_vfx = Trail({0, 0}, 40, 24, BLUE, {255, 0, 0, 0});

    add_component(
        new TransformComponent(this)
    );
    add_component(
        new HealthComponent(this, 10)
    );
    CameraComponent *camera_comp = new CameraComponent(this);
    add_component(
        camera_comp
    );

    CameraManager::bind_camera(camera_comp->get_camera());
}

void TestEntity::process(float delta) {
    particle_sys.process(delta);
    trail_vfx.process(delta);

    TransformComponent *transform_comp = (TransformComponent*)get_component(CompType::TRANSFORM);
    transform_comp->interpolate_velocity(
        Vector2Multiply({
            float(IsKeyDown(KEY_D)) - float(IsKeyDown(KEY_A)),
            float(IsKeyDown(KEY_S)) - float(IsKeyDown(KEY_W))
        }, {700, 700}
    ), 15);

    sprite.set_position(transform_comp->position);
    particle_sys.set_position(transform_comp->position);
    trail_vfx.set_position(transform_comp->position);

    sprite.set_scale({
        1.f + (float)sin(GetTime()*PI) * 0.5f,
        1.f + (float)cos(GetTime()*PI) * 0.5f
    });
    sprite.angle += delta * 180;

    if (IsKeyPressed(KEY_SPACE)) {
        CameraComponent *camera = (CameraComponent*)get_component(CompType::CAMERA);
        camera->shake(64, 0.25);
        std::cout << "Shaking!!!" << std::endl;
    }
}

void TestEntity::draw(float delta) {
    trail_vfx.draw();
    particle_sys.draw();
    sprite.draw();
}