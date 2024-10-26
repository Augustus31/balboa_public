#include "hw1.h"
#include "hw1_scenes.h"
#include "matrix.h"
#include <cmath>
#include <iostream>

using namespace hw1;
Vector2 screen_to_object(Vector2 inp, Matrix3x3 transform);
Matrix3x3 circleTransform(Vector2 center, Real radius);
Image3 fill_color(Shape s, Image3 img, bool antialias, bool alpha);

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

    Image3 img(640 /* width */, 480 /* height */);

    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector2 inp;
            inp.x = x+0.5;
            inp.y = y+0.5;
            Vector2 objectCoords = screen_to_object(inp, circleTransform(center, radius));
            if(pow(objectCoords.x, 2) + pow(objectCoords.y, 2) < 1){
                img(x,y) = color;
            }
            else{
                img(x,y) = Vector3{0.5,0.5,0.5};
            }
        }
    }
    return img;
}

Vector2 screen_to_object(Vector2 inp, Matrix3x3 transform){
    Matrix3x3 inv = inverse(transform);
    Vector3 inp3;
    inp3.x = inp.x;
    inp3.y = inp.y;
    inp3.z = 1;
    Vector3 ret3 = inv*inp3;
    Vector2 ret;
    ret.x = ret3.x;
    ret.y = ret3.y;
    return ret;
}

Matrix3x3 circleTransform(Vector2 center, Real radius){
    Matrix3x3 transform;
    transform(0,0) = radius;
    transform(1,1) = radius;
    transform(0,2) = center.x;
    transform(1,2) = center.y;
    transform(2,2) = Real(1);
    return transform;
}

Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render multiple circles
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    const CircleScene &scene = hw1_2_scenes[scene_id];
    Vector2 res = scene.resolution;
    Vector3 bg = scene.background;

    Image3 img(res.x, res.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = bg;
        }
    }

    for(Circle cir : scene.objects){
        Vector4 bounds{cir.center.x-cir.radius, cir.center.x+cir.radius, cir.center.y-cir.radius, cir.center.y+cir.radius};
        for(int x = max(bounds.x, Real(0)); x <= min(res.x-1, bounds.y); x++){
            for(int y = max(bounds.z, Real(0)); y <= min(res.y-1, bounds.w); y++){
                Vector2 inp{x+0.5,y+0.5};
                Vector2 objectCoords = screen_to_object(inp, circleTransform(cir.center, cir.radius));
                if(pow(objectCoords.x, 2) + pow(objectCoords.y, 2) < 1){
                    img(x,y) = cir.color;
                }
            }
        }
    }
    return img;
}

Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: render multiple shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Vector2 res = scene.resolution;
    Vector3 bg = scene.background;

    Image3 img(res.x, res.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = bg;
        }
    }

    for(Shape s : scene.shapes){
        if (auto *circle = std::get_if<Circle>(&s)) {
            Circle &cir = *circle;
            Vector4 bounds{cir.center.x-cir.radius, cir.center.x+cir.radius, cir.center.y-cir.radius, cir.center.y+cir.radius};
            for(int x = max(bounds.x, Real(0)); x <= min(res.x-1, bounds.y); x++){
                for(int y = max(bounds.z, Real(0)); y <= min(res.y-1, bounds.w); y++){
                    Vector2 inp{x+0.5,y+0.5};
                    Vector2 objectCoords = screen_to_object(inp, circleTransform(cir.center, cir.radius));
                    if(pow(objectCoords.x, 2) + pow(objectCoords.y, 2) < 1){
                        img(x,y) = cir.color;
                    }
                }
            }
        } else if (auto *rectangle = std::get_if<Rectangle>(&s)) {
            Rectangle &rect = *rectangle;
            for(int x = max(rect.p_min.x, Real(0)); x <= min(res.x-1, rect.p_max.x); x++){
                for(int y = max(rect.p_min.y, Real(0)); y <= min(res.y-1, rect.p_max.y); y++){
                    if(x+0.5 >= rect.p_min.x && x+0.5 <= rect.p_max.x && y+0.5 >= rect.p_min.y && y+0.5 <= rect.p_max.y){
                        img(x,y) = rect.color;
                    }
                }
            }

        } else if (auto *triangle = std::get_if<Triangle>(&s)) {
            Triangle &tri = *triangle;
            Vector4 bounds{min(min(tri.p0.x, tri.p1.x), tri.p2.x), max(max(tri.p0.x, tri.p1.x), tri.p2.x), min(min(tri.p0.y, tri.p1.y), tri.p2.y), max(max(tri.p0.y, tri.p1.y), tri.p2.y)};
            for(int x = max(bounds.x, Real(0)); x <= min(res.x-1, bounds.y); x++){
                for(int y = max(bounds.z, Real(0)); y <= min(res.y-1, bounds.w); y++){
                    Vector2 inp{x+0.5,y+0.5};
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
                        img(x,y) = tri.color;
                    }
                }
            }
        }
    }
    return img;
}

