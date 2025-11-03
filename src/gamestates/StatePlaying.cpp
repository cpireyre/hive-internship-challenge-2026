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
    m_skyTextures[0] = ResourceManager::getOrLoadTexture("1.jpg"); // daylight
    if (m_skyTextures[0] == nullptr)
        return false;
    m_skyTextures[1] = ResourceManager::getOrLoadTexture("4.jpg"); // sunset
    if (m_skyTextures[1] == nullptr)
        return false;
    m_skyTextures[2] = ResourceManager::getOrLoadTexture("5.jpg"); // sunset2
    if (m_skyTextures[2] == nullptr)
        return false;
    m_skyTextures[3] = ResourceManager::getOrLoadTexture("2.jpg"); // evening
    if (m_skyTextures[3] == nullptr)
        return false;
    m_skyTextures[4] = ResourceManager::getOrLoadTexture("3.jpg"); // nighttime
    if (m_skyTextures[4] == nullptr)
        return false;
    m_pSprite = std::make_unique<sf::Sprite>(*m_skyTextures[0]);
    if (!m_pSprite)
        return false;
    m_ground.setSize({1024.0f, 356.f});
    m_ground.setPosition({0.0f, ZERO_Y});
    m_ground.setFillColor(sf::Color(0x5C794EFF));


    m_pPlayer = std::make_unique<Player>();
    if (!m_pPlayer || !m_pPlayer->init())
        return false;

    m_pPlayer->setPosition(sf::Vector2f(ZERO_X, ZERO_Y));

    const sf::Font* pFont = ResourceManager::getOrLoadFont("Lavigne.ttf");
    if (pFont == nullptr)
        return false;

    m_clockText = std::make_unique<sf::Text>(*pFont);
    if (!m_clockText)
        return false;
    m_helpText = std::make_unique<sf::Text>(*pFont);
    if (!m_helpText)
        return false;
    m_controlText = std::make_unique<sf::Text>(*pFont);
    if (!m_controlText)
        return false;
    m_powerupText = std::make_unique<sf::Text>(*pFont);
    if (!m_powerupText)
        return false;

    m_clockText->setString("12:00");
    m_clockText->setCharacterSize(90);
    m_clockText->setStyle(sf::Text::Bold);
    sf::FloatRect localBounds = m_clockText->getLocalBounds();
    m_clockText->setOrigin({localBounds.size.x / 120.0f - 10, localBounds.size.y / 120.0f - 10});

    m_helpText->setString("Survive until midnight!");
    m_helpText->setCharacterSize(50);
    m_helpText->setStyle(sf::Text::Bold);
    localBounds = m_helpText->getLocalBounds();
    m_helpText->setOrigin({localBounds.size.x / 2.f - 3000, localBounds.size.y / 1.0f - 200});

    m_controlText->setString("Jump on the red guys with space           Bounce off their heads with space");
    m_controlText->setCharacterSize(50);
    m_controlText->setStyle(sf::Text::Bold);
    localBounds = m_controlText->getLocalBounds();
    m_controlText->setOrigin({localBounds.size.x / 2.f - 1000, localBounds.size.y / 1.0f - 250});

    m_powerupText->setString("Collect yellow things to become invulnerable and go brr");
    m_powerupText->setCharacterSize(50);
    m_powerupText->setStyle(sf::Text::Bold);
    localBounds = m_powerupText->getLocalBounds();
    m_powerupText->setOrigin({localBounds.size.x / 2.f - 5000, localBounds.size.y / 1.0f - 250});

    return true;
}

void    moveText(sf::Text *p, float dt) {
    float currentX = p->getPosition().x;
    float currentY = p->getPosition().y;
    p->setPosition({currentX - 400 * dt, currentY});
}

#include <iostream>
void StatePlaying::update(float dt)
{
    char buffer[100];
    int score = elapsedTime;
    const int hour = 12 + score / 60, minute = score % 60;
    snprintf(buffer, sizeof(buffer), "%02d:%02d",
            hour,
            minute
            );
    m_clockText->setString(buffer);

    if (hour >= 16 && timeOfDayIota == 0) {
        timeOfDayIota++;
        m_pSprite->setTexture(*m_skyTextures[timeOfDayIota]);
        m_ground.setFillColor(sf::Color(0x795D4EFF));
    }
    if (hour >= 18 && timeOfDayIota == 1) {
        timeOfDayIota++;
        m_pSprite->setTexture(*m_skyTextures[timeOfDayIota]);
        m_ground.setFillColor(sf::Color(0x453D3FFF));
    }
    if (hour >= 20 && timeOfDayIota == 2) {
        timeOfDayIota++;
        m_pSprite->setTexture(*m_skyTextures[timeOfDayIota]);
        m_ground.setFillColor(sf::Color(0x2E343AFF));
    }
    if (hour >= 22 && timeOfDayIota == 3) {
        timeOfDayIota++;
        m_pSprite->setTexture(*m_skyTextures[timeOfDayIota]);
        m_ground.setFillColor(sf::Color(0x141C1AFF));
    }
    elapsedTime += 10 * dt;
    if (elapsedTime > 10) {
        moveText(m_helpText.get(), dt);
        moveText(m_controlText.get(), dt);
        moveText(m_powerupText.get(), dt);
    }
    m_timeUntilEnemySpawn -= dt;
    constexpr uint32_t size = 4;
    constexpr float intervals[size] = {0.f, -.5f, 1.f, .25f};

    if (m_timeUntilEnemySpawn < 0.0f)
    {
        m_timeUntilEnemySpawn = enemySpawnInterval + intervals[roll(size)];
        if (boostEntitySpeed) m_timeUntilEnemySpawn /= 2;
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
        pPowerup->update(dt, boostEntitySpeed);
    }
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        pEnemy->update(dt, boostEntitySpeed);
    }
    boostTimer -= dt;
    if (boostTimer <= 0) {
        boostEntitySpeed = false;
        boostTimer = 0;
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

        // Should use getCollisionRadius maybe
        float hitbox = 20;
        if (distance <= hitbox && c.y < pos.y) {
            pEnemy->active = false;
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
        else if (boostTimer <= 0 && distance <= hitbox)
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
        if (pPowerup->active && distance <= hitbox) {
            pPowerup->active = false;
            boostEntitySpeed = true;
            boostTimer = 5;
        }
    }
    // End Playing State on player death
    if (playerDied)
        m_stateStack.popDeferred();
}

void StatePlaying::render(sf::RenderTarget& target) const
{
    target.draw(*m_pSprite);
    target.draw(m_ground);
    target.draw(*m_clockText);
    target.draw(*m_helpText);
    target.draw(*m_controlText);
    target.draw(*m_powerupText);
    for (const std::unique_ptr<Powerup>& pPowerup : m_powerups)
        pPowerup->render(target);
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
        pEnemy->render(target);
    m_pPlayer->render(target);
}

static uint32_t roll(int n) {
    return rand() % n;
}
