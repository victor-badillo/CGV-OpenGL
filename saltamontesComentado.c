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
} ParkPart;

enum pov {FV = 1,  TV = 2, SV = 3, RV = 4}; //Puntos de vista, alzado, planta, perfil, atras

enum pov selectedPov = FV;  //Por defecto vista alzada

//Partes de la estructura
ParkPart base = {{0.0f, 0.0f, 0.0f}, {90.0f, 0.0f, 0.0f}};
ParkPart cone = {{0.0f, 0.0f, 0.0f}, {-90.0f, 0.0f, 0.0f}};
ParkPart arm = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
ParkPart platform = {{0.0f, 0.0f, 0.0f}, {90.0f, 90.0f, 90.0f}};
ParkPart cube1 = {{-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}};
ParkPart cube2 = {{1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}};
ParkPart cube3 = {{-1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
ParkPart cube4 = {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};

GLfloat cubeColors[] = {1.0f, 1.0f, 0.0f}; //Color de los cubitos, amarillo por defecto

GLboolean perspective = GL_TRUE; //Flag para controlar entre perspectiva y ortogonal

GLfloat rotateAllY = 0.0f; //Rotar toda la figura en torno al eje y
GLfloat rotateAllX = 0.0f; //Rotar toda la figura en torno al eje x

GLfloat baseHeight = 1.0f; //Altura de la base
GLfloat baseRadious = 4.0f; //Radio de la base

GLfloat coneHeight = 3.0f; //Altura del cono
GLfloat coneRadious = 4.0f; //Radio del cono

GLfloat platformHeight = 1.5f; //Altura del cono invertido
GLfloat platformRadious = 2.0f; //Radio del cono invertido

GLfloat balance = 0.0f; //Grados de giro del brazo
GLboolean reachTop = GL_TRUE; //Controlar grados de giro 45 grados

GLfloat rotatePlatform = 0.0f; //Rotacion de la plataforma verde y pasajeros

GLboolean clockwise = GL_FALSE; //Flag para controlar el sentido de giro

GLboolean altPressed = GL_FALSE; //Flag para el boton alt

GLboolean unloadPassengers = GL_FALSE; //Flag para saber si bajar las pasajeros

int lastMouseX, lastMouseY;//Coordenadas de la ultima posicion del mouse 

GLfloat automaticSpeed = 1.0f; //Velocidad de giro de las plataformas
GLfloat rotationFactor = 1.0f; //Velocidad de giro de las plataformas normal



//Funcion para escoger el pov
void setCameraView() {
    switch(selectedPov){
       case FV: gluLookAt(0.0, 5.0, 20.0, 0.0, 5.0, 0.0, 0.0, 1.0, 0.0); break;
       case TV: gluLookAt(0.0, 20.0, 0.0, 0.0, 0.0, 4.0, 0.0, 1.0, 0.0); break;
       case SV: gluLookAt(20.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 1.0, 0.0); break;
       case RV: gluLookAt(0.0, 0.0, -20.0, 0.0, 0.0, 2.0, 0.0, 1.0, 0.0); break;   
    }
}

//Funcion para bascular el brazo
void basculate(int x, int y) {
    //Ajuste de posicion inicial del mouse si hay un cambio significativo
    if (fabs(lastMouseX - x) > 20) {
        lastMouseX = x - 1;
    }

    //Calculo del desplazamiento en el eje X
    GLfloat deltaX = fabs((x - lastMouseX) * 0.5f); 

    if (reachTop) {  //Subiendo
        balance += deltaX;
        if (balance >= 45.0f) {  //Si llega al maximo, invertir dirección
            reachTop = GL_FALSE;
        }
    } else {  //Bajando
        balance -= deltaX;
        if (balance <= -45.0f) {  //Si llega al minimo, invertir dirección
            reachTop = GL_TRUE;
        }
    }

    lastMouseX = x;  //Actualizar la última posición del mouse
    glutPostRedisplay(); //Actualizar pantalla
}

//Funcion para manejar los movimientos del raton cuando se mantiene presionado un boton del raton
void mouseMotion(int x, int y) {
        
    GLfloat deltaY = (y - lastMouseY) * 0.5f; //Sensibilidad al movimiento en el eje Y    

    if(altPressed) //Mover brazos con el click medio del raton o pulsando ctrl
        rotateAllX += deltaY;
    
    //Actualizar coordenadas
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
        }
    }
    
}

