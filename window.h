#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QApplication>

#include <QPaintEvent>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QAbstractButton>

#include <QMatrix4x4>
#include <QVector4D>
#include <QVector3D>

#include <QPainter>
#include <QPen>

#include <vector>
#include <cmath>
#include <iostream>


class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();
private:
    void cocentrate(std::vector<QVector4D>& pnts);
    double scalar(const QVector3D& , const QVector3D& );
    double max_coordinate(const std::vector<QVector4D>& vec);
    bool visible_at_oz(const QVector4D& , const QVector4D& );

    QLabel *LFI, *LPSI, *LTETA, *LSCALE, *LSTEPS, *LFI_L, *LPSI_L;
    QDoubleSpinBox *FI, *PSI, *TETA, *SCALE, *FI_L, *PSI_L; QSpinBox *STEPS;
    //QCheckBox *Box_izo;
    double MAX_SCALE=300; int MAX_STEPS=100;
    double PI =4*atan(1);
    void get_normal(QVector4D& vec);
    int dark( const std::vector<QVector4D>& verts, QVector3D light);
protected:
    void paintEvent(QPaintEvent*);

private slots:
    void redrawOnValueChange(double);
   // void redrawOnValueChange(bool);
    void redrawOnValueChange(int);

};


#endif // WINDOW_H
