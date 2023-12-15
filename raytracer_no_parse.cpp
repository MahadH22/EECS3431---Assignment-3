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
    float NEAR = 1.0f;
    float LEFT = -1.0f;
    float RIGHT = 1.0f;
    float BOTTOM = -1.0f;
    float TOP = 1.0f;

    //
    //float widthOfCameraSpace = ((resX / resY) + 1.0f) / 2.0f;
    //float heightOfCameraSpace = ((resY / resX) + 1.0f) / 2.0f;
   // cout<<widthOfCameraSpace<<endl;
  //  cout<<heightOfCameraSpace<<endl;

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

int main(){
    //all the inputs 
    vector<Light> lights;
    Light light1;
    light1.name = "l1";
    light1.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    light1.intensity = glm::vec3(0.3f, 0.3f, 0.3f);
    lights.push_back(light1);

    Light light2;
    light2.name = "l2";
    light2.position = glm::vec4(10.0f, 10.0f, -10.0f, 1.0f);
    light2.intensity = glm::vec3(0.9f, 0.9f, 0.0f);
    lights.push_back(light2);

    Light light3;
    light3.name = "l3";
    light3.position = glm::vec4(-10.0f, 5.0f, -5.0f, 1.0f);
    light3.intensity = glm::vec3(0.0f, 0.0f, 0.9f);
    lights.push_back(light3);

    vector<Sphere> spheres;
    Sphere sphere1;
    sphere1.name = "s1";
    sphere1.position = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);
    sphere1.scale = glm::vec3(2.0f, 4.0f, 2.0f);
    sphere1.color = glm::vec3(0.5f, 0.0f, 0.0f);
    sphere1.kA = 1.0f;
    sphere1.kD = 0.0f;
    sphere1.kS = 0.0f;
    sphere1.kR = 0.0f;
    sphere1.n = 50;
    spheres.push_back(sphere1);

    Sphere sphere2;
    sphere2.name = "s2";
    sphere2.position = glm::vec4(4.0f, 4.0f, -10.0f, 1.0f);
    sphere2.scale = glm::vec3(1.0f, 2.0f, 1.0f);
    sphere2.color = glm::vec3(0.0f, 0.5f, 0.0f);
    sphere2.kA = 1.0f;
    sphere2.kD = 0.0f;
    sphere2.kS = 0.0f;
    sphere2.kR = 0.0f;
    sphere2.n = 50;
    spheres.push_back(sphere2);

    Sphere sphere3;
    sphere3.name = "s3";
    sphere3.position = glm::vec4(-4.0f, 2.0f, -10.0f, 1.0f);
    sphere3.scale = glm::vec3(1.0f, 2.0f, 1.0f);
    sphere3.color = glm::vec3(0.0f, 0.0f, 0.5f);
    sphere3.kA = 1.0f;
    sphere3.kD = 0.0f;
    sphere3.kS = 0.0f;
    sphere3.kR = 0.0f;
    sphere3.n = 50;
    spheres.push_back(sphere3);

    const int resX = 600, resY = 600;
    glm::vec3 ambientLightIntensity = {0.75f, 0.75f, 0.75f};
    glm::vec3 bgColor = {1.0f, 1.0f, 1.0f};

    unsigned char* pixels = new unsigned char[resX * resY * 3];
    calculateTransformationMatrices(spheres);
    startRayTrace(pixels, resX, resY, bgColor, ambientLightIntensity, lights, spheres);
    char filename[] = "output.ppm";
    save_imageP6(resX, resY, filename, pixels);

    delete[] pixels;
    return 0;


}