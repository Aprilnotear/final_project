#include "Mymesh.h"


MyMesh::MyMesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	// Set vertex buffers and attribute pointers
	this->setupMesh();
}

void MyMesh::Draw(Shader* shader) {
	// Bind appropriate textures
	GLuint diffuseNumber = 1;
	GLuint specularNumber = 1;
	GLuint normalNumber = 1;
	GLuint heightNumber = 1;

	for (GLuint i = 0; i < this->textures.size(); i++) {
		// Activate proper texture unit and retreive texture number
		glActiveTexture(GL_TEXTURE0 + i);
		stringstream stream;
		string number;
		string name = this->textures[i].type;

		// Transfer texture data to stream
		if (name == "texture_diffuse") {
			stream << diffuseNumber++;
		}
		else if (name == "texture_specular") {
			stream << specularNumber++;
		}
		else if (name == "texture_normal") {
			stream << normalNumber++;
		}
		else if (name == "texture_height") {
			stream << heightNumber++;
		}
		number = stream.str();

		// Set sampler to the correct texture unit and bind the texture
		glUniform1i(glGetUniformLocation(shader->GetProgram(), (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Reset to defaults
	for (GLuint i = 0; i < this->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void MyMesh::setupMesh() {
	// Create buffers and arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(this->VAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	// Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));

	// Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));

	// Bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitangent));

	glBindVertexArray(0);
}