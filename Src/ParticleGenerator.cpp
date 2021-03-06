//
//  ParticleGenerator.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 24/02/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#include "ParticleGenerator.hpp"
ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, GLuint amount)
    : shader(shader), texture(texture), amount(amount){
    init();
}

void ParticleGenerator::init(){
    // Set up mesh and attribute properties
    GLuint VBO;
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    // Fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // Set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
    glBindVertexArray(0);

    // Create amount default particle instances
    for (GLuint i = 0; i < amount; ++i)//TODO: JobSystem and decent ObjectPool
        particles.push_back(Particle());
}

//Then in each frame, we spawn several new particles with starting values
//and then for each particle that is (still) alive we update their values.
void ParticleGenerator::update(float dt, GameObject &object, GLuint newParticles, glm::vec2 offset){
    // Add new particles
    for (GLuint i = 0; i < newParticles; ++i){
        int unusedParticle = firstUnusedParticle();
        respawnParticle(particles[unusedParticle], object, offset);
    }
    // Update all particles
    for (GLuint i = 0; i < amount; ++i){
        Particle &p = particles[i];
        p.Life -= dt; // reduce life
        if (p.Life > 0.0f){// particle is alive, thus update
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 2.5;
        }
    }
}

// Render all particles
void ParticleGenerator::draw(){
    // Use additive blending (GL_ONE)to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader.use();
    for (Particle particle : particles){
        if (particle.Life > 0.0f){
            shader.setVector2f("offset", particle.Position);
            shader.setVector4f("color", particle.Color);
            texture.bind();
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // Don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;//TODO: check this
GLuint ParticleGenerator::firstUnusedParticle(){//TODO: have a another structure for unusedParticles.
    // First search from last used particle, this will usually return almost instantly
    for (GLuint i = lastUsedParticle; i < amount; ++i){
        if (particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // Otherwise, do a linear search
    for (GLuint i = 0; i < lastUsedParticle; ++i){
        if (particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    // Note that if it reaches this last case, it means  particles are alive for too long,
    // need to spawn less particles per frame and/or simply don't have enough particles reserved.
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset){
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5 + ((rand() % 100) / 100.0f);
    particle.Position = object._position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = object._velocity * 0.1f;
}
