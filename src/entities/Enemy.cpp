#include "Enemy.h"
#include "ResourceManager.h"
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

bool Enemy::init()
{
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("enemy.png");
    constexpr float height[3] = {0, 100, 200};
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(4.5f, 4.5f));
    m_collisionRadius = collisionRadius;
    m_velocity.x = 400.0f;
    m_position.y = 800 - (height[rand() % 3]);
    m_acceleration.x = 0.f;

    return true;
}

void Enemy::update(float dt)
{
    if (m_velocity.x < 1000)
        m_velocity.x += m_acceleration.x * dt;
    m_position.x -= m_velocity.x * dt;
}

void Enemy::render(sf::RenderTarget& target) const
{
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}
