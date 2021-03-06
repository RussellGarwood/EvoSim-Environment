#ifndef ENVIRONMENTSCENE_H
#define ENVIRONMENTSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QString>

class MainWindow;

class EnvironmentScene : public QGraphicsScene
{
public:
        EnvironmentScene();
        MainWindow *mw;

protected:
     /*
     //RJG - These are here in case mouse clicks are needed down the line, I think.
     void mousePressEvent(QGraphicsSceneMouseEvent *event);
     void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
     void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
     */

 private slots:
     void ScreenUpdate();
};

#endif // ENVIRONMENTSCENE_H
