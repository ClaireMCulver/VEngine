#include "Geometry.h"



Geometry::Geometry()
{
}


Geometry::~Geometry()
{
	if (vertexBuffer != NULL)
	{
		delete vertexBuffer;
	}
}

void Geometry::LoadMeshFromDae(char* filePath)
{
	int result = 0;

	// Load the file into memory //
	rapidxml::file<> colladaFile(filePath);

	//Parse the text with rapidxml.
	rapidxml::xml_document<> colladaDoc;
	colladaDoc.parse<0>(colladaFile.data());

	//Go through the xml node tree and find the mesh node 
	rapidxml::xml_node<> *libraryGeometriesNode = colladaDoc.first_node()->first_node("library_geometries");

	rapidxml::xml_node<> *geometryNode = libraryGeometriesNode->first_node();
	rapidxml::xml_node<> *meshNode = geometryNode->first_node();

	//Parse the verticies, normals, uvs and indices.
	rapidxml::xml_node<> *sourceNode = meshNode->first_node();
	std::vector<glm::vec3> vertices = loadVertices(sourceNode);

	sourceNode = sourceNode->next_sibling();
	std::vector <glm::vec3> normals = loadVertices(sourceNode);

	sourceNode = sourceNode->next_sibling();
	std::vector<glm::vec2> UVs = loadUVs(sourceNode);
}

void Geometry::LoadMesh(std::vector<Triangle> &geometry)
{
	// Store the number of vertices in the geometry //
	numVertices = geometry.size() * 3;

	// Store the vertex buffer device side //
	vertexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, geometry.data(), sizeof(Triangle) * geometry.size());
}

void Geometry::Draw(VkCommandBuffer commandBuffer)
{
	const VkBuffer vkBuffer = vertexBuffer->GetVKBuffer();
	const VkDeviceSize offsets[1] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vkBuffer, offsets);

	vkCmdDraw(commandBuffer, numVertices, 1, 0, 0);
}

std::vector<glm::vec3> Geometry::loadVertices(rapidxml::xml_node<> *sourceNode)
{
	rapidxml::xml_node<> *dataNode = sourceNode->first_node();
	rapidxml::xml_attribute<> *dataAttribute;

	//Fetch the list of vertices;
	dataAttribute = dataNode->first_attribute();
	std::stringstream ss(dataNode->value());

	std::string attributeName = dataAttribute->value();
	dataAttribute = dataAttribute->next_attribute();

	//Fetch the actual stride and count from the file, for verification.
	dataNode = dataNode->next_sibling()->first_node();
	int verticesCount = std::stoi(dataNode->first_attribute("count")->value());
	int vertexStride = std::stoi(dataNode->first_attribute("stride")->value());
	assert(vertexStride == 3);

	std::vector<glm::vec3> vertexAttributes;
	vertexAttributes.reserve(verticesCount);

	//Load the vertices from the file into a vec3;
	std::string buf;
	while (ss >> buf)
	{
		vertexAttributes.push_back(glm::vec3());

		vertexAttributes.back().x = std::stof(buf);
		ss >> buf;
		vertexAttributes.back().y = std::stof(buf);
		ss >> buf;
		vertexAttributes.back().z = std::stof(buf);
	}

	return vertexAttributes;
}

std::vector<glm::vec2> Geometry::loadUVs(rapidxml::xml_node<> *sourceNode)
{
	//This will get the third element in the list. The nodes are all named the same and the id attribes would be a pain to parse. Consider reworking this.
	rapidxml::xml_node<> *dataNode = sourceNode->first_node();
	rapidxml::xml_attribute<> *dataAttribute;

	//Fetch the list of vertices;
	dataAttribute = dataNode->first_attribute();
	std::stringstream ss(dataNode->value());

	std::string attributeName = dataAttribute->value();
	dataAttribute = dataAttribute->next_attribute();

	//Fetch the actual stride and count from the file, for verification.
	dataNode = dataNode->next_sibling()->first_node();
	int verticesCount = std::stoi(dataNode->first_attribute("count")->value());
	int vertexStride = std::stoi(dataNode->first_attribute("stride")->value());
	assert(vertexStride == 2);

	std::vector<glm::vec2> vertexAttributes;
	vertexAttributes.reserve(verticesCount);

	//Load the vertices from the file into a vec2;
	std::string buf;
	while (ss >> buf)
	{
		vertexAttributes.push_back(glm::vec2());

		vertexAttributes.back().x = std::stof(buf);
		ss >> buf;
		vertexAttributes.back().y = std::stof(buf);
	}

	return vertexAttributes;
}