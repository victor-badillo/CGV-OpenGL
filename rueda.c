#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#define WIDTH 800
#define HEIGTH 600
#define ORIGIN_X 100
#define ORIGIN_Y 100

//Estructura con posicion y rotacion para las distinas partes de la rueda
typedef struct {
    GLfloat position[3];
    GLfloat rotation[3];
} WheelPart;

enum pov {FV = 1,  TV = 2, SV = 3, RV = 4}; //Puntos de vista, alzado, planta, perfil, atras

enum pov selectedPov = FV;  //Por defecto vista alzada

//Partes de la rueda
WheelPart stick1 = {{0.0f,3.5f, 0.0f}, {00.0f, 0.0f, 0.0f}};
WheelPart stick2 = {{0.0f, 3.5f, 0.0f}, {00.0f, 0.0f, 72.0f}};
WheelPart stick3 = {{0.0f, 3.5f, 0.0f}, {00.0f, 0.0f, 144.0f}};
WheelPart stick4 = {{0.0f, 3.5f, 0.0f}, {00.0f, 0.0f, 216.0f}};
WheelPart stick5 = {{0.0f, 3.5f, 0.0f}, {00.0f, 0.0f, 288.0f}};
WheelPart pedal1 = {{-3.0f, 3.5f, -2.0f}, {00.0f, 0.0f, 288.0f}};
WheelPart pedal2 = {{3.0f, -3.5f, 2.0f}, {00.0f, 0.0f, 288.0f}};

GLfloat pedalsColors[] = {0.5f, 0.0f, 0.5f}; //Color de los pedales, violeta por defecto

GLboolean perspective = GL_TRUE; //Flag para controlar entre perspectiva y ortogonal

GLfloat rotateAllZ = 0.0f; //Rotar toda la figura en torno al eje z
GLfloat rotateAllY = 0.0f; //Rotar toda la figura en torno al eje y
GLfloat rotateAllX = 0.0f; //Rotar toda la figura en torno al eje x

GLboolean clockwise = GL_FALSE; //Flag para controlar el sentido de giro

GLboolean stop = GL_FALSE; //Flag para saber si bajar las pasajeros

GLfloat automaticSpeed = 1.0f; //Multiplicador de velocidad de giro de la rueda
GLfloat rotationFactor = 1.0f; //Velocidad de giro de la rueda

GLboolean altPressed = GL_FALSE; //Flag para el boton alt
GLboolean leftButtonDown = GL_FALSE; //Flag para el click izq

int lastMouseX, lastMouseY; //Coordenadas de la ultima posicion del mouse 


//Funcion para escoger el pov
void setCameraView() {
    switch(selectedPov){
       case FV: gluLookAt(0.0, 5.0, 20.0, 0.0, 5.0, 0.0, 0.0, 1.0, 0.0); break;
       case TV: gluLookAt(0.0, 20.0, 0.0, 0.0, 0.0, 4.0, 0.0, 1.0, 0.0); break;
       case SV: gluLookAt(20.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 1.0, 0.0); break;
       case RV: gluLookAt(0.0, 0.0, -20.0, 0.0, 0.0, 2.0, 0.0, 1.0, 0.0); break;   
    }
}

//Funcion para manejar los movimientos del raton cuando se mantiene presionado un boton del raton
void mouseMotion(int x, int y) {
    
    
    GLfloat deltaX = (x - lastMouseX) * 0.5f; //Sensibilidad al movimiento en el eje X
    GLfloat deltaY = (y - lastMouseY) * 0.5f; //Sensibilidad al movimiento en el eje Y
    
    if (leftButtonDown) { //Mover piernas con el click izquierdo del raton
       rotateAllX += deltaY;
    }else if(altPressed){
       rotateAllY += deltaX;
    }

    //Actualizar coordenadas
    lastMouseX = x; 
    lastMouseY = y;
    
    glutPostRedisplay(); //Actualizar pantalla
}


