#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QImage>

class Worker : public QObject
{
    Q_OBJECT
    public:
        Worker(const QImage& img): bin(img), filePath(QString()) {}
        Worker(const QString& path): bin(QImage()), filePath(path) {}

    public slots:
        void doWork();
        void saveData();
    signals:
        void workFinished();
        void dataIsSaved();
    private:
        QImage bin;
        QString filePath;
};

#endif // WORKER_H
