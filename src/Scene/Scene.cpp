#include "Scene.h"
#include "Scene/Default/DefaultScene.h"

std::shared_ptr<Scene> Scene::makeScene(const std::string& name) {
    return std::shared_ptr<DefaultScene>(new DefaultScene());
}
