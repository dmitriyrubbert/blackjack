#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <string>
#include <vector>
#include <QWidget>
#include <QListView>
using namespace std;

// прототипы
class QLabel;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QString;
class Player;
class Game;
class AbstractPlayer;

const int MAXCARDS = 10;

//отрисовка
class Canvas : public QWidget
{
        Q_OBJECT

 public:

        Canvas(QWidget *parent = 0);
        void setHostStateLabel(QString &s);  /// строка состояния дома / очки
        void setPlayerStateLabel(QString &s);  /// строка состояния игрока / очки
        void setResultLabel(QString &s); /// сообщение о результатах result->setText
        void drawCards(AbstractPlayer&); /// определить чьи карты рисовать взять вектор QPixmap игроки и загнать в массив лейблов класса Canvas (QLabel *cardIconPlayer[MAXCARDS];)
        void setupIcons();  /// взять у Canvas массив лейблов и в каждом создать новый лейбл для карты потом туда ложить Qpixmap
        void clearIcons(); /// спрятать карты cardIconHouse cardIconPlayer инициализируются пустыми картами
        void setEnableHitStandButtons(bool b); /// прячем кнопки

 public slots:
        void startBtn(); /// начало игры
        void hitBtn(); /// кнопка хода
        void standBtn(); /// кнопка пропуска хода

 private:
        QPushButton *startButton; /// начало игры
        QPushButton *hitButton; /// кнопка хода
        QPushButton *standButton; /// кнопка пропуска хода
        QLabel *hostState; /// label очки дома
        QLabel *playerState; /// label очки игрока
        QLabel *result;
        Game *myGame;

        QListView *cardTable; /// стол QListView
        QLabel *cardIconHouse[MAXCARDS];
        QLabel *cardIconPlayer[MAXCARDS]; // как это, массив картинок , ярлыков с картинками
};

class Card // конструктор карт
{
public:

    enum Rank {ACE = 0, TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
               JACK, QUEEN, KING};           /// номинал туз 0

    enum Suit {SPADES, HEARTS, DIAMONDS, CLUBS}; /// масть Пики, черви, бубны, трефы

    /// перегрузка оператора << для печати строки состояния
    friend ostream& operator<<(ostream& os, const Card& aCard);
    
    Card(Rank r = ACE, Suit s = CLUBS, bool ifu = true);  /// конструктор

    /// вернуть номинал карты если карты лицом вверх, или 0, корекция номиналов старших карт
    int getValue() const;

    /// переворот карты
    void flip();

    bool getisFaceUp(); /// вернуть перевернута ли карта
 
    string convert(); /// сформировать путь к картинке в файле ресурсов
    
private:
    Rank rank; /// номинал карт
    Suit suit; /// масть карт
    bool isFaceUp; /// отвечает перевернуты ли карты или нет
};

class Hand
{

public:
	Hand();
    
	virtual ~Hand();

    void add(Card* pCard); /// добавить карту в конец колоды (руки или колода)

    void clear(); /// проитерировав вектор очистить всю память

    int getTotal() const; /// вернуть сумму значений в векторе карт, с коррекцией туза

protected:
    vector<Card*> cards; /// вектор карт для игроков и колоды
};

class AbstractPlayer : public Hand // наследуем руки, нам нужун cards
{
	friend ostream& operator<<(ostream& os, const AbstractPlayer& aAbstractPlayer);

public:
	AbstractPlayer(const string& s = "");

    QString makeLabelString(); /// вернуть строку состояния с текстовым представлением карт и суммой значений

    virtual void makePixmaps(AbstractPlayer& ) = 0 ; /// вернуть по ссылке вектор картинок, тут виртуальна
    
    virtual ~AbstractPlayer();

	virtual bool isHitting() const = 0;

    /// проверка, сдулся если набрал больше 21
    bool isBusted() const;

    /// сообщение ты продул bust(банкрот)
	QString bust() const;

	QString& getLabelQStr() const;

	string getName() const;

protected:
    string name; // имя
    Hand *hand; // карты на руках
    mutable QString labelQStr; //mutable языка С++. Это позволяет нам модифицировать эти переменные в функциях с модификатором const.
};

class Player : public AbstractPlayer // наследуется от абсрактного игрока
{
public:
    Player(const string& name = ""); //конструктр

    virtual ~Player(); //виртуал десруктр

    virtual void makePixmaps(AbstractPlayer& ); /// зафигачить в вектор пиксмапсов, (создав пути к файлу) который содержит экземпляр hand игрока

    /// вернуть число hitCount игрока (getHitCount();)
	virtual bool isHitting() const;

    /// сообщение ты Выиграл
	void win(Canvas &) const; 

    /// сообщение ты продул
	void lose(Canvas &) const;

    /// Ничья
	void push(Canvas &) const;

    static void resetHitCount(); /// сбросить число hitCount игрока
    static void setHitCount(); /// +1 hitCount игрока
    static int getHitCount(); /// вернуть число hitCount игрока

    void resultMessage(string& s) const; // сообщение о выиграше или ...

    vector<QPixmap>& getPixmaps(); /// вернуть по ссылке вектор обьектов QPixmap

private:
    static int hitCount; /// число ходов ( "СТАТИК" переменная существует вне класса)
    vector<QPixmap> pixmaps;
};

class House : public AbstractPlayer  // наследуется от абсрактного игрока
{
public:
	House(const string& name = "House");

    virtual ~House();

    /// зафигачить в вектор пиксмапсов, (создав пути к файлу) который содержит экземпляр hand игрока
    virtual void makePixmaps(AbstractPlayer& ) ;

    /// если тотал >=16 вернуть труфолс
	virtual bool isHitting() const;
  
    void flipFirstCard(bool b); /// перевернуть первую карту

    vector<QPixmap>& getPixmaps(); /// вернуть по ссылке вектор обьектов QPixmap

private:
        vector<QPixmap> pixmaps;
};

class Deck : public Hand // колода наследует руки //берем оттуда  cards
{
public:
	Deck();
    
    virtual ~Deck();

    /// сделать стандартную колоду 52 cards
	void populate();

    //перемешать
	void shuffle();

    //взять карту с колоды
	void deal(Hand& aHand);

	//give additional cards to an abstract player 
	QString additionalCards(AbstractPlayer& aAbstractPlayer);
private:
        vector<QPixmap> pixmaps;
};

class Game // класс игра
{
public:
	Game(const vector<string>& names, Canvas *cnv);
    
	~Game();
    
    void gameStarted(); /// начало игры
	void hitSelected();
	void standSelected();
	bool getStand();
	void setStand(bool b);

private:
    Deck deck; // колода
    House house; // дом
    vector<Player> player; // игроки
    Canvas *canvas; // писюльки
    bool stand; // пропуск хода
};
#endif