//Funcion de temporizador para controlar 's' y giro de las plataformas y sentido de giro
//Se vuelve a llamar cada 16 ms para conseguir una percepcion de movimiento a nuestros ojos. (60 fps)
void timer(int value) {
     
     if(unloadPassengers){ //Para la atraccion
        if (balance <= -40.0f) {
            return; //Detener el temporizador
        }else{
           balance -= 1.0f;
        }
     }

     if(clockwise){ //Sentido horario
        rotateAllY -= rotationFactor * automaticSpeed;
        rotatePlatform += rotationFactor * automaticSpeed;
     }else{ //Sentido antihorario
        rotateAllY += rotationFactor * automaticSpeed;
        rotatePlatform -= rotationFactor * automaticSpeed;
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


//Funcion para dibujar los cubitos
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



void display(){

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpiar
   glLoadIdentity(); // Reiniciar las transformaciones
   
   setCameraView(); // Escoger vista
   
   // Girar toda la atracción sobre el eje x pulsando alt 
   glRotatef(rotateAllX, 1.0f, 0.0f, 0.0f); // Rotar todo en el eje X
   
   // Dibujar base cilíndrica
   glPushMatrix();
   glColor3f(1.0f, 0.627f, 0.529f); // Color salmón
   glRotatef(base.rotation[0], 1.0f, 0.0f, 0.0f); // Colocar el cilindro en vertical
   glutSolidCylinder(baseRadious, baseHeight, 20, 20); 
   glPopMatrix();
   
   // Girar toda la atracción sobre el eje Y (desde la base azul)
   glRotatef(rotateAllY, 0.0f, 1.0f, 0.0f); // Rotar todo en el eje Y

   // Dibujar cono azul
   glPushMatrix();   
   glRotatef(cone.rotation[0], 1.0f, 0.0f, 0.0f);
   glColor3f(0.7f, 0.9f, 1.0f); // Azul claro
   glutSolidCone(coneRadious, coneHeight, 20, 20); 
   glPopMatrix();
   
   // Situar origen en la punta del cono
   glTranslatef(0.0f, coneHeight, 0.0f);
   glRotatef(balance, 0.0f, 0.0f, 1.0f); 
   
   // Dibujar brazo
   glPushMatrix();
   glTranslatef(3.5f, 0.5f, 0.0f);
   glColor3f(0.75f, 0.75f, 0.75f); // Gris claro
   glScalef(7.0f, 1.0f, 1.0f);
   glutSolidCube(1.0f);
   glPopMatrix();
   
   // Situarse en el eje de la plataforma para mantenerla paralela
   glTranslatef(7.0f, 1.5f, 0.0f);
   glRotatef(-balance, 0.0f, 0.0f, 1.0f);
   
   // Rotar toda la plataforma verde y pasajeros
   glPushMatrix();
   glRotatef(-rotateAllY, 0.0f, 1.0f, 0.0f); //Anular giro general para que la velocidad de la plataforma sea cte.
   glRotatef(rotatePlatform, 0.0f, 1.0f, 0.0f); // Aplicar rotación a la plataforma verde

   // Dibujar cono invertido (plataforma verde)
   glPushMatrix();   
   glRotatef(platform.rotation[0], 1.0f, 0.0f, 0.0f); // Colocar al revés
   glColor3f(0.5647f, 0.9333f, 0.5647f); // Verde claro
   glutSolidCone(platformRadious, platformHeight, 20, 20); 
   glPopMatrix();
   
   // Situarse encima de la plataforma para mantener los cubos siempre encima
   glTranslatef(0.0f, 0.5f, 0.0f);
   
   // Dibujar cubos
   drawCubes(); // Los cubos girarán junto con la plataforma
   glPopMatrix(); // Finalizar la rotación de la plataforma y los cubos
   
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
    	case 'q': rotateAllY += 1.0f; break; //rotar todo sobre el eje y
    	case 'a': rotateAllY -= 1.0f; break; //rotar todo sobre el eje y
    	case 'r': clockwise = !clockwise; break; //Cambiar sentido de giro
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
        case 0: automaticSpeed = 1.0f; break; 
        case 1: automaticSpeed = 3.0f; break; 
        case 2: cubeColors[0]= 1.0f; cubeColors[1]= 0.0f; cubeColors[2]= 0.0f; break; //Rojo 
        case 3: cubeColors[0]= 1.0f; cubeColors[1]= 1.0f; cubeColors[2]= 0.0f; break; //Amarillo
        case 4: cubeColors[0]= 0.15f; cubeColors[1]= 0.38f; cubeColors[2]= 0.61f; break; //Azul 
        case 5: cubeColors[0]= 1.0f; cubeColors[1]= 0.0f; cubeColors[2]= 1.0f; break; //Magenta
    }
    glutPostRedisplay();
}

//Configuración y creación del menú
//2 submenus, uno para velocidad y otro color de los cubitos
//Abrir menu con click derecho
void createMenu() {

    int speedSubMenu = glutCreateMenu(menu); //Submenu 1
    glutAddMenuEntry("NORMAL", 0);
    glutAddMenuEntry("TURBO", 1);

    int colorSubMenu = glutCreateMenu(menu); //Submenu 2
    glutAddMenuEntry("ROJO", 2);
    glutAddMenuEntry("AMARILLO", 3);
    glutAddMenuEntry("AZUL", 4);
    glutAddMenuEntry("MAGENTA", 5);
    

    glutCreateMenu(menu); //Crear menu e incluir submenus
    glutAddSubMenu("VELOCIDAD", speedSubMenu);
    glutAddSubMenu("COLOR", colorSubMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON); //Click derecho para abrir el menu
}


void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Color de fondo negro
    glEnable(GL_DEPTH_TEST); //Habilitar el z-buffer para la profundidad
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //Incluir doble buffer y profundidad
    glutInitWindowSize(WIDTH, HEIGTH); //El tamaño de la ventana
    glutInitWindowPosition(ORIGIN_X, ORIGIN_Y); //Posición de la ventana
    glutCreateWindow("PARK");

    glutDisplayFunc(display); //Renderizado
    glutReshapeFunc(reshape); //Cambiar tamaño de la ventana
    glutKeyboardFunc(keyboard); //Función de teclado
    glutMouseFunc(mouse); //Registrar función de manejo de eventos de ratón
    glutMotionFunc(mouseMotion); //Registrar función de manejo de movimiento del ratón
    glutPassiveMotionFunc(basculate); //Bascular el brazo con el raton
    createMenu(); //Menu
    
    initGL(); //Inicializar OpenGL
    glutTimerFunc(16, timer, 0); //Iniciar temporizador
    glutMainLoop(); //Iniciar el bucle principal

    return 0;
}
