#include "Scene.h"
#include "Scene/Default/DefaultScene.h"
#include "Scene/Delaunay/DelaunayScene.h"
#include "Scene/MCC/MCCScene.h"

std::shared_ptr<Scene> Scene::makeScene(const std::string& name) {
    if (name == "default") {
        return std::shared_ptr<DefaultScene>(new DefaultScene());
    }
    else if (name == "delaunay") {
        return std::shared_ptr<DelaunayScene>(new DelaunayScene());
    }
    else if (name == "MCC") {
        return std::shared_ptr<MinimumCircumscribedCircleScene>(new MinimumCircumscribedCircleScene());
    }
    return nullptr;
}
