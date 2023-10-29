#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Animation.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
    b2Vec2 vel = pbody->body->GetLinearVelocity();

    if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
        vel.x = -speed;
        isMoving = true;
    }
    else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
        vel.x = speed;
        isMoving = true;
    }
    else {
        vel.x = 0;
        isMoving = false;
    }

    if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && isTouchingGround) {
        vel.y = jumpImpulse;
        isTouchingGround = false;
    }

    if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && canDash && isMoving) {
        vel.x *= dashMultiplier;
        canDash = false;
    }

    pbody->body->SetLinearVelocity(vel);

    position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 16;
    position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 16;

    app->render->DrawTexture(texture, position.x, position.y);

    if (vel.y > 0)
        isTouchingGround = false;

    return true;
}


bool Player::CleanUp()
{

    return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

    switch (physB->ctype)
    {
    case ColliderType::ITEM:
        LOG("Collision ITEM");
        app->audio->PlayFx(pickCoinFxId);
        break;
    case ColliderType::PLATFORM:
        LOG("Collision PLATFORM");
        isTouchingGround = true;
        canDash = true;
        break;
    case ColliderType::UNKNOWN:
        LOG("Collision UNKNOWN");
        break;
    }
}
