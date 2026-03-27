#include <QApplication>
#include "Scene.h"
#include "LinesCore.h"
#include "DrawingWidgetMT.h"
#include <QTabletEvent>
#include <QOpenGLContext>
#include <fstream>

#include "drwBitmapExporter.h"
#include "LinesApp.h"
#include "drwNetworkManager.h"
#include <QWidget>
#include <QHBoxLayout>
#include "drwDrawingWidget.h"
#include <QTimer>

#include "SideToolbar.h"
#include "drwAspectRatioWidget.h"
#include "TabletStateWidget.h"
#include "drwNetworkConnectDialog.h"
#include "drwGlobalLineParams.h"
#include <QtWidgets>
#include "drwsimplifiedtoolbar.h"
#include "drwGLRenderer.h"

#include "iostream"

#define WIDTH 1920
#define HEIGHT 1080

LinesCore *lines;
LinesApp *linesApp;

const QString DIFF_IMAGE_PATH = "./testData/diff.png";

void decreaseBrushMax()
{
    for (int i = 0; i < 30; i++)
    {
        linesApp->DecreaseBrushSize();
    }
}

void increaseBrushMax()
{
    for (int i = 0; i < 30; i++)
    {
        linesApp->IncreaseBrushSize();
    }
}

void brushTest()
{
    for (int i = 3; i < 13; i++)
    {
        lines->MouseEvent(drwMouseCommand::Press, i * i * 10 + 50, 50, 1.0, 0, 0, 0, 0);
        lines->MouseEvent(drwMouseCommand::Move, i * i * 10 + 50, 200, 1.0, 0, 0, 0, 0);
        lines->MouseEvent(drwMouseCommand::Release, i * i * 10 + 50, 200, 1.0, 0, 0, 0, 0);
        linesApp->IncreaseBrushSize();
    }
    decreaseBrushMax();
}

void fillTest()
{
    linesApp->SetFill(true);
    lines->MouseEvent(drwMouseCommand::Press, 50, 500, 1.0, 0, 0, 0, 0);
    lines->MouseEvent(drwMouseCommand::Move, 50, 1000, 1.0, 0, 0, 0, 0);
    lines->MouseEvent(drwMouseCommand::Move, 1700, 1000, 1.0, 0, 0, 0, 0);
    lines->MouseEvent(drwMouseCommand::Move, 1700, 500, 1.0, 0, 0, 0, 0);
    lines->MouseEvent(drwMouseCommand::Release, 1700, 500, 1.0, 0, 0, 0, 0);
    linesApp->SetFill(false);
}

void eraseTest()
{
    linesApp->SetErasing();
    for (int i = 3; i < 13; i++)
    {
        lines->MouseEvent(drwMouseCommand::Press, i * i * 10 + 50, 600, 1.0, 0, 0, 0, 0);
        lines->MouseEvent(drwMouseCommand::Move, i * i * 10 + 50, 800, 1.0, 0, 0, 0, 0);
        lines->MouseEvent(drwMouseCommand::Release, i * i * 10 + 50, 800, 1.0, 0, 0, 0, 0);
        linesApp->IncreaseBrushSize();
    }
    decreaseBrushMax();
}

int compare(const QString &refImagePath, const QString &testImagePath, double acceptableDiffRate)
{
    QImage refImg(refImagePath);
    QImage testImg(testImagePath);

    if (testImg.isNull() || refImg.isNull())
    {
        std::cerr << "Erreur : Impossible de charger les images" << std::endl;
        return -1;
    }

    if (testImg.size() != refImg.size())
    {
        std::cerr << "Les images doivent avoir les memes dimensions" << std::endl;
        return -1;
    }

    QImage diffImage(testImg.size(), QImage::Format_ARGB32);
    diffImage.fill(Qt::black);
    QColor diffColor(255, 0, 0, 255);

    int similarPixels = 0;
    int totalPixels = testImg.width() * testImg.height();

    for (int y = 0; y < testImg.height(); y++)
    {
        for (int x = 0; x < testImg.width(); x++)
        {
            QColor colorImg1 = testImg.pixelColor(x, y);
            QColor colorImg2 = refImg.pixelColor(x, y);

            if (colorImg1 == colorImg2)
            {
                similarPixels++;
            }
            else
            {
                diffImage.setPixelColor(x, y, diffColor);
            }
        }
    }

    bool saved = diffImage.save(DIFF_IMAGE_PATH);

    double similarityPercentage = (static_cast<double>(similarPixels) / totalPixels) * 100;

    if ((100 - similarityPercentage) > acceptableDiffRate)
    {
        std::cerr << "❌ Échec du test : la différence est supérieure au taux acceptable." << std::endl;
        std::cout << "Pourcentage de pixels similaires : " << similarityPercentage << "%" << std::endl;
        std::cout << "Pourcentage de différence : " << 100 - similarityPercentage << "%" << std::endl;
        std::cout << "Pourcentage de différence acceptable : " << acceptableDiffRate << "%" << std::endl;
        std::exit(1);
    }

    std::cerr << "✅ Test réussi." << std::endl;
    std::cout << "Pourcentage de pixels similaires : " << similarityPercentage << "%" << std::endl;
    std::cout << "Pourcentage de différence : " << 100 - similarityPercentage << "%" << std::endl;
    std::cout << "Pourcentage de différence acceptable : " << acceptableDiffRate << "%" << std::endl;

    return 0;
}

int main(int argc, char **argv)
{
    // Vérifiez si les arguments sont suffisants
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " <ref image path> <test image path> <acceptable diff rate>" << std::endl;
        return -1;
    }

    QApplication app(argc, argv);

    lines = new LinesCore;

    drwNetworkManager *networkManager = new drwNetworkManager();
    networkManager->SetLinesCore(lines);
    lines->SetRemoteIO(networkManager);

    linesApp = new LinesApp(lines, networkManager);

    QWidget *mainWidget = new QWidget;
    mainWidget->setWindowTitle("Lines Test");
    mainWidget->setMinimumSize(WIDTH, HEIGHT);

    QVBoxLayout *drawingAreaLayout = new QVBoxLayout();
    drawingAreaLayout->setContentsMargins(0, 0, 0, 0);

    drwAspectRatioWidget *drawingWidgetContainer = new drwAspectRatioWidget(mainWidget);
    drawingAreaLayout->addWidget(drawingWidgetContainer);

    drwDrawingWidget *glWidget = new drwDrawingWidget(drawingWidgetContainer);
    glWidget->SetLinesCore(lines);
    lines->SetDrawingSurface(glWidget);
    drawingWidgetContainer->setClientWidget(glWidget);
    glWidget->setMinimumSize(WIDTH, HEIGHT);

    QTimer::singleShot(10, []()
                       {
                           brushTest();
                           fillTest();
                           eraseTest(); });

    glWidget->update();
    mainWidget->show();

    QString refImgPath = argv[1];
    QString testImgPath = argv[2];
    double acceptableDiffRate = std::stod(argv[3]);

    QTimer::singleShot(1000, [glWidget, testImgPath]()
                       { 
        glWidget->doneCurrent();
        QImage im = glWidget->grabFramebuffer();
        bool saved = im.save(testImgPath);
        if (!saved) {
            std::cerr << "Failed to save image." << std::endl;
        } });

    QTimer::singleShot(2000, mainWidget, &QWidget::close);
    app.exec();

    return compare(refImgPath, testImgPath, acceptableDiffRate);
}
