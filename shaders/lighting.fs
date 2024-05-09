#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

#define     MAX_LIGHTS_COUNT        2

struct Light {
    int enabled;
    vec3 position;
    vec3 target;
    vec4 color;
};

// Input lighting values
uniform Light lights[MAX_LIGHTS_COUNT];
uniform vec4 ambient;
uniform vec3 viewPos;
uniform vec3 cameraTarget;
uniform float smoothness;

uniform float diffuseFactor;
uniform float specularFactor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewToFrag = normalize(viewPos - fragPosition);
    //vec3 viewSource = normalize(viewPos - cameraTarget);

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    for(int i = 0; i < MAX_LIGHTS_COUNT; i++) {
        if(lights[i].enabled == 1) {
            vec3 light = normalize(lights[i].position - fragPosition);
            float diffuseStrength = max(0.0, dot(light, normal));
            diffuse += diffuseStrength * lights[i].color.xyz * diffuseFactor;

            specular += pow(max(0.0, dot(viewToFrag, reflect(-(light), normal))), smoothness) * specularFactor * 2.0;
        }
    }

    finalColor = (colDiffuse + vec4(specular, 1.0))*vec4(diffuse, 1.0); 
    finalColor += (ambient/5.0)*colDiffuse;
    finalColor = pow(finalColor, vec4(1.0/1.3));
}

