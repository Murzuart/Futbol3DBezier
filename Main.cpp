
// CANCHA DE FUTBOL 3D COLOMBIA 

#include <windows.h> // Inclusión de la biblioteca de Windows para funciones específicas del sistema operativo
#include <GLFW/glfw3.h> // Inclusión de la biblioteca GLFW para la creación de ventanas y manejo de eventos
#include <GL/gl.h> // Inclusión de la biblioteca OpenGL para gráficos 2D y 3D
#include <GL/glu.h> // Inclusión de la biblioteca GLU para utilidades de OpenGL

#define STB_IMAGE_IMPLEMENTATION // Definición para implementar la biblioteca STB para carga de imágenes
#include "stb_image.h" // Inclusión de la biblioteca STB para la carga de texturas

#include <iostream> // Inclusión de la biblioteca para entrada y salida estándar
#include <cmath> // Inclusión de la biblioteca matemática para funciones matemáticas

// ======================================================
// CÁMARA
// ======================================================

float camAngle = 0.0f; // Ángulo de la cámara inicializado a 0 grados
float camRadius = 85.0f; // Radio de la cámara, distancia desde el centro de la escena
float camHeight = 22.0f; // Altura de la cámara sobre el plano de la cancha

// ======================================================
// ANIMACIÓN
// ======================================================

float animTime = 0.0f; // Tiempo de animación inicializado a 0

// ======================================================
// TEXTURAS
// ======================================================

GLuint cespedTexture; // Identificador de la textura del césped
GLuint gradasTexture; // Identificador de la textura de las gradas
GLuint balonTexture; // Identificador de la textura del balón

// ======================================================
// PUNTO 3D
// ======================================================

struct Point3D // Definición de una estructura para representar un punto en 3D
{
    float x, y, z; // Coordenadas del punto en el espacio tridimensional
};

// ======================================================
// CURVA DEL BALÓN
// ======================================================

Point3D P0 = { -28.0f, 1.0f, 0.0f }; // Primer punto de control de la curva del balón
Point3D P1 = { -10.0f, 16.0f, 0.0f }; // Segundo punto de control de la curva del balón
Point3D P2 = { 15.0f, 12.0f, -2.0f }; // Tercer punto de control de la curva del balón
Point3D P3 = { 30.0f, 5.7f, -3.8f }; // Cuarto punto de control de la curva del balón

// ======================================================
// CARGAR TEXTURA
// ======================================================

GLuint loadTexture(const char* filename) // Función para cargar una textura desde un archivo
{
    GLuint texture; // Variable para almacenar el identificador de la textura

    int width; // Variable para almacenar el ancho de la textura
    int height; // Variable para almacenar la altura de la textura
    int nrChannels; // Variable para almacenar el número de canales de color

    unsigned char* data = // Carga de la imagen en memoria
        stbi_load(
            filename, // Nombre del archivo de la textura
            &width, // Dirección de la variable de ancho
            &height, // Dirección de la variable de altura
            &nrChannels, // Dirección de la variable de canales
            0); // Carga de la imagen sin especificar el número de canales

    if (data) // Verificación de si la carga fue exitosa
    {
        glGenTextures(1, &texture); // Generación de un nuevo identificador de textura

        glBindTexture(GL_TEXTURE_2D, texture); // Vinculación de la textura generada

        glTexParameteri( // Configuración de parámetros de la textura
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_REPEAT); // Repetición de la textura en el eje S

        glTexParameteri( // Configuración de parámetros de la textura
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_REPEAT); // Repetición de la textura en el eje T

        glTexParameteri( // Configuración de parámetros de la textura
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR); // Filtro lineal para la reducción de la textura

        glTexParameteri( // Configuración de parámetros de la textura
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR); // Filtro lineal para la ampliación de la textura

        GLenum format = // Determinación del formato de la textura
            (nrChannels == 4) // Si hay 4 canales, se usa formato RGBA
            ? GL_RGBA
            : GL_RGB; // De lo contrario, se usa formato RGB

        glTexImage2D( // Carga de la imagen de la textura en la memoria de OpenGL
            GL_TEXTURE_2D,
            0,
            format, // Formato de la textura
            width, // Ancho de la textura
            height, // Altura de la textura
            0, // Sin borde
            format, // Formato de la textura
            GL_UNSIGNED_BYTE, // Tipo de datos de los píxeles
            data); // Datos de la textura

        std::cout // Mensaje en consola indicando que la textura fue cargada
            << "TEXTURA CARGADA: "
            << filename // Nombre del archivo de la textura
            << std::endl; // Fin de la línea
    }

    stbi_image_free(data); // Liberación de la memoria ocupada por la imagen cargada

    return texture; // Retorno del identificador de la textura
}

// ======================================================
// BÉZIER
// ======================================================

