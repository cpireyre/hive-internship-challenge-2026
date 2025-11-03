#include "StatePlaying.h"
#include "StatePaused.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <memory>
#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>

static uint32_t roll(int n);

StatePlaying::StatePlaying(StateStack& stateStack)
    : m_stateStack(stateStack)
{
}

bool StatePlaying::init()
{
    m_ground.setSize({1024.0f, 1024/3.});
    m_ground.setPosition({0.0f, ZERO_Y});
    m_ground.setFillColor(sf::Color::Green);

    m_pPlayer = std::make_unique<Player>();
    if (!m_pPlayer || !m_pPlayer->init())
        return false;

    m_pPlayer->setPosition(sf::Vector2f(ZERO_X, ZERO_Y));

    return true;
}

#include <iostream>
void StatePlaying::update(float dt)
{
    m_timeUntilEnemySpawn -= dt;
    constexpr uint32_t size = 4;
    constexpr float intervals[size] = {0.f, -.5f, 1.f, .25f};

    if (m_timeUntilEnemySpawn < 0.0f)
    {
        m_timeUntilEnemySpawn = enemySpawnInterval + intervals[roll(size)];
        std::unique_ptr<Enemy> pEnemy = std::make_unique<Enemy>();
        pEnemy->setPosition(sf::Vector2f(1000, ZERO_Y));
        if (pEnemy->init())
            m_enemies.push_back(std::move(pEnemy));
    }

    constexpr float powerupIntervalOffset[size] = {0.f, -.0f, 0.f, .0};
    m_timeUntilPowerupSpawn -= dt;
    if (m_timeUntilPowerupSpawn < 0.0f)
    {
        m_timeUntilPowerupSpawn = powerupSpawnInterval + powerupIntervalOffset[roll(size)];
        std::unique_ptr<Powerup> pPowerup = std::make_unique<Powerup>();
        pPowerup->setPosition(sf::Vector2f(1000, ZERO_Y));
        if (pPowerup->init())
        {
            m_powerups.push_back(std::move(pPowerup));
        }
    }

    bool isPauseKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
    m_hasPauseKeyBeenReleased |= !isPauseKeyPressed;
    if (m_hasPauseKeyBeenReleased && isPauseKeyPressed)
    {
        m_hasPauseKeyBeenReleased = false;
        m_stateStack.push<StatePaused>();
    }

    m_pPlayer->update(dt);

    for (const std::unique_ptr<Powerup>& pPowerup : m_powerups)
    {
        pPowerup->update(dt);
    }
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        pEnemy->update(dt);
    }

    // Detect collisions
    // Ref: https://www.jeffreythompson.org/collision-detection/circle-rect.php
    bool playerDied = false;
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        const sf::Vector2f c = m_pPlayer->getPosition();
        sf::Vector2 pos = pEnemy->m_pSprite->getGlobalBounds().position;
        sf::Vector2 size = pEnemy->m_pSprite->getGlobalBounds().size;
        float testX = c.x;
        float testY = c.y;
        if (c.x < pos.x) testX = pos.x;
        else if (c.x > pos.x + size.x) testX = pos.x + size.x;
        if (c.y < pos.y) testY = pos.y;
        else if (c.y > pos.y + size.y) testY = pos.y + size.y;
        float distX = std::pow(c.x - testX, 2);
        float distY = std::pow(c.y - testY, 2);
        float distance = sqrt(distX + distY);

        // Should use getCollisionRadius or something
        float hitbox = 20;
        if (distance <= hitbox && c.y < pos.y) {
            m_pPlayer->m_acceleration.y = 4000.f;
            m_pPlayer->m_velocity.y = 600.f;
            bool isSpaceKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
            if (isSpaceKeyPressed) {
                constexpr float gravity = 4000.f;
                constexpr float velocity = 1300.f;
                m_pPlayer->m_acceleration.y = gravity;
                m_pPlayer->m_velocity.y = velocity;
            }
        }
        else if (distance <= hitbox)
        {
            playerDied = true;
            break;
        }
    }

    for (const std::unique_ptr<Powerup>& pPowerup : m_powerups)
    {
        const sf::Vector2f c = m_pPlayer->getPosition();
        sf::Vector2 pos = pPowerup->m_pSprite->getGlobalBounds().position;
        sf::Vector2 size = pPowerup->m_pSprite->getGlobalBounds().size;
        float testX = c.x;
        float testY = c.y;
        if (c.x < pos.x) testX = pos.x;
        else if (c.x > pos.x + size.x) testX = pos.x + size.x;
        if (c.y < pos.y) testY = pos.y;
        else if (c.y > pos.y + size.y) testY = pos.y + size.y;
        float distX = std::pow(c.x - testX, 2);
        float distY = std::pow(c.y - testY, 2);
        float distance = sqrt(distX + distY);

        // Should use getCollisionRadius or something
        float hitbox = 25;
        if (distance <= hitbox) {
            pPowerup->active = false;
        }
    }
    // End Playing State on player death
    if (playerDied)
        m_stateStack.popDeferred();
}

void StatePlaying::render(sf::RenderTarget& target) const
{
    target.draw(m_ground);
    for (const std::unique_ptr<Powerup>& pPowerup : m_powerups)
        pPowerup->render(target);
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
        pEnemy->render(target);
    m_pPlayer->render(target);
}

static uint32_t roll(int n) {
    return rand() % n;
}
