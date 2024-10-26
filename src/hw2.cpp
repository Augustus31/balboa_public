#include "hw2.h"
#include "hw2_scenes.h"
#include "hw1_scenes.h"

using namespace hw2;
Matrix4x4 createClipMatrix(double s, double a, double f, double near);
Image3 fill_color(hw1::Shape s, Image3 img, bool antialias, bool alpha);
Image3 draw_projected_triangle(hw1::Shape s, std::vector<Vector3> orig, Image3 img, Image<double>& zbuf, Image3& zcol, std::vector<Vector3> colors, bool antialias);


Matrix4x4 createClipMatrix(double s, double a, double f, double near){
    Matrix4x4 ret{f/(s*a), 0.0, 0.0, 0.0, 0.0, f/s, 0.0, 0.0, 0.0, 0.0, -1.0, -2*near, 0.0, 0.0, -1.0, 0.0};
    return ret;
}

Image3 draw_projected_triangle(hw1::Shape s, std::vector<Vector3> orig, Image3 img, Image<double>& zbuf, Image3& zcol, std::vector<Vector3> colors, bool antialias){
    Vector2 res{img.width, img.height};
    if (auto *triangle = std::get_if<hw1::Triangle>(&s)){
        hw1::Triangle &tri = *triangle;

        Real minX = min(min(tri.p0.x, tri.p1.x),tri.p2.x);
        Real maxX = max(max(tri.p0.x, tri.p1.x),tri.p2.x);
        Real minY = min(min(tri.p0.y, tri.p1.y),tri.p2.y);
        Real maxY = max(max(tri.p0.y, tri.p1.y),tri.p2.y);

        for(int x = max(0.0, minX); x < min(res.x, maxX); x++){
            for(int y = max(0.0, minY); y < min(res.y, maxY); y++){
                if(antialias){
                    int counter = 0;
                    Vector3 uncBg{0.5, 0.5, 0.5};
                    for(int i = 0; i < 4; i++){
                        for(int j = 0; j < 4; j++){
                            Vector3 inpScreen{Real(x) + 0.125 + 0.25*i,Real(y) + 0.125 + 0.25*j,Real(1)};
                            Vector3 inp3 = inverse(tri.transform)*inpScreen;
                            Vector2 inp{inp3.x,inp3.y};
                            Vector2 p01 = tri.p1 - tri.p0;
                            Vector2 n01{p01.y, -p01.x};
                            Vector2 pq0 = inp - tri.p0;
                            Vector2 p12 = tri.p2 - tri.p1;
                            Vector2 n12{p12.y, -p12.x};
                            Vector2 pq1 = inp - tri.p1;
                            Vector2 p20 = tri.p0 - tri.p2;
                            Vector2 n20{p20.y, -p20.x};
                            Vector2 pq2 = inp - tri.p2;

                            Real dot1 = dot(pq0, n01);
                            Real dot2 = dot(pq1, n12);
                            Real dot3 = dot(pq2, n20);
                            if((dot1 < 0 && dot2 < 0 && dot3 < 0) || (dot1 > 0 && dot2 > 0 && dot3 > 0)){
                                //do evaluation of depth here
                                Vector3 p03{tri.p0.x, tri.p0.y, Real(1)};
                                Vector3 p13{tri.p1.x, tri.p1.y, Real(1)};
                                Vector3 p23{tri.p2.x, tri.p2.y, Real(1)};
                                Vector3 p3 = inpScreen;

                                Real b0p = length((cross((p23 - p3), (p13 - p3))))/length((cross((p23 - p03), (p13 - p03))));
                                Real b1p = length((cross((p23 - p03), (p3 - p03))))/length((cross((p23 - p03), (p13 - p03))));
                                Real b2p = length((cross((p3 - p03), (p13 - p03))))/length((cross((p23 - p03), (p13 - p03))));

                                Real B = b0p/orig[0].z + b1p/orig[1].z + b2p/orig[2].z;

                                Real b0 = b0p/orig[0].z/B;
                                Real b1 = b1p/orig[1].z/B;
                                Real b2 = b2p/orig[2].z/B;

                                Real depth = b0 * orig[0].z + b1 * orig[1].z + b2 * orig[2].z;

                                if(zbuf(4*x+i, 4*y+j) < depth){
                                    zbuf(4*x+i, 4*y+j) = depth;
                                    if(colors.size() == 0){
                                        zcol(4*x+i, 4*y+j) = tri.color;
                                    }
                                    else{
                                        zcol(4*x+i, 4*y+j) = b0 * colors[0] + b1 * colors[1] + b2 * colors[2];
                                    }
                                }
                                else{
                                    if(counter == 0){
                                        uncBg = zcol(4*x+i, 4*y+j);
                                    }
                                    else{
                                        uncBg = (uncBg * Real(counter) + zcol(4*x+i, 4*y+j))/(Real(counter)+1);
                                    }
                                    counter++;
                                }
                            }
                            else{
                                if(counter == 0){
                                    uncBg = zcol(4*x+i, 4*y+j);
                                }
                                else{
                                    uncBg = (uncBg * Real(counter) + zcol(4*x+i, 4*y+j))/(Real(counter)+1);
                                }
                                counter++;
                            }
                        }
                    }

                    Vector3 newCol;
                    if(colors.size() == 0){
                        newCol = tri.color * Real(1 - (counter/16.0)) + uncBg * Real((counter/16.0));
                    }
                    else{
                        if(counter == 16){
                            newCol = img(x,y);
                        }
                        else{
                            Vector3 p03{tri.p0.x, tri.p0.y, Real(1)};
                            Vector3 p13{tri.p1.x, tri.p1.y, Real(1)};
                            Vector3 p23{tri.p2.x, tri.p2.y, Real(1)};
                            Vector3 p3{Real(x+0.5), Real(y+0.5), Real(-1)};

                            Real b0p = length((cross((p23 - p3), (p13 - p3))))/length((cross((p23 - p03), (p13 - p03))));
                            Real b1p = length((cross((p23 - p03), (p3 - p03))))/length((cross((p23 - p03), (p13 - p03))));
                            Real b2p = length((cross((p3 - p03), (p13 - p03))))/length((cross((p23 - p03), (p13 - p03))));

                            Real B = b0p/orig[0].z + b1p/orig[1].z + b2p/orig[2].z;

                            Real b0 = b0p/orig[0].z/B;
                            Real b1 = b1p/orig[1].z/B;
                            Real b2 = b2p/orig[2].z/B;

                            Vector3 intCol = b0 * colors[0] + b1 * colors[1] + b2 * colors[2];
                            newCol = intCol * Real(1 - (counter/16.0)) + uncBg * Real((counter/16.0));
                        }
                    }
                    img(x,y) = newCol;
                }
                else{
                    Vector3 inpScreen{Real(x)+0.5,Real(y)+0.5,Real(1)};
                    Vector3 inp3 = inverse(tri.transform)*inpScreen;
                    Vector2 inp{inp3.x,inp3.y};
                    Vector2 p01 = tri.p1 - tri.p0;
                    Vector2 n01{p01.y, -p01.x};
                    Vector2 pq0 = inp - tri.p0;
                    Vector2 p12 = tri.p2 - tri.p1;
                    Vector2 n12{p12.y, -p12.x};
                    Vector2 pq1 = inp - tri.p1;
                    Vector2 p20 = tri.p0 - tri.p2;
                    Vector2 n20{p20.y, -p20.x};
                    Vector2 pq2 = inp - tri.p2;

                    Real dot1 = dot(pq0, n01);
                    Real dot2 = dot(pq1, n12);
                    Real dot3 = dot(pq2, n20);

                    if((dot1 < 0 && dot2 < 0 && dot3 < 0) || (dot1 > 0 && dot2 > 0 && dot3 > 0)){
                        //do evaluation of depth here
                        Vector3 p03{tri.p0.x, tri.p0.y, Real(1)};
                        Vector3 p13{tri.p1.x, tri.p1.y, Real(1)};
                        Vector3 p23{tri.p2.x, tri.p2.y, Real(1)};
                        Vector3 p3 = inpScreen;

                        Real b0p = length((cross((p23 - p3), (p13 - p3))))/length((cross((p23 - p03), (p13 - p03))));
                        Real b1p = length((cross((p23 - p03), (p3 - p03))))/length((cross((p23 - p03), (p13 - p03))));
                        Real b2p = length((cross((p3 - p03), (p13 - p03))))/length((cross((p23 - p03), (p13 - p03))));

                        Real B = b0p/orig[0].z + b1p/orig[1].z + b2p/orig[2].z;

                        Real b0 = b0p/orig[0].z/B;
                        Real b1 = b1p/orig[1].z/B;
                        Real b2 = b2p/orig[2].z/B;

                        Real depth = b0 * orig[0].z + b1 * orig[1].z + b2 * orig[2].z;

                        if(zbuf(x, y) < depth){
                            zbuf(x, y) = depth;

                            if(colors.size() == 0){
                                img(x,y) = tri.color;
                            }
                            else{
                                Vector3 newCol = b0 * colors[0] + b1 * colors[1] + b2 * colors[2];
                                img(x,y) = newCol;
                            }
                        }
                    }
                }
            }
        }
    }
    return img;
}

Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    Vector4 p0h{p0.x, p0.y, p0.z, 1.0};
    Vector4 p1h{p1.x, p1.y, p1.z, 1.0};
    Vector4 p2h{p2.x, p2.y, p2.z, 1.0};

    Matrix4x4 clip = createClipMatrix(s, (double)img.width/img.height, 1.0, z_near);
    Vector4 p0Clip = clip * p0h;
    Vector4 p1Clip = clip * p1h;
    Vector4 p2Clip = clip * p2h;

    Vector4 p0Norm = p0Clip/p0Clip.w;
    Vector4 p1Norm = p1Clip/p1Clip.w;
    Vector4 p2Norm = p2Clip/p2Clip.w;

    if(p0Norm.z <= 1 && p0Norm.z >= -1 && p1Norm.z <= 1 && p1Norm.z >= -1 && p2Norm.z <= 1 && p2Norm.z >= -1){
        Vector2 p0Screen{(img.width * (p0Norm.x + 1.0))/2.0, (img.height * (1.0 - p0Norm.y))/2.0};
        Vector2 p1Screen{(img.width * (p1Norm.x + 1.0))/2.0, (img.height * (1.0 - p1Norm.y))/2.0};
        Vector2 p2Screen{(img.width * (p2Norm.x + 1.0))/2.0, (img.height * (1.0 - p2Norm.y))/2.0};
        hw1::Triangle tri{p0Screen, p1Screen, p2Screen, color, Real(1), Matrix3x3::identity()};
        img = fill_color(tri, img, true, false);
    }



    return img;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }
    // silence warnings, feel free to remove these
    UNUSED(s); 
    UNUSED(z_near);
    UNUSED(scene_id);

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    Image<double> zbuf(img.width * 4, img.height * 4);
    Image3 zcol(img.width * 4, img.height * 4);
    Vector3 bg{0.5, 0.5, 0.5};

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = bg;
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    zbuf(x*4 + i, y*4 + j) = -INFINITY;
                    zcol(x*4 + i, y*4 + j) = bg;
                }
            }
        }
    }

    for(int i = 0; i < mesh.faces.size(); i++){
        Vector3 tri = mesh.faces[i];
        Vector3 color = mesh.face_colors[i];

        Vector3 p0 = mesh.vertices[tri[0]];
        Vector3 p1 = mesh.vertices[tri[1]];
        Vector3 p2 = mesh.vertices[tri[2]];

        Vector4 p0h{p0.x, p0.y, p0.z, 1.0};
        Vector4 p1h{p1.x, p1.y, p1.z, 1.0};
        Vector4 p2h{p2.x, p2.y, p2.z, 1.0};

        Matrix4x4 clip = createClipMatrix(s, (double)img.width/img.height, 1.0, z_near);
        Vector4 p0Clip = clip * p0h;
        Vector4 p1Clip = clip * p1h;
        Vector4 p2Clip = clip * p2h;

        Vector4 p0Norm = p0Clip/p0Clip.w;
        Vector4 p1Norm = p1Clip/p1Clip.w;
        Vector4 p2Norm = p2Clip/p2Clip.w;

        if(p0Norm.z <= 1 && p0Norm.z >= -1 && p1Norm.z <= 1 && p1Norm.z >= -1 && p2Norm.z <= 1 && p2Norm.z >= -1){
            Vector2 p0Screen{(img.width * (p0Norm.x + 1.0))/2.0, (img.height * (1.0 - p0Norm.y))/2.0};
            Vector2 p1Screen{(img.width * (p1Norm.x + 1.0))/2.0, (img.height * (1.0 - p1Norm.y))/2.0};
            Vector2 p2Screen{(img.width * (p2Norm.x + 1.0))/2.0, (img.height * (1.0 - p2Norm.y))/2.0};
            std::vector<Vector3> vec{p0, p1, p2};
            std::vector<Vector3> cor;
            hw1::Triangle tri{p0Screen, p1Screen, p2Screen, color, Real(1), Matrix3x3::identity()};
            img = draw_projected_triangle(tri, vec, img, zbuf, zcol, cor, true);
        }
    }

    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }
    // silence warnings, feel free to remove these
    UNUSED(s); 
    UNUSED(z_near);
    UNUSED(scene_id);

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    Image<double> zbuf(img.width * 4, img.height * 4);
    Image3 zcol(img.width * 4, img.height * 4);
    Vector3 bg{0.5, 0.5, 0.5};

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = bg;
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    zbuf(x*4 + i, y*4 + j) = -INFINITY;
                    zcol(x*4 + i, y*4 + j) = bg;
                }
            }
        }
    }

    for(int i = 0; i < mesh.faces.size(); i++){
        Vector3 tri = mesh.faces[i];
        Vector3 color = mesh.face_colors[i];

        Vector3 p0 = mesh.vertices[tri[0]];
        Vector3 p1 = mesh.vertices[tri[1]];
        Vector3 p2 = mesh.vertices[tri[2]];

        Vector3 c0 = mesh.vertex_colors[tri[0]];
        Vector3 c1 = mesh.vertex_colors[tri[1]];
        Vector3 c2 = mesh.vertex_colors[tri[2]];

        Vector4 p0h{p0.x, p0.y, p0.z, 1.0};
        Vector4 p1h{p1.x, p1.y, p1.z, 1.0};
        Vector4 p2h{p2.x, p2.y, p2.z, 1.0};

        Matrix4x4 clip = createClipMatrix(s, (double)img.width/img.height, 1.0, z_near);
        Vector4 p0Clip = clip * p0h;
        Vector4 p1Clip = clip * p1h;
        Vector4 p2Clip = clip * p2h;

        Vector4 p0Norm = p0Clip/p0Clip.w;
        Vector4 p1Norm = p1Clip/p1Clip.w;
        Vector4 p2Norm = p2Clip/p2Clip.w;

        if(p0Norm.z <= 1 && p0Norm.z >= -1 && p1Norm.z <= 1 && p1Norm.z >= -1 && p2Norm.z <= 1 && p2Norm.z >= -1){
            Vector2 p0Screen{(img.width * (p0Norm.x + 1.0))/2.0, (img.height * (1.0 - p0Norm.y))/2.0};
            Vector2 p1Screen{(img.width * (p1Norm.x + 1.0))/2.0, (img.height * (1.0 - p1Norm.y))/2.0};
            Vector2 p2Screen{(img.width * (p2Norm.x + 1.0))/2.0, (img.height * (1.0 - p2Norm.y))/2.0};
            std::vector<Vector3> vec{p0, p1, p2};
            std::vector<Vector3> cor{c0, c1, c2};
            hw1::Triangle tri{p0Screen, p1Screen, p2Screen, color, Real(1), Matrix3x3::identity()};
            img = draw_projected_triangle(tri, vec, img, zbuf, zcol, cor, true);
        }
    }
    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);
    Image<double> zbuf(img.width * 4, img.height * 4);
    Image3 zcol(img.width * 4, img.height * 4);
    
    Real s = scene.camera.s;
    Real z_near = scene.camera.z_near;
    Vector3 bg = scene.background;

    Matrix4x4 clip = createClipMatrix(s, (double)img.width/img.height, 1.0, z_near);
    Matrix4x4 view = inverse(scene.camera.cam_to_world);

    for(TriangleMesh mesh : scene.meshes){

        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                img(x, y) = bg;
                for(int i = 0; i < 4; i++){
                    for(int j = 0; j < 4; j++){
                        zbuf(x*4 + i, y*4 + j) = -INFINITY;
                        zcol(x*4 + i, y*4 + j) = bg;
                    }
                }
            }
        }

        for(int i = 0; i < mesh.faces.size(); i++){
            Vector3 tri = mesh.faces[i];

            Vector3 p0 = mesh.vertices[tri[0]];
            Vector3 p1 = mesh.vertices[tri[1]];
            Vector3 p2 = mesh.vertices[tri[2]];

            Vector3 c0 = mesh.vertex_colors[tri[0]];
            Vector3 c1 = mesh.vertex_colors[tri[1]];
            Vector3 c2 = mesh.vertex_colors[tri[2]];

            Vector4 p0h{p0.x, p0.y, p0.z, 1.0};
            Vector4 p1h{p1.x, p1.y, p1.z, 1.0};
            Vector4 p2h{p2.x, p2.y, p2.z, 1.0};

            Matrix4x4 model = mesh.model_matrix;

            Vector4 p0Clip = (clip * (view * (model * p0h)));
            Vector4 p1Clip = (clip * (view * (model * p1h)));
            Vector4 p2Clip = (clip * (view * (model * p2h)));

            Vector4 p0Norm = p0Clip/p0Clip.w;
            Vector4 p1Norm = p1Clip/p1Clip.w;
            Vector4 p2Norm = p2Clip/p2Clip.w;

            if(p0Norm.z <= 1 && p0Norm.z >= -1 && p1Norm.z <= 1 && p1Norm.z >= -1 && p2Norm.z <= 1 && p2Norm.z >= -1){
                Vector2 p0Screen{(img.width * (p0Norm.x + 1.0))/2.0, (img.height * (1.0 - p0Norm.y))/2.0};
                Vector2 p1Screen{(img.width * (p1Norm.x + 1.0))/2.0, (img.height * (1.0 - p1Norm.y))/2.0};
                Vector2 p2Screen{(img.width * (p2Norm.x + 1.0))/2.0, (img.height * (1.0 - p2Norm.y))/2.0};
                std::vector<Vector3> vec{p0, p1, p2};
                std::vector<Vector3> cor{c0, c1, c2};
                hw1::Triangle tri{p0Screen, p1Screen, p2Screen, Vector3(), Real(1), Matrix3x3::identity()};
                img = draw_projected_triangle(tri, vec, img, zbuf, zcol, cor, true);
            }
        }
    }

    
    return img;
}

