#include <QtGui>
#include "game.h"
#include <QTextCodec> // подключение класса кодека текста

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8")); //изменения
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")); //изменения
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8")); //изменения

    QApplication app(argc, argv);
	Canvas canvas;
	canvas.show();

	return app.exec();
}
