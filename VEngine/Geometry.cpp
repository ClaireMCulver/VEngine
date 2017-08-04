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

	if (indexBuffer != NULL)
	{
		delete indexBuffer;
	}
}

void Geometry::LoadMeshFromDae(char* filePath)
{
	int result = 0;

	//Load the file into memory
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

	//Parse the number of faces from the PolyList.
	sourceNode = meshNode->first_node("polylist");
	int numberOfFaces = std::stoi(sourceNode->first_attribute("count")->value());

	std::vector<Triangle> geometry;
	geometry.resize(numberOfFaces);

	//Parse the polylist.
	sourceNode = sourceNode->first_node("p");
	std::stringstream polyListSS(sourceNode->value());
	std::string polyListElement;

	std::vector<uint32_t> polyList;
	polyList.resize(numberOfFaces * 9); //Times nine because there are three vertices to a face and three attributes to a vertex

	for (int i = 0, count = polyList.size(); i < count; i++)
	{
		polyListSS >> polyListElement;
		polyList[i] = std::stoi(polyListElement);
	}

	//Build the vertex buffer with the indices.
	for (int i = 0, k = 0, count = geometry.size(); i < count; i++)
	{
		//First vertex attributes
		geometry[i].vertexA = vertices[polyList[k++]];

		geometry[i].normalA = normals[polyList[k++]];

		geometry[i].uvA = UVs[polyList[k++]];


		//second vertex attributes
		geometry[i].vertexB = vertices[polyList[k++]];

		geometry[i].normalB = normals[polyList[k++]];

		geometry[i].uvB = UVs[polyList[k++]];


		//third vertex attributes
		geometry[i].vertexC = vertices[polyList[k++]];

		geometry[i].normalC = normals[polyList[k++]];

		geometry[i].uvC = UVs[polyList[k++]];
	}

	//Create the buffers
	LoadMesh(geometry, polyList);
}

void Geometry::LoadMesh(std::vector<Triangle> &geometry, std::vector<uint32_t> &indices)
{
	// Store the number of vertices in the geometry //
	numVertices = geometry.size() * 3;

	// Store the vertex buffer device side //
	vertexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, geometry.data(), sizeof(Triangle) * geometry.size());

	// Store the index buffer device side //
	indexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices.data(), sizeof(uint32_t) * indices.size());
}

void Geometry::Draw(VkCommandBuffer commandBuffer)
{
	const VkBuffer vkBuffer = vertexBuffer->GetVKBuffer();
	const VkDeviceSize offsets[1] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vkBuffer, offsets);

	//vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetVKBuffer(), 0, VkIndexType::VK_INDEX_TYPE_UINT32);

	//vkCmdDrawIndexed(commandBuffer, numVertices, 1, 0, 0, 0);

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
		vertexAttributes.back().z = std::stof(buf); //Blender has z-up, so we interpret y as z to put it in y-up.
		ss >> buf;
		vertexAttributes.back().y = std::stof(buf);
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