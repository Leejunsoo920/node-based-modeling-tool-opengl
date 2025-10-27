#include "model.h"











unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        //std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


Model::Model(std::string const& path, bool gamma) : gammaCorrection(gamma)
{

    loadModel(path);
    if (meshes[0].vertices.size() == 0)
        return;
    center_cx(meshes[0], this->position);

    //func_list.push_back(&Meshoption::do_update);


}

Model::Model(const Model& other)
{
    this->meshes = other.meshes;
    this->origin_meshes = other.meshes;
    textures_loaded = other.textures_loaded;
    directory = other.directory;
    gammaCorrection = other.gammaCorrection;

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].setup_Mesh();
    }

}





