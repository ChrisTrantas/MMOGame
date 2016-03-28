#include <algorithm>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "Mesh.h"
#include "Game.h"

static float distanceSquared(vec3 a, vec3 b)
{
	float xx = a.x - b.x;
	float yy = a.y - b.y;
	float zz = a.z - b.z;

	return xx*xx + yy*yy + zz*zz;
}

Mesh* Mesh::getMesh(string modelPath)
{
	Mesh* mesh = (Mesh*)ResourceManager::manager->getResource(modelPath);
	if (!mesh)
		mesh = new Mesh(modelPath);
	return mesh;
}

Mesh::Mesh(string modelPath) : Resource(modelPath, MY_TYPE_INDEX)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(modelPath,
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FindInvalidData |
		aiProcess_GenNormals |
		aiProcess_ImproveCacheLocality |
		aiProcess_Triangulate |
		aiProcess_FlipWindingOrder |
		aiProcess_SortByPType);

	size_t vertCount = 0, faceCount = 0;

	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		vertCount += scene->mMeshes[i]->mNumVertices;
		faceCount += scene->mMeshes[i]->mNumFaces * 3;
	}

	vec3 upper, lower;
	size_t vertOffset = 0, faceOffset = 0;
	vertex* verts = new vertex[vertCount];
	unsigned int* faces = new unsigned int[faceCount];

	for (size_t m = 0; m < scene->mNumMeshes; ++m)
	{
		aiMesh* mesh = scene->mMeshes[m];

		for (size_t i = 0; i < mesh->mNumVertices; ++i)
		{
			vertex vert = { DirectX::XMFLOAT3(mesh->mVertices[i].v),
							DirectX::XMFLOAT3(mesh->mNormals[i].v),
							DirectX::XMFLOAT3(mesh->mTangents[i].v),
							DirectX::XMFLOAT2(mesh->mTextureCoords[0][i].x, 1 - mesh->mTextureCoords[0][i].y) };
			verts[vertOffset + i] = vert;

			if (i != 0)
			{
				lower.x = std::min(lower.x, mesh->mVertices[i].x);
				lower.y = std::min(lower.y, mesh->mVertices[i].y);
				lower.z = std::min(lower.z, mesh->mVertices[i].z);
				upper.x = std::max(upper.x, mesh->mVertices[i].x);
				upper.y = std::max(upper.y, mesh->mVertices[i].y);
				upper.z = std::max(upper.z, mesh->mVertices[i].z);
			}
			else if (m == 0)
			{
				upper = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				lower = upper;
			}
		}

		for (size_t i = 0; i < mesh->mNumFaces; ++i)
		{
			faces[faceOffset + i * 3] = vertOffset + mesh->mFaces[i].mIndices[0];
			faces[faceOffset + i * 3 + 1] = vertOffset + mesh->mFaces[i].mIndices[1];
			faces[faceOffset + i * 3 + 2] = vertOffset + mesh->mFaces[i].mIndices[2];
		}

		vertOffset += mesh->mNumVertices;
		faceOffset += mesh->mNumFaces * 3;
	}

	bounds.halfSize = vec3(upper.x - lower.x, upper.y - lower.y, upper.z - lower.z);
	bounds.halfSize /= 2.0f;
	bounds.center.x = lower.x + bounds.halfSize.x;
	bounds.center.y = lower.y + bounds.halfSize.y;
	bounds.center.z = lower.z + bounds.halfSize.z;

	float dist = 0;
	vec3 furthest;
	for (size_t i = 0; i < vertCount; i += 1)
	{
		float distanceCheck = distanceSquared(vec3(verts[i].position.x, verts[i].position.y, verts[i].position.z), bounds.center);
		if (distanceCheck > dist)
		{
			furthest = vec3(verts[i].position.x, verts[i].position.y, verts[i].position.z);
			dist = distanceCheck;
		}
	}

	dist = 0;
	vec3 furthest2;
	for (size_t i = 0; i < vertCount; i += 1)
	{
		float distanceCheck = distanceSquared(vec3(verts[i].position.x, verts[i].position.y, verts[i].position.z), furthest);
		if (distanceCheck > dist)
		{
			furthest2 = vec3(verts[i].position.x, verts[i].position.y, verts[i].position.z);
			dist = distanceCheck;
		}
	}

	bounds.sphere = furthest2 - furthest;
	bounds.radius = length(bounds.sphere) / 2.0f;
	bounds.sphere = furthest + normalize(bounds.sphere) * bounds.radius;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(vertex) * vertCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = verts;

	HR(DEVICE->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(unsigned int) * faceCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = faces;

	HR(DEVICE->CreateBuffer(&ibd, &initialIndexData, &indexBuffer));

	elementArraySize = faceCount;
	
	delete[] verts;
	delete[] faces;
}

Mesh::~Mesh()
{
	ReleaseMacro(vertexBuffer);
	ReleaseMacro(indexBuffer);
}

void Mesh::draw(mat4 &camera, mat4 &perspective, mat4 &model, Material* material)
{
	float explodedModel[16];
	float explodedCamera[16];
	float explodedPerspective[16];

	explodeMat4(transpose(model), explodedModel);
	explodeMat4(transpose(camera), explodedCamera);
	explodeMat4(transpose(perspective), explodedPerspective);

	SimpleVertexShader* vs = material->getVertexShader();
	SimplePixelShader* ps = material->getPixelShader();

	vs->SetMatrix4x4("world", explodedModel);
	vs->SetMatrix4x4("view", explodedCamera);
	vs->SetMatrix4x4("projection", explodedPerspective);

	DirectionalLight light = DEFAULT_LIGHT->getDirectionalLight();
	ps->SetData("light1", &light, sizeof(DirectionalLight));
	light = Light::getLight("light2")->getDirectionalLight();
	ps->SetData("light2", &light, sizeof(DirectionalLight));

	ps->SetShaderResourceView("diffuse", material->diffuse->getSRV());
	ps->SetShaderResourceView("normalMap", material->normalMap->getSRV());
	ps->SetSamplerState("trilinear", material->diffuse->getSamplerState());

	vs->SetShader(true);
	ps->SetShader(true);

	UINT stride = sizeof(vertex);
	UINT offset = 0;

	DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DEVICE_CONTEXT->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DEVICE_CONTEXT->DrawIndexed(elementArraySize, 0, 0);
}
