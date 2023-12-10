#ifndef READER_H
#define READER_H

#include <string>
#include <vector>

#include "Objects.hpp"

using namespace std;
using namespace atividades_cg_1::objects;
using namespace atividades_cg_1::algebra;

namespace atividades_cg_1::reader 
{
    class ObjReader {
        public:
            vector<Vector3d> vertices;
            vector<FourPointsFace> faces;

            Mesh* read_obj_file(std::string file_path);
    };

    class ObjFactory {
        public:
            static Mesh* create_cube();
    };
} // atividades_cg1::reader 


#endif