#include "mainwindow.h"
#include "ui_mainwindow.h"

bool dir; // Not useful

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(0, 0, 1000, 1000);
    ui->graphicsView->setScene(scene);

    // ui->graphicsView->setSceneRect(20, 10, 300, 200);
    setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    //if(event->key() == )
}

void MainWindow::on_EnterButton_clicked()
{
    if(startDrawing) return;
    else startDrawing = true;

    QPoint origin = ui->graphicsView->mapFromGlobal(QCursor::pos());
    QPointF relativeOrigin = ui->graphicsView->mapToScene(origin);

    int a = origin.x() - relativeOrigin.x();
    int b = origin.y() - relativeOrigin.y();

    double width = ui->lineEdit->text().toDouble();
    double height = ui->lineEdit_2->text().toDouble();
    houseWidth = 10 * width;
    houseHeight = 10 * height;

    QPen outlinePen(Qt::blue);
    outlinePen.setWidth(1);

    experiorWall.clear();
    experiorWall.push_back(scene->addLine(0, 0, houseWidth, 0, outlinePen));
    experiorWall.push_back(scene->addLine(houseWidth, 0, houseWidth, houseHeight, outlinePen));
    experiorWall.push_back(scene->addLine(houseWidth, houseHeight, 0, houseHeight, outlinePen));
    experiorWall.push_back(scene->addLine(0, houseHeight, 0, 0, outlinePen));
}

void MainWindow::on_ResetButton_clicked()
{

    for(auto index : experiorWall)
    {
        scene->removeItem(index);
    }
    for(auto index : interiorWall)
    {
        scene->removeItem(index);
    }
    for(auto index : windowList)
    {
        scene->removeItem(index);
    }
    for(auto index : doorList)
    {
        scene->removeItem(index);
    }
    experiorWall.clear();
    interiorWall.clear();
    windowList.clear();
    doorList.clear();
    startDrawing = false;
}


void MainWindow::on_makeWall_clicked()
{
   if(!startDrawing) return;

    wallDrawing = 1;

    qmb.setText("Click the wall, please.");
    qmb.exec();
}

