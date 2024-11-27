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
} BoosterPart;


//Partes de la atraccion
BoosterPart base = {{0.0f, 0.0f, 0.0f}, {90.0f, 0.0f, 0.0f}};
BoosterPart arm = {{0.0f, -2.5f, 1.0f}, {0.0f, 0.0f, 0.0f}};
BoosterPart platform = {{0.0f, -4.5f, 3.5f}, {0.0f, 0.0f, 0.0f}};
BoosterPart cube1 = {{-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}};
BoosterPart cube2 = {{1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}};
BoosterPart cube3 = {{-1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
BoosterPart cube4 = {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};

GLfloat cubeColors[] = {0.0f, 1.0f, 0.0f}; //Colores de los cubitos, verde por defecto

GLboolean perspective = GL_TRUE; //Flag para controlar entre perspectiva y ortogonal

GLfloat rotateAllY = 0.0f; //Rotar toda la figura en torno al eje y
GLfloat rotateAllX = 0.0f; //Rotar toda la figura en torno al eje x

GLfloat baseHeight = 1.0f; //Altura de la base
GLfloat baseRadious = 6.0f; //Radio de la base

GLfloat coneWidth = 1.0f;  //Anchura del cono escalado
GLfloat coneHalfSide = 5.5f; //Mitad de lado del cono escalado
GLfloat coneHeight = 6.0f; //Altura del cono escalado

GLfloat automatic = 0.0f; //Ángulo de giro automatico
GLfloat automaticSpeed = 1.0f; //Multiplicador de giro automatico
GLfloat automaticFactor = 1.0f; //Factor de giro de rotación

GLboolean clockwise = GL_TRUE; //Flag para controlar el sentido de giro, sentido horaio por defecto

GLboolean unloadPassengers = GL_FALSE; //Flag para saber si bajar las pasajeros

int lastMouseX, lastMouseY; //Coordenadas de la ultima posicion del mouse

GLboolean leftButtonDown = GL_FALSE; //Flag para controlar el pulsado de click izq
GLboolean ctrlPressed = GL_FALSE; //Flag para controlar el pulsado de ctrl


//Funcion para manejar los movimientos del raton cuando se mantiene presionado un boton del raton
void mouseMotion(int x, int y) {
    
    GLfloat deltaX = (x - lastMouseX) * 0.5f; //Sensibilidad al movimiento en el eje X
    GLfloat deltaY = (y - lastMouseY) * 0.5f; //Sensibilidad al movimiento en el eje Y
    
    if(ctrlPressed){ //Mover en el eje y si click izq + alt + mov raton
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
    
   if(button == GLUT_LEFT_BUTTON){ //Hacer algo solo si esta pulsado el click izquierdo
      if (glutGetModifiers() == GLUT_ACTIVE_CTRL) { //ctrl pulsado
         if (state == GLUT_DOWN) { //Pulsado
      	    ctrlPressed = GL_TRUE;
            lastMouseX = x;
            lastMouseY = y;
         }else if (state == GLUT_UP) { //Soltar
            ctrlPressed = GL_FALSE;
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


//Funcion de temporizador para controlar 's', giro automatico y sentido de giro
//Se vuelve a llamar cada 16 ms para conseguir una percepcion de movimiento a nuestros ojos. (60 fps)
void timer(int value) {

     if(unloadPassengers){ //Bajar pasajeros
        if (automatic >= -5.0f && automatic <= 5.0f) { //Esta en la parte baja de la atraccion
            automatic = 0.0f; //Fijar el ángulo exacto en 0
            return; //Detener el temporizador
        }
     }

     
     if(!clockwise){ //Sentido antihorario
        if(automatic >= 0.0f && automatic <= 180.0f){  //Sube
           automatic += automaticFactor * automaticSpeed; 
        }else{						//Baja
           automatic += automaticFactor * automaticSpeed * 2; //Velocidad por 2
        }
     }else{ //Sentido horario
        if(automatic <= 0.0f && automatic >= -180.0f){  //Sube
           automatic -= automaticFactor * automaticSpeed; //Velocidad por 2
        }else{						//Baja
           automatic -= automaticFactor * automaticSpeed * 2;        
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
        : glOrtho(-20, 20, -20, 20, -20, 20); 				 //Proyeccion ortogonal
    
    glMatrixMode(GL_MODELVIEW); //Volver a matriz de modelo
}


//Funcion para dibujar el triangulo plano
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


    //Cara inferior
    glBegin(GL_QUADS);
    glVertex3f(side, 0, 0);
    glVertex3f(-side, 0, 0);
    glVertex3f(-side, 0, width);
    glVertex3f(side, 0, width);
    glEnd();

    //Caras laterales
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
   
   glColor3f(cubeColors[0], cubeColors[1], cubeColors[2]); //Color de los cubos, el mismo para todos
   
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


void display(){

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Limpiar
   glLoadIdentity(); //Reiniciar las transformaciones
   
   //Si se usa perspectiva, colocar la camara en una ubicacion adecuada para ver la figura
   if(perspective) gluLookAt(0.0, 4.0, 30.0, 0.0, 4.0, 0.0, 0.0, 1.0, 0.0);
   
   glRotatef(rotateAllY, 0.0f, 1.0f, 0.0f); //Rotar todo en el eje y, tecla q-a
   glRotatef(rotateAllX, 1.0f, 0.0f, 0.0f); //Rotar todo en el eje x, click izq + mov raton
   
   //Dibujar base
   glPushMatrix();
   glColor3f(0.5f, 0.5f, 0.5f); //Gris
   glRotatef(base.rotation[0], 1.0f, 0.0f, 0.0f); //Colocar el cilindro en vertical
   glutSolidCylinder(baseRadious, baseHeight, 20, 20); 
   glPopMatrix();
   
   //Dibujar triangulo
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, - coneWidth / 2.0f); //Moverse la mitad de la anchura hacia adelante para ubicar bien
   glColor3f(0.7f, 0.9f, 1.0f); //Azul claro
   drawFlatCone(coneWidth, coneHalfSide, coneHeight); //ancho, lado/2, altura
   glPopMatrix();
    
   glTranslatef(0.0f, 6.0, 0.0f); //Ahora el origen esta en el pico
   glRotatef(automatic, 0.0f, 0.0f, 1.0f); //Giro automatico
   
   //Dibujar brazo
   glPushMatrix();
   glColor3f(1.0f, 0.5f, 0.5f); //Color salmon
   glTranslatef(arm.position[0],arm.position[1] , arm.position[2]); //Colocar brazo para que coincida el extremo
   glScalef(1.0f, 5.0, 1.0f); //Altura 5
   glutSolidCube(1.0);
   glPopMatrix();
   
   //Dibujar plataforma
   glPushMatrix();
   glColor3f(0.5f, 0.5f, 0.5f); //Gris
   glTranslatef(platform.position[0],platform.position[1] , platform.position[2]);
   glRotatef(-automatic, 0.0f, 0.0f, 1.0f); //Plataforma en paralelo
   glScalef(4.0f, 1.0, 4.0f); //Ajustar dimensiones de la plataforma
   glutSolidCube(1.0);
   glPopMatrix();
   
   glRotatef(-automatic, 0.0f, 0.0f, 1.0f); //Contrarestar giro para que no afecte
   glTranslatef(0.0f, 1.0, 0.0f); //Ahora el origen esta en el pico + 1 (mitad altura base + mitad altura cubo)
   glRotatef(automatic, 0.0f, 0.0f, 1.0f); //Giro automatico para los cubos
   
   //Dibujar cubos
   glPushMatrix();
   glTranslatef(platform.position[0],platform.position[1] , platform.position[2]); //Colocarse encima de la plataforma
   glRotatef(-automatic, 0.0f, 0.0f, 1.0f); //Cubos en paralelo
   drawCubes();
   glPopMatrix();
   
   glutSwapBuffers(); //Intercambiar los buffers
}


//Funcion para manejar teclado
//Mover figura en el eje y con q-a, parar con s, cambio de sentido de giro r, haz de vista p-o
void keyboard(unsigned char key, int x, int y) {
 
    switch(key) {
    	case 'p':
    	case 'P': perspective = GL_TRUE; reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)); break;
    	case 'o':
    	case 'O': perspective = GL_FALSE; reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)); break;
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
    	case 27: exit(0); break; //Salir
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
        case 4: cubeColors[0]= 0.0f; cubeColors[1]= 1.0f; cubeColors[2]= 0.0f; break; //Verde
        case 5: cubeColors[0]= 0.0f; cubeColors[1]= 0.0f; cubeColors[2]= 1.0f; break; //Azul 
        case 6: cubeColors[0]= 1.0f; cubeColors[1]= 0.0f; cubeColors[2]= 1.0f; break; //Magenta
        case 7: exit(0); break; //Salir
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
    glutAddMenuEntry("SALIR", 7); //Opcion para salir del programa
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
    glutCreateWindow("BOOSTER");

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
