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

typedef struct {
    GLfloat position[3];
    GLfloat rotation[3];
} BoosterPart;



BoosterPart column = {{0.0f,0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BoosterPart base = {{0.0f,0.0f, 0.0f}, {90.0f, 0.0f, 0.0f}};
BoosterPart cube1 = {{-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}};
BoosterPart cube2 = {{1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}};
BoosterPart cube3 = {{-1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
BoosterPart cube4 = {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};

GLfloat cubeColors[] = {0.5f, 0.0f, 0.5f}; //Violeta por defecto

enum pov {FV = 1,  TV = 2, SV = 3, RV = 4}; //Puntos de vista, alzado, planta, perfil, atras

enum pov selectedPov = FV;  //Por defecto vista alzada

GLboolean perspective = GL_TRUE;

GLfloat baseHeight = 1.0f; //Altura de la base
GLfloat baseRadious = 3.0f; //Radio de la base

GLfloat columnHeight = 6.0f; //Altura de la columna

GLfloat armHeight = 5.0f; //Altura de los brazos

GLfloat automatic = 0.0f; //Ángulo de giro automatico
GLfloat automaticSpeed = 1.0f; //Velocidad de giro de la rueda
GLfloat rotationFactor = 1.0f; //Velocidad de giro de la rueda

GLboolean clockwise = GL_FALSE; //Flag para controlar el sentido de giro

GLboolean stop = GL_FALSE; //Flag para saber si bajar las pasajeros

GLfloat spin= 0.0f;

GLfloat rotateAllY = 0.0f; //Rotar toda la figura en torno al eje y
GLfloat rotateAllX = 0.0f; //Rotar toda la figura en torno al eje x

GLboolean altPressed = GL_FALSE; //Flag para el boton alt
GLboolean leftButtonDown = GL_FALSE; //Flag para el boton alt

int lastMouseX, lastMouseY;//Coordenadas de la ultima posicion del mouse 



void setCameraView() {
    switch(selectedPov){
       case FV: gluLookAt(0.0, 5.0, 25.0, 0.0, 5.0, 0.0, 0.0, 1.0, 0.0); break;
       case TV: gluLookAt(0.0, 20.0, 0.0, 0.0, 0.0, 4.0, 0.0, 1.0, 0.0); break;
       case SV: gluLookAt(20.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 1.0, 0.0); break;
       case RV: gluLookAt(0.0, 0.0, -20.0, 0.0, 0.0, 2.0, 0.0, 1.0, 0.0); break;   
    }
}






void mouseMotion(int x, int y) {
    
    
    GLfloat deltaX = (x - lastMouseX) * 0.5f; //Sensibilidad al movimiento en el eje X
    GLfloat deltaY = (y - lastMouseY) * 0.5f; //Sensibilidad al movimiento en el eje Y
    
    if (leftButtonDown) { 
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
    	if (glutGetModifiers() == GLUT_ACTIVE_ALT) {  //Obtener modifiers, entra si click izquierdo + ctrl
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



void timer(int value) {

     if(stop){
        if (automatic >= -5.0f && automatic <= 5.0f) { //Esta en la parte baja de la atraccion
            return; // Detener el temporizador
        }
        
        if(clockwise){
           if (automatic <= -175.0f && automatic >= -185.0f) { //Esta en la parte baja de la atraccion
            return; // Detener el temporizador
           }
        }else{
           if (automatic >= 175.0f && automatic <= 185.0f) { //Esta en la parte baja de la atraccion
            return; // Detener el temporizador
           }
        }
     
     }
     
     if(clockwise){
        automatic -= rotationFactor * automaticSpeed;
        spin += rotationFactor * automaticSpeed;

     }else{
        automatic += rotationFactor * automaticSpeed;
        spin -= rotationFactor * automaticSpeed;

     }
     
     automatic = fmod(automatic, 360.0f); 
     glutPostRedisplay(); //Actualizar la pantalla
     glutTimerFunc(16, timer, 0); 

}




void reshape(int width, int height) {
    if (height == 0) height = 1; // Evitar división por cero
    float aspect = (float)width / (float)height;

    glViewport(0, 0, width, height); // Ajustar viewport
    glMatrixMode(GL_PROJECTION); // Matriz de proyección para ajustar la perspectiva
    glLoadIdentity(); // Cargar la matriz identidad
    
    //A LO MEJOR AQUI TENGO QUE LLAMAR SOLO AQUI A SETCAMERAVIEW()
    if(perspective){
        gluPerspective(45.0, aspect, 0.1, 100.0);
    }else {      
        glOrtho(-30,30, -30, 30, -30, 30);
    }
    
    glMatrixMode(GL_MODELVIEW); // Volver a matriz de modelo
}



//Funcion para dibujar los 4 cubos
void drawCubes(){
   
   glColor3f(cubeColors[0], cubeColors[1], cubeColors[2]);
   
   glPushMatrix();
   glTranslatef(cube1.position[0],cube1.position[1] , cube1.position[2]);
   glutSolidCube(1.0);
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(cube2.position[0],cube2.position[1] , cube2.position[2]);
   glutSolidCube(1.0);
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(cube3.position[0],cube3.position[1] , cube3.position[2]);
   glutSolidCube(1.0);
   glPopMatrix();
   
   glPushMatrix();
   glTranslatef(cube4.position[0],cube4.position[1] , cube4.position[2]);
   glutSolidCube(1.0);
   glPopMatrix();
}


void drawPlatform(){
   //Plataforma superior
   glPushMatrix();
   glColor3f(0.5f, 0.5f, 0.5f); // Color de la base
   glTranslatef(0.0f, -0.5f, 0.0f);   
   glScalef(4.0f, 1.0, 4.0f);
   glutSolidCube(1.0);
   glPopMatrix();
   
   //Cubos
   glPushMatrix();
   glTranslatef(0.0f, 0.5f, 0.0f);
   drawCubes();
   glPopMatrix();
}

void display(){

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpiar
   glLoadIdentity(); // Reiniciar las transformaciones
   
   setCameraView(); // Escoger vista

   
   //DIBUJAR
   glRotatef(rotateAllX, 1.0f, 0.0f, 0.0f); // Rotar todo en el eje X
   glRotatef(rotateAllY, 0.0f, 1.0f, 0.0f); // Rotar todo en el eje Y
   
   //Base cilindrica
   glPushMatrix();
   glColor3f(0.5f, 0.5f, 0.5f); //Gris
   glRotatef(base.rotation[0], 1.0f, 0.0f, 0.0f); //Colocar el cilindro en vertical
   glutSolidCylinder(baseRadious, baseHeight, 20, 20); 
   glPopMatrix();
   
   //Columna
   glPushMatrix();
   glColor3f(0.5f, 0.0f, 0.5f); 
   glTranslatef(0.0f, columnHeight / 2, 0.0f);
   glScalef(1.0f,columnHeight, 1.0f);
   glutSolidCube(1.0f);
   glPopMatrix();
   
   //Origen en el eje del circulo del brazo
   glTranslatef(0.0f, columnHeight, 0.5f);
   glRotatef(automatic, 0.0f, 0.0f, 1.0f); //Giro automatico
   
   //Circulo
   glPushMatrix();
   glColor3f(1.0f, 0.627f, 0.529f);
   glutSolidCylinder(1.0f, 1.2f, 20, 20); 
   glPopMatrix();
   
   //Brazo superior
   glPushMatrix();
   glColor3f(1.0f, 0.0f, 1.0f);
   glTranslatef(0.0f, armHeight / 2, 0.5f);
   glScalef(1.0f,armHeight, 1.0f);
   glutSolidCube(1.0f);
   glPopMatrix();
   
   //Brazo superior
   glPushMatrix();
   glColor3f(1.0f, 0.0f, 1.0f); 
   glTranslatef(0.0f, -armHeight / 2, 0.5f);
   glScalef(1.0f,armHeight, 1.0f);
   glutSolidCube(1.0f);
   glPopMatrix();
   
   
   //Plataforma superior
    
   glPushMatrix();
   glTranslatef(0.0f, armHeight, 3.0f); 
   glRotatef(-automatic, 0.0f, 0.0f, 1.0f);
   glRotatef(spin, 0.0f, 0.0f, 1.0f);
   drawPlatform();
   glPopMatrix();
   
   //Plataforma inferior
   glPushMatrix();
   glTranslatef(0.0f, -armHeight, 3.0f); 
   glRotatef(-automatic, 0.0f, 0.0f, 1.0f);
   glRotatef(spin, 0.0f, 0.0f, 1.0f);
   drawPlatform();
   glPopMatrix();

   
   
   
   glutSwapBuffers(); // Intercambiar los buffers
}


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
    	case 's': if(!stop){ //Bajar pasajeros
    	             stop = GL_TRUE;
    	             
    	          }else{  //Volver a activar atraccion
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
        case 0: automaticSpeed = 1.0f; break; 
        case 1: automaticSpeed = 3.0f; break; 
        case 2: cubeColors[0]= 1.0f; cubeColors[1]= 0.0f; cubeColors[2]= 0.0f; break; //Rojo 
        case 3: cubeColors[0]= 0.5f; cubeColors[1]= 0.0f; cubeColors[2]= 0.5f; break; //Amarillo
        case 4: cubeColors[0]= 0.15f; cubeColors[1]= 0.38f; cubeColors[2]= 0.61f; break; //Azul 
        case 5: cubeColors[0]= 1.0f;  cubeColors[1]= 0.0f; cubeColors[2]= 1.0f; break; //Magenta
        case 6: exit(0); break; //Salir
    }
    glutPostRedisplay();
}

//Configuración y creación del menú
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
    glutCreateWindow("BOOSTER");

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
