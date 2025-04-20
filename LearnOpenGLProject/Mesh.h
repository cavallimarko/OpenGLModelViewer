#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
};
struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;  // we store the path of the texture to compare with other textures
};
class Mesh
{
public:
	// mesh data
	std::vector<Vertex>       vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture>      textures;
	unsigned int indicesCount;
	unsigned int verticesCount;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		indicesCount = indices.size();
		verticesCount = vertices.size();
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}
	void Draw(Shader& shader, Camera& camera)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			//std::string number;
			std::string name = textures[i].type;
			/*if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);*/

				//shader.setInt(("material." + name + number).c_str(), i);
				//shader.setInt(("material." + textures[i].type).c_str(), i);
			shader.setInt(std::string("material.").append(textures[i].type), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);
		shader.setVec3("viewPos", camera.Position);
		shader.setFloat("material.shininess", 64.0f);
		shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

		shader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
		shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
		shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	unsigned int GetVAO()
	{
		return VAO;
	}
private:
	//  render data
	unsigned int VAO, VBO, EBO;

	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// normal attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// uv attribute
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// tangent attribute
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

		glBindVertexArray(0);
		//empty buffers, remove from RAM
		vertices = std::vector<Vertex>();
		indices = std::vector<unsigned int>();
	}
};