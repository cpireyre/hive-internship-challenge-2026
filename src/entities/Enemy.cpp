#include "Enemy.h"
#include "ResourceManager.h"
#include "Constants.h"
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

static uint32_t roll(int n);

bool Enemy::init()
{
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("enemy.png");
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    constexpr uint32_t size = 5;
    constexpr float height[size] = {-5, 0, 10, 70, 220};
    m_position.y = ZERO_Y - height[roll(size)];
    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(4.5f, 4.5f));
    m_collisionRadius = collisionRadius;
    m_velocity.x = 400.0f;
    m_acceleration.x = 0.f;

    return true;
}

void Enemy::update(float dt)
{
    m_position.x -= m_velocity.x * dt;
    m_velocity.x -= m_acceleration.x * dt;
}

void Enemy::update(float dt, bool boostEntitySpeed)
{
    constexpr float boost = 450;
    m_position.x -= dt * (m_velocity.x + boost * boostEntitySpeed);
    m_velocity.x -= m_acceleration.x * dt;
}

void Enemy::render(sf::RenderTarget& target) const
{
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}

static uint32_t roll(int n) {
    return rand() % n;
}
