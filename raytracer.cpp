// EECS 3431 Assignment 3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "glm-master/glm/glm.hpp"
#include "glm-master/glm/ext.hpp"
#include <vector>
#define max 50
 

using namespace std;


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


};

struct Sphere
{
    glm::mat4 transMatrix;
    glm::mat4 invMatrix;
    string name;
    glm::vec4 position;
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
    glm::vec4 position;
    glm::vec3 intensity;
};

Inputs inputs;
string strings[max];

FILE* fp;
char* filename;
char ch;

ifstream input;
string line;
string inputName;

 
void split(string s)

{

    istringstream ss(s);
    int i = 0;
    string text;


 
    while (ss >> text)
    {
     
        cout << text << endl;
        
    }
}


void parse(char* filename)
{

 
     



    input.open(filename);
    while (getline(input, line)) {
 
        split(line);
       

    }
    input.close();

  
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
    return 0;
     
     
}