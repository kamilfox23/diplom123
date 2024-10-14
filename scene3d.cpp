#include "scene3d.h"
#include <QLabel>


QTimer *aTimer = new QTimer; 
QTimer *blink_Timer = new QTimer; 
QTimer *start_Timer = new QTimer; 
int fraction(40);
int between(3); 
int marker_pos(2); 
int m_start_pos(2); 
int beg_move_speed(30); 
int cScore(0); 
int start(0); 
int start_count(0);
int blink_freq(0);
int loose(0);
int change_speed(20);
QLinkedList <int> bPosition;

scene3D::scene3D(QWidget *parent) :
    QGLWidget(parent)
{
    gLength = 0.5; 
    gWidth = 3*gLength; 

    remove = 0;
    xTranslate = 0;
    yTranslate = 0;
    connect(aTimer, SIGNAL(timeout()), SLOT(animate())); 
    connect(blink_Timer, SIGNAL(timeout()), SLOT(blink()));
    connect(start_Timer, SIGNAL(timeout()), SLOT(gameStart()));
    //aTimer->start(30);
}

void scene3D::initializeGL()
{
    LoadTextures(); //Вызов загрузки текстур
    qglClearColor(Qt::black);
    glEnable(GL_TEXTURE_2D);		
}

void scene3D::resizeGL(int w, int h)
{
    glLoadIdentity();
    glViewport(0,0, (GLint)w, (GLint)h);
}


