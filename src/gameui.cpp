#include <QtGui>
#include <iostream>
#include <string>
#include <typeinfo>
#include "game.h"

class Game;

Canvas::Canvas(QWidget *parent) : QWidget(parent) // класс канвас наследуется от виждета
 {
    // кнопки старт/ход/стоп
    startButton = new QPushButton(tr("&Старт"));
    startButton->setDefault(true);
    hitButton = new QPushButton(tr("&Ход"));
    standButton = new QPushButton(tr("&Пропустить"));

    // коннект к слотам
	connect(startButton, SIGNAL(clicked()), this, SLOT(startBtn()));
	connect(hitButton, SIGNAL(clicked()), this, SLOT(hitBtn()));
	connect(standButton, SIGNAL(clicked()), this, SLOT(standBtn()));

    // лайауты компоновка
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(startButton);
	buttonLayout->addWidget(hitButton);
	buttonLayout->addWidget(standButton);

    // ростяжка
	buttonLayout->addStretch();

    //стол карт/ лист виев
	cardTable = new QListView;
    cardTable->setStyleSheet("background-color:green;font-weight:bold");
	QLabel *houseLabel = new QLabel(cardTable);
	QLabel *playerLabel = new QLabel(cardTable);
    houseLabel->setText("Банк");
    playerLabel->setText("Игрок");
	houseLabel->move(10,20);
	playerLabel->move(10,150);

    // компоновка по вертикали кнопок и стола
	QVBoxLayout *verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(buttonLayout);
	verticalLayout->addWidget(cardTable);

    // лэйблы результата игроков
    hostState = new QLabel("Банк");
    playerState = new QLabel("Игрок");
    result = new QLabel("Результаты");

    // компоновка по вертикали результатов
	verticalLayout->addWidget(hostState);
	verticalLayout->addWidget(playerState);
	verticalLayout->addWidget(result);

    // главный лайаут привязываем к окну
	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addLayout(verticalLayout, 0,0,1,1);
	setLayout(mainLayout);

    // плюшки
    setWindowTitle(tr("Oh my eggs"));
    resize(500,400);

    // отключить кнопки ход и стоп // замечательные глюки при true
    setEnableHitStandButtons(false);

    setupIcons();
}

void Canvas::setEnableHitStandButtons(bool b) // задействовать кнопки
{
	hitButton->setEnabled(b);
	standButton->setEnabled(b);
}

void Canvas::setupIcons() /// взять у Canvas массив лейблов и в каждом создать новый лейбл для карты потом туда ложить Qpixmap
{
    QPixmap qpx ;/// обьект  QPixmap
    string str; /// строка
	
        for (int i = 0; i < MAXCARDS; i++) { /// пока есть карты
        cardIconHouse[i] = new QLabel(cardTable); /// ранее создали массив лейблов 10 штук указав родителя длист виев
        /// взять у Canvas массив лейблов и в каждом создать новый лейбл для карты потом туда будем ложить Qpixmap
        str = ":/images/blank.png"; /// но они по умолчинию пока невидимы строка с пустой картинкой
        qpx = QPixmap(str.c_str()); /// загнать в обьект пиксмапс пустую картинку
        cardIconHouse[i]->setPixmap(qpx); /// присвоили Pixmaps лейблу в масиве
        cardIconHouse[i]->move(80+71*i,20); /// сдвинули на 1 картинку вправо 20 по Y
	} 

        /// то же для играка
       	for (int i = 0; i < MAXCARDS; i++) {
		cardIconPlayer[i] = new QLabel(cardTable);
        qpx = QPixmap(":/images/blank.png");
		cardIconPlayer[i]->setPixmap(qpx);
        cardIconPlayer[i]->move(80+71*i,150);
	} 
}

void Canvas::clearIcons() /// спрятать карты cardIconHouse cardIconPlayer инициализируются пустыми картами
{
    QPixmap qpx = QPixmap(":/images/blank.png");
    for (int i = 0; i < MAXCARDS; i++) {
		cardIconHouse[i]->setPixmap(qpx);
    }

    for (int i = 0; i < MAXCARDS; i++) {
        cardIconPlayer[i]->setPixmap(qpx);
	} 
}

void Canvas::startBtn() // старт гейм
{
        vector<string> names; // создадим имена игроков
        string name = "Ты";
    	names.push_back(name);

    	//the game 
    	myGame = new Game(names,this);

    	// initial table setup
        myGame->gameStarted(); // и тут игра началась
        setEnableHitStandButtons(true); // а кнопок нету
}

void Canvas::hitBtn() /// кнопка хода, выполнить слот  hitSelected()
{
	myGame->hitSelected();
}

void Canvas::standBtn() /// кнопка хода, выполнить слот  standSelected()
{
	myGame->standSelected();

}

void Canvas::setHostStateLabel(QString &s)  /// строка состояния дома / очки
{
	hostState->setText(s);
}

void Canvas::setPlayerStateLabel(QString &s) /// строка состояния игрока ( принять строку и установить текст виджета playerState )
{
	playerState->setText(s);
}

void Canvas::setResultLabel(QString &s) /// установить текст окна результата
{
	result->setText(s);
}

void Canvas::drawCards(AbstractPlayer &ap) /// определить чьи карты рисовать взять вектор QPixmap игроки и загнать в массив лейблов класса Canvas (QLabel *cardIconPlayer[MAXCARDS];)
{
    QPixmap qpx ; /// создать обьект класса QPixmap

    if(typeid(ap) == typeid(Player)) { /// если рисуем карты и нам передали игрока
        Player *ptr = dynamic_cast<Player *> (&ap); /// преобразуем того кого мы получили в игрока
        int size = (ptr->getPixmaps()).size(); /// создадим размер из размера масива пиксмапсов игрока
        for(int i = 0; i < size ; i++) { /// пока они есть
            qpx = (ptr->getPixmaps())[i]; /// возьмем у игрока все обьекты QPixmap(готовый обьект с картинкой внутри) в векторе pixmaps
            cardIconPlayer[i]->setPixmap(qpx); /// и загоним в массив лейблов класса Canvas (QLabel *cardIconPlayer[MAXCARDS];)
            /// установить для каждого лейбла свой пиксмапс, тоесть отрисовать картинки
		}
	}
    /// и для дома то же
	else if(typeid(ap) == typeid(House)) {
        	House *ptr = dynamic_cast<House *> (&ap);
 		int size = (ptr->getPixmaps()).size();
		for(int i = 0; i < size ; i++) {
        		qpx = (ptr->getPixmaps())[i];
			cardIconHouse[i]->setPixmap(qpx);
		}
	}
}
