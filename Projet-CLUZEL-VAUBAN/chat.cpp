
/*appel aux différentes bibliothèques*/

#include <cstdio>
#include <cstdlib>
#ifdef __APPLE__
#include <GLUT/glut.h> /* Pour Mac OS X */
#else
#include <GL/glut.h>   /* Pour les autres systemes */
#endif
#include <GL/freeglut.h>
#include <jpeglib.h>
#include <jerror.h>
#include <math.h>
#ifdef __WIN32
#pragma comment (lib, "jpeg.lib")
#endif

/*déclaration d'une classe point qui nous servira pour la construction de notre cylindre en primitives*/
class Point{
    public :
    //coordonnées x, y et z du point
    double x;
    double y;
    double z;
    // couleur r, v et b du point
    float r;
    float g;
    float b;
};

/*declaration des variables du cylindre*/
const int n = 10;      //nombre de faces
float h = 0.8;         //hauteur
float r=0.4;           //rayon
Point pCylindre[2*n];  //déclaration d'un tableau qui contiendra les points du cylindre
int fCylindre[n][4];   //déclaration d'un tableau qui contiendra les faces du cylindre, 4 points par face

/*déclaration d'un tableau qui contiendra la texture chargée*/
unsigned char image[256*256*3];

/*déclarations de variables utiles au déplacement de la caméra en fonction de la souris/clavier */
char presse;
int anglex=10, angley, x,y,xold,yold;

/*déclarations de variables qui vont nous servir pour les animations*/
GLfloat angle=-30;
GLfloat angle2=30;
GLfloat angle3=0;
GLfloat distance=0;
GLfloat zoom=10;


/*Prototype des fonctions*/
void affichage();
void clavier(unsigned char touche,int x,int y);
void SpecialClavier( int touche, int x, int y );
void reshape(int x,int y);
void mouse(int bouton,int etat,int x,int y);
void mousemotion(int x,int y);
void loadJpegImage(char *fichier);
void timer(int t);



/*fonction principale*/
int main(int argc,char **argv)
{


  /* initialisation de glut et creation de la fenetre */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); //choix des modes de glut
  glutInitWindowPosition(200,200); // définir position de la fenêtre
  glutInitWindowSize(500,500);     // définir la taille de la fenêtre
  glutCreateWindow("chat");        // création de la fenêtre intitulée chat

  /* Initialisation d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0); // efface la couleur de fond
  glColor3f(1.0,1.0,1.0); // couleur courante définie à blanc
  glEnable(GL_DEPTH_TEST); // active le test de profondeur
  glEnable(GL_COLOR_MATERIAL);



  /*eclairage*/

    //lumière ponctuelle
    GLfloat light1_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light1_position[] = { -5, 6.5, 3,1.0 };

    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.1);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.1);

    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 180.0);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 1.0);

    //lumière directionnelle
    GLfloat light2_ambient[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat light2_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat light2_specular[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat light2_position[] = { 0.0, 1.0, 0.0, 1.0 };

    glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.1);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.1);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.1);



  /*enregistrement des fonctions de rappel*/
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutSpecialFunc(SpecialClavier);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mousemotion);
  glutTimerFunc(1000.0/6.0, timer, 0);



  /*Entree dans la boucle principale glut*/
  glutMainLoop();
  return 0;
}