void scene3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glClearDepth(1.0f);            
    glEnable(GL_DEPTH_TEST);       
    glDepthFunc(GL_LESS);         

    glMatrixMode(GL_PROJECTION);
    gluPerspective (70, 1, 3, 100);

    glPushMatrix();
    glTranslatef(0.0f, -2.1, -3.0);
    glRotatef(27.8, 1.0f, 0.0f, 0.0f);      
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f);  

   
    for (int i=1; i<= 40; i++)
    {
       
        glBegin(GL_LINE_LOOP);
                    z = gLength*i; 
                    z -= line_move; 
                    glColor3f(1.0, 1.0, 1.0);
                    glVertex3f(-gWidth, 0.0, 0.0);
                    glVertex3f(gWidth, 0.0, 0.0);
                    glVertex3f(gWidth, 0.0, -z);
                    glVertex3f(-gWidth, 0.0, -z);
        glEnd();

       
        glBegin(GL_LINES);
                glVertex3f(-gLength, 0.0, 0.0);    glVertex3f(-gLength, 0.0, -z);
                glVertex3f(gLength, 0.0, 0.0);    glVertex3f(gLength, 0.0, -z);
        glEnd();
    }

    
    if(bPosition.isEmpty())
    {
        for (int i=0; i<fraction; i++)
        {
            bPosition.append(randomizePos()); 
        }
    }

  
    int k(1); //Порядковый номер блока

    
    foreach (int i, bPosition)
    {
        z_trans = gLength*k; 
        z_trans -= block_move; 

        if (!(k%between))
        {
            if(i == 1) createBlock(-gWidth, gLength*2, gLength, z_trans);
            if(i == 2) createBlock(-gLength, gLength*2, gLength, z_trans);
            if(i == 3) createBlock(gLength, gLength*2, gLength, z_trans);
        }

        if (k == between) curr_block_pos = i;

        k++;
    }

  
    z_marker = gLength*(m_start_pos-1);
    if (!(blink_freq%2))
    {
        if (marker_pos == 1) createMarker(-gWidth, gLength, z_marker, 1);
        if (marker_pos == 2) createMarker(-gLength, gLength, z_marker, 1);
        if (marker_pos == 3) createMarker(gLength, gLength, z_marker, 1);
    }

    if(start == 0)
    {
        glEnable(GL_BLEND);

        /* --- ENTER --- */
        glPushMatrix();

        glLoadIdentity();
        glTranslatef(-0.85, 0.1, 0);

        qglColor(Qt::white);
        glBindTexture(GL_TEXTURE_2D, texHandle[11]);

        int entSize = 3;
        float entWidth = 0.13*entSize;
        float entHeight = 0.14*entSize;

        glBegin(GL_POLYGON);
                glVertex3f(0, 0, 0);                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(0, entHeight, 0);        glTexCoord2f(1.0f, 1.0f);
                glVertex3f(entWidth, entHeight, 0); glTexCoord2f(1.0f, 0.0f);
                glVertex3f(entWidth, 0, 0);         glTexCoord2f(0.0f, 0.0f);
        glEnd();

        /* --- ESC --- */
        glTranslatef(1.32, 0, 0);

        qglColor(Qt::white);
        glBindTexture(GL_TEXTURE_2D, texHandle[12]);

        int escSize = 3;
        float escWidth = 0.111*escSize;
        float escHeight = 0.087*escSize;

        glBegin(GL_POLYGON);
                glVertex3f(0, 0, 0);                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(0, escHeight, 0);        glTexCoord2f(1.0f, 1.0f);
                glVertex3f(escWidth, escHeight, 0); glTexCoord2f(1.0f, 0.0f);
                glVertex3f(escWidth, 0, 0);         glTexCoord2f(0.0f, 0.0f);
        glEnd();

        glPopMatrix();
        glDisable(GL_BLEND);
    }
  

    if (start == 1)
    {
        cScore = 0; //Оюнуляем игровой счет
        loose = 0;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPushMatrix();

        glLoadIdentity();
        glTranslatef(-0.14, 0.58, 0);

        float scHeight = 0.25;
        float numWidth = (scHeight/3)*2; 

        if(start_count == 0) setNumber(0.3, numWidth, scHeight, 3);
        if(start_count == 1) setNumber(0.3, numWidth, scHeight, 2);
        if(start_count == 2) setNumber(0.3, numWidth, scHeight, 1);
        if(start_count == 3 || start_count == 4)
        {
            qglColor(Qt::green);
            glBindTexture(GL_TEXTURE_2D, texHandle[13]);

            float goSize = 3;
            float goWidth = 0.096*goSize;
            float goHeight = 0.077*goSize;

            glBegin(GL_POLYGON);
                    glVertex3f(0, 0, 0);                glTexCoord2f(0.0f, 1.0f);
                    glVertex3f(0, goHeight, 0);        glTexCoord2f(1.0f, 1.0f);
                    glVertex3f(goWidth, goHeight, 0); glTexCoord2f(1.0f, 0.0f);
                    glVertex3f(goWidth, 0, 0);         glTexCoord2f(0.0f, 0.0f);
            glEnd();

            if(start_count == 4)
            {
                start_Timer->stop();
                start_count = 0;
                start = 2;
                move_speed = beg_move_speed; 
                aTimer->start(move_speed);
            }
        }

        glPopMatrix();
        glDisable(GL_BLEND);
    }


    /* --------------------- */

    int maxSize(40); //Макс. размер списка

    int inter = between - m_start_pos;
    int diff (remove - inter);

    if ( !(diff%between) )
    {
       
        if(curr_block_pos == marker_pos)
        {
            aTimer->stop(); 
            if(start == 2)
            {
                blink_Timer->start(150);
                start = 0;
                loose = 1;
            }

        }
    }


    if(bPosition.size() >= (40 + between)) 
    {
        int del = bPosition.size() - maxSize; 
        for(int i = 0; i<del; i++)
        {
            bPosition.removeFirst(); 
        block_move = block_move - (del*gLength); 
    }

    showScore(cScore); //Выводим счет на экран

  
    if(blink_freq == 6)
    {
        blink_Timer->stop();
        blink_freq = 0;
    }

    if (loose == 1 && !(blink_freq%2)) gameOver();

    glPopMatrix();
}


int scene3D::randomizePos(void)
{
    int p = qrand() % 3 +1; //Рандом [1;3]

    return p;
}


void scene3D::gameOver()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();

    glLoadIdentity();
    glTranslatef(-0.97, 0.7, 0);

    qglColor(Qt::red);
    glBindTexture(GL_TEXTURE_2D, texHandle[14]);

    int gmoSize = 3;
    float gmoWidth = 0.186*gmoSize;
    float gmoHeight = 0.029*gmoSize;

    glBegin(GL_POLYGON);
            glVertex3f(0, 0, 0);                glTexCoord2f(0.0f, 1.0f);
            glVertex3f(0, gmoHeight, 0);        glTexCoord2f(1.0f, 1.0f);
            glVertex3f(gmoWidth, gmoHeight, 0); glTexCoord2f(1.0f, 0.0f);
            glVertex3f(gmoWidth, 0, 0);         glTexCoord2f(0.0f, 0.0f);
    glEnd();

    glPopMatrix();
    glDisable(GL_BLEND);
}

