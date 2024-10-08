#include <entity.hpp>

// <Entity>
Entity::Entity(std::string name): death_queued {false}, name {name} {}
Entity::Entity(): death_queued {false}, name {"Entity"} {}
Entity::~Entity() {
    for (auto& comp_pair: comps) {
        delete comp_pair.second;
    }
}

std::string Entity::get_name() {
    return name;
}

void Entity::set_name(std::string new_name) {
    name = new_name;
}

std::unordered_set<std::string>& Entity::get_groups() {
    return groups;
}


std::vector<Component*> Entity::get_components() {
    std::vector<Component*> comp_arr {};
    for (auto comp: comps) {
        comp_arr.push_back(comp.second);
    }
    return comp_arr;
}

void Entity::process(float delta) {}
void Entity::draw(float delta) {}

void Entity::process_components(float delta) {
    for (auto& comp_pair: comps) {
        comp_pair.second->process(delta);
    }
}

void Entity::draw_components(float delta) {

    for (auto& comp_pair: comps) {
        comp_pair.second->draw(delta);
    }
}

void Entity::queue_free() {
    death_queued = true;
}

bool Entity::is_death_queued() {
    return death_queued;
}

// Groups
void Entity::join_group(std::string group) {
    groups.insert(group);
}

void Entity::leave_group(std::string group) {
    groups.erase(group);
}

bool Entity::is_in_group(std::string group) {
    return groups.find(group) != groups.end();
}

// Components
void Entity::add_component(Component *comp) {
    comps[comp->type] = comp;
}

void Entity::remove_component(ComponentType type) {
    comps.erase(type);
}

bool Entity::has_component(ComponentType type) {
    return (comps.find(type) != comps.end());
}

Component *Entity::get_component(ComponentType type) {
    if (!has_component(type)) {
        std::cout << "WARNING: component not found, returning nullptr" << std::endl;
        return nullptr;
    }
    return comps[type];
}