//Función para manejar los eventos de pulsación de botones del raton
void mouse(int button, int state, int x, int y) {
    
    if(button == GLUT_LEFT_BUTTON){ //Click izquierdo
    	if (glutGetModifiers() == GLUT_ACTIVE_ALT) {  //Obtener modifiers, entra si click izquierdo + alt
    	   if (state == GLUT_DOWN) { //Pulsado
    	      lastMouseX = x;
              lastMouseY = y;
              altPressed = GL_TRUE;
           } else if (state == GLUT_UP) { //Soltar
              altPressed = GL_FALSE;
           }
        }else{ //Solo se pulsa el click izquierdo
           if (state == GLUT_DOWN) { //Pulsado
              leftButtonDown = GL_TRUE;
              lastMouseX = x;
              lastMouseY = y;
           }else if (state == GLUT_UP) { //Soltar
              leftButtonDown = GL_FALSE;
           }
        }  
    }
    
}


//Funcion de temporizador para controlar 's', giro automatico y sentido de giro
//Se vuelve a llamar cada 16 ms para conseguir una percepcion de movimiento a nuestros ojos. (60 fps)
void timer(int value) {

     if(stop) return;
     
     if(clockwise){
        rotateAllZ += rotationFactor * automaticSpeed;
     }else{
        rotateAllZ -= rotationFactor * automaticSpeed;
     }
         
     glutPostRedisplay(); //Actualizar la pantalla
     glutTimerFunc(16, timer, 0); 

}


//Función para ajustar el tamaño de la ventana de visualización
void reshape(int width, int height) {
    height = (height == 0) ? 1 : height; //Evitar división por cero

    glViewport(0, 0, width, height); //Ajustar viewport
    glMatrixMode(GL_PROJECTION); //Matriz de proyección para ajustar la perspectiva
    glLoadIdentity(); //Cargar la matriz identidad
    
    perspective 
        ? gluPerspective(45.0, (float)width / (float)height, 0.1, 100.0) //Proyección en perspectiva
        : glOrtho(-30, 30, -30, 30, -30, 30); 				 //Proyeccion ortogonal
    
    glMatrixMode(GL_MODELVIEW); //Volver a matriz de modelo
}


//Funcion para dibujar metal vertical
void drawVertical(){
   glScalef(0.2f, 12.0f, 0.2f);
   glutSolidCube(1.0);
}

//Funcion para dibujar metal horizontal
void drawHorizontal(){
   glScalef(3.0f, 0.2f, 0.2f);
   glutSolidCube(1.0);
}

//Funcion para dibujar metal perpendicular
void drawPerpendicular(){
   glScalef(0.2f, 0.2f, 1.0f);
   glutSolidCube(1.0);
}


void drawStructure(){
   
   glColor3f(0.8f, 0.8f, 0.8f);
   
   //Vertical
   glPushMatrix();
   glTranslatef(-1.5f, 5.9f, -0.5f);
   drawVertical();
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(-1.5f, 5.9f, 0.5f);
   drawVertical();
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(1.5f, 5.9f, -0.5f);
   drawVertical();
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(1.5f, 5.9f, 0.5f);
   drawVertical();
   glPopMatrix();
   
   //Horizontal
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, -0.5f);
   drawHorizontal();
   glPopMatrix();
     
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, 0.5f);
   drawHorizontal();
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(0.0f, 11.8f, -0.5f);
   drawHorizontal();
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(0.0f, 11.8f, 0.5f);
   drawHorizontal();
   glPopMatrix();
   
   //Perpendicular
   glPushMatrix();
   glTranslatef(-1.5f, 0.0f, 0.0f);
   drawPerpendicular();
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(1.5f, 0.0f, 0.0f);
   drawPerpendicular();
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(-1.5f,11.8f, 0.0f);
   drawPerpendicular();
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(1.5f, 11.8f, 0.0f);
   drawPerpendicular();
   glPopMatrix();
   
   
}

