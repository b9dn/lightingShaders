#include "include/raylib.h"
#include "include/raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

#define GLSL_VERSION            330
#define MAX_LIGHTS_COUNT        2

class Light {
private:
    unsigned int id;

    int enabled;
    Vector3 position;
    Vector3 target;
    Color color;
    
    int enabledLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;

    float move_step = 0.1f;
    
public:
    Light(int id, Vector3 position, Vector3 target, Color color, Shader shader) {
        this->id = id;
        this->enabled = 1;
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

    void update_light_col(Color color) {
        this->color = color;
    }

    void toggle() {
        enabled ^= 1;
    }

    void move_left() {
        position.x -= move_step;
    }
    
    void move_right() {
        position.x += move_step;
    }

    void move_forward() {
        position.z += move_step;
    }

    void move_backward() {
        position.z -= move_step;
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
    float floorDiffuseFactor = 1.0;
    float floorSpecularFactor = 1.0;

    Model sphere = LoadModelFromMesh(GenMeshSphere(1.0f, 30, 30));

    float sphere1DiffuseFactor = 1.0;
    float sphere1SpecularFactor = 0.0;

    float sphere2DiffuseFactor = 0.8;
    float sphere2SpecularFactor = 0.2;
    
    float sphere3DiffuseFactor = 0.7;
    float sphere3SpecularFactor = 0.1;
    
    float sphere4DiffuseFactor = 0.3;
    float sphere4SpecularFactor = 1.0;

#ifdef PHONG_DEFAULT
    Shader shader = LoadShader(TextFormat("shaders/lighting.vs", GLSL_VERSION),
                               TextFormat("shaders/lighting_phong_default.fs", GLSL_VERSION));
#else
    Shader shader = LoadShader(TextFormat("shaders/lighting.vs", GLSL_VERSION),
                               TextFormat("shaders/lighting.fs", GLSL_VERSION));
#endif

    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int cameraTarget = GetShaderLocation(shader, "cameraTarget");

    float smoothness = 16.0;
    int smoothnessLoc = GetShaderLocation(shader, "smoothness");
    
    float ambientFactor = 0.3f;
    int ambientLoc = GetShaderLocation(shader, "ambient");

    int diffuseFactorLoc = GetShaderLocation(shader, "diffuseFactor");

    int specularFactorLoc = GetShaderLocation(shader, "specularFactor");

    floor.materials[0].shader = shader;
    sphere.materials[0].shader = shader;

    Color lightColors[MAX_LIGHTS_COUNT] = { WHITE, BLUE };

    Light lights[MAX_LIGHTS_COUNT] = {
        Light(0, (Vector3){ -13, 1, -10 }, Vector3Zero(), lightColors[0], shader),
        Light(1, (Vector3){ 13, 1, -10 }, Vector3Zero(), lightColors[1], shader)
    };

    bool cursorEnabled = false;

    SetTargetFPS(60);
    DisableCursor();
    //--------------------------------------------------------------------------------------

    // Main game loop
    while(!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        if(!cursorEnabled)
            UpdateCamera(&camera, CAMERA_PERSPECTIVE);

        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, cameraTarget, &camera.target, SHADER_UNIFORM_VEC3);

        float ambient[4] = {ambientFactor, ambientFactor, ambientFactor, 1.0f};
        SetShaderValue(shader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);

        SetShaderValue(shader, smoothnessLoc, &smoothness, SHADER_UNIFORM_FLOAT);
        
        if(IsKeyPressed(KEY_B)) 
            lights[0].toggle();
        if(IsKeyPressed(KEY_R)) 
            lights[1].toggle();

        if(IsKeyPressed(KEY_LEFT_ALT)) {
            if(cursorEnabled) {
                DisableCursor();
                cursorEnabled = false;
            }
            else {
                EnableCursor();
                cursorEnabled = true;
            }
        }

        if(IsKeyDown(KEY_L)) {
            lights[0].move_left();
        }

        if(IsKeyDown(KEY_H)) {
            lights[0].move_right();
        }
        
        if(IsKeyDown(KEY_K)) {
            lights[0].move_forward();
        }
        
        if(IsKeyDown(KEY_J)) {
            lights[0].move_backward();
        }
        
        for(int i = 0; i < MAX_LIGHTS_COUNT; i++) {
            lights[i].update_light_col(lightColors[i]);
            lights[i].update(shader);
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                SetShaderValue(shader, diffuseFactorLoc, &floorDiffuseFactor, SHADER_UNIFORM_FLOAT);
                SetShaderValue(shader, specularFactorLoc, &floorSpecularFactor, SHADER_UNIFORM_FLOAT);
                DrawModel(floor, Vector3Zero(), 1.0f, DARKBLUE);
                /* DrawModelEx(floor, (Vector3){0.0f, 15.0f, 15.0f}, (Vector3){1.0f, 0.0f, 0.0f}, -90.0f, Vector3One(), DARKBLUE); */

                SetShaderValue(shader, diffuseFactorLoc, &sphere1DiffuseFactor, SHADER_UNIFORM_FLOAT);
                SetShaderValue(shader, specularFactorLoc, &sphere1SpecularFactor, SHADER_UNIFORM_FLOAT);
                DrawModel(sphere, (Vector3){-4.5f, 1.0f, 0.0f}, 1.0f, WHITE);
                

                SetShaderValue(shader, diffuseFactorLoc, &sphere2DiffuseFactor, SHADER_UNIFORM_FLOAT);
                SetShaderValue(shader, specularFactorLoc, &sphere2SpecularFactor, SHADER_UNIFORM_FLOAT);
                DrawModel(sphere, (Vector3){-1.5f, 1.0f, 0.0f}, 1.0f, WHITE);


                SetShaderValue(shader, diffuseFactorLoc, &sphere3DiffuseFactor, SHADER_UNIFORM_FLOAT);
                SetShaderValue(shader, specularFactorLoc, &sphere3SpecularFactor, SHADER_UNIFORM_FLOAT);
                DrawModel(sphere, (Vector3){1.5f, 1.0f, 0.0f}, 1.0f, WHITE);


                SetShaderValue(shader, diffuseFactorLoc, &sphere4DiffuseFactor, SHADER_UNIFORM_FLOAT);
                SetShaderValue(shader, specularFactorLoc, &sphere4SpecularFactor, SHADER_UNIFORM_FLOAT);
                DrawModel(sphere, (Vector3){4.5f, 1.0f, 0.0f}, 1.0f, WHITE);


                for(int i = 0; i < MAX_LIGHTS_COUNT; i++) {
                    if(lights[i].is_enabled())
                        DrawSphereEx(lights[i].get_position(), 0.2f, 8, 8, lights[i].get_color());
                    else 
                        DrawSphereWires(lights[i].get_position(), 0.2f, 8, 8, ColorAlpha(lights[i].get_color(), 0.3f));
                }

                DrawGrid(30, 1.0f);

            EndMode3D();

            DrawFPS(10, 10);

            DrawText("R, B to toggle lights", 10, 40, 20, DARKGRAY);
            DrawText("Left-ALT enable/disable mouse", 10, 60, 20, DARKGRAY);
            DrawText("HJKL move right light", 10, 80, 20, DARKGRAY);
            GuiSliderBar((Rectangle){10, 110, 300, 30}, NULL, "Ambient factor", &ambientFactor, 0.0f, 1.0f);
            GuiSliderBar((Rectangle){10, 150, 300, 30}, NULL, "Smoothness factor", &smoothness, 1.0f, 100.0f);
            GuiColorPicker((Rectangle){10, 190, 300, 40}, "Color first light", &lightColors[0]);
            GuiColorPicker((Rectangle){10, 240, 300, 40}, "Color first light", &lightColors[1]);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadModel(floor);
    UnloadModel(sphere);
    UnloadShader(shader);

    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}

