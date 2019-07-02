
#include "mesh.h"

#include <iostream>


//MESHENTRY
//================================
Mesh::MeshEntry::MeshEntry()
{
    vertexBuffer = 0;
    indexBuffer = 0;
    numIndices  = 0;
    matIndex = 0;
};

Mesh::MeshEntry::~MeshEntry()
{
    if (vertexBuffer != 0)
    {
        glDeleteBuffers(1, &vertexBuffer);
    }

    if (indexBuffer != 0)
    {
        glDeleteBuffers(1, &indexBuffer);
    }
}

void Mesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
                          const std::vector<unsigned int>& Indices)
{
    numIndices = (unsigned int)Indices.size();

    glGenBuffers(1, &vertexBuffer);
  	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, &Indices[0], GL_STATIC_DRAW);
}


//MESH
//================================================
Mesh::Mesh()
{
}


Mesh::~Mesh()
{
    Clear();
}


void Mesh::Clear()
{
    //for (unsigned int i = 0 ; i < m_Textures.size() ; i++) {
    //    SAFE_DELETE(m_Textures[i]);
    //}
}


bool Mesh::LoadMesh(const std::string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();
    
    bool result = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
    

    if (pScene) {
        result = InitFromScene(pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
    }

	std::cout << "Succesfully Loaded the mesh" << std::endl;

    return result;
}

bool Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{  
    m_entries.resize(pScene->mNumMeshes);
    //m_Textures.resize(pScene->mNumMaterials);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }


    return InitMaterials(pScene, Filename);
}

void Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
    m_entries[Index].matIndex = paiMesh->mMaterialIndex;
    
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
				glm::vec2(pTexCoord->x, pTexCoord->y),
				glm::vec3(pNormal->x, pNormal->y, pNormal->z));

        Vertices.push_back(v);
    }

    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_entries[Index].Init(Vertices, Indices);
}

bool Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        //m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string FullPath = Dir + "/" + Path.data;
                //m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

                //if (!m_Textures[i]->Load()) {
                //    printf("Error loading texture '%s'\n", FullPath.c_str());
                //    delete m_Textures[i];
                //    m_Textures[i] = NULL;
                //    result = false;
                //}
                //else {
                //    printf("Loaded texture '%s'\n", FullPath.c_str());
                //}
            }
        }

        // Load a white texture in case the model does not include its own texture
        //if (!m_Textures[i]) {
        //    m_Textures[i] = new Texture(GL_TEXTURE_2D, "../Content/white.png");
		//
        //    result = m_Textures[i]->Load();
        //}
    }

    return Ret;
}

void Mesh::Render()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    for (unsigned int i = 0 ; i < m_entries.size() ; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, m_entries[i].vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_entries[i].indexBuffer);

        const unsigned int matIndex = m_entries[i].matIndex;

        //if (matIndex < m_Textures.size() && m_Textures[matIndex]) {
        //    m_Textures[matIndex]->Bind(GL_TEXTURE0);
        //}

        glDrawElements(GL_TRIANGLES, m_entries[i].numIndices, GL_UNSIGNED_INT, 0);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