Point3D getBezierPoint(
    float t,
    Point3D p0,
    Point3D p1,
    Point3D p2,
    Point3D p3)
{
    float u = 1.0f - t;

    float tt = t * t;
    float uu = u * u;

    float uuu = uu * u;
    float ttt = tt * t;

    Point3D p;

    p.x =
        uuu * p0.x +
        3 * uu * t * p1.x +
        3 * u * tt * p2.x +
        ttt * p3.x;

    p.y =
        uuu * p0.y +
        3 * uu * t * p1.y +
        3 * u * tt * p2.y +
        ttt * p3.y;

    p.z =
        uuu * p0.z +
        3 * uu * t * p1.z +
        3 * u * tt * p2.z +
        ttt * p3.z;

    return p;
}
// ======================================================
// TECLADO
// ======================================================

void key_callback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{
    if (action == GLFW_PRESS ||
        action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_LEFT)
            camAngle -= 0.05f;

        if (key == GLFW_KEY_RIGHT)
            camAngle += 0.05f;

        if (key == GLFW_KEY_UP)
            camRadius -= 2.0f;

        if (key == GLFW_KEY_DOWN)
            camRadius += 2.0f;

        if (key == GLFW_KEY_W)
            camHeight += 1.0f;

        if (key == GLFW_KEY_S)
            camHeight -= 1.0f;
    }
}

// ======================================================
// CIELO COLOMBIA
// ======================================================

void drawSky()
{
    glDisable(GL_TEXTURE_2D);

    float s = 900.0f;

    // AMARILLO

    glColor3f(1.0f, 0.85f, 0.0f);

    glBegin(GL_QUADS);

    glVertex3f(-s, 450, -s);
    glVertex3f(s, 450, -s);
    glVertex3f(s, 250, -s);
    glVertex3f(-s, 250, -s);

    glVertex3f(s, 450, s);
    glVertex3f(-s, 450, s);
    glVertex3f(-s, 250, s);
    glVertex3f(s, 250, s);

    glVertex3f(-s, 450, s);
    glVertex3f(-s, 450, -s);
    glVertex3f(-s, 250, -s);
    glVertex3f(-s, 250, s);

    glVertex3f(s, 450, -s);
    glVertex3f(s, 450, s);
    glVertex3f(s, 250, s);
    glVertex3f(s, 250, -s);

    glVertex3f(-s, 450, -s);
    glVertex3f(s, 450, -s);
    glVertex3f(s, 450, s);
    glVertex3f(-s, 450, s);

    glEnd();

    // AZUL

    glColor3f(0.0f, 0.2f, 1.0f);

    glBegin(GL_QUADS);

    glVertex3f(-s, 250, -s);
    glVertex3f(s, 250, -s);
    glVertex3f(s, 150, -s);
    glVertex3f(-s, 150, -s);

    glVertex3f(s, 250, s);
    glVertex3f(-s, 250, s);
    glVertex3f(-s, 150, s);
    glVertex3f(s, 150, s);

    glEnd();

    // ROJO

    glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);

    glVertex3f(-s, 150, -s);
    glVertex3f(s, 150, -s);
    glVertex3f(s, 50, -s);
    glVertex3f(-s, 50, -s);

    glVertex3f(s, 150, s);
    glVertex3f(-s, 150, s);
    glVertex3f(-s, 50, s);
    glVertex3f(s, 50, s);

    glEnd();
}

// ======================================================
// CÉSPED EXTERIOR
// ======================================================

void drawOuterGrass()
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, cespedTexture);

    glColor3f(0.6f, 0.6f, 0.6f);

    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex3f(-250, -0.1f, -250);

    glTexCoord2f(50, 0);
    glVertex3f(250, -0.1f, -250);

    glTexCoord2f(50, 50);
    glVertex3f(250, -0.1f, 250);

    glTexCoord2f(0, 50);
    glVertex3f(-250, -0.1f, 250);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}
// ======================================================
// ESTADIO
// ======================================================