/* ------======= СОЗДАЕМ МАРКЕР =======------ */
void scene3D::createMarker (float Xo, float width, float depth, float white)
{
    glPushMatrix();

    glTranslatef(Xo, 0.0, -depth);

    glBegin(GL_QUADS);
    if (white == 1) qglColor(Qt::white);
    else qglColor(Qt::black);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, 0.0, -width);
            glVertex3f(width*2, 0.0, -width);
            glVertex3f(width*2, 0.0, 0.0);
    glEnd();

    glPopMatrix();
}


void scene3D::createBlock (float Xo, float width, float height, float depth)
{
    glPushMatrix();

    glTranslatef(Xo, 0.0, -depth);

    glBegin(GL_QUADS);
           
            qglColor(Qt::yellow);
            glVertex3f(width, height, 0.0);
            glVertex3f(width, height, -height);
            glVertex3f(width, 0.0, -height);
            glVertex3f(width, 0.0, 0.0);

           
            qglColor(Qt::blue);
            glVertex3f(0.0, 0.0, -height);
            glVertex3f(0.0, height, -height);
            glVertex3f(0.0, height, 0.0);
            glVertex3f(0.0, 0.0, 0.0);

          
            qglColor(Qt::green);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, height, 0.0);
            glVertex3f(width, height, 0.0);
            glVertex3f(width, 0.0, 0.0);

            /*Верхняя сторона*/
            qglColor(Qt::red);
            glVertex3f(0.0, height, 0.0);
            glVertex3f(0.0, height, -height);
            glVertex3f(width, height, -height);
            glVertex3f(width, height, 0.0);

    glEnd();
    glPopMatrix();
}


void scene3D::gameStart()
{
    start_count++;
    qDebug() << "start_count = " << start_count;
    updateGL();
}


void scene3D::blink()
{
    qDebug() << "blink_freq = " << blink_freq;
    blink_freq += 1;
    updateGL();
}


