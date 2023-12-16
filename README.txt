EECS 3431 Assignment 3 by Puranjay Kumar and Mahad Hassan

Test cases:

- The code compiles and is compatible with all test cases.
- All test cases return expected output except for the following:
    - Reflections and shadows are not properly implemented due to time constraints.
    - Shadow test case does not cast shadows.
    - The given reflection test case has very unexpected output.
    - The specular test case has very similar output to the key, but not exact.

Regarding GLM, invert.cpp, and ppm.cpp:

- GLM is included. 
- ppm.cpp is included and used to generated the final output.
- invert.cpp was included just in case, but not used in raytrace.cpp at all.
- invert.cpp was giving the following error at line 27: identifier "fabs" is undefined.
- We used GLM library's inverse function instead.
- We also used GLM's various other functionalies, such as translate and scale. 


Usage:

g++ -o raytracer raytracer.cpp
raytracer.exe <your-test-case-txt-file> 

The .txt file must be in the same directory as raytracer.cpp!!
Output ppm will be generated in the same directory!!