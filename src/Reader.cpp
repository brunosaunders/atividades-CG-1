#include "Reader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
using namespace atividades_cg_1::reader;

vector<string> custom_split(string str, char separator) {
    vector<string> strings;
    int startIndex = 0, endIndex = 0;
    for (int i = 0; i <= str.size(); i++) {
        
        // If we reached the end of the word or the end of the input.
        if (str[i] == separator || i == str.size()) {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            strings.push_back(temp);
            startIndex = endIndex + 1;
        }
    }
    return strings;
}

Mesh* ObjReader::read_obj_file(string file_path)
{
    std::ifstream file(file_path);
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v")
        {
            float x; float y; float z;
            iss >> x >> y >> z;
            this->vertices.push_back(Vector3d(x,y,z));
        }
        else if (type == "f")
        {
            vector<Vector3d> face_vertices;

            FourPointsFace f;
            int vertix_index;

            std::string output;

            while (iss >> output) {
                vertix_index = std::stoi(custom_split(output, '/')[0]) - 1;
                face_vertices.push_back(this->vertices[vertix_index]);
            }
            this->faces.push_back(FourPointsFace(face_vertices[0], face_vertices[1], face_vertices[2], face_vertices[3]));
        }
    }
    return new Mesh(this->faces);
}

Mesh* ObjFactory::create_cube() {
    ObjReader reader;
    Mesh* mesh = reader.read_obj_file("../blender/cube.obj");

    Matrix translation_matrix = MatrixTransformations::translation(0,30,-50);
    Matrix scale_matrix = MatrixTransformations::scale(mesh->get_center(), 30,30,30);
    // mesh->apply_rotation_transformation(M_PI/6, Y_AXIS);
    // mesh->apply_rotation_transformation(M_PI/6, X_AXIS);
    mesh->apply_transformation(scale_matrix);
    mesh->apply_transformation(translation_matrix);
    mesh->get_center().print();
    // mesh->set_color(Color(180,20,20));
    return mesh;
}