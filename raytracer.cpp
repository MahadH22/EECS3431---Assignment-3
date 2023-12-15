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
#define MAX 50
 

using namespace std;


 

struct Sphere
{
    string name;
    glm::vec4 position;
    glm::vec3 scale;
    glm::vec3 color;
    float kA; 
    float kD; 
    float kS;
    float kR;
    int n;
    glm::mat4 transMatrix;
    glm::mat4 invMatrix;


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
string strings[MAX];

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

float intersect(glm::vec4 rayOrigin, glm::vec3 rayDirection, const Sphere &sphere) {
    //t is negative if there is no intersection
    float t = -1.0f;
    //Use the sphere's inverseMatrix transformation to bring the ray relative to the sphere
    glm::vec4 rayOriginSphere = sphere.invMatrix * rayOrigin;
    glm::vec4 rayDirectionSphere = sphere.invMatrix * glm::vec4(rayDirection, 0.0f);

    //Calcualte A (ray direction * ray direction) from lecture notes
    float A = glm::dot(glm::vec3(rayDirectionSphere), glm::vec3(rayDirectionSphere));
    //Calculate B (ray origin * ray direction) from lecture notes
    float B = 2.0f * glm::dot(glm::vec3(rayOriginSphere), glm::vec3(rayDirectionSphere));
    //Calculate C (ray origin * ray origin) from lecture notes
    float C = glm::dot(glm::vec3(rayOriginSphere), glm::vec3(rayOriginSphere)) - 1.0f;  // Sphere radius is 1 in local space
    //Calculate Discriminant: Formula used is b^2 - 4ac
    float discriminant = (B * B) - (4 * A * C);

    //Calculating the 2 roots: Formula used is (-b +/- sqrt(discriminant)) / 2a
    if(discriminant >= 0) {
        float t0 = (-B - sqrt(discriminant)) / (2.0f * A);
        float t1 = (-B + sqrt(discriminant)) / (2.0f * A);
        if (t0 < t1) {
            t = t0;
        } else {
            t = t1;
        }
    }

    return t;
}
void calculateTransformationMatrices(vector<Sphere>& spheres) {
    //Identity matrix
    const glm::mat4 identity = glm::mat4(1.f);

    //Loop through all the spheres
    for (Sphere& sphere : spheres) {
        // Create the transformation matrix 
        //Formula M = Translation * Scale * Rotation. Assuming no rotation here.
        sphere.transMatrix = glm::translate(identity, glm::vec3(sphere.position)) * glm::scale(identity, glm::vec3(sphere.scale));
        // Inverse of the transformation matrix, just using glm library
        sphere.invMatrix = glm::inverse(sphere.transMatrix);
    }
}

glm::vec3 phongColorCalculation(glm::vec4 &intersectionPoint, const Sphere &sphere, std::vector<Light> &lights, const glm::vec3 &ambientIntensity) {
    // Calculate ambient color
    glm::vec3 ambientColor = sphere.color * sphere.kA * ambientIntensity;
    //Add the ambient color
    glm::vec3 color = ambientColor;

    //Normal vector at the intersection point
    glm::vec3 normal = intersectionPoint - sphere.position;
    //Scale the normal vector according to the sphere's scale
    normal = normal / (sphere.scale * sphere.scale);
    //Normalize the vector
    normal = glm::normalize(normal);

    //Vector starting from intersection point looking back at the camera, which the camera is always at (0,0,0) so just add a negative to intersectionPoint vector.
    glm::vec3 viewDir = -glm::normalize(intersectionPoint);


    for (const Light& light : lights) {
        //Normalized vector starting from the intersection point to the light source
        glm::vec3 lightDir = glm::normalize(light.position - intersectionPoint);

        // Diffuse component
        float diff = std::max(glm::dot(normal, lightDir), 0.0f);
        glm::vec3 diffuseColor = sphere.color * sphere.kD * diff * light.intensity;

        // Specular component
        glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
        float spec = pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), sphere.n);
        glm::vec3 specularColor = sphere.kS * spec * light.intensity;

        //Add all the colors together
        color += diffuseColor + specularColor;
    }

    // Bring color range to 0, 1 using glm library. 
    color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
    return color; 
}

void startRayTrace(unsigned char* pixels, int resX, int resY, const glm::vec3 &bgColor, const glm::vec3 &ambientIntensity, vector<Light> &lights, vector<Sphere> &spheres) {
    //camera parameters
    float NEAR = inputs.near;
    float LEFT = inputs.left;
    float RIGHT = inputs.right;
    float BOTTOM = inputs.bottom;
    float TOP = inputs.top;

     //loop through all the pixels to "shoot" out rays
    for (int y = 0; y < resY; ++y) {
        for (int x = 0; x < resX; ++x) {

            //Normalized device coordinates of the pixel
            float u = ((2.0f * x / resX) - 1.0f);
            float v = ((2.0f * y / resY) - 1.0f);
            v *= -1; //invert the y axis 

            //the camera is assumed to be at 0,0,0 at all times, so ray origin will always be 0,0,0
            glm::vec4 rayOrigin(0.0f, 0.0f, 0.0f, 1.0f);
            
            //Normalized ray direction vector toward the near plane 
            glm::vec3 rayDirection(u, v, -NEAR);
            rayDirection = glm::normalize(rayDirection);

            //The closest t value of the closest point of intersection from the ray origin to any object
            //set to -1.0 if there is no intersection
            float closest_t = -1.0f;
            //Pointer to the closest sphere that has intersection point 
            Sphere* closestSphere = nullptr;
            
            //Loop through all spheres and calculate intersection point t. 
            for (auto& sphere : spheres) {
                //calculate intersection point t.
                float t = intersect(rayOrigin, rayDirection, sphere);
                //If the t is smaller that the current closest_t, use that t to calculate color.
                //If t 
                if(t >= 0.0f && (closest_t < 0.0f || t < closest_t)) {
                    closest_t = t;
                    closestSphere = &sphere;
                }   
            }
            
            if(closestSphere) {
                //P = O + tD formula. 
                glm::vec4 intersectionPoint = rayOrigin + closest_t * glm::vec4(rayDirection, 0.0f);
                //Calculate the color at this point using the given phong illumination model 
                glm::vec3 pixelColor = phongColorCalculation(intersectionPoint, *closestSphere, lights, ambientIntensity);
                //Assign the colors in the pixel array
                pixels[(y * resX + x) * 3] = (unsigned char)(pixelColor[0] * 255.0f);
                pixels[(y * resX + x) * 3 + 1] = (unsigned char)(pixelColor[1]* 255.0f);
                pixels[(y * resX + x) * 3 + 2] = (unsigned char)(pixelColor[2] * 255.0f);
            }
            //set pixel to background color
            else {
                pixels[(y * resX + x) * 3] = (unsigned char)(bgColor[0] * 255.0f);
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
    calculateTransformationMatrices(inputs.spheres);
    startRayTrace(pixels, inputs.resX, inputs.resY, inputs.bkg, inputs.ambience, inputs.lights, inputs.spheres);
    char filename[] = "output.ppm";
    save_imageP6(inputs.resX, inputs.resY, filename, pixels);
 
    delete[] pixels;
 
    return 0;
     
     
}