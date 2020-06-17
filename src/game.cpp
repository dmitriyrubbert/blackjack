#include <iostream> // std::cout
#include <typeinfo>
#include <string> // std::string
#include <vector> // std::vector
#include <algorithm> // std::random_shuffle
#include <ctime> // std::time
#include <sstream> // склеить инты в строку
#include <QMessageBox> // диалоговые окна

#include "game.h" // хеадер

using namespace std;

class QLabel;
class QString;

// ### C A R D S ### //
Card::Card(Rank r, Suit s, bool isFace):  rank(r), suit(s), isFaceUp(isFace) // инициализация карт картами
{} 

int Card::getValue() const /// вернуть номинал карты если карты лицом вверх, или 0, корекция номиналов старших карт
{
    /// если карты лицом вниз вернуть исходную value = 0
	int value = 0;
    if (isFaceUp) /// если карты лицом вверх
	{
        /// value = номиналу карты
		value = rank;
        /// если значение карты больше 10 ( enum 11,12,13 валет дама король считаются как 10)
		if (value > 10)
                    value = 10;
    	}
        return value; /// вернуть номинал
}

void Card::flip() /// переворот карты
{
    	isFaceUp = !isFaceUp;
}

string Card::convert()  /// сформировать пут к картинке в файле ресурсов
{
    ostringstream oss; /// склеить разные типы в одну строку
    string str; /// ее вернем

        if (isFaceUp) /// если карта лицом
            oss << ":images/" << suit << rank << ".gif";

        else /// или вернуть обратную сторону, если карта перевернута
            oss << ":images/" << "xx" << ".gif";

        str = oss.str(); /// склеить
        return str; /// и вернуть строкой
}

bool Card::getisFaceUp() /// вернуть перевернута ли карта
 {
     return isFaceUp;
 }

// ### H A N D S ### //
Hand::Hand() // конструктор руки, зарезервировать 5 ячеек в векторе
{
	cards.reserve(5);
}

Hand::~Hand()  // деструктор, очистить вектор
{
    clear();// Removes all elements from the vector (which are destroyed), leaving the container with a size of 0.
    // но сделаем это по своему
}

void Hand::add(Card* pCard) /// добавить карту в конец колоды (руки или колода)
{
    cards.push_back(pCard);
}
 
void Hand::clear() /// проитерировав вектор очистить всю память
{
    vector<Card*>::iterator iter = cards.begin(); /// итератор, которым будем перебирать и удалять
    for (iter = cards.begin(); iter != cards.end(); ++iter) /// пока есть карты
	{
        delete *iter; /// удалить обьект из вектора
            *iter = nullptr; /// прирсвоить 0 или nullptr (MAKE_CXXFLAGS += -std=c++11)
    	}
        /// очистить вектор указателей
    	cards.clear();
}

int Hand::getTotal() const /// вернуть сумму значений в векторе карт, с коррекцией туза и переворотом первой карты
{
        /// если карт нет то, return 0
    	if ( cards.empty())
        	return 0;

        /// Если первая карта перевернута, то карты лицом вниз, return 0 (не показывать сумму для дома в начале игры)
       if (!cards[0]->getisFaceUp()){
            return 0;}

        /// посчитать значение карт, тузы будут как 1
    	int total = 0;
        vector<Card*>::const_iterator iter; /// итератор
        for (iter = cards.begin(); iter != cards.end(); ++iter) /// пока есть карты
            total += (*iter)->getValue(); /// глянуть номинал каждой карты и записать в total
                  
        /// если hand содержит тузы
        bool containsAce = false; /// по умолчанию фолс
        for (iter = cards.begin(); iter != cards.end(); ++iter) /// пока есть карты
            if ((*iter)->getValue() == Card::ACE) /// если посмотрев значение карты она равна Тузу Card::Rank ACE
            containsAce = true; /// руки содержат туз тру

        /// если руки содержат туз и общее число карт меньше или равно 10
        if (containsAce && total <= 10)
        /// добавить к очкам 10 - значение туза, т.к он у нас блин равен 0
            total += 11;
        else if (containsAce)
            /// или прибавлю еденицу, по неизвестному правилу туза (если карт много то туз можно считать за 1, иначе как 11)
            total+=1;
            
        return total; /// вернуть общю сумму
}

// ### A B S T R A C T   P L A Y E R ### //
AbstractPlayer::AbstractPlayer(const string& s): name(s) // конструктр абстракного класса игрок
{ }

AbstractPlayer::~AbstractPlayer()  // десруктор
{ }

