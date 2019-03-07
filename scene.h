#ifndef SCENE_H
#define SCENE_H

#include <memory>

namespace ou {

class Scene
{
    std::unique_ptr<class Planet> m_planet;
    std::unique_ptr<class Player> m_player;

    int m_windowWidth, m_windowHeight;

public:
    Scene();
    ~Scene();
    void render();

    int windowWidth() const;
    int windowHeight() const;

    const Player& player() const;
};

}

#endif // SCENE_H