void drawRim(){
   glPushMatrix();
   glRotatef(stick1.rotation[2], 0.0f, 0.0f, 1.0f);
   glTranslatef(stick1.position[0],stick1.position[1] , stick1.position[2]);
   glScalef(1.0f, 5.0f, 0.2f);
   glutSolidCube(1.0);
   glPopMatrix();
   
   //Palo 2
   glPushMatrix();
   glRotatef(stick2.rotation[2], 0.0f, 0.0f, 1.0f);
   glTranslatef(stick2.position[0],stick2.position[1] , stick2.position[2]);
   glScalef(1.0f, 5.0f, 0.2f);
   glutSolidCube(1.0);
   glPopMatrix();
   
   //Palo 3
   glPushMatrix();
   glRotatef(stick3.rotation[2], 0.0f, 0.0f, 1.0f);
   glTranslatef(stick3.position[0],stick3.position[1] , stick3.position[2]);
   glScalef(1.0f, 5.0f, 0.2f);
   glutSolidCube(1.0);
   glPopMatrix();
   
   //Palo 4
   glPushMatrix();
   glRotatef(stick4.rotation[2], 0.0f, 0.0f, 1.0f);
   glTranslatef(stick4.position[0],stick4.position[1] , stick4.position[2]);
   glScalef(1.0f, 5.0f, 0.2f);
   glutSolidCube(1.0);
   glPopMatrix();
   
   //Palo 5
   glPushMatrix();
   glRotatef(stick5.rotation[2], 0.0f, 0.0f, 1.0f);
   glTranslatef(stick5.position[0],stick5.position[1] , stick5.position[2]);
   glScalef(1.0f, 5.0f, 0.2f);
   glutSolidCube(1.0);
   glPopMatrix();
}


void display(){

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpiar
   glLoadIdentity(); // Reiniciar las transformaciones
   
   setCameraView(); // Escoger vista
   
   glRotatef(rotateAllY, 0.0f, 1.0f, 0.0f); //Girar todo sobre eje x con el click izq y raton
   glRotatef(rotateAllX, 1.0f, 0.0f, 0.0f); //Girar todo sobre eje y con alt + raton
   
   //DIBUJAR
   //Carroceria
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, 1.0f);
   drawStructure();
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, -1.0f);
   drawStructure();
   glPopMatrix();
   
   //Aplicar rotacion a todo
   glRotatef(rotateAllZ, 0.0f, 0.0f, 1.0f); //Girar la rueda
   
   
   //Llanta
   glPushMatrix();
   glColor3f(0.7f, 0.7f, 0.7f);
   drawRim();
   glPopMatrix();
   
   
   //Toroide pequeño
   // radio tubo, radio exterior, n segmentos circulo del tubo, n segmentos circulo mayor
   glPushMatrix();
   glColor3f(0.7f, 0.7f, 0.7f);
   glutSolidTorus(0.5, 1.0, 30, 30);
   glPopMatrix();
   
   //Toroide grande
   // radio tubo, radio exterior, n segmentos circulo del tubo, n segmentos circulo mayor
   glPushMatrix();
   glColor3f(1.0f, 0.8f, 0.2f);
   glutSolidTorus(0.5, 6.0, 30, 30);
   glPopMatrix();
   
   //Pedales
   glPushMatrix();
   glColor3f(pedalsColors[0], pedalsColors[1], pedalsColors[2]); 
   glTranslatef(pedal1.position[0],pedal1.position[1] , pedal1.position[2]);
   glRotatef(-rotateAllZ, 0.0f, 0.0f, 1.0f); //Girar la rueda
   glScalef(2.5f, 1.0f, 1.0f);
   glutSolidCube(1.0);
   glPopMatrix();
   
   glPushMatrix();
   glColor3f(pedalsColors[0], pedalsColors[1], pedalsColors[2]); 
   glTranslatef(pedal2.position[0],pedal2.position[1] , pedal2.position[2]);
   glRotatef(-rotateAllZ, 0.0f, 0.0f, 1.0f); //Girar la rueda
   glScalef(2.5f, 1.0f, 1.0f);
   glutSolidCube(1.0);
   glPopMatrix();
   
   
   glutSwapBuffers(); // Intercambiar los buffers
}

