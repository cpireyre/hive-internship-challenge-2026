#pragma once
#include <cmath>
#include "Constants.h"
#include "IState.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "entities/Powerup.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class StatePlaying : public IState
{
public:
    StatePlaying(StateStack& stateStack);
    ~StatePlaying() = default;

    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;
    std::unique_ptr<sf::Text> m_clockText;
    std::unique_ptr<sf::Text> m_helpText;
    std::unique_ptr<sf::Text> m_controlText;
    const sf::Texture* m_skyTextures[5];
    std::unique_ptr<sf::Sprite> m_pSprite;

private:
    static constexpr const float enemySpawnInterval = 1.0f;
    float m_timeUntilEnemySpawn = enemySpawnInterval;
    static constexpr const float powerupSpawnInterval = 12.f;
    float m_timeUntilPowerupSpawn = powerupSpawnInterval;

    StateStack& m_stateStack;
    std::unique_ptr<Player> m_pPlayer;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Powerup>> m_powerups;
    sf::RectangleShape m_ground;
    sf::RectangleShape m_sky;
    bool m_hasPauseKeyBeenReleased = true;
    bool boostEntitySpeed = false;
    float boostTimer = 0;
    float elapsedTime = 0;
    int timeOfDayIota = 0;

    void updateCollisions();
};
