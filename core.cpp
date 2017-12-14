#include "core.h"
#include "single_area.h"
#include "mgraphics.h"

#include <QVector>
#include <QImage>
#include <QStack>
#include <QPoint>
#include <QDebug>
#include <QIODevice>
#include <QDate>
#include <QTime>
#include <QTextStream>

MGraphics::Labels MGraphics::data_01;
MGraphics::Objects MGraphics::data_obj;
const QRgb black = qRgb(0,0,0);

inline bool inner(qint32 x, qint32 y, MGraphics::Labels& V)
{//return true if d is inner point
 //dangerous function. May cause out of range!
    return (V[y][x + 1])&&(V[y][x - 1])&&(V[y + 1][x])&&(V[y - 1][x]);
}

inline bool isBlack(qint32 x, qint32 y, const QImage& im)
{
    return im.pixel(x,y) == black;
}

void fill(const QImage& img, MGraphics::Labels& V, qint32 _x, qint32 _y, quint64 L)
{
  QPoint t;
  QStack<QPoint> depth;
  depth.push(QPoint(_x,_y));
  const qint32 w = img.width();
  const qint32 h = img.height();

  while (!depth.empty())
  {
    t = depth.pop();
    qint32 x = t.rx();
    qint32 y = t.ry();
    V[y][x] = L; // filling.

    if((x + 1 < w)&&(isBlack(x+1,y,img))&&(V[y][x + 1] == 0))
    {
        depth.push(QPoint(x+1,y));
    }
    if((x - 1> -1)&&(isBlack(x-1,y,img))&&(V[y][x - 1] == 0))
    {
        depth.push(QPoint(x-1,y));
    }
    if((y + 1< h)&&(isBlack(x,y+1,img))&&(V[y + 1][x] == 0))
    {
        depth.push(QPoint(x,y+1));
    }
    if((y - 1> -1)&&(isBlack(x,y-1,img))&&(V[y - 1][x] == 0))
    {
        depth.push(QPoint(x,y-1));
    }
  }
}

void Core::select()//heavy function
{//main calculate
    qint32 const _h = bin.height();
    qint32 const _w = bin.width();
    quint64 L = 1; // starting id value

    MGraphics::Labels Labels
            (_h, MGraphics::Labels_row(_w,0));

//labeling__________________________________________________________________________
    for(qint32 y = 0; y < _h; ++y)
      for(qint32 x = 0; x < _w; ++x)
      {
       if((!Labels[y][x])&&(isBlack(x,y,bin)))
       {
         fill(bin,Labels,x,y,L++);//very fast!
       }
      }

//form objects______________________________________________________________________
    const quint64 size = --L; // size == num of objects
    MGraphics::Objects V(size);

    for(quint64 i = 0;i < size; ++i)
     {
       V[i] = S_area(i);
     }
//-----------------------------------------------------------------------------

    if ((size > 0)&&(_h > 2)&&(_w > 2))
    {
    for(int y = 1; y < _h - 1; ++y)//general case
      for(int x = 1; x < _w - 1; ++x)//general case
      {
          auto id = Labels[y][x];
          if ((id > 0)&&(id < size + 1))
          {
            QPoint t(x,y);
            V[id - 1].add_main(t);

            if (!inner(x,y,Labels))
            {
                V[id - 1].add_cont(t);
            }
          }
      }
    for(int x = 1; x < _w - 1; ++x)//top case
     {
        auto id = Labels[0][x];
        if ((id > 0)&&(id < size + 1))
        {
           QPoint t(x,0);
           V[id - 1].add_main(t);
           V[id - 1].add_cont(t);
        }
     }
    for(int x = 1; x < _w - 1; ++x)//bottom case
     {
        auto id = Labels[_h - 1][x];
        if ((id > 0)&&(id < size + 1))
                {
                   QPoint t(x,_h - 1);
                   V[id - 1].add_main(t);
                   V[id - 1].add_cont(t);
                }
     }
    for(int y = 0; y < _h; ++y)//left case
     {
         auto id = Labels[y][0];
         if ((id > 0)&&(id < size + 1))
         {
            QPoint t(0,y);
            V[id - 1].add_main(t);
            V[id - 1].add_cont(t);
         }
     }
    for(int y = 0; y < _h; ++y)//right case
     {
         auto id = Labels[y][_w - 1];
         if ((id > 0)&&(id < size + 1))
         {
            QPoint t(_w - 1,y);
            V[id - 1].add_main(t);
            V[id - 1].add_cont(t);
         }
     }
   }

    MGraphics::data_01 = std::move(Labels);
    MGraphics::data_obj = std::move(V);

    emit SelectingFinished();
}

void SaveToFile(QTextStream& out)
{//FORMAT "id"."x"."y"."x"."y"."x"."y"."x"."y"..."C"."x"."y"...;
    for (qint32 i = 0; i < MGraphics::data_obj.size(); ++i)
    {
        out << QString::number(i) + ".";
        for (qint32 j = 0; j < MGraphics::data_obj[i].Points.size(); ++j)
        {
          out << QString::number(MGraphics::data_obj[i].Points[j].x())+ ".";
          out << QString::number(MGraphics::data_obj[i].Points[j].y())+ ".";
        }
        out << QString("C.");
        for (qint32 j = 0; j < MGraphics::data_obj[i].CPoints.size(); ++j)
        {
          out << QString::number(MGraphics::data_obj[i].CPoints[j].x())+ ".";
          out << QString::number(MGraphics::data_obj[i].CPoints[j].y())+ ".";
        }
        out << QString(';');
    }
   //out << QString(';');
}


void Core::saveData()
{
       QFile file(filePath);
       if (file.open(QIODevice::WriteOnly))
       {//FORMAT "id"."x"."y"."x"."y"."x"."y"."x"."y"..."C"."x"."y"...;
           QTextStream stream(&file);
           SaveToFile(stream);
       }
     file.close();

     emit dataIsSaved();
}