void scene3D::animate()
{
    if(line_move >= gLength)
    {
        line_move = 0.0;
        remove += 1; 
        cScore += 1;

       
        if (!(cScore%change_speed) && cScore != 0 && move_speed > 0)
        {
            move_speed -= 5;
            aTimer->start(move_speed);
            qDebug() << "move_speed = " << move_speed;
        }

        bPosition.append(randomizePos());
    else
    {
        line_move += 0.1;
        block_move += 0.1;
    }

    updateGL();
}


void scene3D::showScore(int score)
{
    glPushMatrix();

    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLoadIdentity();
    glTranslatef(-0.98, 0.85, 0.0);

    float scHeight = 0.13;
    float scWidth = scHeight*4;
    float numWidth = (scHeight/3)*2; 

    qglColor(Qt::white);

 
    glBindTexture(GL_TEXTURE_2D, texHandle[10]);
    glBegin(GL_POLYGON);
            glVertex3f(0.0, 0.0, 0.0);glTexCoord2f(0.0f, 1.0f);
            glVertex3f(0.0, scHeight, 0.0);glTexCoord2f(1.0f, 1.0f);
            glVertex3f(scWidth, scHeight, 0.0);glTexCoord2f(1.0f, 0.0f);
            glVertex3f(scWidth, 0.0, 0.0);glTexCoord2f(0.0f, 0.0f);
    glEnd();

  
    QString str; 
    QString symbol;
    int num_temp;
    str.setNum(score);

    glTranslatef(scWidth+0.03, 0, 0);

    for (int i=0; i<str.size(); i++)
    {
        symbol = str.at(i);
        num_temp = symbol.toInt();
        setNumber(i, numWidth, scHeight, num_temp);
    }

    glDisable(GL_BLEND);

    glPopMatrix();
}


void scene3D::setNumber(float Xo, float width, float height, int number)
{
    float x0 = width*Xo;

    glPushMatrix();

    glTranslatef(x0, 0.0, 0.0);

    if (number == 0) glBindTexture(GL_TEXTURE_2D, texHandle[0]); 
    if (number == 1) glBindTexture(GL_TEXTURE_2D, texHandle[1]);
    if (number == 2) glBindTexture(GL_TEXTURE_2D, texHandle[2]);
    if (number == 3) glBindTexture(GL_TEXTURE_2D, texHandle[3]);
    if (number == 4) glBindTexture(GL_TEXTURE_2D, texHandle[4]);
    if (number == 5) glBindTexture(GL_TEXTURE_2D, texHandle[5]);
    if (number == 6) glBindTexture(GL_TEXTURE_2D, texHandle[6]);
    if (number == 7) glBindTexture(GL_TEXTURE_2D, texHandle[7]);
    if (number == 8) glBindTexture(GL_TEXTURE_2D, texHandle[8]);
    if (number == 9) glBindTexture(GL_TEXTURE_2D, texHandle[9]);

    glBegin(GL_POLYGON);
            glVertex3f(0.0, 0.0, 0.0);glTexCoord2f(0.0f, 1.0f);
            glVertex3f(0.0, height, 0.0);glTexCoord2f(1.0f, 1.0f);
            glVertex3f(width, height, 0.0);glTexCoord2f(1.0f, 0.0f);
            glVertex3f(width, 0.0, 0.0);glTexCoord2f(0.0f, 0.0f);
    glEnd();

    glPopMatrix();
}

void scene3D::LoadTextures()
{
 
    glEnable(GL_TEXTURE_2D);
    texHandle[0] = bindTexture(QImage(":/Textures/Textures/Zero.png"));
    texHandle[1] = bindTexture(QImage(":/Textures/Textures/One.png"));
    texHandle[2] = bindTexture(QImage(":/Textures/Textures/Two.png"));
    texHandle[3] = bindTexture(QImage(":/Textures/Textures/Three.png"));
    texHandle[4] = bindTexture(QImage(":/Textures/Textures/Four.png"));
    texHandle[5] = bindTexture(QImage(":/Textures/Textures/Five.png"));
    texHandle[6] = bindTexture(QImage(":/Textures/Textures/Six.png"));
    texHandle[7] = bindTexture(QImage(":/Textures/Textures/Seven.png"));
    texHandle[8] = bindTexture(QImage(":/Textures/Textures/Eight.png"));
    texHandle[9] = bindTexture(QImage(":/Textures/Textures/Nine.png"));
    texHandle[10] = bindTexture(QImage(":/Textures/Textures/Score.png"));
    texHandle[11] = bindTexture(QImage(":/Textures/Textures/Play.png"));
    texHandle[12] = bindTexture(QImage(":/Textures/Textures/Exit.png"));
    texHandle[13] = bindTexture(QImage(":/Textures/Textures/GO.png"));
    texHandle[14] = bindTexture(QImage(":/Textures/Textures/Game_Over.png"));

   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

}


void scene3D::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
            /* ----------------- */
            case Qt::Key_Left: {
                if(marker_pos != 1) marker_pos -= 1;

                updateGL();
            break;
            /* ----------------- */
            case Qt::Key_Right:
                if(marker_pos != 3) marker_pos += 1;

                updateGL();
            break;
            /* ----------------- */
                case Qt::Key_W:
                    yTranslate += 0.01;
                    qDebug() << "yTranslate = " << yTranslate;
                    updateGL();
                break;
            /* ----------------- */
                case Qt::Key_S:
                    yTranslate -= 0.01;
                    qDebug() << "yTranslate = " << yTranslate;
                    updateGL();
                break;
                    /* ----------------- */
                        case Qt::Key_A:
                            xTranslate += 0.01;
                            qDebug() << "xTranslate = " << xTranslate;
                            updateGL();
                        break;
                    /* ----------------- */
                        case Qt::Key_D:
                            xTranslate -= 0.01;
                            qDebug() << "xTranslate = " << xTranslate;
                            updateGL();
                        break;
                    /* ------ИГРАТЬ------- */
                        case Qt::Key_Return:
                            qDebug() << "start = " << start;
                            if (start == 0)
                            {
                                start = 1;
                                updateGL();
                                start_Timer->start(1000);
                                qDebug() << "start = " << start;
                            }
                        break;
                            /* ------ВЫХОД------ */
                                case Qt::Key_Escape:
                                    close();
                                break;

          }
     default:
     break;
     }
}

/*virtual*/ void scene3D::mousePressEvent(QMouseEvent* pe)
{
    m_ptPosition = pe->pos();
}

/*virtual*/ void scene3D::mouseMoveEvent(QMouseEvent* pe)
{
    x_Rotate += 180* (GLfloat)(pe->y() - m_ptPosition.y()) / height();
    y_Rotate += 180* (GLfloat)(pe->x() - m_ptPosition.x()) / width();
    updateGL();

    m_ptPosition = pe->pos();
