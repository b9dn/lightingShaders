g++ -ggdb lighting.cpp -I .\include\ -L.\lib\ -lraylib -lopengl32 -lwinmm -lgdi32 -o lighting
g++ -ggdb lighting.cpp -I .\include\ -L.\lib\ -lraylib -lopengl32 -lwinmm -lgdi32 -DPHONG_DEFAULT -o phong_default
