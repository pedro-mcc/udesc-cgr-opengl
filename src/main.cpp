// gcc main.cpp -lglfw -lGL -lGLU -lm -o particles && ./particles

#include <particle.h>
#include <window.h>

int main() {
    GLFWwindow* window = initWindow(1280, 720);

    // --- shader source strings (or load from file) ---
    const char* vertSrc = particle.vert;
    const char* fragSrc = /* ... paste particle.frag ... */;
    GLuint shader = createProgram(vertSrc, fragSrc);

    setupBuffers();
    initParticles();

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // additive blending → glow effect

    glm::mat4 proj = glm::perspective(glm::radians(60.f), 1280.f/720.f, 0.1f, 100.f);
    glm::mat4 view = glm::lookAt(glm::vec3(0,2,8), glm::vec3(0,1,0), glm::vec3(0,1,0));
    glm::mat4 mvp  = proj * view;

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float  dt  = float(now - lastTime);
        lastTime   = now;

        updateParticles(dt);
        uploadParticles();

        glClearColor(0.02f, 0.02f, 0.05f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);
        glUniformMatrix4fv(glGetUniformLocation(shader, "uMVP"),
                           1, GL_FALSE, &mvp[0][0]);

        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}