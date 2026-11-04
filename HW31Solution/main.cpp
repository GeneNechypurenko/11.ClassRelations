#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

/*
==========================================================================================================================================
                            ДОМАШНЄ ЗАВДАННЯ 31 тема: вкладені класи, композиція та агрегація.
==========================================================================================================================================
*/

// ==================== РІВЕНЬ 1. ====================

class Song
{
public:
    class Duration
    {
    private:
        int minutes;
        int seconds;

    public:
        Duration(int minutes, int seconds) : minutes(minutes), seconds(seconds)
        {
            if (this->minutes < 0)
            {
                this->minutes = 0;
            }
            if (this->seconds < 0)
            {
                this->seconds = 0;
            }

            this->minutes += this->seconds / 60;
            this->seconds %= 60;
        }

        int getTotalSeconds() const
        {
            return minutes * 60 + seconds;
        }

        Duration operator+(const Duration& other) const
        {
            return Duration(0, getTotalSeconds() + other.getTotalSeconds());
        }

        void print() const
        {
            cout << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
        }
    };

private:
    string title;
    string artist;
    //  4. Song і Duration — це композиція чи агрегація? А Playlist і Song? Поясніть, за якою ознакою ви це визначили.
    Duration duration;

public:
    /*
     1. Чому поле duration у Song треба ініціалізувати саме у списку ініціалізації? Що буде, якщо замість цього написати
     duration = Duration(minutes, seconds); у тілі конструктора?
     2. У якому порядку викликаються конструктори Song і Duration під час створення пісні? А деструктори під час знищення?
     */
    Song(const string& title, const string& artist, int minutes, int seconds)
        : title(title), artist(artist), duration(minutes, seconds)
    {}

    Duration getDuration() const
    {
        return duration;
    }

    void print() const
    {
        cout << artist << " — " << title << " (";
        duration.print();
        cout << ")";
    }
};

// ==================== РІВЕНЬ 2. ====================

class Playlist
{
private:
    string name;
    //  4. Song і Duration — це композиція чи агрегація? А Playlist і Song? Поясніть, за якою ознакою ви це визначили.
    Song** songs;
    int count;

public:
    Playlist(const string& name) : name(name), songs(nullptr), count(0)
    {}

    /*
        6. (Рівень 2) Навіщо заборонене копіювання плейлиста? Що станеться в кінці програми, якщо його дозволити
                      і написати Playlist copy = morning; ?
    */
    Playlist(const Playlist&) = delete;
    Playlist& operator=(const Playlist&) = delete;

    /*
        5. (Рівень 2) Чому деструктор Playlist робить delete[] songs, але НЕ робить delete для кожного songs[i]?
                      Що станеться, якщо додати такий delete?
    */
    ~Playlist()
    {
        delete[] songs;
    }

    bool add(Song* song)
    {
        if (song == nullptr)
        {
            return false;
        }
        for (int i = 0; i < count; i++)
        {
            if (songs[i] == song)
            {
                return false;
            }
        }

        Song** newSongs = new Song * [count + 1];
        for (int i = 0; i < count; i++)
        {
            newSongs[i] = songs[i];
        }
        newSongs[count] = song;

        delete[] songs;
        songs = newSongs;
        count++;
        return true;
    }

    Song::Duration totalDuration() const
    {
        Song::Duration total(0, 0);
        for (int i = 0; i < count; i++)
        {
            total = total + songs[i]->getDuration();
        }
        return total;
    }

    void show() const
    {
        cout << "Плейлист «" << name << "», пісень: " << count << endl;
        for (int i = 0; i < count; i++)
        {
            cout << "  ";
            songs[i]->print();
            cout << endl;
        }
        cout << "  Загальна тривалість: ";
        totalDuration().print();
        cout << endl;
    }

    // ==================== РІВЕНЬ 3 ====================
    //  7. (Рівень 3) remove прибирає пісню з плейлиста, але не видаляє її. Чому саме так?
    bool remove(Song* song)
    {
        for (int i = 0; i < count; i++)
        {
            if (songs[i] == song)
            {
                for (int j = i; j < count - 1; j++)
                {
                    songs[j] = songs[j + 1];
                }
                count--;
                return true;
            }
        }
        return false;
    }
};

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    cout << "===== РІВЕНЬ 1 =====" << endl;
    Song coffee("Ранкова кава", "Гурт Світанок", 3, 7);
    Song city("Нічне місто", "Ліна Вітер", 0, 250);   // 250 секунд → 4:10

    coffee.print();
    cout << endl;
    city.print();
    cout << endl;

    
    //  3. Чому в main ви пишете Song::Duration, а не просто Duration? Що зміниться, якщо оголосити Duration у private-секції класу Song?
    Song::Duration total = coffee.getDuration() + city.getDuration();
    cout << "Разом: ";
    total.print();
    cout << endl;

    cout << endl << "===== РІВЕНЬ 2 =====" << endl;
    Song rain("Біг під дощем", "Пульс", 3, 45);
    Song field("Вітер у полі", "Гурт Світанок", 2, 58);
    Song evening("Повільний вечір", "Тиша", 6, 15);

    Playlist morning("Ранкова пробіжка");
    morning.add(&coffee);
    morning.add(&rain);
    morning.add(&field);

    Playlist chill("Вечірній чіл");
    chill.add(&city);
    chill.add(&evening);
    chill.add(&field);                    // та сама пісня — в обох плейлистах

    if (!morning.add(&coffee))
    {
        cout << "«Ранкова кава» вже є в плейлисті" << endl;
    }
    if (!chill.add(nullptr))
    {
        cout << "Порожню пісню не додано" << endl;
    }

    cout << endl;
    morning.show();
    chill.show();

    cout << endl << "===== РІВЕНЬ 3 =====" << endl;
    morning.remove(&field);
    cout << "«Вітер у полі» прибрано з ранкового плейлиста" << endl << endl;

    morning.show();
    chill.show();                         // у вечірньому пісня залишилась

    return 0;
}

/*
  1. Чому поле duration у Song треба ініціалізувати саме у списку ініціалізації? Що буде, якщо замість цього
     написати duration = Duration(minutes, seconds); у тілі конструктора?
  2. У якому порядку викликаються конструктори Song і Duration під час створення пісні? А деструктори під час знищення?
  3. Чому в main ви пишете Song::Duration, а не просто Duration? Що зміниться, якщо оголосити Duration
     у private-секції класу Song?
  4. Song і Duration — це композиція чи агрегація? А Playlist і Song? Поясніть, за якою ознакою ви це визначили.
  5. (Рівень 2) Чому деструктор Playlist робить delete[] songs, але НЕ робить delete для кожного songs[i]?
     Що станеться, якщо додати такий delete?
  6. (Рівень 2) Навіщо заборонене копіювання плейлиста? Що станеться в кінці програми, якщо його дозволити
     і написати Playlist copy = morning; ?
  7. (Рівень 3) remove прибирає пісню з плейлиста, але не видаляє її. Чому саме так?

  ВІДПОВІДІ НА ЧАСТИНУ «ПОЯСНИ» (еталон для розбору)

  1. 

  2. 

  3. 

  4. 

  5. 

  6. 

  7. 
*/
