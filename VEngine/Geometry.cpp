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

	//if (indexBuffer != NULL)
	//{
	//	delete indexBuffer;
	//}
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
	sourceNode = meshNode->first_node("triangles");
	if (sourceNode == NULL)
	{
		sourceNode = meshNode->first_node("polylist");
	}
	int numberOfFaces = std::stoi(sourceNode->first_attribute("count")->value());

	std::vector<Triangle> geometry;
	geometry.resize(numberOfFaces);

	//Parse the polylist.
	sourceNode = sourceNode->first_node("p");
	std::stringstream polyListSS(sourceNode->value());
	std::string polyListElement;

	std::vector<uint32_t> polyList;
	polyList.resize(numberOfFaces * 9); //Times nine because there are three vertices to a face and three attributes to a vertex

	for (int i = 0, count = (int)polyList.size(); i < count; i++)
	{
		polyListSS >> polyListElement;
		polyList[i] = std::stoi(polyListElement);
	}

	//Build the vertex buffer with the indices.
	for (int i = 0, k = 0, count = (int)geometry.size(); i < count; i++)
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


		//Tangents and BiTangents - thank you http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
		glm::vec3 deltaPos1 = geometry[i].vertexB - geometry[i].vertexA;
		glm::vec3 deltaPos2 = geometry[i].vertexC - geometry[i].vertexA;

		glm::vec2 deltaUV1 = geometry[i].uvB - geometry[i].uvA;
		glm::vec2 deltaUV2 = geometry[i].uvC - geometry[i].uvA;
		
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

		geometry[i].tangentA = geometry[i].tangentB = geometry[i].tangentC = glm::normalize(tangent);
		geometry[i].biTangentA = geometry[i].biTangentB = geometry[i].biTangentC = glm::normalize(bitangent);
	}
	
	//Create the buffers
	LoadMesh(geometry, polyList);
}

//void Geometry::LoadMeshFromObj(char * filePath)
//{
//	tinyobj::attrib_t attrib;
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> materials;
//
//	std::string err;
//	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filePath);
//
//	if (!err.empty()) { // `err` may contain warning message.
//		std::cerr << err << std::endl;
//	}
//
//	if (!ret) {
//		exit(1);
//	}
//
//	tinyobj::shape_t shape = shapes[0];
//	assert((int)attrib.vertices.size() % 3 == 0);
//	int numberOfFaces = (int)attrib.vertices.size() / 3;
//
//	std::vector<Triangle> geometry;
//	geometry.resize(numberOfFaces);
//
//	// Loop over faces(polygon)
//	size_t index_offset = 0;
//	for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
//		int fv = shape.mesh.num_face_vertices[f];
//		assert(fv == 3); //Not supporting n-gons
//
//
//		tinyobj::index_t idx = shape.mesh.indices[index_offset + 0];
//		//First vertex attributes
//		geometry[f].vertexA = { attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2] };
//
//		geometry[f].normalA = { attrib.normals[3 * idx.vertex_index + 0], attrib.normals[3 * idx.vertex_index + 1], attrib.normals[3 * idx.vertex_index + 2] };
//
//		geometry[f].uvA = { attrib.texcoords[3 * idx.vertex_index + 0], attrib.texcoords[3 * idx.vertex_index + 1] };
//
//
//		idx = shape.mesh.indices[index_offset + 1];
//		//second vertex attributes
//		geometry[f].vertexB = { attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2] };
//
//		geometry[f].normalB = { attrib.normals[3 * idx.vertex_index + 0], attrib.normals[3 * idx.vertex_index + 1], attrib.normals[3 * idx.vertex_index + 2] };
//
//		geometry[f].uvB = { attrib.texcoords[3 * idx.vertex_index + 0], attrib.texcoords[3 * idx.vertex_index + 1] };
//
//
//		idx = shape.mesh.indices[index_offset + 2];
//		//third vertex attributes
//		geometry[f].vertexC = { attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2] };
//
//		geometry[f].normalC = { attrib.normals[3 * idx.vertex_index + 0], attrib.normals[3 * idx.vertex_index + 1], attrib.normals[3 * idx.vertex_index + 2] };
//
//		geometry[f].uvC = { attrib.texcoords[3 * idx.vertex_index + 0], attrib.texcoords[3 * idx.vertex_index + 1] };
//
//		// Loop over vertices in the face.
//		for (size_t v = 0; v < fv; v++) {
//			// access to vertex
//			tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
//			tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
//			tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
//			tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
//			tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
//			tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
//			tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
//			tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
//			tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
//			// Optional: vertex colors
//			// tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
//			// tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
//			// tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
//		}
//		index_offset += fv;
//	}
//	
//	//Create the buffers
//	std::vector<unsigned int> polylist;
//
//	LoadMesh(geometry, polylist);
//}

void Geometry::LoadMesh(std::vector<Triangle> &geometry, std::vector<uint32_t> &indices)
{
	// Store the number of vertices in the geometry //
	numVertices = (uint32_t)geometry.size() * 3;

	// Store the vertex buffer device side //
	vertexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, geometry.data(), sizeof(Triangle) * geometry.size());

	// Store the index buffer device side //
	//indexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices.data(), sizeof(uint32_t) * indices.size());
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
		vertexAttributes.back().z = std::stof(buf); //Blender has z-up, so we interpret y as z to put it in y-up. Negative to be in the correct direction.
		ss >> buf;
		vertexAttributes.back().y = -std::stof(buf);
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