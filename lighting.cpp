#include "include/raylib.h"
#include "include/raymath.h"

#define GLSL_VERSION            330
#define MAX_LIGHTS_COUNT        2

class Light {
private:
    unsigned int id;

    bool enabled;
    Vector3 position;
    Vector3 target;
    Color color;
    
    int enabledLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    
public:
    Light(int id, Vector3 position, Vector3 target, Color color, Shader shader) {
        this->id = id;
        this->enabled = true;
        this->position = position;
        this->target = target;
        this->color = color;

        enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", id));
        positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", id));
        targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", id));
        colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", id));
    }

    void update(Shader shader) {
        SetShaderValue(shader, enabledLoc, &enabled, SHADER_UNIFORM_INT);

        float position[3] = { this->position.x, this->position.y, this->position.z };
        SetShaderValue(shader, positionLoc, position, SHADER_UNIFORM_VEC3);

        float target[3] = { this->target.x, this->target.y, this->target.z };
        SetShaderValue(shader, targetLoc, target, SHADER_UNIFORM_VEC3);

        float color[4] = { (float)this->color.r/(float)255, (float)this->color.g/(float)255, 
                           (float)this->color.b/(float)255, (float)this->color.a/(float)255 };
        SetShaderValue(shader, colorLoc, color, SHADER_UNIFORM_VEC4);
    }

    void toggle() {
        enabled ^= true;
    }

    bool is_enabled() {
        return enabled;
    }

    Vector3 get_position() {
        return position;
    }

    Color get_color() {
        return color;
    }
};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1500;
    const int screenHeight = 900;

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(screenWidth, screenHeight, "Lighting shaders");

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 3.0f, -5.0f };
    camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Model floor = LoadModelFromMesh(GenMeshPlane(30.0f, 30.0f, 3, 3));
    Model sphere1 = LoadModelFromMesh(GenMeshSphere(1.0f, 30, 30));
    
    Shader shader = LoadShader(TextFormat("shaders/lighting.vs", GLSL_VERSION),
                               TextFormat("shaders/lighting.fs", GLSL_VERSION));

    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int cameraTarget = GetShaderLocation(shader, "cameraTarget");
    
    int ambientLoc = GetShaderLocation(shader, "ambient");
    float ambientFactor = 0.1f;
    float ambient[4] = {ambientFactor, ambientFactor, ambientFactor, 1.0f};
    SetShaderValue(shader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);

    floor.materials[0].shader = shader;
    sphere1.materials[0].shader = shader;

    Light lights[MAX_LIGHTS_COUNT] = {
        Light(0, (Vector3){ -13, 1, -10 }, Vector3Zero(), BLUE, shader),
        Light(1, (Vector3){ 13, 1, -10 }, Vector3Zero(), RED, shader)
    };

    SetTargetFPS(60);
    DisableCursor();
    //--------------------------------------------------------------------------------------

    // Main game loop
    while(!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_PERSPECTIVE);

        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, cameraTarget, &camera.target, SHADER_UNIFORM_VEC3);
        
        if(IsKeyPressed(KEY_B)) 
            lights[0].toggle();
        if(IsKeyPressed(KEY_R)) 
            lights[1].toggle();
        
        for(int i = 0; i < MAX_LIGHTS_COUNT; i++)
            lights[i].update(shader);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawModel(floor, Vector3Zero(), 1.0f, DARKBLUE);
                /* DrawModelEx(floor, (Vector3){0.0f, 15.0f, 15.0f}, (Vector3){1.0f, 0.0f, 0.0f}, -90.0f, Vector3One(), DARKBLUE); */
                DrawModel(sphere1, (Vector3){-4.5f, 1.0f, 0.0f}, 1.0f, WHITE);
                DrawModel(sphere1, (Vector3){-1.5f, 1.0f, 0.0f}, 1.0f, WHITE);
                DrawModel(sphere1, (Vector3){1.5f, 1.0f, 0.0f}, 1.0f, WHITE);
                DrawModel(sphere1, (Vector3){4.5f, 1.0f, 0.0f}, 1.0f, WHITE);

                for(int i = 0; i < MAX_LIGHTS_COUNT; i++)
                {
                    if(lights[i].is_enabled())
                        DrawSphereEx(lights[i].get_position(), 0.2f, 8, 8, lights[i].get_color());
                    else 
                        DrawSphereWires(lights[i].get_position(), 0.2f, 8, 8, ColorAlpha(lights[i].get_color(), 0.3f));
                }

                DrawGrid(30, 1.0f);

            EndMode3D();

            DrawFPS(10, 10);

            DrawText("R, B to toggle lights", 10, 40, 20, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadModel(floor);
    UnloadModel(sphere1);
    UnloadShader(shader);

    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}