void drawStadium()
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(
        GL_TEXTURE_2D,
        gradasTexture);

    glColor3f(1, 1, 1);

    float h = 30.0f;

    // FONDO

    glBegin(GL_QUADS);

    glTexCoord2f(0, 1);
    glVertex3f(-90, 0, -80);

    glTexCoord2f(1, 1);
    glVertex3f(90, 0, -80);

    glTexCoord2f(1, 0);
    glVertex3f(60, h, -120);

    glTexCoord2f(0, 0);
    glVertex3f(-60, h, -120);

    glEnd();

    // ATRÁS

    glBegin(GL_QUADS);

    glTexCoord2f(0, 1);
    glVertex3f(90, 0, 80);

    glTexCoord2f(1, 1);
    glVertex3f(-90, 0, 80);

    glTexCoord2f(1, 0);
    glVertex3f(-60, h, 120);

    glTexCoord2f(0, 0);
    glVertex3f(60, h, 120);

    glEnd();

    // IZQUIERDA

    glBegin(GL_QUADS);

    glTexCoord2f(0, 1);
    glVertex3f(-80, 0, 80);

    glTexCoord2f(1, 1);
    glVertex3f(-80, 0, -80);

    glTexCoord2f(1, 0);
    glVertex3f(-120, h, -60);

    glTexCoord2f(0, 0);
    glVertex3f(-120, h, 60);

    glEnd();

    // DERECHA

    glBegin(GL_QUADS);

    glTexCoord2f(0, 1);
    glVertex3f(80, 0, -80);

    glTexCoord2f(1, 1);
    glVertex3f(80, 0, 80);

    glTexCoord2f(1, 0);
    glVertex3f(120, h, 60);

    glTexCoord2f(0, 0);
    glVertex3f(120, h, -60);

    glEnd();

    // ESQUINA FRENTE IZQUIERDA

    glBegin(GL_TRIANGLES);

    glTexCoord2f(0, 0);
    glVertex3f(-80, 0, -80);

    glTexCoord2f(1, 0);
    glVertex3f(-120, h, -60);

    glTexCoord2f(0.5, 1);
    glVertex3f(-60, h, -120);

    glEnd();

    // ESQUINA FRENTE DERECHA

    glBegin(GL_TRIANGLES);

    glTexCoord2f(0, 0);
    glVertex3f(80, 0, -80);

    glTexCoord2f(1, 0);
    glVertex3f(120, h, -60);

    glTexCoord2f(0.5, 1);
    glVertex3f(60, h, -120);

    glEnd();

    // ESQUINA ATRÁS IZQUIERDA

    glBegin(GL_TRIANGLES);

    glTexCoord2f(0, 0);
    glVertex3f(-80, 0, 80);

    glTexCoord2f(1, 0);
    glVertex3f(-120, h, 60);

    glTexCoord2f(0.5, 1);
    glVertex3f(-60, h, 120);

    glEnd();

    // ESQUINA ATRÁS DERECHA

    glBegin(GL_TRIANGLES);

    glTexCoord2f(0, 0);
    glVertex3f(80, 0, 80);

    glTexCoord2f(1, 0);
    glVertex3f(120, h, 60);

    glTexCoord2f(0.5, 1);
    glVertex3f(60, h, 120);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// ======================================================
// CANCHA
// ======================================================

void drawField()
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, cespedTexture);

    glColor3f(1, 1, 1);

    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex3f(-40, 0, -25);

    glTexCoord2f(10, 0);
    glVertex3f(40, 0, -25);

    glTexCoord2f(10, 10);
    glVertex3f(40, 0, 25);

    glTexCoord2f(0, 10);
    glVertex3f(-40, 0, 25);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}
// ======================================================
// LÍNEAS DE LA CANCHA
// ======================================================

void drawFieldLines()
{
    glDisable(GL_LIGHTING);

    glColor3f(1, 1, 1);

    glLineWidth(4);

    // Bordes

    glBegin(GL_LINE_LOOP);

    glVertex3f(-40, 0.05f, -25);
    glVertex3f(40, 0.05f, -25);
    glVertex3f(40, 0.05f, 25);
    glVertex3f(-40, 0.05f, 25);

    glEnd();

    // Línea central

    glBegin(GL_LINES);

    glVertex3f(0, 0.05f, -25);
    glVertex3f(0, 0.05f, 25);

    glEnd();

    // Círculo central

    glBegin(GL_LINE_LOOP);

    for (int i = 0; i < 100; i++)
    {
        float angle =
            2.0f * 3.1416f * i / 100;

        float x = 6 * cos(angle);
        float z = 6 * sin(angle);

        glVertex3f(x, 0.05f, z);
    }

    glEnd();

    glEnable(GL_LIGHTING);
}

// ======================================================
// PORTERÍA
// ======================================================

void drawGoal()
{
    glDisable(GL_LIGHTING);

    glColor3f(1, 1, 1);

    glLineWidth(7);

    glBegin(GL_LINES);

    // Poste izquierdo

    glVertex3f(30, 0, -4);
    glVertex3f(30, 6, -4);

    // Poste derecho

    glVertex3f(30, 0, 4);
    glVertex3f(30, 6, 4);

    // Travesaño

    glVertex3f(30, 6, -4);
    glVertex3f(30, 6, 4);

    glEnd();

    glEnable(GL_LIGHTING);
}

// ======================================================
// LUCES
// ======================================================

void drawLights()
{
    glDisable(GL_LIGHTING);

    glPointSize(10);

    glBegin(GL_POINTS);

    glColor3f(1, 1, 1);

    for (float x = -70; x <= 70; x += 5)
    {
        glVertex3f(x, 35, -35);
        glVertex3f(x, 35, 35);
    }

    glEnd();

    glEnable(GL_LIGHTING);
}

