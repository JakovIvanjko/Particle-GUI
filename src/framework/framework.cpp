#include <framework.hpp>

RenderTexture2D ui_layer, game_layer, composition_layer;

ShaderPtr post_processing_ptr; 
TexturePtr noise_texture,
           paper_texture;
        
float background_color[4] = {0, 0, 0, 1};

float desired_zoom = 1;
Vector2 last_mouse = {0, 0};

clock_t frame_timer;
unsigned long frame_time;

bool unlimited_framerate = false;
bool debug_ui = false;
bool particle_ui = true;


void Framework::init(std::string title, Vector2 resolution, int window_scale, bool _debug_window) {
    srand(time(NULL));
    res = resolution;
    debug_ui = _debug_window;

    // Init
    InitWindow(res.x * window_scale, res.y * window_scale, (title).c_str());
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);

    frame_timer = clock();

    BeginDrawing();
    rlImGuiSetup(true);
    rlImGuiBegin();
    ImGui::Begin("Editor GUI");
    ImGui::SetWindowFontScale(2);
    ImGui::End();
    rlImGuiEnd();
    EndDrawing();

    ui_layer   = LoadRenderTexture(res.x, res.y);
    game_layer = LoadRenderTexture(res.x, res.y);
    composition_layer = LoadRenderTexture(res.x, res.y);
    SetTargetFPS(60);

    InitAudioDevice();

    // Default camera
    Camera2D* blank_camera = new Camera2D();
    blank_camera->target = {0, 0};
    blank_camera->offset = {260, 90};
    blank_camera->rotation = 0;
    blank_camera->zoom = 1;

    CameraManager::bind_camera(blank_camera);

    // Init modules
    Easing::InitEasingFuncs();
    CameraManager::init();
    ColliderManager::init();
    AreaManager::init();

    // Loading post processing shader and it's textures
    post_processing_ptr = ShaderManager::get("post_processing.glsl"); 
    noise_texture = TextureManager::get("post_processing/noise.png");
    paper_texture = TextureManager::get("post_processing/paper.png");
}

void Framework::debug_gui() {
    rlImGuiBegin();
    ImVec4 performance_color = frame_time > (1.0/60.0 * 1000) ?
        ImVec4(1, 0, 0, 1) : ImVec4(0, 1, 0, 1);

    ImGui::TextColored(
        performance_color,
        ("FPS: " + std::to_string(GetFPS())).c_str()
    );
    ImGui::TextColored(
        performance_color,
        ("Frame time: " + std::to_string(frame_time) + "ms/16.66ms").c_str()
    );

    bool before = unlimited_framerate;
    ImGui::Checkbox("Unlimited framerate", &unlimited_framerate);
    if (before != unlimited_framerate)
        SetTargetFPS(unlimited_framerate ? 0 : 60);

    ImGui::Text(("Entity count: " + std::to_string(
        SceneManager::scene_on->entity_count()
    )).c_str());
    ImGui::Text(("Component count: " + std::to_string(
        ComponentManager::component_count()
    )).c_str());

    if (ImGui::CollapsingHeader("Entities:")) {
        ImGui::Indent(25.f);
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.7f, 1.0f, 0.15f));

        int i = 0;
        for (Entity* entity: SceneManager::scene_on->get_entities()) {
            i++;
            std::string ent_name = entity->get_name().c_str();

            if (ImGui::CollapsingHeader(ent_name.c_str())) {
                ImGui::Indent(25.f);
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.7f, 1.0f, 0.1f));

                if (ImGui::CollapsingHeader(("Components##" + std::to_string(i)).c_str())) {
                    ImGui::Indent(25.f);
                    for (auto component: entity->get_components()) {
                        component->draw_gui_info();
                    }
                    ImGui::Unindent(25.f);
                }
                if (ImGui::CollapsingHeader(("Groups##" + std::to_string(i)).c_str())) {
                    ImGui::Indent(25.f);
                    for (auto group: entity->get_groups()) {
                        ImGui::Text(group.c_str());
                    }
                    ImGui::Unindent(25.f);
                }
                ImGui::Unindent(25.f);
                ImGui::PopStyleColor();
            }
        }
        ImGui::Unindent(25.f);
        ImGui::PopStyleColor();
    }

    ImGui::Checkbox("Draw areas", &DRAW_AREAS);
    ImGui::Checkbox("Draw colliders", &DRAW_COLLIDERS);

    ImGui::ColorEdit4("Background color", background_color);
    rlImGuiEnd();
}


void Framework::process_modules(float delta) {
        // Processing data managers
    TextureManager::unload_check();
    ParticleDataManager::unload_check();
    
    AudioManager::unload_check();
    AudioManager::process_track_fade(delta);

    ShaderManager::unload_check();
    ShaderManager::update_uniforms();

    ColliderManager::reload_colliders();
    AreaManager::reload_areas();

    // Hot reloading
    if (TryingToHotReload()) {
        TextureManager::reload();
        ShaderManager::reload();
        AudioManager::reload();
        ParticleDataManager::reload();
    }
}

void Framework::process_scene(float delta) {
    SceneManager::scene_on->process(delta);
    SceneManager::scene_on->process_entities(delta);
}

void Framework::draw_game_layer(float delta) {
    // Game layer
    BeginTextureMode(game_layer);
    BeginMode2D(*global_camera);
    ClearBackground(Float4ToColor(background_color));

    SceneManager::scene_on->draw_entities(delta);
    DrawableManager::draw();
    
    ColliderManager::draw_debug();
    AreaManager::draw_debug();

    EndMode2D();        
    EndTextureMode();
}

void Framework::draw_ui_layer(float delta) {
    // Ui layer
    BeginTextureMode(ui_layer);
    ClearBackground({0, 0, 0, 0});

    DrawableManager::draw_ui();
    EndTextureMode();
}