Image3 fill_color(Shape s, Image3 img, bool antialias, bool alpha){
    Vector2 res{img.width, img.height};
    if (auto *circle = std::get_if<Circle>(&s)) {
        Circle &cir = *circle;
        for(int x = 0; x < res.x; x++){
            for(int y = 0; y < res.y; y++){
                if(antialias){
                    int counter = 0;
                    for(int i = 0; i < 4; i++){
                        for(int j = 0; j < 4; j++){
                            Vector3 inpScreen{Real(x) + 0.125 + 0.25*i,Real(y) + 0.125 + 0.25*j,Real(1)};
                            Vector3 inp3 = inverse(cir.transform)*inpScreen;
                            Vector2 inp{inp3.x,inp3.y};
                            if(pow(inp.x-cir.center.x, 2) + pow(inp.y-cir.center.y, 2) < pow(cir.radius,2)){
                                counter++;
                            }
                        }
                    }
                    Vector3 newCol = cir.color * Real((counter/16.0)) + img(x,y) * Real(1 - (counter/16.0));
                    img(x,y) = alpha ? cir.alpha * newCol + (1-cir.alpha) * img(x,y) : newCol;
                }
                else{
                    Vector3 inpScreen{Real(x)+0.5,Real(y)+0.5,Real(1)};
                    Vector3 inp3 = inverse(cir.transform)*inpScreen;
                    Vector2 inp{inp3.x,inp3.y};
                    if(pow(inp.x-cir.center.x, 2) + pow(inp.y-cir.center.y, 2) < pow(cir.radius,2)){
                        img(x,y) = alpha ? cir.alpha * cir.color + (1-cir.alpha) * img(x,y) : cir.color;
                    }
                }     
            }
        }
    } else if (auto *rectangle = std::get_if<Rectangle>(&s)) {
        Rectangle &rect = *rectangle;
        for(int x = 0; x < res.x; x++){
            for(int y = 0; y < res.y; y++){
                if(antialias){
                    int counter = 0;
                    for(int i = 0; i < 4; i++){
                        for(int j = 0; j < 4; j++){
                            Vector3 inpScreen{Real(x) + 0.125 + 0.25*i,Real(y) + 0.125 + 0.25*j,Real(1)};
                            Vector3 inp3 = inverse(rect.transform)*inpScreen;
                            Vector2 inp{inp3.x,inp3.y};
                            if(inp.x >= rect.p_min.x && inp.x <= rect.p_max.x && inp.y >= rect.p_min.y && inp.y <= rect.p_max.y){
                                counter++;
                            }
                        }
                    }
                    Vector3 newCol = rect.color * Real((counter/16.0)) + img(x,y) * Real(1 - (counter/16.0));
                    img(x,y) = alpha ? rect.alpha * newCol + (1-rect.alpha) * img(x,y) : newCol;
                }
                else{
                    Vector3 inpScreen{Real(x)+0.5,Real(y)+0.5,Real(1)};
                    Vector3 inp3 = inverse(rect.transform)*inpScreen;
                    Vector2 inp{inp3.x,inp3.y};
                    if(inp.x >= rect.p_min.x && inp.x <= rect.p_max.x && inp.y >= rect.p_min.y && inp.y <= rect.p_max.y){
                        img(x,y) = alpha ? rect.alpha * rect.color + (1-rect.alpha) * img(x,y) : rect.color;
                    }
                }
            }
        }
    } else if (auto *triangle = std::get_if<Triangle>(&s)){
        Triangle &tri = *triangle;
        for(int x = 0; x < res.x; x++){
            for(int y = 0; y < res.y; y++){
                if(antialias){
                    int counter = 0;
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
                                counter++;
                            }
                        }
                    }
                    Vector3 newCol = tri.color * Real((counter/16.0)) + img(x,y) * Real(1 - (counter/16.0));
                    img(x,y) = alpha ? tri.alpha * newCol + (1-tri.alpha) * img(x,y) : newCol;
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
                        img(x,y) = alpha ? tri.alpha * tri.color + (1-tri.alpha) * img(x,y) : tri.color;
                    }
                }
            }
        }
    }
    return img;
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 bg = scene.background;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = bg;
        }
    }

    for(Shape s : scene.shapes){
        img = fill_color(s, img, false, false);
    }

    return img;
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 bg = scene.background;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = bg;
        }
    }

    for(Shape s : scene.shapes){
        img = fill_color(s, img, true, false);
    }

    return img;
}

Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 bg = scene.background;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = bg;
        }
    }

    for(Shape s : scene.shapes){
        img = fill_color(s, img, true, true);
    }

    return img;
}
