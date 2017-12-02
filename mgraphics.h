#ifndef MGRAPHICS_H
#define MGRAPHICS_H

#include "single_area.h"

#include <memory>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QStack>
#include <QSlider>
#include <QLabel>
#include <QPoint>
#include <QString>
#include <QColor>

class MGraphics : public QGraphicsView
{
    Q_OBJECT
    using PXitem = QGraphicsPixmapItem;
    using pItem = std::unique_ptr<PXitem>;
    using Labels = QVector<QVector<std::size_t>>;
    using Objects = QVector<S_area>;
    using pSlider = std::unique_ptr<QSlider>;
    using pLabel = std::unique_ptr<QLabel>;
public:
   MGraphics();
   ~MGraphics();

   static Labels data_01;
   static Objects data_obj;

   const QImage& get_bin_img()
   {
       return b_img;
   }
   void backward();
   void forward();
   void autoThreshold();

protected:
  void mouseMoveEvent(QMouseEvent *event)override;
  void mousePressEvent(QMouseEvent *event)override;
  void mouseReleaseEvent(QMouseEvent *event)override;
  void drawBackground(QPainter *painter, const QRectF &rect)override;
  void keyPressEvent(QKeyEvent *event)override;
  void wheelEvent(QWheelEvent *event)override;

public slots:

  void load_from_file(const QString&);
  void setCursor_mode(char);
  void ObjectsColorChange(QColor);
  void RandomColorize();
  void setThickness(int);

private slots:

  void Slider_Change(int);
  void OSlider_Change(int);
  void Slider_Release();

private:
  bool on_img(int,int);
  QPoint transform(QPoint);
  QPoint drawCurve_andGetCenter(QImage&); //return centerMass of curve
  void ShowObjectUnderCursor(QMouseEvent*);
  bool decide_to_draw(QPoint);
  bool dataIsReady();
  bool isCorrectRelease(QMouseEvent*);
  void PXtoNull(pItem&&);

  int thickness_pen;
  char cursor_mode; //0 - view(nothing), 1 - draw, 2 - erase
  bool drawingFlag;
  QColor ColorObj;

  pLabel txt;
  pLabel otxt;
  pSlider Slider;
  pSlider OSlider;
  QPixmap draw_pix;
  QPixmap erase_pix;

  QGraphicsScene scene;
  QPixmap pm;
  QImage b_img;
  QStack<QImage> Ctrl_Z, Ctrl_Y;

  pItem sourceItem_from_image;
  pItem titem;
  pItem track_item;
  pItem randItem;
};

#endif // MGRAPHICS_H