//Funcion para manejar teclado
void keyboard(unsigned char key, int x, int y) {

    switch(key) {
    	case 'p':
    	case 'P': perspective = GL_TRUE; reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)); break;
    	case 'o':
    	case 'O': perspective = GL_FALSE; reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)); break;
    	case '1': selectedPov = FV; break; //Alzado
    	case '2': selectedPov = TV; break; //Perfil
    	case '3': selectedPov = SV; break; //Planta
    	case '4': selectedPov = RV; break; //Atras
    	case 'r': clockwise = !clockwise; break;
    	case 's': 
    	          if(!stop){ //Parar rueda
    	             stop = GL_TRUE;
    	             
    	          }else{  //Girar rueda
    	             stop = GL_FALSE;
    	             glutTimerFunc(16, timer, 0);
    	          }
    	          break;
    	case 27: exit(0); break;
    }
    
    glutPostRedisplay();
}


//Función del menú principal
void menu(int option) {
    switch (option) {
        case 0: automaticSpeed = 1.0f; break; //Normal
        case 1: automaticSpeed = 3.0f; break; //Turbo
        case 2: pedalsColors[0]= 1.0f; pedalsColors[1]= 0.0f; pedalsColors[2]= 0.0f; break; //Rojo 
        case 3: pedalsColors[0]= 0.5f; pedalsColors[1]= 0.0f; pedalsColors[2]= 0.5f; break; //Amarillo
        case 4: pedalsColors[0]= 0.15f; pedalsColors[1]= 0.38f; pedalsColors[2]= 0.61f; break; //Azul 
        case 5: pedalsColors[0]= 1.0f;  pedalsColors[1]= 0.0f; pedalsColors[2]= 1.0f; break; //Magenta
        case 6: exit(0); break; //Salir
    }
    glutPostRedisplay();
}

//Configuración y creación del menú
//2 submenus, uno para velocidad y otro color de los pedales
//Abrir menu con click derecho
void createMenu() {

    int speedSubMenu = glutCreateMenu(menu); //Submenu 1
    glutAddMenuEntry("NORMAL", 0);
    glutAddMenuEntry("TURBO", 1);

    int colorSubMenu = glutCreateMenu(menu); //Submenu 2
    glutAddMenuEntry("ROJO", 2);
    glutAddMenuEntry("VIOLETA", 3);
    glutAddMenuEntry("AZUL", 4);
    glutAddMenuEntry("MAGENTA", 5);
    

    glutCreateMenu(menu); //Crear menu e incluir submenus
    glutAddSubMenu("VELOCIDAD", speedSubMenu);
    glutAddSubMenu("COLOR", colorSubMenu);
    glutAddMenuEntry("SALIR", 6);
    glutAttachMenu(GLUT_RIGHT_BUTTON); //Click derecho para abrir el menu
}


void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Color de fondo negro
    glEnable(GL_DEPTH_TEST); // Habilitar el z-buffer para la profundidad
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Incluir doble buffer y profundidad
    glutInitWindowSize(WIDTH, HEIGTH); // El tamaño de la ventana
    glutInitWindowPosition(ORIGIN_X, ORIGIN_Y); // Posición de la ventana
    glutCreateWindow("PARK");

    glutDisplayFunc(display); // Renderizado
    glutReshapeFunc(reshape); // Cambiar tamaño de la ventana
    glutKeyboardFunc(keyboard); // Función de teclado
    glutMouseFunc(mouse); //Registrar función de manejo de eventos de ratón
    glutMotionFunc(mouseMotion); //Registrar función de manejo de movimiento del ratón
    createMenu(); //Menu
    
    initGL(); // Inicializar OpenGL
    glutTimerFunc(16, timer, 0); //Iniciar temporizador
    glutMainLoop(); // Iniciar el bucle principal

    return 0;
}