string AbstractPlayer::getName() const // вернуть имя
{ 
    	return name;
}

QString& AbstractPlayer::getLabelQStr()  const /// ВЕРНУТЬ getLabelQStr
{ 
    	return labelQStr;
}

bool AbstractPlayer::isBusted() const /// проверка, сдулся если набрал больше 21
{ 
        return (getTotal() > 21);
}

QString AbstractPlayer::bust() const /// сообщение ты продул bust(банкрот)
{
    	string str = "";
    	ostringstream oss ;
    	oss << "";
        oss << name << " проиграл! ";
    	str = oss.str();
    	return str.c_str();
}

// ### P L A Y E R ### //
int Player::hitCount = 0; /// обьявим для игрока hitCount = 0;

Player::Player(const string& name):  AbstractPlayer(name) // конструктор плеера от абстрактного
{ } 

Player::~Player() //  деструктор
{ }

int Player::getHitCount() /// вернуть число hitCount игрока
{
 	return hitCount;
}
   
void Player::resetHitCount()  /// сбросить число hitCount игрока
{
 	hitCount = 0;
}

void Player::setHitCount()  /// +1 hitCount игрока
{
 	hitCount++;
}

bool Player::isHitting() const /// вернуть число hitCount игрока (getHitCount();)
{
    	return getHitCount();
}

void Player::win(Canvas &cnvs) const /// сообщение ты Выиграл
{
    	string s = "";
    	s.append(name);
        s.append(" выиграл!");
        resultMessage(s);
        cnvs.setEnableHitStandButtons(false); /// деактивировать кнопки
}

void Player::lose(Canvas &cnvs) const /// сообщение ты продул
{
    	string s = "";
    	s.append(name);
        s.append(" проиграл!");
    	resultMessage(s);
        cnvs.setEnableHitStandButtons(false);
}

void Player::push(Canvas &cnvs) const /// Ничья
{
    	string s = "";
        s.append("Ничья!");
    	resultMessage(s);
	cnvs.setEnableHitStandButtons(false);
}

void Player::resultMessage(string& s) const // вывод результатов
{
    	QMessageBox msgBox;
        msgBox.setWindowTitle("Результаты ...");
        msgBox.setText(s.c_str());
    	msgBox.exec();
}

// ### H O U S E ### //
House::House(const string& name) :  AbstractPlayer(name) // оструктор хаоса :)) наследует от абстрактного игрока
{ }

House::~House()
{ }

bool House::isHitting() const /// если тотал >=16 вернуть тру
{
    bool b = (getTotal() <= 16);
	return b;
}   

void House::flipFirstCard(bool b) /// перевернуть первую карту
{
        if (!(cards.empty()))  { /// если вектор карт (экземпляр Hand) на руках не пуст
            if(b) cards[0]->flip(); /// перевернуть первую карту
    	}
}

// ### D E S K ### //
Deck::Deck() // колода конструктор
{ 
        cards.reserve(52); // забить 52 карты в резерв
        populate(); // заполним
}

Deck::~Deck()
{ }


void Deck::populate() /// заселить колоду:) 52 карты
{
        clear(); /// очистить память
        /// сделать колоду 52 карты
        for (int s = Card::SPADES; s <= Card::CLUBS; s++) /// перебор масти от пик до треф
            for (int r = Card::ACE; r <= Card::KING; r++) /// перебор номиналов
                if(r!=1) /// если rank == 1 то пипец, тузов вам не видать (ранк у нас туз =1, потом сразу 2,3,4,..)
                    add(new Card(static_cast<Card::Rank>(r), static_cast<Card::Suit>(s))); /// добавить в конец колоды карту
        /// преобразовав инты в масти и номиналы static_castтом
}

void Deck::shuffle() // перемешаем при помощи STL algoritm random_shuffle
{
        random_shuffle(cards.begin(), cards.end());
}

void Deck::deal(Hand& aHand) // сделка // взять карту с колоды в руки игроку
{
    // cards : наследуют Hand class
        if (!cards.empty()) { // если карты не пусты
            aHand.add(cards.back()); // создаем руки и заполняем //back() Возвращает ссылку на последним элементом вектора.
            // добавляем в руки одну последнюю карту с колоды
            cards.pop_back(); //  Удалить последнюю карту с колоды
    	}
    	else {
            cout << "Out of cards. Unable to deal."; // карт нету
    	}
}

