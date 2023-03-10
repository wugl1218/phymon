#include "mc_visualization.h"
#include "Common.h"
#include <QtMath>
#include <QPainter>
#include <QPainterPath>

mc_visualization::mc_visualization(QWidget *parent)
    : QWidget(parent)
{
}
void mc_visualization::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
    emit pressed();
}

void mc_visualization::mouseReleaseEvent(QMouseEvent* event)
{
    emit released();
}

void mc_visualization::paintEvent(QPaintEvent *event)
{
    Common* common = Common::instance();

   int count = visualizetionlist.size();

    if(count<3)
    {
        QPainter painter(this);
        painter.translate(width()/2., height()/2.);
        Common::draw_text(painter,0,0, Qt::AlignLeft | Qt::AlignVCenter, "Pleasa add at least three vital signs");
        return;
    }
    QFont data_font ;
    QFont value_font ;
    bool Show_pen =1;
    data_font.setPixelSize(22);
    data_font.setFamily("Arial [Mono]");
    value_font.setPixelSize(16);
    value_font.setFamily("Arial [Mono]");

   int radius = height()/2-60;

   /*
    * data須提供 data_name,data[n],low,high,color
    *
    *
    *
    */
   QPainter painter(this);
   painter.setPen(Qt::NoPen);
   painter.setBrush(QColor(7, 22, 40));
   painter.drawRect(0,0,width(),height());
   //设置为抗锯齿,并且设置画笔颜色为浅灰
   painter.setRenderHint(QPainter::Antialiasing);
   QColor pen_color =QColor(255,255,255);
   painter.setPen(pen_color);
   painter.setFont(value_font);
   /*
    * 绘图设备的坐标原点(0,0)在左上角，水平向右增长，垂直向下增长。
    * 首先先平移坐标原点，让原点在绘图设备的中心
    */
   painter.translate(width()/2., height()/2.);
   QPainterPath RT;
   QPainterPath path;
   QPainterPath alarm_line;
   float alarm_line_start_x,alarm_line_start_y;
   QColor visualizetionColor=common->css.alarm_green; //圖形顏色
    if(count>9)return;
   //开始绘制多边形
   for (int i = 0; i < count; ++i)
   {
       //设中心点到边的垂线与半径的夹角为degree=(360/count)/2即：
        float degree = 180./count;

       //先将坐标进行旋转，然后再绘制
    //    painter.rotate(2 * degree);
       //设边长的一半为x,则x = radius*sin(degree)
       //原点到边的距离为y，则y = radius*cos(degree)
       //在程序中三角函数都是以弧度为基准，所以要先将角度转化成弧度。
        float radian1 = qDegreesToRadians(degree*2*(i-1));
        float x1    = radius * qSin(radian1); //QT座標系與笛卡爾座標系不同
        float y1    = radius * qCos(radian1);
        float radian2 = qDegreesToRadians(degree*2*(i));
        float x2    = radius * qSin(radian2); //QT座標系與笛卡爾座標系不同
        float y2    = radius * qCos(radian2);
 //       RT.lineTo(x1,y1);
       //绘制该三角区块
        if(i==0)
            path.moveTo(x1, y1);
        path.lineTo(x2, y2);
        painter.setPen(pen_color);
        painter.drawPath(path);
        painter.setRenderHint(QPainter::Antialiasing, true);

        float high=visualizetionlist[i].higher;
        float low=visualizetionlist[i].lower;
        float avg=qRound((high+low)/2);
        float max =high*2-avg;
        float min =low*2-avg;
        QString dataname= visualizetionlist[i].dataname;
        bool REVERSE_DIRECTION = y2 > y1 -1;
        bool VERTICAL = abs(x2 - x1) <= 1;
        QList<float> data;
        std::string querystr = "patient_id MATCH '";
            querystr.append(common->patient_id);
            querystr.append("' AND vmd_id MATCH '");
            querystr.append(common->vmd_id);
            querystr.append("' AND mdc_code MATCH '");
            querystr.append(dataname.toStdString());
            querystr.append("'");

        dds::sub::cond::QueryCondition cond(
                    dds::sub::Query(common->visualizetion_observation_reader, querystr),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->visualizetion_observation_reader.select().condition(cond).read();
        for (auto sample :samples)
        {
            dds::core::xtypes::DynamicData& d = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
            float value=d.value<float>("value");
            if(visualizetionlist[i].highest<value ||visualizetionlist[i].lowest >value)
                visualizetionColor=common->css.alarm_red;
            else if(visualizetionColor!=common->css.alarm_red&&(visualizetionlist[i].higher<value ||visualizetionlist[i].lower >value))
                visualizetionColor=common->css.alarm_yellow;
            data<<value;
        }
        data<<50;
        if(REVERSE_DIRECTION) //重新排列data
            std::sort(data.begin(), data.end(),std::greater<int>());
        else
            std::sort(data.begin(), data.end(),std::less<int>());

        float x_ideal,y_ideal;
        for (int j=0 ;j<data.size();++j)
        {
            if (REVERSE_DIRECTION)
            {
                float proportion=(data[j]-min)/(max-min);  //計算即時資料在線上位置
                float slope = 1.0 * (y1 - y2) / (x1 - x2);
                double intercept = y2 - slope * x2;
                x_ideal = proportion * (x1 - x2) + x2;;
                y_ideal = slope * x_ideal + intercept;;
                if (VERTICAL) {
                    x_ideal = x2;
                    y_ideal = (int) (proportion * (y1 - y2) + y2);
                }
            }
            else
            {
                float proportion=(data[j]-min)/(max-min);  //計算即時資料在線上位置
                float slope = 1.0 * (y2 - y1) / (x2 - x1);
                double intercept = y1 - slope * x1;
                x_ideal = proportion * (x2 - x1) + x1;;
                y_ideal = slope * x_ideal + intercept;;
                if (VERTICAL) {
                    x_ideal = x1;
                    y_ideal = (int) (proportion * (y2 - y1) + y1);
                }
            }
            if(i==0&&j==0)
                RT.moveTo(x_ideal, y_ideal);
            else
                RT.lineTo(x_ideal, y_ideal);
        }

       int length1,length2;
       float proportion1 =1.4; //調整距離圓心位置
       painter.setFont(data_font);
       if((x1+(x2-x1)*0.50)<0) //讓左側文字靠左
        {
           length1= dataname.length()*10;
           length2= QString::number(avg).length()*5;

        }
       else if(x1+(x2-x1)*0.50<1)
        {
           length1= dataname.length()*5;
           length2= 0;
        }
       else
        {
           length1=0;
           length2=0;
        }


        Common::draw_text(painter, (x1+(x2-x1)*0.50)*proportion1-length1, (y1+(y2-y1)*0.50)*proportion1, Qt::AlignLeft | Qt::AlignVCenter, dataname);
        if (REVERSE_DIRECTION)
        {
           painter.setFont(value_font);
           proportion1 =1.15; //距離中心點距離
           Common::draw_text(painter, (x2+(x1-x2)*0.92)*proportion1-length2, (y2+(y1-y2)*0.92)*proportion1, Qt::AlignLeft | Qt::AlignVCenter, QString::number(high*2-avg));
           Common::draw_text(painter, (x2+(x1-x2)*0.71)*proportion1-length2, (y2+(y1-y2)*0.71)*proportion1, Qt::AlignLeft | Qt::AlignVCenter, QString::number(high));
           Common::draw_text(painter, (x2+(x1-x2)*0.50)*proportion1-length2, (y2+(y1-y2)*0.50)*proportion1, Qt::AlignLeft | Qt::AlignVCenter, QString::number(avg));
           Common::draw_text(painter, (x2+(x1-x2)*0.29)*proportion1-length2, (y2+(y1-y2)*0.29)*proportion1, Qt::AlignLeft | Qt::AlignVCenter,QString::number(low));
           Common::draw_text(painter, (x2+(x1-x2)*0.10)*proportion1-length2, (y2+(y1-y2)*0.10)*proportion1, Qt::AlignLeft | Qt::AlignVCenter, QString::number(low*2-avg));

        }
        else
        {
           painter.setFont(value_font);
           proportion1 =1.15; //距離中心點距離
           Common::draw_text(painter, (x1+(x2-x1)*0.92)*proportion1-length2, (y1+(y2-y1)*0.92)*proportion1, Qt::AlignLeft | Qt::AlignVCenter, QString::number(high*2-avg));
           Common::draw_text(painter, (x1+(x2-x1)*0.71)*proportion1-length2, (y1+(y2-y1)*0.71)*proportion1, Qt::AlignLeft | Qt::AlignVCenter, QString::number(high));
           Common::draw_text(painter, (x1+(x2-x1)*0.50)*proportion1-length2, (y1+(y2-y1)*0.50)*proportion1, Qt::AlignLeft | Qt::AlignVCenter, QString::number(avg));
           Common::draw_text(painter, (x1+(x2-x1)*0.28)*proportion1-length2, (y1+(y2-y1)*0.28)*proportion1, Qt::AlignLeft | Qt::AlignVCenter,QString::number(low));
           Common::draw_text(painter, (x1+(x2-x1)*0.10)*proportion1-length2, (y1+(y2-y1)*0.10)*proportion1, Qt::AlignLeft | Qt::AlignVCenter, QString::number(low*2-avg));
       }
        if(i==0) //警戒線
            {
            alarm_line_start_x=x1+(x2-x1)*0.25,alarm_line_start_y=y1+(y2-y1)*0.25;
            alarm_line.moveTo(x1+(x2-x1)*0.75, y1+(y2-y1)*0.75);
            }
        else
        {
            alarm_line.lineTo(x1+(x2-x1)*0.25, y1+(y2-y1)*0.25);
            alarm_line.moveTo(x1+(x2-x1)*0.75, y1+(y2-y1)*0.75);
        }
        if(i==count-1) //警戒線回起點
            alarm_line.lineTo(alarm_line_start_x,alarm_line_start_y);
       QPainterPath line;
       proportion1 =1.05;
       line.moveTo((x1+(x2-x1)*1.00), (y1+(y2-y1)*1.00));
       line.lineTo((x1+(x2-x1)*1.00)*proportion1, (y1+(y2-y1)*1.00)*proportion1);
       line.moveTo((x1+(x2-x1)*0.75), (y1+(y2-y1)*0.75));
       line.lineTo((x1+(x2-x1)*0.75)*proportion1, (y1+(y2-y1)*0.75)*proportion1);
       line.moveTo((x1+(x2-x1)*0.50), (y1+(y2-y1)*0.50));
       line.lineTo((x1+(x2-x1)*0.50)*proportion1, (y1+(y2-y1)*0.50)*proportion1);
       line.moveTo((x1+(x2-x1)*0.25), (y1+(y2-y1)*0.25));
       line.lineTo((x1+(x2-x1)*0.25)*proportion1, (y1+(y2-y1)*0.25)*proportion1);
       line.moveTo((x1+(x2-x1)*0.00), (y1+(y2-y1)*0.00));
       line.lineTo((x1+(x2-x1)*0.00)*proportion1, (y1+(y2-y1)*0.00)*proportion1);
       painter.drawPath(line);
   }
   painter.setPen(Qt::blue);
   alarm_line.closeSubpath();
   painter.drawPath(alarm_line);
   painter.setPen(visualizetionColor);
   RT.closeSubpath();
   painter.drawPath(RT);
   painter.fillPath(RT, visualizetionColor);
}

void mc_visualization::set_modle_count(int count)
{
    modle_count =count;
    update();
}
