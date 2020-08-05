#ifndef MODEL_HPP
#define MODEL_HPP

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.hpp"

unsigned int load_texture_from_file(const std::filesystem::path texture_path)
{
    // Create texture ID.
    unsigned int texture;
    glGenTextures(1, &texture);

    // Load texture.
    int width;
    int height;
    int num_channels;
    unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &num_channels, 0);
    if (data)
    {
        GLenum format;
        if (num_channels == 1)
            format = GL_RED;
        else if (num_channels == 3)
            format = GL_RGB;
        else if (num_channels == 4)
            format = GL_RGBA;

        // Generate texture.
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load texture at " << texture_path << '\n';
        stbi_image_free(data);
    }

    return texture;
}


class Model
{
public:
    explicit Model(std::filesystem::path path_) : path(path_) {}

    bool init();
    void draw(Shader&);
private:
    // Model data.
    std::vector<Mesh> meshes;
    std::filesystem::path path;
    std::filesystem::path directory;
    std::vector<Texture> loaded_textures;

    bool load_model();
    void process_node(aiNode*, const aiScene*);
    Mesh process_mesh(aiMesh*, const aiScene*);
    std::vector<Texture> load_material_textures(aiMaterial*,
        aiTextureType,
        std::string);
};

bool Model::init()
{
    return load_model();
}

void Model::draw(Shader& shader)
{
    for (std::size_t i = 0; i < meshes.size(); i++)
        meshes[i].draw(shader);
}

bool Model::load_model()
{
    Assimp::Importer importer;
    std::cout << "Importing scene from " << path << '\n';
    const aiScene* scene = importer.ReadFile(path.string(),
        aiProcess_Triangulate);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << '\n';
        return false;
    }

    directory = path.parent_path();

    process_node(scene->mRootNode, scene);

    return true;
}

void Model::process_node(aiNode* node, const aiScene* scene)
{
    // Process all of the node's meshes, if any.
    for (std::size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* assimp_mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh my_mesh = process_mesh(assimp_mesh, scene);
        my_mesh.init();
        meshes.push_back(my_mesh);
    }

    // Process child nodes recursively.
    for (std::size_t i = 0; i < node->mNumChildren; i++)
        process_node(node->mChildren[i], scene);
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Process vertices.
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        // Vertex positions.
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        // Vertex normals.
        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        // Vertex texture coordinates.
        if (mesh->mTextureCoords[0])
        {
            vertex.tex_coords.x = mesh->mTextureCoords[0][i].x;
            vertex.tex_coords.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.tex_coords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process indices.
    for (std::size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (std::size_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Process material.
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuse_maps = load_material_textures(material,
            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(std::end(textures), std::begin(diffuse_maps),
            std::end(diffuse_maps));

        // std::vector<Texture> specular_maps = load_material_textures(material,
        //     aiTextureType_SPECULAR, "texture_specular");
        // textures.insert(std::end(textures), std::begin(specular_maps),
        //     std::end(specular_maps));
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_textures(aiMaterial* material,
    aiTextureType type, std::string type_name)
{
    std::vector<Texture> textures;
    for (std::size_t i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);

        bool skip = false;

        for (std::size_t j = 0; j < loaded_textures.size(); j++)
        {
            if (std::strcmp(loaded_textures[j].path.c_str(), str.C_Str()) == 0)
            {
                textures.push_back(loaded_textures[j]);
                skip = true;
                break;
            }
        }

        // Load textures that have not already been loaded.
        if (!skip)
        {
            Texture texture;
            std::filesystem::path texture_path = directory / str.C_Str();

            std::cout << "Loading texture from " << texture_path << '\n';
            texture.id = load_texture_from_file(texture_path);
            texture.type = type_name;
            texture.path = str.C_Str();
            textures.push_back(texture);
            loaded_textures.push_back(texture);
        }
    }
    return textures;
}

#endif /* MODEL_HPP */