QString Deck::additionalCards(AbstractPlayer& aAbstractPlayer) /// №№№ !!!!
{
    // Продолжают до тех пор, пока игрок не вылетел или хочет еще один ход
    if(aAbstractPlayer.isHitting() && !aAbstractPlayer.isBusted())
            deal(aAbstractPlayer); // дать всем по карте
        return aAbstractPlayer.makeLabelString();
} 

// ### G A M E ### //
Game::Game(const vector<string>& names, Canvas *cnv):canvas(cnv)
{
    	// create a vector of players from a vector of names       
    	vector<string>::const_iterator pName;
    	for (pName = names.begin(); pName != names.end(); ++pName)      
        	player.push_back(Player(*pName));

    	srand(time(0));    //seed the random number generator
    	deck.populate();
    	deck.shuffle();
}

Game::~Game()
{ }

void Game::gameStarted()
{
    QString qstr = " ";

    /// забрать у всех карты силой
    vector<Player>::iterator iterPlayer; /// итератор
    for (iterPlayer = player.begin(); iterPlayer != player.end(); ++iterPlayer) /// пока есть игроки
        iterPlayer->clear(); /// очистить вектор карт у игроков
        house.clear(); /// очистить все карты у банка

    Player::resetHitCount(); /// сбросить ходы

    canvas->clearIcons(); /// убрать карты, сделать невидимыми
    canvas->setResultLabel(qstr); /// заполнить результатирующюю строку статуса пока пуста
    /// по умолчанию true
    setStand(true);

    /// выдать по карты каждому
    for (int i = 0; i < 2; ++i) {
        for (iterPlayer = player.begin(); iterPlayer != player.end(); ++iterPlayer) { /// пока есть игроки
            deck.deal(*iterPlayer); /// всем игрокам
		}
            deck.deal(house);/// и дому

        }

    /// спрятать у банка первую карту
    if(!Player::getHitCount()) /// если игрок не ходил
        house.flipFirstCard(true);  /// переернуть первую карту дому

        /// показать всем руки
        for (iterPlayer = player.begin(); iterPlayer != player.end(); ++iterPlayer) {  /// пока есть игроки
            qstr = iterPlayer->makeLabelString(); /// вернуть строку состояния с текстовым представлением карт и суммой значений
            canvas->setPlayerStateLabel(qstr); /// отрисовать строку состояния для игрока
            iterPlayer->makePixmaps(*iterPlayer); /// зафигачить в вектор пиксмапсов, (создав пути к файлу) который содержит экземпляр hand игрока
            canvas->drawCards(*iterPlayer); /// определить чьи карты рисовать взять вектор QPixmap игрока и загнать в массив лейблов класса Canvas (QLabel *cardIconPlayer[MAXCARDS];)
        } /// тоже самое для дома
    	qstr = house.makeLabelString();
    	canvas->setHostStateLabel(qstr);
    	house.makePixmaps(house);
        canvas->drawCards(house);
}

