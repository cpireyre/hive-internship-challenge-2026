#include "Powerup.h"
#include "ResourceManager.h"
#include "Constants.h"
#include <cmath>
#include <sys/time.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

static uint32_t roll(int n);
static int64_t	now_ms(void);

bool Powerup::init()
{
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("some_yellow_thing.png");
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    constexpr uint32_t size = 3;
    constexpr float height[size] = {20, 70, 290};
    m_position.y = ZERO_Y - height[roll(size)];
    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(2.0f, 2.0f));
    m_collisionRadius = collisionRadius;
    m_velocity.x = 400.0f;
    m_acceleration.x = 0.f;

    return true;
}

void Powerup::update(float dt)
{
    m_position.x -= m_velocity.x * dt;
}

void Powerup::render(sf::RenderTarget& target) const
{
    if (active) {
        m_pSprite->setPosition(m_position);
        target.draw(*m_pSprite);
    }
}

static uint32_t roll(int n) {
    return rand() % n;
}
