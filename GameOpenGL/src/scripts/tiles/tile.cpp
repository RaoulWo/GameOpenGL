#include "tile.h"
#include "scripts/player.h"
#include "scripts/game_manager.h"

glm::vec3 Tile::GetPosition() {
    return position_;
}

void Tile::SetPosition(glm::vec3 position) {
    position_ = position;
}

void Tile::AddConnection(std::shared_ptr<Tile> tile, Direction direction) {
    connections_[direction] = tile;
}

void Tile::SetHasPlayer(bool hasPlayer)
{
	hasPlayer_ = hasPlayer;
}

std::shared_ptr<Tile> Tile::GetConnection(Direction direction)
{
    return connections_[direction];
}

const std::string& Tile::GetRepresentation() const {
    return stringRepresentation_;
}

bool Tile::IsWalkable(Direction direction) const
{
    return isWalkable_[direction];
}

void Tile::AttachToScene(std::shared_ptr<Scene> scene)
{
    // Create tile at position
    Entity entity = scene->CreateEntity("Tile", position_);
    // Add sprite renderer component
    entity.AddComponent<SpriteRendererComponent>(color_);

    // If tile has player, add player component
    if (hasPlayer_) {
        // Set player position to tile position + 1 on z axis (to be above tile)
        glm::vec3 playerPosition = glm::vec3(position_.x, position_.y, position_.z + 1.0f);

        Entity player = scene->CreateEntity("Player", playerPosition);
        player.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        player.AddComponent<ScriptComponent>().Bind<Player>();

        GameManager::GetInstance().SetTileContainingPlayer(std::make_shared<Tile>(*this));
    }
}

Tile::Tile() {
    size_ = glm::vec3(1.0f, 1.0f, 1.0f);
    position_ = glm::vec3(0.0f);
    color_ = glm::vec4(1.0f);
    for (int i = 0; i < 4; i++) { isWalkable_[i] = false; };
    hasPlayer_ = false;

    for(int i = 0; i < 4; i++) {
		connections_[i] = nullptr;
	}
}