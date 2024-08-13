void OGLWidget::initializeGL()
{
int side = qMin(OGLWidget::width(), OGLWidget::height());
   glClearColor(0,0,0,1);
   glEnable(GL_MULTISAMPLE);
   glEnable( GL_LINE_SMOOTH );
   glEnable( GL_POLYGON_SMOOTH );
   glEnable(GL_BLEND);
   glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
   glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
   glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
   glPolygonMode(GL_FRONT, GL_FILL);
   glViewport((OGLWidget::width() - side) / 2, (OGLWidget::height() - side) / 2, side, side);
   glMatrixMode(GL_PROJECTION);
   glOrtho(0, OGLWidget::width(), OGLWidget::height(), 0,0, 1);

}

void OGLWidget::paintGL()
{
    SCREEN_WIDTH=QOpenGLWidget::width();
    SCREEN_HEIGHT=QOpenGLWidget::height();
    glClear(GL_COLOR_BUFFER_BIT);
    glwDrawRoundedRectGradientFill(0,5,SCREEN_WIDTH-5,SCREEN_HEIGHT-5,10,0xffffff,0x999999);
    return;

}

void OGLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,width, height, 0,0, 1);
    glMatrixMode(GL_MODELVIEW);
    return;
}