void Game::hitSelected() /// ход
{         

    QString qstr; /// строка для разных целей
    vector<Player>::iterator iterPlayer; /// создаем итератор вектора игроков класса игра
        /// Показать первую карту дома, которая была скрыта
        if(!Player::getHitCount()) /// если текущий игрок не ходил
            house.flipFirstCard(true); /// показать первую карту дома

        /// дать по дополнительной карте игрокам
        for (iterPlayer = player.begin(); iterPlayer != player.end(); ++iterPlayer) { /// пока есть игроки
            iterPlayer->setHitCount(); /// прибавить всем игрокам по ходу так как кнопка ход нажата
            qstr = deck.additionalCards(*iterPlayer); /// дать по дополнительной карте игрокам, сформировав и вернув строку состояния qstr

            canvas->setPlayerStateLabel(qstr); /// ( взять строку qsrt  и установить текст виджета playerState )
            iterPlayer->makePixmaps(*iterPlayer); /// зафигачить в вектор пиксмапсов, (создав пути к файлу) который содержит экземпляр hand игрока
            canvas->drawCards(*iterPlayer); /// отрисовать карты
        }

	
        /// дать карту дому
    	qstr = deck.additionalCards(house);
        // перерисовка
        house.makeLabelString();
        canvas->setHostStateLabel(qstr);
    	house.makePixmaps(house);
        canvas->drawCards(house);

        /// если хата сдулась
        if(house.isBusted()) { /// проверить если карты дома больше 21
                qstr =  house.bust(); /// записать в строку имя и сообщение что продул
                canvas->setResultLabel(qstr); /// сообщение о результатах result->setText
            for (iterPlayer = player.begin(); iterPlayer != player.end(); ++iterPlayer) { /// пока есть игроки
                if (!iterPlayer->isBusted()) { /// если игрок не продул
                iterPlayer-> win(*canvas); /// то естесно побидил
                }
            /// или игрок продул
			else {
                iterPlayer->lose(*canvas); /// сообщение в resultMessege продул
            }
    		}
        }
        else { /// если хата не продула
            for (iterPlayer = player.begin(); iterPlayer != player.end(); ++iterPlayer) {/// пока есть игроки

                /// если игрок не продул
                if (!(iterPlayer->isBusted())) {

                    if(iterPlayer->isHitting()) /// если игрок ходил
                        break; /// выйти
                    /// игрок жмет стоп
                    else {
                        if(house.isHitting()) { /// если у дома в сумме >=16 дом берет карту
                            qstr = deck.additionalCards(house); /// дать дополнительных карт дому
                            house.makeLabelString(); /// сделать строку карт
                            house.makePixmaps(house); /// сделать массив карт
                            canvas->drawCards(house); /// отрисовать их
                        }
                    }

                    /// тут решаеться судьба
                    if(iterPlayer->getTotal() > house.getTotal()) { /// если у игрока БОЛЬШЕ чем у дома
                        iterPlayer->win(*canvas); /// то игрок выиграл
                    }
                    else if(iterPlayer->getTotal() < house.getTotal()) /// если у игрока МЕНЬШЕ чем у дома
                        iterPlayer->lose(*canvas); /// игрок продул
                    else
                        iterPlayer->push(*canvas); /// или пуш ничья
                }

                /// игрок продул
                else {
                    iterPlayer->lose(*canvas); /// диалоговое окно продул
                    qstr = iterPlayer->bust(); /// строка продул
                    canvas->setResultLabel(qstr); /// вывести строку в строку состояния продул
                }
            }
        }
}

/// Игрок решили стоп, дом ходит если в сумме <= 16 или пропускает ход
void Game::standSelected()
{
    vector<Player>::iterator iterPlayer; /// итератор игроков
    QString qstr;

    if(Player::getHitCount()){ /// если игрок ходил
        house.flipFirstCard(false);    /// дом прячет первую карту
        Player::setHitCount(); /// игроку ход +1
    }
    else { /// или если игрок не ходил, дом вскрывает карты
        house.flipFirstCard(true);
        Player::setHitCount(); /// игроку ход +1
	}

    /// дать карт дому
    qstr = deck.additionalCards(house);
    /// перерисовать карты дома
    //house.makeLabelString(); ### STR 438 ###
    canvas->setHostStateLabel(qstr);
    house.makePixmaps(house);
	canvas->drawCards(house);

    /// если дом продул
    if(house.isBusted()) {
        qstr = house.bust();
        canvas->setResultLabel(qstr); /// вывести статус в строке состояния продул

        for (iterPlayer = player.begin(); iterPlayer != player.end(); ++iterPlayer) { /// пока есть игроки
            if (!iterPlayer->isBusted()) { /// если они не продули
                iterPlayer-> win(*canvas); /// то они победили
            }
        }
    } /// если дом НЕ продул
    else {
        for (iterPlayer = player.begin(); iterPlayer != player.end(); ++iterPlayer) { /// пока есть игроки
            if(!house.isHitting()) {  /// если у дома в сумме >=16
                /// некая эмуляция поведения дома чтобы он мог воздержаться от хода если у него много карт
                if(iterPlayer-> getTotal() > house.getTotal()) /// если у игрока больше
                    iterPlayer->win(*canvas); /// он крут
                else if(iterPlayer->getTotal() < house.getTotal()) /// если у игрока меньше
                    iterPlayer->lose(*canvas); /// продул
                else
                    iterPlayer->push(*canvas); /// ничья
            }
		}
	}
}

void Game::setStand(bool b) /// установить стоячку
{         
	stand = b;
}

bool Game::getStand() // узнать кто стоит
{         
	return stand;
}

// ### E L S E ### //

/// Перегрузка << оператора так объект карты будет быть отправлен на cout
ostream& operator<<(ostream& os, const Card& aCard)
{
    const string RANKS[] = {"Туз", ":)","2", "3", "4", "5", "6", "7", "8", "9",
                            "10", "Валет", "Дама", "Король"};
    const string SUITS[] = {"пики", "черви", "бубны", "крести"};

    if (aCard.isFaceUp) /// если карты лицом вверх
        os << RANKS[aCard.rank] <<" "<< SUITS[aCard.suit] <<","; /// взять из списка сопоставление в строку
    else
        os << ":)"; /// или пусто, карта перевернута
    return os;
}

