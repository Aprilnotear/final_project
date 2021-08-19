# final_project

This is a project achieved Forward+ rendering and Deferred rendering. The source codes are in the ../Project file. To switch the rendering technique, you just need to change the "renderer" in main.cpp and choose the technique you want. Some codes are based on learnopengl tutorial(Model.h, Mymesh.h) and tutorial in CSC8502.

Environment: Win32 Debug VS2019

This project uses Third-party library Assimp to import model. Therefore, make sure the Library Directories(../Win32/Debug/) is set and input the linker (assimp-vc142-mt.lib, assimp-vc142-mtd.lib, nclgl.lib) correctly.

In Forward+ renderer, there are three mode (DEFAULT, DEPTH, LIGHT) can choose.

Mode
  DEFAULT : final shading
  DEPTH   : depth map debug
  LIGHT   : light culling debug

Camera movement:
W    : Forward
S    : Backward
A    : Left
D    : Right
Shift: Up
Space: Down


Video link: https://1drv.ms/u/s!AqYbdg8lwDpZsGnoTk9jY-wCzC9z?e=3otE1o

Due to I am not convenient upload my video to youtube, I create a OneDrive link. The above OneDrive link contains two videos, one is the presentation of Forward+ rendering, and the other is Deferred rendering.

If there is any problem with the link or the project, please send me e-mail so that I can fix that ASAP.

Yang Lu
Y.Lu62@newcastle.ac.uk
