// EECS 3431 Assignment 3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include "glm-master/glm/glm.hpp"
#include "glm-master/glm/ext.hpp"
#include "ppm.cpp"
#include <vector>
#define max 50
 

using namespace std;


 

struct Sphere
{
    glm::mat4 transMatrix;
    glm::mat4 invMatrix;
    string name;
    glm::vec4 position;
    glm::vec3 scale;
    glm::vec3 color;
    float kA; 
    float kD; 
    float kS;
    float kR;
    int n;

};


struct Light
{
    string name;
    glm::vec4 position;
    glm::vec3 intensity;
};



struct Inputs
{

    float near;
    float left;
    float right;
    float top;
    float bottom;
    int resX;
    int resY;
    glm::vec3 bkg;
    glm::vec3 ambience;
    string output;
    vector<Sphere> spheres;
    vector<Light> lights;

};

Inputs inputs;
string strings[max];

FILE* fp;
char* filename;
char ch;

ifstream input;
string line;
string inputName;
vector <string> broken;

// Method for splitting each string of characters in the .txt file and storing them in a vector.
void split(string s)

{

    istringstream ss(s);
    string text;

    while (ss >> text)
    {
        broken.push_back(text);
        
    }
}


// Method for parsing the required data from the .txt file.
void parse(char* filename)
{

    input.open(filename);
    while (getline(input, line)) {
 
        split(line);
       
    }
    input.close();

    unsigned int brokenSize = broken.size();

   
    for (unsigned int i = 0; i < brokenSize; i++)
    {
        if (broken[i] == "NEAR")
        {

            inputs.near = stof(broken[i + 1]);
        }

        else if (broken[i] == "LEFT")
        {
            inputs.left = stof(broken[i + 1]);

        }

        else if (broken[i] == "RIGHT")
        {
            inputs.right = stof(broken[i + 1]);

        }

        else if (broken[i] == "BOTTOM")
        {
            inputs.bottom = stof(broken[i + 1]);

        }

        else if (broken[i] == "TOP")
        {
            inputs.top = stof(broken[i + 1]);

        }

        else if (broken[i] == "RES")
        {
            inputs.resX = stof(broken[i + 1]);
            inputs.resY = stof(broken[i + 2]);
        }
 

        else if (broken[i] == "SPHERE")
        {
            Sphere sphere;

            sphere.name = broken[i + 1];
            
            sphere.position[0] = stof(broken[i + 2]);
            sphere.position[1] = stof(broken[i + 3]);
            sphere.position[2] = stof(broken[i + 3]);
            sphere.position[3] = 0;

            sphere.scale.x = stof(broken[i + 4]);
            sphere.scale.y = stof(broken[i + 5]);
            sphere.scale.z = stof(broken[i + 6]);

            sphere.color.r = stof(broken[i + 7]);
            sphere.color.g = stof(broken[i + 8]);
            sphere.color.b = stof(broken[i + 9]);

            sphere.kA = stof(broken[i + 10]);
            sphere.kD = stof(broken[i + 11]);
            sphere.kS = stof(broken[i + 12]);
            sphere.kR = stof(broken[i + 13]);

            sphere.n = stof(broken[i + 14]);
            inputs.spheres.push_back(sphere);



        }

        else if (broken[i] == "LIGHT")
        {

            Light light;

            light.name = broken[i + 1];

            light.position[0] = stof(broken[i + 2]);
            light.position[1] = stof(broken[i + 3]);
            light.position[2] = stof(broken[i + 4]);
            light.position[3] = 0;

            light.intensity.r = stof(broken[i + 5]);
            light.intensity.g = stof(broken[i + 6]);
            light.intensity.b = stof(broken[i + 7]);

            inputs.lights.push_back(light);
              
        }

        else if (broken[i] == "BACK")
        {
            inputs.bkg.r = stof(broken[i + 1]);
            inputs.bkg.g = stof(broken[i + 2]);
            inputs.bkg.b = stof(broken[i + 3]);

        }

        else if (broken[i] == "AMBIENT")
        {
            inputs.ambience.r = stof(broken[i + 1]);
            inputs.ambience.g = stof(broken[i + 2]);
            inputs.ambience.b = stof(broken[i + 3]);

        }

        else if (broken[i] == "OUTPUT")
        {
        
            inputs.output = broken[i + 1];
        }


    }
    
}

bool intersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, vector<Sphere> spheres) {
    return false;
}

void shootRays(unsigned char* pixels, int resX, int resY, const glm::vec3 &bgColor, const glm::vec3 &ambientIntensity, vector<Light> lights, vector<Sphere> spheres) {
    //camera parameters
    float NEAR = inputs.near;
    float LEFT = inputs.left;
    float RIGHT = inputs.right;
    float BOTTOM = inputs.bottom;
    float TOP = inputs.top;
    //loop through pixels
    for (int y = 0; y < resY; ++y) {
        for (int x = 0; x < resX; ++x) {
            float u = LEFT + (RIGHT - LEFT) * ((x) / float(resX));
            float v = BOTTOM + (NEAR - BOTTOM) * ((y)  / float(resY));

            glm::vec3 rayOrigin(0.0f, 0.0f, 0.0f);
            glm::vec3 rayDirection(u, v, -NEAR);
            rayDirection = glm::normalize(rayDirection);

            if(intersect(rayOrigin, rayDirection, spheres)) {

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

int main(int argc, char* argv[])
{
    if (argc < 2)

    {
        printf("Missing test case.\n");
        return(1);
    }

    else
    {
        filename = argv[1];
    }

    parse(filename);

    unsigned char* pixels = new unsigned char[inputs.resX * inputs.resY * 3];

    shootRays(pixels, inputs.resX, inputs.resY, inputs.bkg, inputs.ambience, inputs.lights, inputs.spheres);
    char filename[] = "test_output.ppm";
    save_imageP3(inputs.resX, inputs.resY, filename, pixels);
 

 
    return 0;
     
     
}