std::vector<std::string> open_files = {"jump.json", "test.json"};

void Framework::run() {
    while (!WindowShouldClose()) {
        // Delta time calc
        float delta = fminf(GetFrameTime(), 1/20.f);

        // Resizing window
        if (IsWindowResized()) {
            SetWindowSize(GetScreenWidth(), GetScreenHeight());
        }

        // Processing
        process_modules(delta);
        process_scene(delta);

        auto cam = CameraManager::get_camera();

        cam->zoom = Lerpi(cam->zoom, desired_zoom, 20);

        Vector2 mouse_diff = Vector2Subtract(mouse_screen_pos(), last_mouse);

        last_mouse = mouse_screen_pos();
        cam->target = Vector2Subtract(cam->target, Vector2Scale(mouse_diff, (1.f/desired_zoom) * (int)(IsPressed("shoot") && mouse_screen_pos().x > 160)));

        desired_zoom += GetMouseWheelMove() * 0.2f * desired_zoom * (int)(mouse_screen_pos().x > 160);

        // Drawing start 
        frame_timer = clock();
        BeginDrawing();
        
        Framework::draw_game_layer(delta);
        Framework::draw_ui_layer(delta);
        // Compose UI and game layer
        BeginTextureMode(composition_layer);
        DrawTexturePro(game_layer.texture,
            {0, 0, res.x, res.y},
            {0, 0, res.x, res.y},
            {0, 0},
            0,
            WHITE
        );
        DrawTexturePro(ui_layer.texture,
            {0, 0, res.x, res.y},
            {0, 0, res.x, res.y},
            {0, 0},
            0,
            WHITE
        );
        EndTextureMode();

        // Post processing uniforms
        Shader post_processing = *post_processing_ptr.get();
        BeginShaderMode(post_processing);

        float timer = GetTime();
        float aspect_ratio = res.x / res.y;

        SetShaderValue(post_processing, GetShaderLocation(post_processing, "time"), &timer, SHADER_UNIFORM_FLOAT);
        SetShaderValue(post_processing, GetShaderLocation(post_processing, "aspect_ratio"), &aspect_ratio, SHADER_UNIFORM_FLOAT);
        
        // Post processing texture binding
        SetShaderValueTexture(post_processing, GetShaderLocation(post_processing, "noise_texture"), *noise_texture.get());
        SetShaderValueTexture(post_processing, GetShaderLocation(post_processing, "paper_texture"), *paper_texture.get());

        Vector2 camera_offset = Vector2Divide(global_camera->target, res);
        SetShaderValue(
            post_processing,
            GetShaderLocation(post_processing, "camera_offset"),
            &camera_offset,
            SHADER_ATTRIB_VEC2
        );

        // Drawing composition layer on display
        float draw_width  = GetScreenWidth();
        float draw_height = GetScreenWidth() * (res.y / res.x);

        if (draw_height > GetScreenHeight()) {
            draw_height = GetScreenHeight();
            draw_width  = GetScreenHeight() * (res.x / res.y);
        }

        ClearBackground(BLACK);
        DrawTexturePro(composition_layer.texture,
            {0, 0, res.x, res.y},
            {(GetScreenWidth() - draw_width) * .5f, (GetScreenHeight() - draw_height) * .5f, draw_width, draw_height},
            {0, 0},
            0,
            Color{255, 255, 255}
        );
        EndShaderMode();

        /*if (debug_ui) {
            Framework::debug_gui();
        }*/

        if(particle_ui){
            rlImGuiBegin();
            ImGui::Begin("Editor GUI");

            Player* player = (Player*)SceneManager::scene_on->get_entity("player");

            ImGui::BeginTabBar("FileTab");
            ImGui::SameLine();
            ImGui::BeginChild(
                "##FileTabScroll",
                ImVec2(0, ImGui::GetFrameHeightWithSpacing() + 10),
                false,
                ImGuiWindowFlags_HorizontalScrollbar
            );

            for (int i = 0; i < player->particlesystems.size(); i++) {
                ImGui::SameLine();

                std::string filepath = open_files[i].c_str();
                size_t pos = filepath.find_last_of("/\\");
                std::string filename = (pos == std::string::npos) ? filepath : filepath.substr(pos + 1);

                bool highlight = i == player->sys_open;
                if (highlight)
                    ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.5, 1, 1});

                if (ImGui::Button(filename.c_str())) {
                    player->sys_open = i;
                }
                if (highlight)
                    ImGui::PopStyleColor();
            }
            ImGui::SameLine();
            if (ImGui::Button(" - ")) {
                delete player->particlesystems[player->sys_open];

                player->particlesystems.erase(
                    player->particlesystems.begin() + player->sys_open
                );
                open_files.erase(open_files.begin() + player->sys_open);
            }

            ImGui::SameLine();
            if (ImGui::Button(" + ")) {
                std::string filename = choose_file();

                if (filename != "") {
                    open_files.push_back(filename);
                    player->particlesystems.push_back(new ParticleSystem(filename));
                }
            }
            ImGui::EndChild();
            ImGui::EndTabBar();

            ImGui::Dummy({0, 5});

            player->particlesystems[player->sys_open]->particle_gui();

            ImGui::End();
            rlImGuiEnd();
        };

        clock_t new_frame_timer = clock();
        frame_time  = new_frame_timer - frame_timer;
        EndDrawing();

    }
    deinit();
}

void Framework::deinit() {
    // Unload remaining assets
    TextureManager::unload_all();
    ParticleDataManager::unload_all();
    ShaderManager::unload_all();
    SceneManager::unload_all();
    AudioManager::unload_all();

    rlImGuiShutdown();
    CloseWindow();
}