void affichage()
{
    /*initalisation projection et caméra*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-zoom,zoom,-zoom,zoom,-zoom,zoom);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-4.0,0.0,0.0);
    glRotatef(anglex,1.0,0.0,0.0);
    glRotatef(angley,0.0,1.0,0.0);

    /*effacement de l'image avec la couleur de fond*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,0.0);


/*
    //Repère
    //axe x en rouge
    glBegin(GL_LINES);
        glColor3f(1.0,0.0,0.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(1, 0,0.0);
    glEnd();
    //axe des y en vert
    glBegin(GL_LINES);
    	glColor3f(0.0,1.0,0.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 1,0.0);
    glEnd();
    //axe des z en bleu
    glBegin(GL_LINES);
    	glColor3f(0.0,0.0,1.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 0,1.0);
    glEnd();*/


glPushMatrix();//chat en entier
glTranslatef(-distance,0.0,0.0); //animation qui fait bouger le chat en entier


    glPushMatrix();// tête en entier

        // dessin de la tête du chat
        glPushMatrix();
            glColor3f(1.0,0.5,0.0);
            glTranslatef(5.0,2.0,0.0);
            glutSolidSphere(0.8,10,10);
        glPopMatrix();

        //dessin museau du chat
        glPushMatrix();
            glColor3f(1.0,1.0,1.0);
            glTranslatef(4.4,1.8,0.0);
            glutSolidSphere(0.3,10,10);
        glPopMatrix();

        //dessin du nez du chat
        glPushMatrix();
            glColor3f(0.8,0.4,0.4);
            glTranslatef(4.15,1.85,0.0);
            glutSolidSphere(0.1,10,10);
        glPopMatrix();

        //dessin yeux du chat
            //partie gauche
            glPushMatrix();
                glColor3f(0.6,0.8,0.5);
                glTranslatef(4.4,2.0,-0.4);
                glutSolidSphere(0.15,10,10);
            glPopMatrix();

            //partie droite
            glPushMatrix();
                glColor3f(0.6,0.8,0.5);
                glTranslatef(4.4,2.0,0.4);
                glutSolidSphere(0.15,10,10);
            glPopMatrix();

        //dessin pupilles
            //partie droite
            glPushMatrix();
            glColor3f(0.0,0.0,0.0);
            glTranslatef(4.24,1.85,0.42);
            glScalef(0.3,0.3,0.3);
                glBegin(GL_POLYGON);
                    glVertex3f(0.15,1.0,0.0);
                    glVertex3f(-0.075,0.75,0.15);
                    glVertex3f(-0.15,0.5,0.2);
                    glVertex3f(-0.075,0.25,0.15);
                    glVertex3f(0.15,0.0,0.0);
                    glVertex3f(-0.075,0.25,-0.15);
                    glVertex3f(-0.15,0.5,-0.2);
                    glVertex3f(-0.075,0.75,-0.15);
                glEnd();
            glPopMatrix();

            //partie gauche
            glPushMatrix();
            glColor3f(0.0,0.0,0.0);
            glTranslatef(4.24,1.85,-0.42);
            glScalef(0.3,0.3,0.3);
                glBegin(GL_POLYGON);
                    glVertex3f(0.15,1.0,0.0);
                    glVertex3f(-0.075,0.75,0.15);
                    glVertex3f(-0.15,0.5,0.2);
                    glVertex3f(-0.075,0.25,0.15);
                    glVertex3f(0.15,0.0,0.0);
                    glVertex3f(-0.075,0.25,-0.15);
                    glVertex3f(-0.15,0.5,-0.2);
                    glVertex3f(-0.075,0.75,-0.15);
                glEnd();
            glPopMatrix();

        //dessin des moustaches
            //partie droite
           glPushMatrix();
            glTranslatef(4.41,1.7,0.3);
                glBegin(GL_LINES);
                    glColor3f(0.0,0.0,0.0);
                    glVertex3f(0.0,0.0,0.0);
                    glVertex3f(0.0,0.0,0.4);
                glEnd();
                glBegin(GL_LINES);
                    glColor3f(0.0,0.0,0.0);
                    glVertex3f(0.0,0.05,0.0);
                    glVertex3f(0.0,0.25,0.4);
                glEnd();
                glBegin(GL_LINES);
                    glColor3f(0.0,0.0,0.0);
                    glVertex3f(0.0,-0.05,0.0);
                    glVertex3f(0.0,-0.25,0.4);
                glEnd();
            glPopMatrix();

            // partie gauche
            glPushMatrix();
            glTranslatef(4.41,1.7,-0.3);
                glBegin(GL_LINES);
                    glColor3f(0.0,0.0,0.0);
                    glVertex3f(0.0,0.0,0.0);
                    glVertex3f(0.0,0.0,-0.4);
                glEnd();
                glBegin(GL_LINES);
                    glColor3f(0.0,0.0,0.0);
                    glVertex3f(0.0,0.05,0.0);
                    glVertex3f(0.0,0.25,-0.4);
                glEnd();
                glBegin(GL_LINES);
                    glColor3f(0.0,0.0,0.0);
                    glVertex3f(0.0,-0.05,0.0);
                    glVertex3f(0.0,-0.25,-0.4);
                glEnd();
            glPopMatrix();

        //dessin oreilles du chat
        glPushMatrix();

        loadJpegImage("./oreille.jpg");
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,image);
        glEnable(GL_TEXTURE_2D);

            //partie droite
            glPushMatrix();
            glTranslatef(5.0,2.5,0.45);
            glRotatef(30,1.0,0.0,0.0);
            glRotatef(20,0.0,0.0,1.0);
            glScalef(0.6,0.5,0.6);
                glBegin(GL_TRIANGLE_STRIP);
                    glColor3f(1.0,1.0,1.0);
                    glTexCoord2f(0.0,0.0); glVertex3f(-0.5,0.0,-0.5);
                    glTexCoord2f(0.5,0.8); glVertex3f(0.0,1.0,0.0);
                    glTexCoord2f(0.5,0.0); glVertex3f(0.0,0.0,0.0);
                    glTexCoord2f(1.0,0.0); glVertex3f(-0.5,0.0,0.5);
                glEnd();
            glPopMatrix();

            //partie gauche
            glPushMatrix();
            glTranslatef(5.0,2.5,-0.45);
            glRotatef(-30,1.0,0.0,0.0);
            glRotatef(20,0.0,0.0,1.0);
            glScalef(0.6,0.5,0.6);
                glBegin(GL_TRIANGLE_STRIP);
                    glColor3f(1.0,1.0,1.0);
                    glTexCoord2f(0.0,0.0); glVertex3f(-0.5,0.0,-0.5);
                    glTexCoord2f(0.5,0.8); glVertex3f(0.0,1.0,0.0);
                    glTexCoord2f(0.5,0.0); glVertex3f(0.0,0.0,0.0);
                    glTexCoord2f(1.0,0.0); glVertex3f(-0.5,0.0,0.5);
                glEnd();
            glPopMatrix();

        glDisable(GL_TEXTURE_2D);
        glPopMatrix(); //fin des oreilles

    glPopMatrix(); //fin de la tête totale


    //dessin cou du chat
    glPushMatrix();
        //initialisation du tableau des points du cylindre
        for(int i =0; i < n; i++)
          {
            pCylindre[i].x = r*cos((i*2*M_PI)/n);
            pCylindre[i].y = h/2;
            pCylindre[i].z = r*sin((i*2*M_PI)/n);
            pCylindre[i+n].x = r*cos((i*2*M_PI)/n);
            pCylindre[i+n].y = -h/2;
            pCylindre[i+n].z = r*sin((i*2*M_PI)/n);
          }
        //initialisation du tableau des faces du cylindre
        for(int i=0; i <n;i++)
            {
                fCylindre[i][0] = i;
                fCylindre[i][1] = i+n;
                fCylindre[i][2] = (i+1)%n+n;
                fCylindre[i][3] = (i+1)%n;
            }
        //chargement de la texture
        loadJpegImage("./collierchat.jpg");

        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,image);
        glEnable(GL_TEXTURE_2D);

        //transformations sur le cylindre
        glTranslatef(5.6,1.6,0.0);
        glRotatef(45,0.0,0.0,1.0);
        glRotatef(-15,0.0,1.0,0.0);

        //Application de la texture face par face
        /*for (i=0;i<n;i++)
            {
              glBegin(GL_POLYGON);
                  glTexCoord2f(0.0,0.0);   glVertex3f(pCylindre[fCylindre[i][0]].x,pCylindre[fCylindre[i][0]].y, pCylindre[fCylindre[i][0]].z);
                  glTexCoord2f(0.0,1.0);   glVertex3f(pCylindre[fCylindre[i][1]].x,pCylindre[fCylindre[i][1]].y, pCylindre[fCylindre[i][1]].z);
                  glTexCoord2f(1.1,1.0);   glVertex3f(pCylindre[fCylindre[i][2]].x,pCylindre[fCylindre[i][2]].y, pCylindre[fCylindre[i][2]].z);
                  glTexCoord2f(1.0,0.0);   glVertex3f(pCylindre[fCylindre[i][3]].x,pCylindre[fCylindre[i][3]].y, pCylindre[fCylindre[i][3]].z);
              glEnd();

            }*/
        // Application de la texture enroulée autour du cylindre

        for (int i=0;i<n;i++)
        {
          glBegin(GL_POLYGON);
              glTexCoord2f((float)i/n,0.0);                  glVertex3f(pCylindre[fCylindre[i][0]].x,pCylindre[fCylindre[i][0]].y, pCylindre[fCylindre[i][0]].z);
              glTexCoord2f((float)i/n,1.0);                  glVertex3f(pCylindre[fCylindre[i][1]].x,pCylindre[fCylindre[i][1]].y, pCylindre[fCylindre[i][1]].z);
              glTexCoord2f(((float)i/n)+((float)1/n),1.0);   glVertex3f(pCylindre[fCylindre[i][2]].x,pCylindre[fCylindre[i][2]].y, pCylindre[fCylindre[i][2]].z);
              glTexCoord2f(((float)i/n)+((float)1/n),0.0);   glVertex3f(pCylindre[fCylindre[i][3]].x,pCylindre[fCylindre[i][3]].y, pCylindre[fCylindre[i][3]].z);
          glEnd();
        }
    glPopMatrix();

    //dessin du corps
    glPushMatrix();
        glColor3f(1.0,0.5,0.0);
        glTranslatef(6.75,1.3,0.0);
        glScalef(1.5,0.7,0.8);
        glutSolidSphere(0.9,20,20);
    glPopMatrix();

    //dessin de la queue
    glPushMatrix();
    glColor3f(1.0,0.5,0.0);
        //articulation1
        glPushMatrix();
            glTranslatef(8.0,1.3,0.0);
            glutSolidSphere(0.13,10,10);
        glPopMatrix();

        //morceau1
        glPushMatrix();
            glTranslatef(8.1,1.3,0.0);
            glRotated(90,0.0,1.0,0.0);
            glutSolidCylinder(0.12,0.35,10,10);
        glPopMatrix();


        glPushMatrix();    //pour l'animation d'une partie de la queue
        glTranslatef(8.5,1.3,0.0);
        glRotatef(angle3,1.0,0.0,0.0);
        glTranslatef(-8.5,-1.3,0.0);

            //articulation2
            glPushMatrix();
                glTranslatef(8.5,1.3,0.0);
                glutSolidSphere(0.13,10,10);
            glPopMatrix();

            //morceau2
            glPushMatrix();
                glTranslatef(8.5,2.15,0.0);
                glRotated(90,1.0,0.0,0.0);
                glutSolidCylinder(0.12,0.8,10,10);
            glPopMatrix();

            //articulation3
            glPushMatrix();
                glColor3f(1.0,1.0,1.0);
                glTranslatef(8.5,2.2,0.0);
                glutSolidSphere(0.13,10,10);
            glPopMatrix();

            //morceau3
            glPushMatrix();
                glColor3f(1.0,1.0,1.0);
                glTranslatef(8.5,2.25,0.0);
                glRotated(90,0.0,1.0,0.0);
                glRotated(-30,1.0,0.0,0.0);
                glutSolidCylinder(0.12,0.5,10,10);
            glPopMatrix();

        glPopMatrix();//fin partie animée queue
    glPopMatrix();//fin de la queue en entier

    //dessins des pattes
    glPushMatrix();
        //pattes avant
            //partie droite
            glColor3f(1.0,0.5,0.0);
                //Cuisse
                glPushMatrix();
                    glTranslatef(6.15,1.0,0.35);
                    glScalef(1.2,1.5,0.7);
                    glutSolidSphere(0.4,10,10);
                glPopMatrix();

                glPushMatrix(); // pour l'animation d'une partie de la patte
                glTranslatef(6.25,0.9,0.35);
                glRotated(angle,0.0,0.0,1.0);
                glTranslatef(-6.25,-0.9,-0.35);
                    //jambe
                    glPushMatrix();
                        glTranslatef(6.25,0.9,0.35);
                        glRotated(90,1.0,0.0,0.0);
                        glutSolidCylinder(0.15,0.8,10,10);
                    glPopMatrix();
                    //pied
                    glPushMatrix();
                        glColor3f(1.0,1.0,1.0);
                        glTranslatef(6.18,0.1,0.35);
                        glScalef(1.5,1.0,1.0);
                        glutSolidSphere(0.15,10,10);
                    glPopMatrix();
                glPopMatrix();//fin de la partie animée de la patte

            //partie gauche
            glColor3f(1.0,0.5,0.0);
                    //Cuisse
                     glPushMatrix();
                        glTranslatef(6.15,1.0,-0.35);
                        glScalef(1.2,1.5,0.7);
                        glutSolidSphere(0.4,10,10);
                    glPopMatrix();

                    glPushMatrix();// pour l'animation d'une partie de la patte
                    glTranslatef(6.25,0.9,-0.35);
                    glRotated(angle2,0.0,0.0,1.0);
                    glTranslatef(-6.25,-0.9,0.35);
                        //jambe
                        glPushMatrix();
                            glTranslatef(6.25,0.9,-0.35);
                            glRotated(90,1.0,0.0,0.0);
                            glutSolidCylinder(0.15,0.8,10,10);
                        glPopMatrix();
                        //pied
                        glPushMatrix();
                            glColor3f(1.0,1.0,1.0);
                            glTranslatef(6.18,0.1,-0.35);
                            glScalef(1.5,1.0,1.0);
                            glutSolidSphere(0.15,10,10);
                        glPopMatrix();
                glPopMatrix(); //fin de la partie animée de la patte
        //pattes arrières
            //partie droite
            glColor3f(1,0.5,0);
                //Cuisse
                glPushMatrix();
                    glTranslatef(7.5,1,0.35);
                    glScalef(1.2,1.5,0.7);
                    glutSolidSphere(0.4,10,10);
                glPopMatrix();

                glPushMatrix(); // pour l'animation d'une partie de la patte
                glTranslatef(7.6,0.9,0.35);
                glRotated(angle,0.0,0.0,1.0);
                glTranslatef(-7.6,-0.9,-0.35);
                    //jambe
                    glPushMatrix();
                        glTranslatef(7.6,0.9,0.35);
                        glRotated(90,1.0,0.0,0.0);
                        glutSolidCylinder(0.15,0.8,10,10);
                    glPopMatrix();
                    //pied
                    glPushMatrix();
                        glColor3f(1.0,1.0,1.0);
                        glTranslatef(7.57,0.1,0.35);
                        glScalef(1.5,1.0,1.0);
                        glutSolidSphere(0.15,10,10);
                    glPopMatrix();
                glPopMatrix();//fin de la partie animée de la patte

            //partie gauche
            glColor3f(1.0,0.5,0.0);
                //Cuisse
                 glPushMatrix();
                    glTranslatef(7.5,1.0,-0.35);
                    glScalef(1.2,1.5,0.7);
                    glutSolidSphere(0.4,10,10);
                glPopMatrix();

                glPushMatrix();// pour l'animation d'une partie de la patte
                glTranslatef(7.6,0.9,-0.35);
                glRotated(angle2,0.0,0.0,1.0);
                glTranslatef(-7.6,-0.9,0.35);
                    //jambe
                    glPushMatrix();
                        glTranslatef(7.6,0.9,-0.35);
                        glRotated(90,1.0,0.0,0.0);
                        glutSolidCylinder(0.15,0.8,10,10);
                    glPopMatrix();
                    //pied
                    glPushMatrix();
                        glColor3f(1.0,1.0,1.0);
                        glTranslatef(7.57,0.1,-0.35);
                        glScalef(1.5,1.0,1.0);
                        glutSolidSphere(0.15,10,10);
                    glPopMatrix();
                glPopMatrix();//fin de la partie animée de la patte

    glPopMatrix();// fin des pattes

