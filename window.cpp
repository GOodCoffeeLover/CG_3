#include "window.h"
#include <QPainter>
#include <iostream>
#include <QPen>
#include <QVector3D>
void Window::redrawOnValueChange(double){
 update();
}
/*void Window::redrawOnValueChange(bool){
 update();
}*/
void Window::redrawOnValueChange(int){
 update();
}

void Window::get_normal(QVector4D& vec){
    if(vec[3]==0)
        return;
    vec[0]/=vec[3];
    vec[1]/=vec[3];
    vec[2]/=vec[3];
    vec[3]/=vec[3];

}

double Window::scalar(const QVector3D& a , const QVector3D& b){
    return a.x()*b.x() + a.y()*b.y() + a.z()*b.z();
}

bool Window::visible_at_oz(const QVector4D& a, const QVector4D& b){
    return (scalar(QVector3D::normal(a.toVector3D(), b.toVector3D()), QVector3D(0, 0, 1)))>=0;

}

double Window::max_coordinate(const std::vector<QVector4D>& vec){
    double max1=0, max=(
     (vec[0].x()>vec[0].y())?vec[0].x():vec[0].y() > vec[0].z())?
      (vec[0].x()>vec[0].y())?vec[0].x():vec[0].y()
       :vec[0].z(); //max(x[0],y[0],z[0]);
    for(int i=1; i<vec.size(); ++i){
        max1=((vec[i].x()>vec[i].y())?vec[i].x():vec[i].y() > vec[i].z())?
              (vec[i].x()>vec[i].y())?vec[i].x():vec[i].y()
               :vec[i].z(); //max(x[i], y[i], z[i])
        max=(max>max1)?max:max1;
    }
    return max;

}

Window::Window(QWidget *parent)
    : QMainWindow(parent){
     LTETA= new QLabel("teta(rotate ox)", this);
     LTETA->move(10, 10);
     TETA = new QDoubleSpinBox(this);
     TETA -> setRange(-PI, PI);
     TETA -> setSingleStep(PI/60);
     TETA -> setValue(0.0);
     TETA -> move(10, 35);
     connect(TETA, SIGNAL(valueChanged(double)), this, SLOT(redrawOnValueChange(double)));

     LPSI= new QLabel("psi (rotate oy)", this);
     LPSI->move(10, 60);
     PSI = new QDoubleSpinBox(this);
     PSI -> setRange(-PI, PI);
     PSI -> setSingleStep(PI/60);
     PSI -> setValue(0.0);
     PSI -> move(10, 85);
     connect(PSI, SIGNAL(valueChanged(double)), this, SLOT(redrawOnValueChange(double)));

     LFI= new QLabel("fi (rotate oz)", this);
     LFI->move(10, 110);
     FI  = new QDoubleSpinBox(this);
     FI -> setRange(-PI, PI);
     FI -> setSingleStep(PI/60);
     FI -> setValue(0.0);
     FI -> move(10, 135);
     connect(FI, SIGNAL(valueChanged(double)), this, SLOT(redrawOnValueChange(double)));

     LSCALE= new QLabel("Scale", this);
     LSCALE->move(10, 160);
     SCALE = new QDoubleSpinBox(this);
     SCALE -> setRange(1, MAX_SCALE);
     SCALE -> setSingleStep(1);
     SCALE -> setValue(150.);
     SCALE -> move(10, 185);
     connect(SCALE, SIGNAL(valueChanged(double)), this, SLOT(redrawOnValueChange(double)));

     LSTEPS= new QLabel("Steps", this);
     LSTEPS->move(10, 210);
     STEPS = new QSpinBox(this);
     STEPS -> setRange(3, MAX_STEPS);
     STEPS -> setSingleStep(1);
     STEPS -> setValue(10);
     STEPS -> move(10, 235);
     connect(STEPS, SIGNAL(valueChanged(int)), this, SLOT(redrawOnValueChange(int)));

     LFI_L= new QLabel("fi_l", this);
     LFI_L->move(10, 260);
     FI_L = new QDoubleSpinBox(this);
     FI_L -> setRange(-PI, PI);
     FI_L -> setSingleStep(0.05);
     FI_L -> setValue(0.0);
     FI_L -> move(10, 285);
     connect(FI_L, SIGNAL(valueChanged(double)), this, SLOT(redrawOnValueChange(double)));

     LPSI_L= new QLabel("psi_l", this);
     LPSI_L->move(10, 310);
     PSI_L = new QDoubleSpinBox(this);
     PSI_L -> setRange(-PI, PI);
     PSI_L -> setSingleStep(0.05);
     PSI_L -> setValue(PI/4);
     PSI_L -> move(10, 335);
     connect(PSI_L, SIGNAL(valueChanged(double)), this, SLOT(redrawOnValueChange(double)));
}

int Window::dark( const std::vector<QVector4D>& verts, QVector3D light){
    QVector3D center{0,0,0};
    for(QVector4D vert : verts)
        center+=vert.toVector3D();
    center[0]/=verts.size();
    center[1]/=verts.size();
    center[2]/=verts.size();
    light=-(center-light);
   // std::cout<<light.x()<<' '<<light.y()<<' '<<light.z()<<std::endl;
    QVector3D v1=(verts[1]-verts[0]).toVector3D(),
              v2=(verts[2]-verts[0]).toVector3D();

    QVector3D norm(QVector3D::normal(v1, v2));
  //  std::cout<<scalar(QVector3D(0,0,1), norm);
    double angl=scalar(light,norm)/(light.length()*norm.length());
    //return 60+60*(1-angl);
    return 100-70*angl;
}

