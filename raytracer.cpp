//EECS 3431 Assignment 3 by Puranjay Kumar and Mahad Hassan

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

struct Ray
{
    glm::vec4 origin;
    glm::vec3 direction;
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

glm::vec3 rayTrace(glm::vec3 &ambientIntensity, vector<Light> &lights, vector<Sphere> &spheres, Ray &ray, int depth);

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


/// PARSE FUNCTION STARTS HERE ///
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
            sphere.position[2] = stof(broken[i + 4]);
            sphere.position[3] = 0;

            sphere.scale.x = stof(broken[i + 5]);
            sphere.scale.y = stof(broken[i + 6]);
            sphere.scale.z = stof(broken[i + 7]);

            sphere.color.r = stof(broken[i + 8]);
            sphere.color.g = stof(broken[i + 9]);
            sphere.color.b = stof(broken[i + 10]);

            sphere.kA = stof(broken[i + 11]);
            sphere.kD = stof(broken[i + 12]);
            sphere.kS = stof(broken[i + 13]);
            sphere.kR = stof(broken[i + 14]);

            sphere.n = stof(broken[i + 15]);
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
/// PARSE FUNCTION ENDS HERE ///

//Calculates the transfomation matrices of all spheres in a given array
void calculateTransformationMatrices(vector<Sphere>& spheres) {
    //Identity matrix
    const glm::mat4 identity = glm::mat4(1.f);

    //Loop through all the spheres
    for (Sphere& sphere : spheres) {
        // Create the transformation matrix. Made use of glm library here
        //Formula M = Translation * Scale * Rotation. Assuming no rotation here though.
        sphere.transMatrix = glm::translate(identity, glm::vec3(sphere.position)) * glm::scale(identity, glm::vec3(sphere.scale));
        // Inverse of the transformation matrix, using glm library
        sphere.invMatrix = glm::inverse(sphere.transMatrix);
    }
}


//Calculates the intersection point t of given sphere
float intersect(Ray ray, Sphere &sphere) {
    //t is negative if there is no intersection
    float t = -1.0f;
    //Use the sphere's inverseMatrix transformation to bring the ray relative to the sphere
    Ray sphereRay;
    sphereRay.origin = sphere.invMatrix * ray.origin;
    sphereRay.direction = sphere.invMatrix * glm::vec4(ray.direction, 0.0f);

    //Calcualte A (ray direction * ray direction) 
    float A = glm::dot(glm::vec3(sphereRay.direction), glm::vec3(sphereRay.direction));
    //Calculate B (ray origin * ray direction) 
    float B = 2.0f * glm::dot(glm::vec3(sphereRay.origin), glm::vec3(sphereRay.direction));
    //Calculate C (ray origin * ray origin)
    float C = glm::dot(glm::vec3(sphereRay.origin), glm::vec3(sphereRay.origin)) - 1.0f;  // Sphere radius is 1 in local space
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

glm::vec3 phongColorCalculation(glm::vec4 &intersectionPoint, Sphere &sphere, vector<Sphere> &spheres, vector<Light> &lights, glm::vec3 &ambientIntensity, Ray &ray, int depth) {
    if(depth > 3) {
        return glm::vec3(0.0f);
    }
    // Ambient compenent
    glm::vec3 ambientColor = sphere.color * sphere.kA * ambientIntensity;

    //Add the ambient color
    glm::vec3 color = ambientColor;

    //Normal vector at the intersection point
    glm::vec3 N = intersectionPoint - sphere.position;
    //Divide normal vector by sphere's scale
    N = N / (sphere.scale * sphere.scale);
    //Normalize the vector
    N = glm::normalize(N);

    //Normalized vector starting from intersection point looking back at the camera, so just add a negative to intersectionPoint vector.
    glm::vec3 V = -glm::normalize(intersectionPoint);


    for (const Light& light : lights) {
        //Normalized vector starting from the intersection point to the light source
        glm::vec3 L = glm::normalize(light.position - intersectionPoint);

        //Direction of the reflected light, formula: 2 * (L * N) * N - L
        glm::vec3 R = glm::normalize(2.0f * (glm::dot(L, N)) * (N) - L);
    
        //Diffuse component: kD * light intensity * (N * L) * color
        float diffDot = glm::dot(N, L);       
        glm::vec3 diffuseColor = sphere.kD * light.intensity * diffDot * sphere.color;

        //Specular component: kS * light intensity * (R * V)^n 
        float specDot = pow(std::max(glm::dot(V, R), 0.0f), sphere.n);
        glm::vec3 specularColor = sphere.kS * light.intensity * specDot;

        //Reflections 
        if (sphere.kR > 0.0f) {
            Ray reflectedRay;
            //Calculate reflected ray formula: -2 * (N * c) * N + c 
            reflectedRay.direction = (-2.0f * (glm::dot(N, ray.direction)) * (N) + ray.direction);
            //P = O + tD
            //offset t by * 0.001
            reflectedRay.origin = intersectionPoint + glm::vec4(ray.direction * 0.001f, 0.0f); 

            // Recursively call to trace the reflected ray
            glm::vec3 reflectedColor = rayTrace(ambientIntensity, lights, spheres, reflectedRay, depth+1);

            // Add the reflected color
            color += sphere.kR * reflectedColor;
        }
        //Add all the colors together
        color += diffuseColor + specularColor;
    }

    // Bring color range to 0-1 using glm library. 
    color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
    return color; 
}

glm::vec3 rayTrace(glm::vec3 &ambientIntensity, vector<Light> &lights, vector<Sphere> &spheres, Ray &ray, int depth) {
    //If recursion depth is more than 3, return black color
    if(depth > 3) {
        return glm::vec3(0.0f);
    }
    glm::vec3 pixelColor;

    //normalize the ray's direction vector
    ray.direction = glm::normalize(ray.direction);
    //The closest t value of the closest point of intersection from the ray origin to an object
     //set to -1.0 if there is no intersection
    float closest_t = -1.0f;
    //Pointer to the closest sphere that has the intersection point 
    Sphere* closestSphere = nullptr;
            
    //Loop through all spheres and calculate intersection point t. 
    for (auto& sphere : spheres) {
    //calculate intersection point t.
        float t = intersect(ray, sphere);
        //If a t has been calculated (which means an intersection exists), but closest_t is still negative, update closest_t to that t.
        //If the t is smaller than the current closest_t, update closest_t to that t.
        if(t >= 0.0f && (closest_t < 0.0f || t < closest_t)) {
            closest_t = t;
            closestSphere = &sphere;
        }   
        if(closestSphere) {
            //P = O + tD formula. 
            glm::vec4 intersectionPoint = ray.origin + closest_t * glm::vec4(ray.direction, 0.0f);
            //Calculate the color at this point using the given phong illumination model 
            pixelColor = phongColorCalculation(intersectionPoint, *closestSphere, spheres, lights, ambientIntensity, ray, depth);
        }
        //set pixel to background color if no intersection occured (closestSphere is still null)
        else {
            pixelColor = inputs.bkg;
        }
    }
    
    return pixelColor;

}

 
int main(int argc, char* argv[])
{
    //Parse the file
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

    //initialize pixel array
    unsigned char* pixels = new unsigned char[inputs.resX * inputs.resY * 3];

    //calcualte the transformation matrices of the spheres beforehand
    calculateTransformationMatrices(inputs.spheres);

    //loop through all the pixels to "shoot" out rays
    glm::vec3 pixelColor;
    for (int y = 0; y < inputs.resY; ++y) {
        for (int x = 0; x < inputs.resX; ++x) {
            //Normalized device coordinates of the pixel
            float u = ((2.0f * x / inputs.resX) - 1.0f);
            float v = ((2.0f * y / inputs.resY) - 1.0f);
            v *= -1; //invert the y axis 

            Ray ray;
            //the camera is assumed to be at 0,0,0 at all times, so ray origin will always be 0,0,0
            ray.origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            
            //Ray direction vector toward the near plane (into the pixel)
            ray.direction = glm::vec3(u, v, -inputs.near);

            //Start raytracing to get the color for the current pixel
            pixelColor = rayTrace(inputs.ambience, inputs.lights, inputs.spheres, ray, 0);
            pixels[(y * inputs.resX + x) * 3] = (unsigned char)(pixelColor[0] * 255.0f);
            pixels[(y * inputs.resX  + x) * 3 + 1] = (unsigned char)(pixelColor[1] * 255.0f);
            pixels[(y * inputs.resX  + x) * 3 + 2] = (unsigned char)(pixelColor[2] * 255.0f);                        
        }
    }
    

    //converting the filename string to char array to work with the provided ppm exporter
    char file[inputs.output.length() + 1]; 
    strcpy(file, inputs.output.c_str());

    //change p6 or p3 generation here
    save_imageP6(inputs.resX, inputs.resY, file, pixels);
 
    delete[] pixels;


    return 0;
     
     
}