// Перегрузка << оператора так объект AbstractPlayer может быть отправлен на cout // нужно для makeLabelString вывод карт на руках
ostream& operator<<(ostream& os, const AbstractPlayer& aAbstractPlayer)
{
    	string str = "";
    	ostringstream oss;
    	oss << "" ;
        //	os << aAbstractPlayer.name << ":   ";
    	oss << aAbstractPlayer.name << ":   ";
    
    	vector<Card*>::const_iterator pCard;
        if (!aAbstractPlayer.cards.empty()) { // если у игрока есть карты
            for (pCard = aAbstractPlayer.cards.begin();pCard != aAbstractPlayer.cards.end(); ++pCard) { // пока есть карты
                //карта
            	oss << *(*pCard) << "   ";
        	}
            if (aAbstractPlayer.getTotal() != 0) { // если тотал не 0
                    oss << "(" << aAbstractPlayer.getTotal() << ")"; // то вывести сумму
        	}
    	}
    	else {
 //       	os << "<empty>";
        	oss << "<empty>";
    	}
        
    	str = oss.str();
    	aAbstractPlayer.labelQStr = str.c_str();
    	return os;
}

/*void AbstractPlayer::makePixmaps(AbstractPlayer &ap) // виртуальная ф-я которая переопределяеться наследниками нафик ненуна
{ }*/

vector<QPixmap>& Player::getPixmaps()  /// вернуть по ссылке вектор обьектов QPixmap
{
	return pixmaps;
}

vector<QPixmap>& House::getPixmaps() /// вернуть по ссылке вектор обьектов QPixmap
{
	return pixmaps;
}

void Player::makePixmaps(AbstractPlayer &ab) /// зафигачить в вектор пиксмапсов, (создав пути к файлу) который содержит экземпляр hand игрока
{
        string str; /// временная переменная
        vector<Card*>::const_iterator pCard; /// создадим итератор вектора Card наследованного абстрактным игроком у дома
        if (!cards.empty()) { /// если карты на руках не пусты
		
            Player *ptr = dynamic_cast<Player *> (&ab); /// преобразование типов из абстрактного в просто игрока

            ptr->pixmaps.clear(); /// очистить вектор карт
            for (pCard = cards.begin(); pCard != cards.end(); ++pCard) { /// пока есть карты
            str = (*pCard)->convert(); /// сделать путь к картинке карты
            pixmaps.push_back(QPixmap(str.c_str())); /// зафигачить в вектор пиксмапсов который содержит экземпляр hand игрока
			
        	}
    	}
}

void House::makePixmaps(AbstractPlayer &ab) /// зафигачить в вектор пиксмапсов, (создав пути к файлу) который содержит экземпляр hand игрока
{
       	string str;
        vector<Card*>::const_iterator pCard; /// итератор
        if (!cards.empty()) { /// если карты не пусты
		
                House *ptr = dynamic_cast<House *> (&ab); /// из абстрактного игрока в house

            ptr->pixmaps.clear(); /// очистить вектор обьектов пиксмапсов
            for (pCard = cards.begin();pCard != cards.end(); ++pCard) { /// пока есть карты
            str = (*pCard)->convert(); /// создать для каждой строку к файлу
            pixmaps.push_back(QPixmap(str.c_str())); /// захерачить в вертор обьектов пиксмапс
			
        	}
    	}
}

QString AbstractPlayer::makeLabelString() /// вернуть строку состояния с текстовым представлением карт и суммой значений
{
        string str = ""; /// строка которую вернем
        ostringstream oss; /// преобразование типов в строку
        oss << "" ;
        oss << name << ":   "; /// имя игрока +
    
        vector<Card*>::const_iterator pCard; /// итератор вектора экземпляра карт
        if (!cards.empty()) { /// если карты не пусты
            for (pCard = cards.begin(); pCard != cards.end(); ++pCard) { /// пока есть карты
                    oss << *(*pCard) << "   "; /// в строку их
        	}
                if (getTotal() != 0) { /// если сумма карт не равна 0
                    oss << "(" << getTotal() << ")"; /// добавить в строку сумму карт
        	}
    	}
    	else {
            oss << "<empty>"; /// или "<empty>"
    	}
        
    	str = oss.str();
        return str.c_str(); /// вернуть чаровую строку
}
