#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include "glm-master/glm/glm.hpp"
#include "glm-master/glm/ext.hpp"
#include "ppm.cpp"
#include <vector>


using namespace std;


struct Sphere
{
    string name;
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 scale;
    float kA; 
    float kD; 
    float kS;
    float kR;
    int n;

};

struct Light
{
    string name;
    glm::vec3 position;
    glm::vec3 intensity;
};

bool intersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Sphere &sphere) {
    return false;
}

void shootRays(unsigned char* pixels, int resX, int resY, const glm::vec3 &bgColor, const glm::vec3 &ambientIntensity, const Light &light, const Sphere &sphere) {
    //camera parameters
    float NEAR = 1.0f;
    float LEFT = -1.0f;
    float RIGHT = 1.0f;
    float BOTTOM = -1.0f;
    float TOP = 1.0f;
    //loop through pixels
    for (int y = 0; y < resY; ++y) {
        for (int x = 0; x < resX; ++x) {
            float u = LEFT + (RIGHT - LEFT) * ((x) / float(resX));
            float v = BOTTOM + (NEAR - BOTTOM) * ((y)  / float(resY));

            glm::vec3 rayOrigin(0.0f, 0.0f, 0.0f);
            glm::vec3 rayDirection(u, v, -NEAR);
            rayDirection = glm::normalize(rayDirection);

            if(intersect(rayOrigin, rayDirection, sphere)) {

            }
            //set pixel to background color
            else {
                pixels[(y * resX + x) * 3 + 0] = (unsigned char)(bgColor[0] * 255.0f);
                pixels[(y * resX + x) * 3 + 1] = (unsigned char)(bgColor[1] * 255.0f);
                pixels[(y * resX + x) * 3 + 2] = (unsigned char)(bgColor[2] * 255.0f);
            }
        }
    }

}

int main(){

    //all the inputs 
    const int resX = 600, resY = 600;
    glm::vec3 ambientLightIntensity = {0.75f, 0.75f, 0.75f};
    glm::vec3 bgColor = {1.0f, 1.0f, 1.0f};

    unsigned char* pixels = new unsigned char[resX * resY * 3];

    Light light = {"l1", {-5.0f, 10.0f, 5.0f}, {1.0f, 1.0f, 1.0f}};

    Sphere s1 = {"s1", {0.0f, 0.0f, -10.0f}, {2.0f, 4.0f, 2.0f}, {0.5f, 0.0f, 0.0f}, 1.0f, 0.0f, 0.0f, 0.0f, 50};
    /*
    Sphere spheres[] = {
        {"s1", {0.0f, 0.0f, -10.0f}, {20.0f, 4.0f, 2.0f}, {0.5f, 0.0f, 0.0f}, 1.0f, 0.0f, 0.0f, 0.0f, 50.0f}, // Red sphere
        {"s2", {4.0f, 4.0f, -10.0f}, {1.0f, 2.0f, 1.0f}, {0.0f, 0.5f, 0.0f}, 1.0f, 0.0f, 0.0f, 0.0f, 50.0f},
        {"s3", {-4.0f, 2.0f, -10.0f}, {1.0f, 2.0f, 1.0f}, {0.0f, 0.0f, 0.5f}, 1.0f, 0.0f, 0.0f, 0.0f, 50.0f}  

    }; */

    shootRays(pixels, resX, resY, bgColor, ambientLightIntensity, light, s1);
    char filename[] = "test_output.ppm";
    save_imageP3(resX, resY, filename, pixels);

    delete[] pixels;
    return 0;


}