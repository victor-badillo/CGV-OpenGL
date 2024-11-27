#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#define WIDTH 800 //Anchura ventana
#define HEIGTH 600 //Altura ventana
#define ORIGIN_X 100 //Coordenada x ventana
#define ORIGIN_Y 100 //Coordenada y ventana

//Estructura con posicion y rotacion para las distinas partes de la atraccion
typedef struct {
    GLfloat position[3];
    GLfloat rotation[3];
} ParkPart;

enum pov {FV = 1,  TV = 2, SV = 3, RV = 4}; //Puntos de vista, alzado, planta, perfil, atras

enum pov selectedPov = FV;  //Por defecto vista alzada

//Partes de la estructura
ParkPart base = {{0.0f, 0.0f, 0.0f}, {90.0f, 0.0f, 0.0f}};
ParkPart arm = {{0.0f, -2.5f, 1.0f}, {0.0f, 0.0f, 0.0f}};
ParkPart platform = {{0.0f, -4.5f, 3.5f}, {0.0f, 0.0f, 0.0f}};
ParkPart cube1 = {{-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}};
ParkPart cube2 = {{1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}};
ParkPart cube3 = {{-1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
ParkPart cube4 = {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};

GLfloat cubeColors[] = {0.5647f, 0.9333f, 0.5647f}; //Colores de los cubitos, verde por defecto

GLboolean perspective = GL_TRUE; //Flag para controlar entre perspectiva y ortogonal

GLfloat rotateAllY = 0.0f; //Rotar toda la figura en torno al eje y
GLfloat rotateAllX = 0.0f; //Rotar toda la figura en torno al eje x

GLfloat baseHeight = 1.0f; //Altura de la base
GLfloat baseRadious = 6.0f; //Radio de la base

GLfloat coneWidth = 1.0f;  //Anchura del cono escalado
GLfloat coneHalfSide = 6.0f; //Mitad de lado del cono escalado
GLfloat coneHeight = 6.0f; //Altura del cono escalado

GLfloat automatic = 0.0f; //Ángulo de giro automatico
GLfloat automaticSpeed = 1.0f; //Ángulo de giro automatico
GLfloat automaticDirection = 1.0f; //Dirección de la rotación

GLboolean clockwise = GL_TRUE; //Flag para controlar el sentido de giro, sentido horaio por defecto

GLboolean unloadPassengers = GL_FALSE; //Flag para saber si bajar las pasajeros

int lastMouseX, lastMouseY; //Coordenadas de la ultima posicion del mouse

GLboolean leftButtonDown = GL_FALSE; //Flag para controlar el pulsado de click izq
GLboolean altPressed = GL_FALSE; //Flag para controlar el pulsado de alt


//Funcion para manejar los movimientos del raton cuando se mantiene presionado un boton del raton
void mouseMotion(int x, int y) {
    
    GLfloat deltaX = (x - lastMouseX) * 0.5f; //Sensibilidad al movimiento en el eje X
    GLfloat deltaY = (y - lastMouseY) * 0.5f; //Sensibilidad al movimiento en el eje Y
    
    if(altPressed){ //Mover en el eje y si click izq + alt + mov raton
       rotateAllY += deltaX;
    }else if (leftButtonDown) { //Mover en el eje x con el click izquierdo del raton
       rotateAllX += deltaY;
    }

    //Actualizar coordenadas
    lastMouseX = x; 
    lastMouseY = y;
    
    glutPostRedisplay(); //Actualizar pantalla
}


//Función para manejar los eventos de pulsación de botones del raton
void mouse(int button, int state, int x, int y) {
    
   if(button == GLUT_LEFT_BUTTON){ //Hacer algo solo si esta pulsado el click izquierod
      if (glutGetModifiers() == GLUT_ACTIVE_ALT) {
         if (state == GLUT_DOWN) { //Pulsado
      	    altPressed = GL_TRUE;
            lastMouseX = x;
            lastMouseY = y;
         }else if (state == GLUT_UP) { //Soltar
            altPressed = GL_FALSE;
         }
      }else{
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


//Funcion para escoger el pov
void setCameraView() {
    switch(selectedPov){
       case FV: gluLookAt(0.0, 5.0, 20.0, 0.0, 5.0, 0.0, 0.0, 1.0, 0.0); break;
       case TV: gluLookAt(0.0, 20.0, 0.0, 0.0, 0.0, 4.0, 0.0, 1.0, 0.0); break;
       case SV: gluLookAt(20.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 1.0, 0.0); break;
       case RV: gluLookAt(0.0, 0.0, -20.0, 0.0, 0.0, 2.0, 0.0, 1.0, 0.0); break;   
    }
}


//Funcion de temporizador para controlar 's', giro automatico y sentido de giro
//Se vuelve a llamar cada 16 ms para conseguir una percepcion de movimiento a nuestros ojos. (60 fps)
void timer(int value) {

     if(unloadPassengers){ //Bajar pasajeros
        if (automatic >= -5.0f && automatic <= 5.0f) { //Esta en la parte baja de la atraccion
            automatic = 0.0f; // Fijar el ángulo exacto en 0
            return; // Detener el temporizador
        }
     }

     
     if(clockwise){ //sentido horario
        if(automatic >= 0.0f && automatic <= 180.0f){  //Sube
           automatic += automaticDirection * automaticSpeed; 
        }else{						//Baja
           automatic += automaticDirection * automaticSpeed * 2;
        }
     }else{ //sentido antihorario
        if(automatic <= 0.0f && automatic >= -180.0f){  //Sube
           automatic -= automaticDirection * automaticSpeed;
        }else{						//Baja
           automatic -= automaticDirection * automaticSpeed * 2;        
        }
     }
          
     automatic = fmod(automatic, 360.0f); //Limitar el angulo al rango [0, 360) usando el modulo
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
        : glOrtho(-21, 21, -21, 21, -21, 21); 				 //Proyeccion ortogonal
    
    glMatrixMode(GL_MODELVIEW); //Volver a matriz de modelo
}


//Funcion para dibujar el triangulo
void drawFlatCone(GLdouble width, GLdouble side, GLdouble height){   
    
    //Caras triangulares
    glBegin(GL_TRIANGLES);
    glVertex3f(0, height, 0);
    glVertex3f(side, 0, 0);
    glVertex3f(-side, 0, 0);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex3f(0, height, width);
    glVertex3f(side, 0, width);
    glVertex3f(-side, 0, width);
    glEnd();


    // Cara inferior
    glBegin(GL_QUADS);
    glVertex3f(side, 0, 0);
    glVertex3f(-side, 0, 0);
    glVertex3f(-side, 0, width);
    glVertex3f(side, 0, width);
    glEnd();

    // Cara laterales
    glBegin(GL_QUADS);
    glVertex3f(0, height, 0);
    glVertex3f(0, height, width);
    glVertex3f(side, 0, width);
    glVertex3f(side, 0, 0);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(0, height, 0);
    glVertex3f(0, height, width);
    glVertex3f(-side, 0, width);
    glVertex3f(-side, 0, 0);
    glEnd();


} 


//Funcion para dibujar los 4 cubos
void drawCubes(){
   
   glColor3f(cubeColors[0], cubeColors[1], cubeColors[2]);
   
   glPushMatrix();
   //glRotatef(-arm.rotation[2], 0.0f, 0.0f, 1.0f); //Compensar giro en el eje z
   glRotatef(-automatic, 0.0f, 0.0f, 1.0f);
   glTranslatef(cube1.position[0],cube1.position[1] , cube1.position[2]);
   glutSolidCube(1.0);
   glPopMatrix();
   
   glPushMatrix();
   //glRotatef(-arm.rotation[2], 0.0f, 0.0f, 1.0f); //Compensar giro en el eje z
   glRotatef(-automatic, 0.0f, 0.0f, 1.0f);
   glTranslatef(cube2.position[0],cube2.position[1] , cube2.position[2]);
   glutSolidCube(1.0);
   glPopMatrix();
   
   glPushMatrix();
   //glRotatef(-arm.rotation[2], 0.0f, 0.0f, 1.0f); //Compensar giro en el eje z
   glRotatef(-automatic, 0.0f, 0.0f, 1.0f);
   glTranslatef(cube3.position[0],cube3.position[1] , cube3.position[2]);
   glutSolidCube(1.0);
   glPopMatrix();
   
   glPushMatrix();
   //glRotatef(-arm.rotation[2], 0.0f, 0.0f, 1.0f); //Compensar giro en el eje z
   glRotatef(-automatic, 0.0f, 0.0f, 1.0f);
   glTranslatef(cube4.position[0],cube4.position[1] , cube4.position[2]);
   glutSolidCube(1.0);
   glPopMatrix();
}


void display(){

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Limpiar
   glLoadIdentity(); //Reiniciar las transformaciones
   
   setCameraView(); //Escoger vista
   
   glRotatef(rotateAllY, 0.0f, 1.0f, 0.0f); //Rotar todo en el eje y, tecla q y a
   glRotatef(rotateAllX, 1.0f, 0.0f, 0.0f); //Rotar todo en el eje x, click izq + mov raton
   
   //Dibujar base
   glPushMatrix();
   glColor3f(0.5f, 0.5f, 0.5f); //Gris
   glRotatef(base.rotation[0], 1.0f, 0.0f, 0.0f); //Colocar el cilindro en vertical
   glutSolidCylinder(baseRadious, baseHeight, 20, 20); 
   glPopMatrix();
   
   //Dibujar cono escalado
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, - coneWidth / 2.0f); //Moverse la mitad de la anchura hacia adelante para ubicar bien
   glColor3f(0.7f, 0.9f, 1.0f); //Azul claro
   drawFlatCone(coneWidth, coneHalfSide, coneHeight); //ancho, lado/2, altura
   glPopMatrix();
    
   //Dibujar brazo
   glPushMatrix();
   glTranslatef(0.0f, 6.0, 0.0f); //Ahora el origen esta en el pico
   //glRotatef(arm.rotation[2], 0.0f, 0.0f, 1.0f); //Rotar todo el brazo 
   glRotatef(automatic, 0.0f, 0.0f, 1.0f); //Giro automatico
   glColor3f(1.0f, 0.627f, 0.529f); //Color salmon
   glTranslatef(arm.position[0],arm.position[1] , arm.position[2]); //Colocar brazo para que coincida el extremo
   glScalef(1.0f, 5.0, 1.0f); //Altura 5
   glutSolidCube(1.0);
   glPopMatrix();
   
   //Dibujar plataforma
   glPushMatrix();
   glTranslatef(0.0f, 6.0, 0.0f); //Ahora el origen esta en el pico
   //glRotatef(arm.rotation[2], 0.0f, 0.0f, 1.0f); //Rotar la plataforma
   glRotatef(automatic, 0.0f, 0.0f, 1.0f); //Giro automatico
   glColor3f(0.5f, 0.5f, 0.5f); // Color de la base
   glTranslatef(platform.position[0],platform.position[1] , platform.position[2]);
   glRotatef(-automatic, 0.0f, 0.0f, 1.0f); //Compensar
   glScalef(4.0f, 1.0, 4.0f);
   glutSolidCube(1.0);
   glPopMatrix();
   
   //Dibujar cubos
   glPushMatrix();
   glTranslatef(0.0f, 7.0, 0.0f); //Cambio el origen para que los cubos se mantengan encima de la plataforma
   //glRotatef(arm.rotation[2], 0.0f, 0.0f, 1.0f); //Aplicar rotacion del brazo
   glRotatef(automatic, 0.0f, 0.0f, 1.0f); //Giro automatico
   glTranslatef(platform.position[0],platform.position[1] , platform.position[2]); 
   drawCubes();
   glPopMatrix();
   
   
   glutSwapBuffers(); //Intercambiar los buffers
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
    	case 'q': rotateAllY += 1.0f; break; //rotar todo sobre el eje y
    	case 'a': rotateAllY -= 1.0f; break; //rotar todo sobre el eje y
    	case 'r': clockwise = !clockwise; break;
    	case 's': 
    	          if(!unloadPassengers){ //Bajar pasajeros
    	             unloadPassengers = GL_TRUE;
    	             
    	          }else{  //Volver a activar atraccion
    	             unloadPassengers = GL_FALSE;
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
        case 0: automaticSpeed = 0.5f; break; //Lento
        case 1: automaticSpeed = 1.0f; break; //Normal
        case 2: automaticSpeed = 2.0f; break; //Rapido
        case 3: cubeColors[0]= 1.0f; cubeColors[1]= 0.0f; cubeColors[2]= 0.0f; break; //Rojo 
        case 4: cubeColors[0]= 0.5647f; cubeColors[1]= 0.9333f; cubeColors[2]= 0.5647f; break; //Verde
        case 5: cubeColors[0]= 0.15f; cubeColors[1]= 0.38f; cubeColors[2]= 0.61f; break; //Azul 
        case 6: cubeColors[0]= 1.0f; cubeColors[1]= 0.0f; cubeColors[2]= 1.0f; break; //Magenta
    }
    glutPostRedisplay();
}

//Configuración y creación del menú
//2 submenus, uno para velocidad y otro color de los cubitos
//Abrir menu con click derecho
void createMenu() {

    int speedSubMenu = glutCreateMenu(menu); //Submenu 1
    glutAddMenuEntry("LENTO", 0);
    glutAddMenuEntry("NORMAL", 1);
    glutAddMenuEntry("RAPIDO", 2);

    int colorSubMenu = glutCreateMenu(menu); //Submenu 2
    glutAddMenuEntry("ROJO", 3);
    glutAddMenuEntry("VERDE", 4);
    glutAddMenuEntry("AZUL", 5);
    glutAddMenuEntry("MAGENTA", 6);
    

    glutCreateMenu(menu); //Crear menu e incluir submenus
    glutAddSubMenu("VELOCIDAD", speedSubMenu);
    glutAddSubMenu("COLOR", colorSubMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON); //Click derecho para abrir el menu
}


void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Color de fondo negro
    glEnable(GL_DEPTH_TEST); //Habilitar el z-buffer para la profundidad
}

//Función principal para inicializar GLUT y los callbacks
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //Incluir doble buffer y profundidad
    glutInitWindowSize(WIDTH, HEIGTH); //Tamaño de la ventana
    glutInitWindowPosition(ORIGIN_X, ORIGIN_Y); //Posición de la ventana
    glutCreateWindow("NORIA");

    glutDisplayFunc(display); //Renderizado
    glutReshapeFunc(reshape); //Cambiar tamaño de la ventana
    glutKeyboardFunc(keyboard); //Función de teclado
    glutMouseFunc(mouse); //Registrar función de manejo de eventos de ratón
    glutMotionFunc(mouseMotion); //Registrar función de manejo de movimiento del ratón
    createMenu(); //Menu
    
    initGL(); // Inicializar OpenGL
    glutTimerFunc(16, timer, 0); //Iniciar temporizador
    glutMainLoop(); // Iniciar el bucle principal

    return 0;
}
