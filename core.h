#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QImage>

class Core : public QObject
{
    Q_OBJECT
    public:
        Core(const QImage& img): bin(img), filePath(QString()) {}
        Core(const QString& path): bin(QImage()), filePath(path) {}

    public slots:
        void select();
        void saveData();
    signals:
        void SelectingFinished();
        void dataIsSaved();
    private:
        QImage bin;
        QString filePath;
};

#endif // WORKER_H
