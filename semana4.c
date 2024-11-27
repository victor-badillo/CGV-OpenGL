#include <stdbool.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

//Altura y anchura de la ventana
#define WIDTH 800
#define HEIGTH 600
//Coordenadas de la ventana
#define ORIGIN_X 100
#define ORIGIN_Y 100

//Estructura con posicion y rotacion para las distinas partes del cuerpo
typedef struct {
    GLfloat position[3];
    GLfloat rotation[3];
} BodyPart;

//Enum para asignar un int a cada extremidad
enum limb {LA = 1, RA = 2, LL = 3, RL = 4};

//Variable para especificar la extremidad seleccionada, brazo izquierdo por defecto
enum limb selectedLimb = LA;

//Brazo izquierdo
BodyPart hipL = {{0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart upperLegL = {{0.0f, -1.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart kneeL = {{0.0f, -3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart lowerLegL = {{0.0f, -1.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart footL = {{0.0f, -2.5f, 0.0f}, {90.0f, 0.0f, 0.0f}};

//Brazo derecho
BodyPart hipR = {{0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart upperLegR = {{0.0f, -1.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart kneeR = {{0.0f, -3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart lowerLegR = {{0.0f, -1.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart footR = {{0.0f, -2.5f, 0.0f}, {90.0f, 0.0f, 0.0f}};

//Cabeza y torso
BodyPart core = {{0.0f, 3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart head = {{0.0f, 7.0f, 0.0f}, {0.0f, 270.0f, 0.0f}};

//Brazo izquerdo
BodyPart shoulderL = {{0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart bicepsL = {{0.0f, -1.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart elbowL = {{0.0f, -3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart forearmL = {{0.0f, -1.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart handL = {{0.0f, -2.5f, 0.0f}, {90.0f, 0.0f, 0.0f}};

//Brazo derecho
BodyPart shoulderR = {{0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart bicepsR = {{0.0f, -1.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart elbowR = {{0.0f, -3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart forearmR = {{0.0f, -1.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
BodyPart handR = {{0.0f, -2.5f, 0.0f}, {90.0f, 0.0f, 0.0f}};

GLboolean moveAll = GL_FALSE; //Flag para escoger movimiento entre todo o extremo
GLboolean wireframeMode = GL_TRUE; //Flag para modo alambre o sólido
GLfloat rotationSpeed = 1.0f; //Velocidad de rotación de las piernas

GLfloat automatic = 0.0f; //Ángulo de rotación de las piernas
GLfloat rotationDirection = 1.0f; //Dirección de la rotación

int lastMouseX, lastMouseY; //Coordenadas de la ultima posicion del mouse
GLboolean leftButtonDown = GL_FALSE; //Flag para el click izquierdo del raton
GLboolean middleButtonDown = GL_FALSE; //Flag para el click medio del raton

GLboolean automaticMove = GL_TRUE; //Flag para el movimiento automatico

GLboolean ctrlPressed = GL_FALSE; //Flag para mover los brazos con la tecla ctrl


//Funcion para manejar los movimientos del raton cuando se mantiene presionado un boton del raton
//El movimiento del raton de arriba abajo mueve las piernas hacia delante y hacia atras como se espera
//El movimiento del raton de izquierda a derecha rota las piernas sobre si mismas como se espera
void mouseMotion(int x, int y) {
    
    
    GLfloat deltaX = (x - lastMouseX) * 0.5f; //Sensibilidad al movimiento en el eje X
    GLfloat deltaY = (y - lastMouseY) * 0.5f; //Sensibilidad al movimiento en el eje Y
    

    if(middleButtonDown || ctrlPressed){ //Mover brazos con el click medio del raton o pulsando ctrl

        if (moveAll) {
            shoulderL.rotation[0] += deltaY; //Mover el hombro izquierdo en el eje X
	    shoulderL.rotation[1] += deltaX; //Mover el hombro izquierdo en el eje Y
            shoulderR.rotation[0] += deltaY; //Mover el hombro derecho en el eje X
            shoulderR.rotation[1] += deltaX; //Mover el hombro derecho en el eje Y
        } else {
            elbowL.rotation[0] += deltaY; //Mover el codo en el eje X
            elbowL.rotation[1] += deltaX; //Mover el codo en el eje Y
            elbowR.rotation[0] += deltaY; //Mover el codo en el eje X
            elbowR.rotation[1] += deltaX; //Mover el codo en el eje Y
        }

    }else if (leftButtonDown) { //Mover piernas con el click izquierdo del raton
        
        if (moveAll) {
            hipL.rotation[0] += deltaY; //Mover la pierna en el eje X
            hipL.rotation[1] += deltaX; //Mover la pierna en el eje Y
            hipR.rotation[0] += deltaY; //Mover la pierna en el eje X
            hipR.rotation[1] += deltaX; //Mover la pierna en el eje Y
        } else {
            kneeL.rotation[0] += deltaY; //Mover la rodilla en el eje X
            kneeL.rotation[1] += deltaX; //Mover la rodilla en el eje Y
            kneeR.rotation[0] += deltaY; //Mover la rodilla en el eje X
            kneeR.rotation[1] += deltaX; //Mover la rodilla en el eje Y
        }
    }

    //Actualizar coordenadas
    lastMouseX = x; 
    lastMouseY = y;
    
    glutPostRedisplay(); //Actualizar pantalla
}


//Función para manejar los eventos de pulsación de botones del raton
void mouse(int button, int state, int x, int y) {
    
    if(button == GLUT_LEFT_BUTTON){ //Click izquierdo
    	if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {  //Obtener modifiers, entra si click izquierdo + ctrl
    	   if (state == GLUT_DOWN) { //Pulsado
    	      lastMouseX = x;
              lastMouseY = y;
              ctrlPressed = GL_TRUE;
           } else if (state == GLUT_UP) { //Soltar
              ctrlPressed = GL_FALSE;
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
    }else if(button == GLUT_MIDDLE_BUTTON){ //Click medio
        if (state == GLUT_DOWN) { //Pulsado
            middleButtonDown = GL_TRUE;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) { //Soltar
            middleButtonDown = GL_FALSE;
        }
    }
    
}

//Funcion de temporizador para controlar la rotacion automatica de las piernas
//Se vuelve a llamar cada 16 ms para conseguir una percepcion de movimiento a nuestros ojos. (60 fps)
void timer(int value) {

    if (automaticMove) {
        
        automatic += rotationDirection * rotationSpeed; //Incrementar el angulo de rotacion de la cadera      
        automatic = fmod(automatic, 360.0f); //Limitar el angulo al rango [0, 360) usando el modulo

        glutPostRedisplay(); //Actualizar la pantalla
        //Llamar al temporizador de nuevo si automaticMove sigue siendo true
        glutTimerFunc(16, timer, 0); 
    }
}



//Esfera magenta
void drawSphere() {
    glColor3f(1.0f, 0.0f, 1.0f);
    if (wireframeMode) {
        glutWireSphere(0.5, 20, 20);
    } else {
        glutSolidSphere(0.5, 50, 50);
    }
}

//Cubo naranja
void drawCube() {
    glColor3f(1.0f, 0.5f, 0.0f);
    if (wireframeMode) {
        glutWireCube(1.0);
    } else {
        glutSolidCube(1.0);
    }
}

//Cono magenta
void drawFigure(){
   glColor3f(1.0f, 0.0f, 1.0f);
    if (wireframeMode) {
        glutWireCone(0.5, 2.0, 20, 20);
    } else {
        glutSolidCone(0.5, 2.0, 20, 20);
    }
}


//Función para dibujar una extremidad
void drawLimb(BodyPart* hip, BodyPart* upperLeg, BodyPart* knee, BodyPart* lowerLeg, BodyPart* foot) {

    //Aplicar transformaciones para la esfera superior
    glTranslatef(hip->position[0], hip->position[1], hip->position[2]);
    glRotatef(hip->rotation[0], 1.0f, 0.0f, 0.0f);
    glRotatef(hip->rotation[1], 0.0f, 1.0f, 0.0f);
    glRotatef(hip->rotation[2], 0.0f, 0.0f, 1.0f);
    drawSphere();

    //Dibujar el primer cubo
    glPushMatrix(); //Guardar estado
    glTranslatef(upperLeg->position[0], upperLeg->position[1], upperLeg->position[2]);
    glScalef(1.0f, 2.0f, 1.0f); //Escalo el cubo para que tenga el doble de altura
    drawCube();
    glPopMatrix(); //Restaurar estado

    //Aplicar transformaciones para la segunda esfera
    glTranslatef(knee->position[0], knee->position[1], knee->position[2]);
    glRotatef(knee->rotation[0], 1.0f, 0.0f, 0.0f);
    glRotatef(knee->rotation[1], 0.0f, 1.0f, 0.0f);
    glRotatef(knee->rotation[2], 0.0f, 0.0f, 1.0f);
    drawSphere();

    //Dibujar el segundo cubo
    glPushMatrix();
    glTranslatef(lowerLeg->position[0], lowerLeg->position[1], lowerLeg->position[2]);
    glScalef(1.0f, 2.0f, 1.0f); //Escalo el cubo para que tenga el doble de altura
    drawCube();
    glPopMatrix();

    //Dibujar el cono 
    glPushMatrix();
    glTranslatef(foot->position[0], foot->position[1], foot->position[2]);
    glRotatef(foot->rotation[0], 1.0f, 0.0f, 0.0f);
    drawFigure();
    glPopMatrix();
}


//Dibujar torso, cubo escalado
void drawBody(){
    glTranslatef(core.position[0], core.position[1], core.position[2]);
    glScalef(3.0f,6.0f,1.0f);
    drawCube();
}

//Dibujar la cabeza
void drawHead(){
    glTranslatef(head.position[0], head.position[1], head.position[2]);
    glRotatef(head.rotation[1], 0.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 0.0f, 1.0f);
    if (wireframeMode) {
        glutWireTeapot(1.0);
    } else {
        glutSolidTeapot(1.0);
    }
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Limpiar
    glLoadIdentity(); //Reiniciar las transformaciones

    gluLookAt(0.0, 5.0, 20, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //Cambiar la posicion de la camara para ver bien la figura
    
    //Dibujar cabeza
    glPushMatrix();
    drawHead();
    glPopMatrix();
    
    //Dibujar brazo izquierdo desde vista usuario
    glPushMatrix();
    glTranslatef(-2.0f, 6.0f, 0.0f);
    drawLimb(&shoulderL, &bicepsL, &elbowL, &forearmL, &handL);
    glPopMatrix();
    
    //Dibujar brazo derecho desde vista usuario
    glPushMatrix();
    glTranslatef(2.0f, 6.0f, 0.0f);
    drawLimb(&shoulderR, &bicepsR, &elbowR, &forearmR, &handR);
    glPopMatrix();
    
    //Dibujar cuerpo
    glPushMatrix();
    drawBody();
    glPopMatrix();

    //Dibujar pierna izquierda desde vista usuario
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, 0.0f);
    glRotatef(automatic, 0.0f, 1.0f, 0.0f); //Rotar automáticamente alrededor del eje Y
    drawLimb(&hipL, &upperLegL, &kneeL, &lowerLegL, &footL);
    glPopMatrix();

    //Dibujar pierna derecha desde vista usuario
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef(automatic, 0.0f, 1.0f, 0.0f); //Rotar automáticamente alrededor del eje Y
    drawLimb(&hipR, &upperLegR, &kneeR, &lowerLegR, &footR);
    glPopMatrix();

    glutSwapBuffers(); //Intercambiar los buffers
}

//Función para ajustar el tamaño de la ventana de visualización
void reshape(int width, int height) {
    if (height == 0) height = 1; //Evitar división por cero
    float aspect = (float)width / (float)height;

    glViewport(0, 0, width, height); //Ajustar viewport
    glMatrixMode(GL_PROJECTION); //Matriz de proyección para ajustar la perspectiva
    glLoadIdentity(); //Cargar la matriz identidad
    gluPerspective(45.0, aspect, 0.1, 100.0); //Visión perspectiva
    glMatrixMode(GL_MODELVIEW); //Volver a matriz de modelo
}

//Eje x -> q-a  Eje y -> w-s  Eje z -> e-d   (LA -> 1  RA -> 2  LL -> 3  RL -> 4) (Cambiar entre todo y extremo con r)
void keyboard(unsigned char key, int x, int y) {

    GLfloat* rotation;

    //Declarar rotación dependiendo de la parte seleccionada (part)
    if(selectedLimb == 1) { //Brazo izquierdo
        rotation = moveAll ? shoulderL.rotation : elbowL.rotation;
    } else if(selectedLimb == 2) { //Brazo derecho
        rotation = moveAll ? shoulderR.rotation : elbowR.rotation;
    } else if(selectedLimb == 3) { //Pierna izquierda
        rotation = moveAll ? hipL.rotation : kneeL.rotation;
    } else { //Pierna derecha
        rotation = moveAll ? hipR.rotation : kneeR.rotation;
    }

    switch (key) {
        case '1': selectedLimb = LA; break; //Seleccionar brazo izquierdo
        case '2': selectedLimb = RA; break; //Seleccionar brazo derecho
        case '3': selectedLimb = LL; break; //Seleccionar pierna izquierda
        case '4': selectedLimb = RL; break; //Seleccionar pierna derecha
        case 'q': rotation[0] += 1.0f; break; //Adelante en el eje X
        case 'a': rotation[0] -= 1.0f; break; //Atrás en el eje X
        case 'w': rotation[1] += 1.0f; break; //Derecha en el eje Y
        case 's': rotation[1] -= 1.0f; break; //Izquierda en el eje Y
        case 'e': rotation[2] += 1.0f; break; //Sentido horario en el eje Z
        case 'd': rotation[2] -= 1.0f; break; //Sentido antihorario en el eje Z
        case 'r': moveAll = !moveAll; break; // Alternar entre mover todo o el extremo
        case 27: exit(0); break; //Cerrar el programa
    }

    glutPostRedisplay(); //Actualizar la pantalla después de realizar cambios
}



//Función del menú principal
void menu(int option) {
    switch (option) {
        case 0: wireframeMode = GL_FALSE; break; //Sólido
        case 1: wireframeMode = GL_TRUE; break; //Alambre
        case 2: rotationSpeed = 0.5f; break; //Velocidad baja
        case 3: rotationSpeed = 1.0f; break; //Velocidad normal
        case 4: rotationSpeed = 2.0f; break; //Velocidad alta
        case 5: if(!automaticMove) {automaticMove = GL_TRUE; glutTimerFunc(16, timer, 0); } break; //Activar giro automatico
        case 6: automaticMove = GL_FALSE; break; //Desactivar giro automatico
        case 7: exit(0); break; //Salir
    }
    glutPostRedisplay();
}

//Configuración y creación del menú
//3 submenus, uno para solido/alambre, otro para velocidad de giro automatico y otro para activar/desactivar el giro automatico
//Abrir menu con click derecho
void createMenu() {

    int wireframeSubMenu = glutCreateMenu(menu); //Submenu 1
    glutAddMenuEntry("SOLIDO", 0);
    glutAddMenuEntry("ALAMBRE", 1);

    int speedSubMenu = glutCreateMenu(menu); //Submenu 2
    glutAddMenuEntry("BAJA", 2);
    glutAddMenuEntry("NORMAL", 3);
    glutAddMenuEntry("ALTA", 4);
    
    int automaticSubMenu = glutCreateMenu(menu); //Submenu 3
    glutAddMenuEntry("ON", 5);
    glutAddMenuEntry("OFF", 6);

    glutCreateMenu(menu); //Crear menu e incluir submenus
    glutAddSubMenu("VISUALIZACION", wireframeSubMenu);
    glutAddSubMenu("VELOCIDAD DE ROTACION", speedSubMenu);
    glutAddSubMenu("GIRO AUTOMATICO", automaticSubMenu);
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //Incluyo doble buffer y profundidad
    glutInitWindowSize(WIDTH, HEIGTH); //El tamaño de la ventana
    glutInitWindowPosition(ORIGIN_X, ORIGIN_Y); //Posición de la ventana
    glutCreateWindow("RESULTADO FINAL CUERPO ENTERO");
    

    glutDisplayFunc(display); //Renderizado
    glutReshapeFunc(reshape); //Cambiar tamaño de la ventana
    glutKeyboardFunc(keyboard); //Función para captar teclas no especiales
    glutMouseFunc(mouse); //Registrar función de manejo de eventos de ratón
    glutMotionFunc(mouseMotion); //Registrar función de manejo de movimiento del ratón
    createMenu(); //Menu

    initGL(); //Inicializar OpenGL
    if (automaticMove) { //Flag para activar rotación automatica de las piernas, podría estar en false por defecto y seguiria funcionando
        glutTimerFunc(16, timer, 0); //Iniciar temporizador
    }
    glutMainLoop(); //Iniciar el bucle principal

    return 0;
}