void MainWindow::mousePressEvent(QMouseEvent *mouse)
{
    if(!startDrawing || !wallDrawing) return;

    QPoint origin = ui->graphicsView->mapFromGlobal(QCursor::pos());
    QPointF relativeOrigin = ui->graphicsView->mapToScene(origin);

    int x = relativeOrigin.x();
    int y = relativeOrigin.y();

    if(x < 0 || y < 0 || x > houseWidth || y > houseHeight)
    {
        qmb.setText("Please Click in the house.");
        qmb.exec();
        return;
    }

    if(wallDrawing == 1)
    {
        wallDrawing++;

        double min = INF, length;
        QGraphicsLineItem *find;
        for(auto index : experiorWall)
        {
            if(index->line().x1() == index->line().x2())
                length = abs(index->line().toLine().x1() - x);
            else length = abs(index->line().toLine().y1() - y);

            if(length < min)
            {
                min = length;
                find = index;
            }
        }

        if(find->line().x1() == find->line().x2())
        {
            x = find->line().toLine().x1();
            dir = true;
        }
        else
        {
            y = find->line().toLine().y1();
            dir = false;
        }

        wallx1 = x;
        wally1 = y;

    }
    else if(wallDrawing == 2)
    {
        wallDrawing = 0;

        wallx2 = x;
        wally2 = y;

        if(dir) wally2 = wally1;
        else wallx2 = wallx1;

        QPen outlinePen(Qt::red);
        outlinePen.setWidth(2);
        interiorWall.push_back(scene->addLine(wallx1, wally1, wallx2, wally2, outlinePen));
    }
    else if(wallDrawing == 3) // window
    {
        double min = INF, length;
        QGraphicsLineItem *Object;

        for(auto index : experiorWall)
        {
            if(index->line().x1() == index->line().x2())
                length = abs(index->line().toLine().x1() - x);
            else length = abs(index->line().toLine().y1() - y);

            if(length < min)
            {
                min = length;
                Object = index;
            }
        }

        for(auto index : interiorWall)
        {
            if(index->line().x1() == index->line().x2())
                length = abs(index->line().toLine().x1() - x);
            else length = abs(index->line().toLine().y1() - y);

            if(length < min)
            {
                min = length;
                Object = index;
            }
        }

        if(min > 20)
        {
            qmb.setText("Too far for the wall.");
            qmb.exec();
            return;
        }

        if(Object->line().x1() == Object->line().x2())
        {
            x = Object->line().toLine().x1();
            dir = true;
        }
        else
        {
            y = Object->line().toLine().y1();
            dir = false;
        }

        for(auto index : windowList)
        {
            if(index->line().x1() == index->line().x2())
                length = sqrt((index->line().toLine().x1() - x) * (index->line().toLine().x2() - x)
                        + ((index->line().toLine().y1() + index->line().toLine().y2()) / 2 - y)
                        * ((index->line().toLine().y1() + index->line().toLine().y2()) / 2 - y));
            else length = sqrt((index->line().toLine().y1() - y) * (index->line().toLine().y2() - y)
                               + ((index->line().toLine().x1() + index->line().toLine().x2()) / 2 - x)
                               * ((index->line().toLine().x1() + index->line().toLine().x2()) / 2 - x));

            if(length < 30)
            {
                scene->removeItem(index);
                windowList.erase(std::find(windowList.begin(), windowList.end(), index));
                return;
            }
        }

        QPen outlinePen(Qt::green);
        outlinePen.setWidth(5);
        if(!dir)
        {
            if((Object->line().toLine().x1() + Object->line().toLine().x2() / 2) > x)
                windowList.push_back(scene->addLine(x, y, x + 20, y, outlinePen));
            else windowList.push_back(scene->addLine(x, y, x - 20, y, outlinePen));
        }
        else
        {
            if((Object->line().toLine().y1() + Object->line().toLine().y2() / 2) > y)
                windowList.push_back(scene->addLine(x, y, x, y + 20, outlinePen));
            else windowList.push_back(scene->addLine(x, y, x, y - 20, outlinePen));
        }

        // wallDrawing = 0;
    }
    else if(wallDrawing == 4) // door
    {
        double min = INF, length;
        QGraphicsLineItem *Object;

        for(auto index : experiorWall)
        {
            if(index->line().x1() == index->line().x2())
                length = abs(index->line().toLine().x1() - x);
            else length = abs(index->line().toLine().y1() - y);

            if(length < min)
            {
                min = length;
                Object = index;
            }
        }

        for(auto index : interiorWall)
        {
            if(index->line().x1() == index->line().x2())
                length = abs(index->line().toLine().x1() - x);
            else length = abs(index->line().toLine().y1() - y);

            if(length < min)
            {
                min = length;
                Object = index;
            }
        }

        if(min > 20)
        {
            qmb.setText("Too far for the wall.");
            qmb.exec();
            return;
        }

        if(Object->line().x1() == Object->line().x2())
        {
            x = Object->line().toLine().x1();
            dir = true;
        }
        else
        {
            y = Object->line().toLine().y1();
            dir = false;
        }

        for(auto index : windowList)
        {
            if(index->line().x1() == index->line().x2())
                length = sqrt((index->line().toLine().x1() - x) * (index->line().toLine().x2() - x)
                        + ((index->line().toLine().y1() + index->line().toLine().y2()) / 2 - y)
                        * ((index->line().toLine().y1() + index->line().toLine().y2()) / 2 - y));
            else length = sqrt((index->line().toLine().y1() - y) * (index->line().toLine().y2() - y)
                               + ((index->line().toLine().x1() + index->line().toLine().x2()) / 2 - x)
                               * ((index->line().toLine().x1() + index->line().toLine().x2()) / 2 - x));

            if(length < 30)
            {
                scene->removeItem(index);
                windowList.erase(std::find(windowList.begin(), windowList.end(), index));
                return;
            }
        }

        QPen outlinePen(Qt::black);
        outlinePen.setWidth(20);
        if(!dir)
        {
            if((Object->line().toLine().x1() + Object->line().toLine().x2() / 2) > x)
                windowList.push_back(scene->addLine(x, y, x + 20, y, outlinePen));
            else windowList.push_back(scene->addLine(x, y, x - 20, y, outlinePen));
        }
        else
        {
            if((Object->line().toLine().y1() + Object->line().toLine().y2() / 2) > y)
                windowList.push_back(scene->addLine(x, y, x, y + 20, outlinePen));
            else windowList.push_back(scene->addLine(x, y, x, y - 20, outlinePen));
        }

//        QString imgPath = "C:/Qt/Qt5.10.1/image/door.jpg";
//        QImage img(imgPath);
//        QPixmap pix = QPixmap::fromImage(img);
//        pix = pix.scaled(30, 30);

//        scene->addPixmap(pix);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    QPoint origin = ui->graphicsView->mapFromGlobal(QCursor::pos());
    QPointF relativeOrigin = ui->graphicsView->mapToScene(origin);

    int x = relativeOrigin.x();
    int y = relativeOrigin.y();

    ui->lineEdit->setText(QString::number(x));
}

void MainWindow::on_DoorButton_clicked()
{
    if(wallDrawing == 4)
    {
        wallDrawing = 0;
        return;
    }
    wallDrawing = 4;
}

void MainWindow::on_WindowButton_clicked()
{
    if(wallDrawing == 3)
    {
        wallDrawing = 0;
        return;
    }
    wallDrawing = 3;



    bfs();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    double length;

    QPoint origin = ui->graphicsView->mapFromGlobal(QCursor::pos());
    QPointF relativeOrigin = ui->graphicsView->mapToScene(origin);

    int a = origin.x(); int b = origin.y();


    int x = relativeOrigin.x();
    int y = relativeOrigin.y();

    for(auto index : interiorWall)
    {
        if(index->line().x1() == index->line().x2())
            length = sqrt((index->line().toLine().x1() - x) * (index->line().toLine().x2() - x)
                           + ((index->line().toLine().y1() + index->line().toLine().y2()) / 2 - y)
                           * ((index->line().toLine().y1() + index->line().toLine().y2()) / 2 - y));
        else length = sqrt((index->line().toLine().y1() - y) * (index->line().toLine().y2() - y)
                           + ((index->line().toLine().x1() + index->line().toLine().x2()) / 2 - x)
                           * ((index->line().toLine().x1() + index->line().toLine().x2()) / 2 - x));

        if(length < 50)
        {
            scene->removeItem(index);
            interiorWall.erase(std::find(interiorWall.begin(), interiorWall.end(), index));
            return;
        }
    }
}

void MainWindow::bfs()
{
    QPoint origin = ui->graphicsView->mapFromGlobal(QCursor::pos());
    QPointF relativeOrigin = ui->graphicsView->mapToScene(origin);

    int a = origin.x() - relativeOrigin.x();
    int b = origin.y() - relativeOrigin.y();

    QPixmap qPix = QPixmap::grabWidget(ui->graphicsView);
    QImage image(qPix.toImage());

    QRgb val[3] = {qRgb(255, 0 ,0), qRgb(0, 255, 0), qRgb(0, 0, 255)};
    int count = 0;

    int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    for(int i = 1; i <= houseWidth; i++)
    {
        for(int j = 1; j <= houseHeight; j++)
        {
            QColor color(image.pixelColor(i + a, j + b));
            if(color.blue() == 255 && color.red() == 255 && color.green() == 255)
            {
                std::queue<std::pair<int, int> > temp;
                temp.push(std::make_pair(i, j));
                while(1)
                {
                    if(temp.empty())
                    {
                        count++;
                        break;
                    }
                    int tx = temp.front().first;
                    int ty = temp.front().second;
                    temp.pop();

                    for(int k=0; k<4; k++)
                    {
                        int x = tx + dx[k];
                        int y = ty + dy[k];
                        if(x < 0 || y < 0 || x > houseWidth || y > houseWidth) continue;
                        QColor color2(image.pixelColor(x + a, y + b));
                        if(color2.blue() != 255 || color2.red() != 255 || color2.green() != 255) continue;
                        image.setPixelColor(x + a, y + b,  val[count]);
                        temp.push(std::make_pair(x, y));
                    }
                }
            }
        }
    }

    QPixmap pixmap = QPixmap::fromImage(image);
    scene->addPixmap(pixmap);
}