glPopMatrix(); //fin du chat


// dessin du sol
glPushMatrix();

    //chargement de la texture
    loadJpegImage("./herbe.jpg");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,image);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    //transformations sur l'objet sol
    glTranslatef(-50.0,0.0,-50.0);
    glScalef(100.0,0.0,200.0);

    glBegin(GL_POLYGON);
        glTexCoord2f(0.0,0.0); glVertex3f(0.0,0.0,0.0);
        glTexCoord2f(10.0,0.0); glVertex3f(1.0,0.0,0.0);
        glTexCoord2f(10.0,10.0); glVertex3f(1.0,0.0,1.0);
        glTexCoord2f(0.0,10.0); glVertex3f(0.0,0.0,1.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);

glPopMatrix();


// création lampadaire
glPushMatrix();
    //poteau
    glPushMatrix();
        glColor3f(0.2,0.1,0.0);
        glTranslatef(0.0,5.0,-3.0);
        glRotatef(90,1.0,0.0,0.0);
        glutSolidCylinder(0.3,5,10,10);
    glPopMatrix();


    // partie lampe du lampadaire
    glPushMatrix();
        //chargement de la texture
        loadJpegImage("./tuile.jpg");
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,image);
        glEnable(GL_TEXTURE_2D);

        glTranslatef(0.0,6.7,-3.0);
        glScalef(0.7,0.7,0.7);
        glColor3f(1.0,1.0,1.0);

            //partie haute de la lampe
            glBegin(GL_POLYGON);
              // face avant
              glTexCoord2f(0.4,0.4);   glVertex3f( 0.0, 0.5, 0.0);
              glTexCoord2f(0.4,0.0); glVertex3f(-1.0, -1.0, 1.0);
              glTexCoord2f(0.0,0.4); glVertex3f(1.0, -1.0, 1.0);

              // face droite
              glTexCoord2f(0.4,0.4); glVertex3f(0.0, 0.5, 0.0);
              glTexCoord2f(0.4,0.0); glVertex3f(1.0, -1.0, 1.0);
              glTexCoord2f(0.0,0.4);  glVertex3f(1.0, -1.0, -1.0);

              // face arrière
              glTexCoord2f(0.4,0.4); glVertex3f(0.0, 0.5, 0.0);
              glTexCoord2f(0.4,0.0); glVertex3f(1.0, -1.0, -1.0);
              glTexCoord2f(0.0,0.4); glVertex3f(-1.0, -1.0, -1.0);

              // face gauche
              glTexCoord2f(0.4,0.4); glVertex3f( 0.0, 0.5, 0.0);
              glTexCoord2f(0.4,0.0); glVertex3f(-1.0,-1.0,-1.0);
              glTexCoord2f(0.0,0.4); glVertex3f(-1.0,-1.0, 1.0);
            glEnd();
        glDisable(GL_TEXTURE_2D);


        //partie basse de la lampe
        glColor3f(0.2,0.1,0.0);
        glLineWidth(3); // changement épaisseur des lignes
        glBegin(GL_LINE_STRIP);
            // face avant
            glVertex3f( 1.0, -1.0, 1.0);
            glVertex3f(-1.0, -1.0, 1.0);
            glVertex3f(0.0, -3.0, 0.0);

            // face droite
            glVertex3f(1.0, -1.0, -1.0);
            glVertex3f(1.0, -1.0, 1.0);
            glVertex3f(0.0, -3.0, 0.0);

            // face arrière
            glVertex3f(-1.0, -1.0, -1.0);
            glVertex3f(1.0, -1.0, -1.0);
            glVertex3f(0.0, -3.0, 0.0);

            // face gauche
            glVertex3f( -1.0, -1.0, 1.0);
            glVertex3f(-1.0,-1.0,-1.0);
            glVertex3f(0.0,-3.0, 0.0);

        glEnd();
        glLineWidth(1);// retour valeur par défaut épaisseur lignes
    glPopMatrix(); // fin partie lampe du lampadaire

    //ampoule
    GLfloat jaune[]={1.0,1.0,0.0,1.0};
    glPushMatrix();
        glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,jaune);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,jaune);
        glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,jaune);
        glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,10);
        glColor3f(1.0,1.0,0.0);
        glTranslatef(0.0,5.9,-3.0);
        glutSolidSphere(0.3,10,10);
    glPopMatrix();