void Window::cocentrate(std::vector<QVector4D>& pnts){
    QVector4D centr(0, 0, 0, 0);
    for(auto pnt: pnts)
        centr +=pnt;
    centr/=pnts.size();
    for(auto& pnt: pnts)
        pnt-=centr-QVector4D(0, 0, 0, 1);
}

Window::~Window(){}

void Window::paintEvent(QPaintEvent* ){
 bool draw_izo =false; // Box_izo->isChecked();
 //vertexes of polygon
 //cocentrate(Vertexes); //move center of figure to (0,0,0)
 int steps=STEPS->value();
  std::vector<QVector4D> Vertexes;
  double fi1=0.0, psi1=0.0, R=10,
         max_fi=2*PI, max_psi=PI/2,
         step_fi=max_fi/steps, step_psi=max_psi/steps;
  for(psi1=0.0; psi1<max_psi+0.01/*step_psi*/; psi1+=step_psi)
    for(fi1=0.0; fi1<max_fi+0.01/*step_fi*/; fi1+=step_fi)
         Vertexes.push_back(QVector4D{R*cos(fi1)*cos(psi1), R*sin(fi1)*cos(psi1), R*sin(psi1), 1});
  Vertexes.push_back(QVector4D{0, 0, 0, 1});
 double fi   = FI   -> value() /*oz o*/,
       psi  = PSI  -> value() /*oy ^*/,
        teta = TETA -> value() /*ox ->*/,
        scale= SCALE-> value() / (max_coordinate(Vertexes)*MAX_SCALE) * ((height()<width())?height():width())/2;
 double fi_l=FI_L->value(), psi_l=PSI_L->value();
 //polygon edges
 std::vector<std::vector<unsigned int>> polygs;

 for(int i=0; i<steps; ++i)
     for(int j=0; j<steps; ++j){
         polygs.push_back(std::vector<unsigned int>{ j+(i+1)*(steps+1), j+i*(steps+1), (j+1)%(steps+1)+i*(steps+1)});
         if(i!=steps-1)
             polygs.push_back(std::vector<unsigned int>{j+(i+1)*(steps+1), (j+1)%(steps+1)+i*(steps+1), (j+1)%(steps+1)+(i+1)*(steps+1)});
         else
             polygs.push_back(std::vector<unsigned int>{ j, Vertexes.size()-1, j+1});
     }
 double zv=20.0, zs=10.0;
 double izo_scale=zv/zs;
 //Transformation Matrixes
 QMatrix4x4
   rotate_oz(cos(fi), -sin(fi), 0, 0,
             sin(fi),  cos(fi), 0, 0,
                   0,        0, 1, 0,
                   0,        0, 0 ,1),
   rotate_oy(cos(psi), 0,-sin(psi), 0,
                    0, 1,        0, 0,
             sin(psi), 0, cos(psi), 0,
                    0, 0,        0, 1),
   rotate_ox(1,         0,          0, 0,
             0, cos(teta), -sin(teta), 0,
             0, sin(teta),  cos(teta), 0,
             0,         0,          0, 1),
   move_to_center_window(        1,          0, 0, 0,
                                 0,          1, 0, 0,
                                 0,          0, 1, 0,
                         width()/2, height()/2, 0, 1),
   increase(scale, 0, 0, 0,
                0, scale, 0, 0,
                0, 0, scale, 0,
                0, 0,     0, 1),
     izometric((zv-zs), 0, 0, 0,
               0, (zv-zs), 0, 0,
               0, 0, 1, -1,
               0, 0, 0, zv),
     izometric_scale(izo_scale, 0, 0, 0,
                     0, izo_scale, 0, 0,
                     0, 0, izo_scale, 0,
                     0, 0, 0, 1);
 //Tranform all vertexes at (0,0,0) and move to center of window
  for(auto& vert: Vertexes){
     vert = vert * rotate_ox * rotate_oy *rotate_oz;
     if(draw_izo){
        vert = vert * izometric;
         get_normal(vert);
         vert = vert * izometric_scale;
     }
     vert = vert * increase * move_to_center_window;
  }
 //prepare QPainter
 QPainter pntr(this);

// QPen vis_pen(Qt::blue, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
 //QPen invis_pen(Qt::blue, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
 //pntr.setPen(vis_pen);
 for(auto polyg :polygs){
       if(visible_at_oz(Vertexes[polyg[1]]-Vertexes[polyg[0]],
                        Vertexes[polyg[2]]-Vertexes[polyg[0]])){
           QPoint pnts[3] = {{Vertexes[polyg[0]].x(), Vertexes[polyg[0]].y()},
                             {Vertexes[polyg[1]].x(), Vertexes[polyg[1]].y()},
                             {Vertexes[polyg[2]].x(), Vertexes[polyg[2]].y()}};
           QColor Color(0, 155, 0);
           QVector3D light_point{width()/2+10*scale*sin(fi_l)*sin(psi_l), height()/2+10*scale*cos(fi_l)*sin(psi_l), +10*scale*cos(psi_l)};//{width()+3*scale*cos(fi_l)*cos(psi_l), height()+3*scale*sin(fi_l)*cos(psi_l),3*scale*sin(psi_l)};
           pntr.setBrush(QBrush(Color.darker(dark({Vertexes[polyg[0]], Vertexes[polyg[1]], Vertexes[polyg[2]]},
                                                  light_point))));
           QPen pen(Color.darker(10+dark({Vertexes[polyg[0]], Vertexes[polyg[1]], Vertexes[polyg[2]]},
                                      light_point)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
           pntr.setPen(pen);
           pntr.drawPolygon(pnts, 3);
      }
   }
}
