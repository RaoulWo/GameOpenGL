#include "scene.h"

#include "components.h"
#include "entity.h"
#include "renderer/renderer.h"
#include "renderer/renderer_2d.h"

#include <glm/glm.hpp>
#include <memory>

Scene::Scene()
{
	// Here we create an entity, internally it is simply an uint32_t.
	// Because it is just an integer, it does not contain any methods or data.
	// Any time we want to modify an entity, we need to use the registry.
	entt::entity entity = registry_.create();

	// This is how components are added to an entity. 
	// Fist we need to specify the entity we want to add the component to, then the component arguments.
	registry_.emplace<TransformComponent>(entity, glm::mat4(1.0f));

	// Checking if a component exists and then getting it looks like this.
	if (registry_.all_of<TransformComponent>(entity))
		TransformComponent& transform = registry_.get<TransformComponent>(entity);

	// Iterating over all entities that have a TransformComponent looks like this.
	// First we need to get a view of all entities that have the component.
	// Then we can iterate over the view and get the entity from it.
	// A use case for this would be to render all sprite/mesh components.
	auto view = registry_.view<TransformComponent>();
	for (auto entity : view)
	{
		TransformComponent& transform = view.get<TransformComponent>(entity);
	}
}

Scene::~Scene()
{
}

void Scene::OnUpdate(DeltaTime deltaTime)
{
	//Camera* mainCamera = nullptr;
	//{
	//	auto group = registry_.group<TransformComponent>(entt::get<CameraComponent>);
	//	for (auto entity : group)
	//	{
	//		auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

	//		if (camera.isPrimary)
	//		{
	//			mainCamera = &camera.camera;
	//			break;
	//		}
	//	}
	//}

	//if (mainCamera)
	//{
	//	// Renderer2D::BeginScene(*mainCamera, transform);
	//}

	// Here, we get all entities that have a transform and sprite renderer component and issue a draw call for them.
	auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
	for (auto entity : group)
	{
		auto [transform, spriteRenderer] = group.get<TransformComponent, SpriteRendererComponent>(entity);
		Renderer2D::DrawQuad(transform.transform, spriteRenderer.color);
	}

	if (mainCamera)
	{
		Renderer2D::BeginScene(mainCamera->projection(), *transform);

		// Here, we get all entities that have a transform and sprite renderer component and issue a draw call for them.
		auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, spriteRenderer] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawQuad(transform.transform, spriteRenderer.color);
		}

		Renderer2D::EndScene();
	}
}

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
	viewport_width_ = width;
	viewport_height_ = height;
	
	// Resize all cameras with non-fixed aspect ratios.
	auto view = registry_.view<CameraComponent>();
	for (auto entity : view)
	{
		auto& cameraComponent = view.get<CameraComponent>(entity);
		if (!cameraComponent.hasFixedAspectRatio)
		{
			// In order to update the aspect ratio, the projection matrix needs to be recalculated.
			//cameraComponent.camera.set_aspect_ratio((float)width / (float)height);

		}
	}
}

/// <summary>
/// Creates an entity with a tag and transform component.
/// </summary>
Entity Scene::CreateEntity(const std::string& name)
{
	Entity entity = Entity(registry_.create(), this);

	// Here, all entities are created with a tag and transform component.
	TagComponent& tag = entity.AddComponent<TagComponent>();
	tag.tag = name.empty() ? "Entity" : name;
	entity.AddComponent<TransformComponent>();

	return entity;
}
