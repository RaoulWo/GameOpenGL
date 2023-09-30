#include "core.h"
#include "events/key_event.h"

#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer : public Layer
{
public:
    ExampleLayer()
        : Layer("Example"), camera_(-1.6f, 1.6f, -0.9f, 0.9f)
    {
        float vertices[7 * 3] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };

        unsigned int indices[3] = {
            0, 1, 2
        };

        // Create a vertex array.
        vertex_array_.reset(new VertexArray());

        // Create a vertex buffer.
        std::shared_ptr<VertexBuffer> vertexBuffer;
        vertexBuffer.reset(new VertexBuffer(vertices, sizeof(vertices)));

        // Create and set a layout for the vertex buffer.
        VertexBufferLayout layout = {
            { VertexBufferAttributeType::Float3, "a_Position" },
            { VertexBufferAttributeType::Float4, "a_Color" }
        };
        vertexBuffer->set_layout(layout);
        
        // Add the vertex buffer after (!) the layout is set.
        vertex_array_->add_vertex_buffer(vertexBuffer);

        // Create an index buffer.
        std::shared_ptr<IndexBuffer> indexBuffer;
        indexBuffer.reset(new IndexBuffer(indices, sizeof(indices) / sizeof(uint32_t)));

        // Set the index buffer.
        vertex_array_->set_index_buffer(indexBuffer);

        float rectangleVertices[4 * 3] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
        };

        unsigned int rectangleIndices[6] = {
            0, 1, 2,
            2, 3, 0
        };

        rectangle_vertex_array_.reset(new VertexArray());

        std::shared_ptr<VertexBuffer> rectangleVertexBuffer;
        rectangleVertexBuffer.reset(new VertexBuffer(rectangleVertices, sizeof(rectangleVertices)));

        rectangleVertexBuffer->set_layout({
            { VertexBufferAttributeType::Float3, "a_Position" }
        });

        rectangle_vertex_array_->add_vertex_buffer(rectangleVertexBuffer);

        std::shared_ptr<IndexBuffer> rectangleIndexBuffer;
        rectangleIndexBuffer.reset(new IndexBuffer(rectangleIndices, sizeof(rectangleIndices) / sizeof(uint32_t)));
        
        rectangle_vertex_array_->set_index_buffer(rectangleIndexBuffer);

        std::string vertexSource = R"(
			    #version 330 core
			    
			    layout(location = 0) in vec3 a_Position;
                layout(location = 1) in vec4 a_Color;

                uniform mat4 u_projectionViewMatrix;
                uniform mat4 u_modelMatrix;

			    out vec3 v_Position;
                out vec4 v_Color;

			    void main()
			    {
				    v_Position = a_Position;
                    v_Color = a_Color;
				    gl_Position = u_projectionViewMatrix * u_modelMatrix * vec4(a_Position, 1.0);	
			    }
		    )";

        std::string fragmentSource = R"(
			    #version 330 core
			    
			    layout(location = 0) out vec4 color;

			    in vec3 v_Position;
                in vec4 v_Color;

			    void main()
			    {
				    color = vec4(v_Position * 0.5 + 0.5, 1.0);
                    color = v_Color;
			    }
		    )";

        // Create a shader with hardcoded vertex and fragment source code.
        shader_.reset(new Shader(vertexSource, fragmentSource));

        std::string blueVertexSource = R"(
			    #version 330 core
			    
			    layout(location = 0) in vec3 a_Position;

                uniform mat4 u_projectionViewMatrix;
                uniform mat4 u_modelMatrix;

			    out vec3 v_Position;

			    void main()
			    {
				    v_Position = a_Position;
				    gl_Position = u_projectionViewMatrix * u_modelMatrix * vec4(a_Position, 1.0);	
			    }
		    )";

        std::string blueFragmentSource = R"(
			    #version 330 core
			    
			    layout(location = 0) out vec4 color;

			    in vec3 v_Position;

			    void main()
			    {
				    color = vec4(0.2f, 0.3f, 0.8f, 1.0f);
			    }
		    )";

        // Create a shader with hardcoded vertex and fragment source code.
        solid_blue_shader_.reset(new Shader(blueVertexSource, blueFragmentSource));
    }

    void OnUpdate(DeltaTime deltaTime) override
    {
        // Camera movement
        if (Input::IsKeyPressed(CG_KEY_LEFT))
            camera_pos_.x -= camera_speed_ * deltaTime;
        else if (Input::IsKeyPressed(CG_KEY_RIGHT))
            camera_pos_.x += camera_speed_ * deltaTime;

        if (Input::IsKeyPressed(CG_KEY_DOWN))
            camera_pos_.y -= camera_speed_ * deltaTime;
        else if (Input::IsKeyPressed(CG_KEY_UP))
            camera_pos_.y += camera_speed_ * deltaTime;

        // Camera rotation
        if (Input::IsKeyPressed(CG_KEY_Q))
            camera_rot_ += camera_rot_speed_ * deltaTime;
        else if (Input::IsKeyPressed(CG_KEY_E))
            camera_rot_ -= camera_rot_speed_ * deltaTime;

        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        RenderCommand::Clear();

        // Test setting the camera position and rotation.
        camera_.set_position(camera_pos_);
        camera_.set_rotation(camera_rot_);

        // Takes in the camera object which calculates the projection view matrix (P * V) for the whole scene.
        Renderer::BeginScene(camera_);

        glm::mat4 rotation(1.0f);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        // Render grid.
        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 20; x++)
            {
                glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
                glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos);
                // Calculate model matrix (M = T * R * S).
                glm::mat4 modelMatrix = translation * rotation * scale;
                Renderer::Submit(solid_blue_shader_, rectangle_vertex_array_, modelMatrix);
            }
        }

        Renderer::Submit(shader_, vertex_array_);

        // Does nothing for now.
        Renderer::EndScene();
    }

    void OnEvent(Event& event) override
    {

    }

private:
    std::shared_ptr<VertexArray> vertex_array_;
    std::shared_ptr<Shader> shader_;
    std::shared_ptr<VertexArray> rectangle_vertex_array_;
    std::shared_ptr<Shader> solid_blue_shader_;

    OrthographicCamera camera_;

    glm::vec3 camera_pos_ = { 0.0f, 0.0f, 0.0f };
    float camera_rot_ = 0.0f;

    float camera_speed_ = 1.0f;  // in units per second
    float camera_rot_speed_ = 45.0f;  // in degrees per second
};

class Sandbox : public Application
{
public:
    Sandbox()
    {
        PushLayer(new ExampleLayer());
    }
};

int main(int argc, char** argv)
{
    Log::Initialize();

    LOG_DEBUG("Initialized logger!");

    auto app = new Sandbox;
    app->Run();
    delete app;
}