// ======================================================
// CURVA BÉZIER
// ======================================================

void drawBezierCurve()
{
    glDisable(GL_LIGHTING);

    glColor3f(1, 0, 0);

    glLineWidth(5);

    glBegin(GL_LINE_STRIP);

    for (int i = 0; i <= 100; i++)
    {
        float t =
            (float)i / 100.0f;

        Point3D p =
            getBezierPoint(
                t,
                P0,
                P1,
                P2,
                P3);

        glVertex3f(
            p.x,
            p.y,
            p.z);
    }

    glEnd();

    glEnable(GL_LIGHTING);
}
// ======================================================
// PUNTOS DE CONTROL
// ======================================================

void drawControlPoints()
{
    glDisable(GL_LIGHTING);

    glPointSize(12);

    glBegin(GL_POINTS);

    glColor3f(1, 1, 0);
    glVertex3f(P0.x, P0.y, P0.z);

    glColor3f(0, 1, 0);
    glVertex3f(P1.x, P1.y, P1.z);

    glColor3f(0, 0, 1);
    glVertex3f(P2.x, P2.y, P2.z);

    glColor3f(1, 0, 0);
    glVertex3f(P3.x, P3.y, P3.z);

    glEnd();

    glEnable(GL_LIGHTING);
}

// ======================================================
// BALÓN
// ======================================================

void drawBall(float t)
{
    Point3D currentPos =
        getBezierPoint(
            t,
            P0,
            P1,
            P2,
            P3);

    glPushMatrix();

    glTranslatef(
        currentPos.x,
        currentPos.y,
        currentPos.z);

    glRotatef(
        animTime * 500,
        1,
        1,
        0);

    glEnable(GL_TEXTURE_2D);

    glBindTexture(
        GL_TEXTURE_2D,
        balonTexture);

    GLUquadric* quad =
        gluNewQuadric();

    gluQuadricTexture(
        quad,
        GL_TRUE);

    gluSphere(
        quad,
        1.0f,
        100,
        100);

    gluDeleteQuadric(quad);

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// ======================================================
// MAIN
// ======================================================

int main()
{
    if (!glfwInit())
    {
        return -1;
    }

    GLFWwindow* window =
        glfwCreateWindow(
            1600,
            900,
            "FUTBOL 3D COLOMBIA",
            NULL,
            NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(
        window,
        key_callback);

    // ==================================================
    // OPENGL
    // ==================================================

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);

    // ==================================================
    // LUCES
    // ==================================================

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(
        GL_FRONT,
        GL_AMBIENT_AND_DIFFUSE);
    GLfloat lightPos[] =
    {
        0.0f,
        50.0f,
        0.0f,
        1.0f
    };

    glLightfv(
        GL_LIGHT0,
        GL_POSITION,
        lightPos);

    GLfloat ambient[] =
    {
        0.8f,
        0.8f,
        0.8f,
        1.0f
    };

    glLightfv(
        GL_LIGHT0,
        GL_AMBIENT,
        ambient);

    GLfloat diffuse[] =
    {
        1.0f,
        1.0f,
        1.0f,
        1.0f
    };

    glLightfv(
        GL_LIGHT0,
        GL_DIFFUSE,
        diffuse);

    // ==================================================
    // TEXTURAS
    // ==================================================

    cespedTexture =
        loadTexture(
            "textures/cesped.jpg");

    gradasTexture =
        loadTexture(
            "textures/gradas.jpg");

    balonTexture =
        loadTexture(
            "textures/balon.png");

    // ==================================================
    // LOOP
    // ==================================================

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(
            0.0f,
            0.0f,
            0.0f,
            1.0f);

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT);

        // ==============================================
        // PROYECCIÓN
        // ==============================================

        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        gluPerspective(
            45.0,
            1600.0 / 900.0,
            0.1,
            1000.0);

        // ==============================================
        // CÁMARA
        // ==============================================

        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();

        float camX =
            camRadius * sin(camAngle);

        float camZ =
            camRadius * cos(camAngle);

        gluLookAt(
            camX,
            camHeight,
            camZ,

            0,
            8,
            0,

            0,
            1,
            0);

        // ==============================================
        // DIBUJOS
        // ==============================================

        drawSky();

        drawOuterGrass();

        drawStadium();

        drawField();

        drawFieldLines();

        drawGoal();

        drawLights();

        drawBezierCurve();

        drawControlPoints();

        drawBall(animTime);

        // ==============================================
        // ANIMACIÓN
        // ==============================================

        animTime += 0.0007f;

        if (animTime >= 1.0f)
        {
            animTime = 0.0f;
        }

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}