glPopMatrix();// fin du lampadaire

glFlush();

//On echange les buffers
glutSwapBuffers();
}


/*fonction qui paramètre des actions avec des touches clavier*/
void clavier(unsigned char touche,int x,int y)
{
  switch (touche)
    {
    case 'p': /* affichage en mode plein */
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glutPostRedisplay();
      break;
    case 'f': /* affichage en mode fil de fer */
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glutPostRedisplay();
      break;
    case 's' : /* Affichage en mode sommets seuls */
      glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
      glutPostRedisplay();
      break;
    case 'd': /* affichage du test de profondeur*/
      glEnable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'D':/* désactivation du test de profondeur*/
      glDisable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;

    case 'q' : /*la touche 'q' permet de quitter le programme */
      exit(0);
      break;

    case 'o' :  //animation queue du chat

        if (angle3 >40)
        {angle3=-40;}
        else
        {angle3+=10;}
        glutPostRedisplay();
    break;
    case 'z':       //permet de dézoomer
        if(zoom<20)
        zoom++;
        glutPostRedisplay();
    break;
    case'Z':        //permet de zoomer
        if(zoom>5)
        zoom--;
        glutPostRedisplay();
    break;

    case 'a' :
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT1);
        glutPostRedisplay();
    break;

    case 'A':
        glDisable(GL_LIGHT1);
        glutPostRedisplay();
    break;

    case 'b':
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT2);
        glutPostRedisplay();
    break;

    case 'B':
        glDisable(GL_LIGHT2);
        glutPostRedisplay();
    break;

    case 'c':
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    break;

    case 'C':
        glDisable(GL_LIGHT0);
        glutPostRedisplay();
    break;

    case 'e':
        glDisable(GL_LIGHTING);
        glutPostRedisplay();
    break;
    }
}

/* fonction qui paramètre des actions avec des touches spéciales du clavier*/
void SpecialClavier( int touche, int x, int y )
{
switch (touche){

    case GLUT_KEY_DOWN :
        anglex+=10;
        glutPostRedisplay();
    break;

    case GLUT_KEY_UP:
        anglex-=10;
        glutPostRedisplay();
    break;

    case GLUT_KEY_RIGHT:
        angley+=10;
        glutPostRedisplay();
    break;

    case GLUT_KEY_LEFT:
        angley-=10;
        glutPostRedisplay();
    break;
}
}

/*fonction qui gère changement de taille de la fenetre*/
void reshape(int x,int y)
{
  if (x<y)
    glViewport(0,(y-x)/2,x,x);
  else
    glViewport((x-y)/2,0,y,y);
}


/*fonctions qui gèrent le déplacement de la caméra en fonction de la souris*/
void mouse(int button, int state,int x,int y)
{
  /* si on appuie sur le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    presse = 1; /* le booleen presse passe a 1 (vrai) */
    xold = x; /* on sauvegarde la position de la souris */
    yold=y;
  }
  /* si on relache le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    presse=0; /* le booleen presse passe a 0 (faux) */
}

void mousemotion(int x,int y)
  {
    if (presse) /* si le bouton gauche est presse */
    {
      /* on modifie les angles de rotation de l'objet
	 en fonction de la position actuelle de la souris et de la derniere
	 position sauvegardee */
      anglex=anglex+(x-xold);
      angley=angley+(y-yold);
      glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
    }

    xold=x; /* sauvegarde des valeurs courante de le position de la souris */
    yold=y;
  }

/*fonction d'actualisation de la scène selon un temps imparti utilisé pour l'animation*/
    void timer(int t)
    {
    /* mise à jour de l'animation */
    // animation du chat qui avance
    distance+=0.2;
    if(distance>15)
        distance=-15;
    // animation des pattes
    angle2-=5;
    angle+=5;
    if(angle>30)
    {angle-=30;
    angle2+=30;
    }

    glutPostRedisplay();
    glutTimerFunc(1000.0/6.0, timer, 0); // appel récursif
    }


/*fonction pour charger une image depuis un fichier*/
void loadJpegImage(char *fichier)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;
  unsigned char *ligne;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

#ifdef __WIN32
  if (fopen_s(&file,fichier,"rb") != 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#elif __GNUC__
    if ((file = fopen(fichier,"rb")) == 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#endif
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);

  if ((cinfo.image_width!=256)||(cinfo.image_height!=256)) {
    fprintf(stdout,"Erreur : l'image doit etre de taille 256x256\n");
    exit(1);
  }
  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }

  jpeg_start_decompress(&cinfo);
  ligne=image;
  while (cinfo.output_scanline<cinfo.output_height)
    {
      ligne=image+3*256*cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,&ligne,1);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);


}

