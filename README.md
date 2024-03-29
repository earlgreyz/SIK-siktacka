# Sieci Komputerowe
## Zadanie 2 - duże

Zadanie polega na napisaniu nieco uproszczonego klona pewnej kultowej
gry dla wielu graczy, zwanej **Netacka** (https://pwmarcz.pl/netacka/).
Należy zapoznać się z oryginałem, żeby nabrać rozeznania, jak się w nią gra.

Zadanie składa się z dwóch części.

## Część A

Zaimplementować **serwer gry**, który będzie umożliwiał podłączenie się
klientom (graczom), zarządzał stanem gry, odbierał informacje o wykonywanych
ruchach oraz rozsyłał klientom zmiany stanu gry.

Serwer pamięta wszystkie zdarzenia dla bieżącej partii i przesyła je
w razie potrzeby graczom. Rodzaje zdarzeń opisane są w specyfikacji protokołu
komunikacyjnego.

## Część B

Zaimplementować klienta gry, który będzie komunikował się z dwiema aplikacjami:

* z serwerem gry;
*  z graficznym interfejsem użytkownika (GUI), dostarczonym wraz z niniejszą
   treścią zadania.

Sposób komunikacji z *GUI* gry opisany jest w dalszej części niniejszego tekstu.

Klient dba także o to, żeby aplikacja GUI otrzymywała polecenia **w kolejności
zgodnej z przebiegiem partii oraz bez duplikatów**.

Wybraliśmy taką architekturę, żeby nie wymagać w samym kliencie obsługi
interfejsu graficznego.


### Parametry wywołania programów

#### Serwer:

```sh
./siktacka-server [-W n] [-H n] [-p n] [-s n] [-t n] [-r n]
```

* -W n – szerokość planszy w pikselach (domyślnie `800`)
* -H n – wysokość planszy w pikselach (domyślnie `600`)
* -p n – numer portu (domyślnie `12345`)
* -s n – liczba całkowita wyznaczająca szybkość gry (parametr
 `ROUNDS_PER_SEC` w opisie protokołu, domyślnie `50`)
* -t n – liczba całkowita wyznaczająca szybkość skrętu (parametr 
 `TURNING_SPEED`, domyślnie `6`)
* -r n – ziarno generatora liczb losowych (opisanego poniżej)

*Żeby nie męczyć się z parsowaniem parametrów linii komend, można użyć np.
funkcji getopt z biblioteki standardowej (https://linux.die.net/man/3/getopt).*

#### Klient:

```sh
./siktacka-client player_name game_server_host[:port] [ui_server_host[:port]]
```

**Wartości domyślne:**

* port serwera gry – `12345`
* nazwa serwera interfejsu użytkownika – `localhost`
* port serwera interfejsu użytkownika – `12346`

Parametry game_server_host oraz `ui_server_host` mogą być adresami `IPv4`, `IPv6`
lub nazwami węzłów.

Parametr `player_name` musi być zgodny z wymaganiami protokołu komunikacyjnego.

## Protokół komunikacyjny klient-serwer

Do komunikacji między klientami oraz serwerem wykorzystywany jest protokół
opisany poniżej.

1. **Wymiana danych**

 Wymiana danych odbywa się po UDP.

2. **Format datagramów**
 
 W datagramach przesyłane są dane binarne, zgodne z poniżej zdefiniowanym
 formatem komunikatów.

3. **Porządek bajtów**
 W komunikatach wszystkie liczby przesyłane są w sieciowej kolejności bajtów
 *(big-endian)*.

4. **Komunikaty od klienta do serwera**

 Komunikat od klienta do serwera ma kolejno następujące pola:
 * `session_id` (8 bajtów, liczba bez znaku)
 * `turn_direction` (1 bajt, liczba ze znakiem: −1 → lewo, +1 → prawo, 0 → prosto)
 * `next_expected_event_no` (4 bajty, liczba bez znaku)
 * `player_name` (0–64 znaków ASCII o wartościach z przedziału 33–126; w szczególności spacje nie są dozwolone)
 
 Klient wysyła taki datagram **co 20 ms**.
 
 Komunikacja odbywa się zawsze, nawet jeśli partia się jeszcze nie rozpoczęła lub już się zakończyła.

 Pole `turn_direction` wskazuje, czy gracz chce skręcać (czy ma wciśniętą
 którąś ze strzałek w lewo lub prawo na klawiaturze).

 Puste pole `player_name` oznacza, że klient nie ma zamiaru włączać się do
 gry, jednakże chętnie poobserwuje, co się dzieje na planszy.

 Pole `session_id` jest takie samo dla wszystkich datagramów wysłanych przez
 danego klienta. Klient przy uruchomieniu ustala `session_id` na bieżący czas
 wyrażony w mikrosekundach od *1970-01-01 00:00:00 +0000 (UTC)*.

5. **Komunikaty od serwera do klienta**

 Komunikat od serwera do klienta ma kolejno następujące pola:
 * `game_id` (4 bajty, liczba bez znaku)
 * `events` (zmienna liczba rekordów, zgodnych z poniższą specyfikacją)

 Serwer wysyła taki komunikat natychmiast po odebraniu komunikatu od klienta.
 Wysyła zdarzenia o numerach, począwszy od odebranego `next_expeced_event_no`
 aż do ostatniego dostępnego. Jeśli takich zdarzeń nie ma, serwer nic nie
 wysyła w odpowiedzi.

 Serwer także wysyła taki komunikat do wszystkich klientów, gdy pojawia się
 nowe zdarzenie.

 **Maksymalny rozmiar pola danych** datagramu UDP wysyłanego przez serwer
 wynosi **512 bajtów**.

 Jeśli serwer potrzebuje wysłać więcej zdarzeń, niż może zmieścić w jednym
 datagramie UDP, wysyła je w kolejnych komunikatach. W tym przypadku wszystkie
 oprócz ostatniego muszą zawierać maksymalną możliwą liczbę zdarzeń.

 Pole `game_id` służy do identyfikacji bieżącej partii w sytuacji, gdy do
 klienta mogą dochodzić opóźnione datagramy z uprzednio zakończonej partii.

6. **Rekordy opisujące zdarzenia**

 Rekord opisujący zdarzenie ma następujący format:
 * `len` (4 bajty, liczba bez znaku, sumaryczna długość pól event_*)
 * `event_no` (4 bajty, liczba bez znaku, dla każdej partii kolejne wartości,
  począwszy od zera)
 * `event_type` (1 bajt)
 * `event_data` (zależy od typu, patrz opis poniżej)
 * `crc32` (4 bajty, liczba bez znaku, suma kontrolna obejmująca
  rekord od pola len do pola event_data włącznie, obliczona standardowym algorytmem 
  CRC-32-IEEE)

 **Możliwe rodzaje zdarzeń:**
 * `NEW_GAME`
   * `event_type`: 0
   * `event_data`:
     * `maxx` (4 bajty, szerokość planszy w pikselach, liczba bez znaku)
     * `maxy` (4 bajty, wysokość planszy w pikselach, liczba bez znaku)
     * Następnie lista nazw graczy zawierająca dla każdego z graczy:
       * `player_name` (jak w punkcie „4. Komunikaty od klienta do serwera”)
       * znak `'\0'`

 * `PIXEL`
   * `event_type`: 1
   * `event_data`:
     * `player_number` (1 bajt)
     * `x` (4 bajty, odcięta, liczba bez znaku)
     * `y` (4 bajty, rzędna, liczba bez znaku)

 * `PLAYER_ELIMINATED`
   * `event_type`: 2
   * `event_data`:
     * `player_number` (1 bajt)

 * `GAME_OVER`
   * `event_type`: 3
   * `event_data`: brak

7. **Generator liczb losowych**

   Na potrzeby losowania stanu początkowego, należy użyć poniższego
   deterministycznego generatora liczb losowych. Kolejne wartości zwracane
   przez ten generator wyrażone są wzorem:

   ```tex
   r_0 = seed
   r_i = (r_{i-1} * 279470273) mod 4294967291
   ```

   *(uwaga na konieczność użycia typów 64-bitowych)*, gdzie seed jest
   przekazywany do serwera za pomocą parametru -r *(domyślnie wartość zwracana
   przez time(NULL))*.

   Należy użyć dokładnie takiego generatora, żeby umożliwić automatyczne
   testowanie rozwiązania.

8. **Stan gry**
 Podczas partii serwer utrzymuje stan gry, w skład którego wchodzą m.in.:
 * numer partii (`game_id`), wysyłany w każdym wychodzącym datagramie,
 * bieżące współrzędne głowy węża każdego z graczy oraz kierunek jego ruchu
  (jako liczby zmiennoprzecinkowe o co najmniej podwójnej precyzji),
 * zdarzenia wygenerowane od początku gry (patrz punkt „6. Rekordy opisujące
  zdarzenia” oraz dalej),
 * zajęte piksele planszy.

 Warto tu podkreślić, że bieżąca pozycja graczy jest obliczana i przechowywana
 w formacie zmiennoprzecinkowym, podczas gdy w zdarzeniach oraz przy
 obliczaniu zderzeń stosuje się reprezentację na liczbach całkowitych.

 Lewy górny róg planszy ma współrzędne (0, 0), odcięte rosną w prawo,
 a rzędne w dół.

 W przypadku potrzeby konwersji pozycji zmiennoprzecinkowej na
 całkowitoliczbową, stosuje się zaokrąglanie w dół. Uznajemy, że pozycja
 po zaokrągleniu znajduje się na planszy, jeśli rzędna znajduje się
 w przedziale domkniętym `[0, maxy - 1]`, a odcięta w przedziale
 `[0, maxx - 1]`.

 Kierunek ruchu jest wyrażony w stopniach, zgodnie z ruchem wskazówek
 zegara, a 0 oznacza kierunek w prawo.

9. **Podłączanie i odłączanie graczy**

 Podłączenie nowego gracza może odbyć się **w dowolnym momencie**. Wystarczy,
 że serwer odbierze prawidłowy komunikat od nowego klienta. Jeśli nowy
 gracz podłączy się **podczas partii**, staje się jej **obserwatorem**, otrzymuje
 informacje o wszystkich zdarzeniach, które miały miejsce od początku
 partii. **Do walki dołącza w kolejnej partii.**

 Jeśli podłączy się gracz, który w komunikatach przesyła **puste pole**
 `player_name`, to taki gracz nie walczy, ale może obserwować rozgrywane
 partie.

 **Brak komunikacji** od gracza przez **2 sekundy** skutkuje jego **odłączeniem**.
 Jeśli gra się już rozpoczęła, **wąż** takiego gracza **nie znika** i nadal porusza
 się wg algorytmu z punktu *„11. Przebieg partii”*.

 **Klienty są identyfikowane** za pomocą par `(gniazdo, session_id)`, jednakże
 otrzymanie komunikatu z gniazda istniejącego klienta, aczkolwiek
 z większym niż dotychczasowe `session_id`, jest równoznaczne z **odłączeniem
 istniejącego klienta i podłączeniem nowego**. Komunikaty z mniejszym niż
 dotychczasowe `session_id` należy ignorować.

 **Pakiety otrzymane z nieznanego dotychczas gniazda, jednakże z nazwą
 podłączonego już klienta, są ignorowane.**

10. **Rozpoczęcie partii i zarządzanie podłączonymi klientami**
 Do **rozpoczęcia partii** potrzeba, aby wszyscy **podłączeni gracze**
 (o niepustej nazwie) **nacisnęli strzałkę** _(przysłali turn_direction != 0)_
 oraz żeby tych graczy było **co najmniej dwóch**.

 **Kolejność graczy** ustala się, ustawiając ich **w kolejności alfabetycznej** po
 nazwach. Ta kolejność jest następnie wykorzystana przy wypełnianiu pól
 zdarzenia `NEW_GAME` oraz wszędzie tam, gdzie pojawia się w pseudokodzie
 sformułowanie _„dla kolejnych graczy”_.

 Stan gry jest inicjowany w następujący sposób (kolejność wywołań `rand()` ma
 znaczenie i należy użyć generatora z punktu „_7. Generator liczb losowych_”):

 ```python
   game_id = rand()
   wygeneruj zdarzenie NEW_GAME
   dla kolejnych graczy:
     x_głowy_gracza = (rand() mod maxx) + 0.5
     y_głowy_gracza = (rand() mod maxy) + 0.5
     kierunek = rand() mod 360
     jeśli piksel głowy (po zaokrągleniu) jest już zajęty, to
       wygeneruj zdarzenie PLAYER_ELIMINATED
     w przeciwnym razie:
       wygeneruj zdarzenie PIXEL
 ```

 A zatem można odpaść już na starcie.

11. **Przebieg partii**
 Partia składa się z tur. Tura trwa `1/ROUNDS_PER_SEC` sekundy.
 ```python
   dla kolejnych graczy:
     jeśli ostatni turn_direction = −1, to:
       kierunek_w_stopniach_gdzie_0_jest_do_gory −= TURNING_SPEED
     jeśli ostatni turn_direction = +1, to:
       kierunek_w_stopniach_gdzie_0_jest_do_gory += TURNING_SPEED
     przesuń głowę gracza o 1 w bieżącym kierunku
     jeśli przesuniecie nie zmieniło piksela po zaokrągleniu do wartości \
         całkowitych, to:
       continue
     jeśli nowy piksel (po zaokrągleniu) jest już zajęty \
         lub wypada poza planszą, to:
       wygeneruj zdarzenie PLAYER_ELIMINATED
     w przeciwnym razie:
       wygeneruj zdarzenie PIXEL
 ```
 Te zasady gry różnią się nieco od oryginalnej Netacki, w szczególności:
 * Węże nie mają dziur, przez które można przechodzić.
 * Węże są dość cienkie i przy odpowiedniej dozie szczęścia można
  skorzystać z „efektu tunelowego”, jeśli kolejne piksele akurat nie
  pokryją się z już zajętymi, np. w taki sposób:
  ```
  .1...
  ..122
  2221.
  ....1
  ```

12. **Zakończenie partii**
 W momencie, gdy na planszy zostanie tylko jeden gracz, gra się kończy.
 Generowane jest zdarzenie `GAME_OVER`.

 Serwer wciąż obsługuje komunikację z klientami w kontekście zakończonej
 gry.

 Jeśli po zakończeniu partii serwer otrzyma od każdego podłączonego klienta
 (o niepustej nazwie) co najmniej jeden komunikat z `turn_direction != 0`,
 rozpoczyna kolejną partię.

 *Klienty muszą radzić sobie z sytuacją, kiedy po rozpoczęciu nowej gry
 będą dostawać jeszcze stare, opóźnione datagramy z poprzedniej gry.*


## Protokół komunikacyjny klient-GUI
Do komunikacji między klientem oraz aplikacją graficznego interfejsu gracza
wykorzystywany jest protokół opisany poniżej.

1. **Wymiana danych**
 Wymiana danych odbywa się po TCP.

2. **Format komunikatów**
 Komunikaty przesyłane są w formie tekstowej, każdy w osobnej linii. Liczby
 są reprezentowane dziesiętnie. Linia zakończona jest znakiem o kodzie ASCII
 `10`.

 Jeśli linia składa się z kilku wartości, to wysyłający powinien te wartości
 oddzielić pojedynczą spacją i nie dołączać dodatkowych białych znaków
 na początku ani na końcu.

3. **Odbiór komunikatów**
 Odbiorca niepoprawnego komunikatu ignoruje go.
 Odbiór komunikatu nie jest potwierdzany żadną wiadomością zwrotną.

4. **Komunikaty od klienta do GUI**
 Aplikacja graficznego interfejsu użytkownika akceptuje następujące
 komunikaty:
 * `NEW_GAME maxx maxy player_name1 player_name2 …`
 * `PIXEL x y player_name`
 * `PLAYER_ELIMINATED player_name`

 Nazwa gracza to ciąg 1–64 znaków ASCII o wartościach z przedziału 33–126.
 Współrzędne x, y to liczby całkowite, odpowiednio od 0 do maxx − 1 lub maxy − 1.
 Lewy górny róg planszy ma współrzędne (0, 0), odcięte rosną w prawo, a rzędne w dół.

5. **Komunikaty od GUI do klienta**
 Aplikacja graficznego interfejsu użytkownika wysyła następujące komunikaty:
 * `LEFT_KEY_DOWN`
 * `LEFT_KEY_UP`
 * `RIGHT_KEY_DOWN`
 * `RIGHT_KEY_UP`


## Ustalenia dodatkowe

1. Programy powinny umożliwiać komunikację zarówno przy użyciu **IPv4**, jak i **IPv6**.

2. W implementacji programów duże kolejki komunikatów, zdarzeń itp. powinny być
   alokowane dynamicznie.

3. Przy parsowaniu ciągów zdarzeń z datagramów przez klienta:
 * pierwsze zdarzenie z **niepoprawną sumą kontrolną** powoduje zaprzestanie
  przetwarzania kolejnych w tym datagramie, ale poprzednie pozostają w mocy;
 * rekord z **poprawną sumą kontrolną**, znanego typu, jednakże z **bezsensownymi
  wartościami** powoduje zakończenie klienta z odpowiednim komunikatem i kodem
  wyjścia 1;
 * pomija się zdarzenia z poprawną sumą kontrolną oraz nieznanym typem.

4. Program klienta w przypadku błędu połączenia z serwerem gry lub aplikacją GUI
   powinien się zakończyć z kodem wyjścia 1, uprzednio wypisawszy zrozumiały
   komunikat na standardowe wyjście błędów.

   Program serwera powinien być odporny na sytuacje błędne, które
   dają szansę na kontynuowanie działania. Intencja jest taka, że serwer
   powinien móc być uruchomiony na stałe bez konieczności jego restartowania,
   np. w przypadku kłopotów komunikacyjnych, czasowej niedostępności sieci,
   zwykłych zmian jej konfiguracji itp.

5. Serwer nie musi obsługiwać więcej niż 42 podłączonych graczy jednocześnie.
   Dodatkowi gracze ponad limit nie mogą jednak przeszkadzać wcześniej
   podłączonym.

6. Programy powinny być napisane zrozumiale. Tu można znaleźć wartościowe
   wskazówki w tej kwestii:

   http://www.maultech.com/chrislott/resources/cstyle/LinuxKernelCodingStyle.txt

7. W serwerze opóźnienia w komunikacji z podzbiorem klientów nie mogą
   wpływać na jakość komunikacji z pozostałymi klientami.

   Analogicznie w kliencie opóźnienia w komunikacji z GUI nie mogą wpływać
   na regularność wysyłania komunikatów do serwera gry.

   Patrz też:
   https://stackoverflow.com/questions/4165174/when-does-a-udp-sendto-block

8. Czynności okresowe (tury oraz wysyłanie komunikatów od klienta do serwera)
   powinny być wykonywane w odstępach niezależnych od czasu przetwarzania
   danych. Implementacja, która np. robi sleep(20ms) nie spełnia tego warunku.
   Dopuszczalne są krótkofalowe odchyłki, ale długofalowo średni odstęp
   czynności musi być zgodny ze specyfikacją.

9. Polecam stosować zasadę niezawodności Postela:

   http://old.rufuspollock.org/2007/02/22/the-robustness-principle

10. Na połączeniu klienta z aplikacją interfejsu użytkownika powinien zostać
    wyłączony algorytm Nagle'a w celu zminimalizowania opóźnień transmisji.

11. Patrz też:
    http://man7.org/linux/man-pages/man2/gettimeofday.2.html
    https://stackoverflow.com/questions/809902/64-bit-ntohl-in-c

12. Przy przetwarzaniu sieciowych danych binarnych należy używać typów
    o ustalonej szerokości:

    http://en.cppreference.com/w/c/types/integer

13. W przypadku otrzymania niepoprawnych argumentów linii komend, programy
    powinny wypisywać stosowny komunikat na stdandardowe wyjście błędów
    i zwracać kod 1.


##  Oddawanie rozwiązania

Można oddać rozwiązanie tylko części A lub tylko części B, albo obu części.

Rozwiązanie ma:
– działać w środowisku Linux;
– być napisane w języku C lub C++ z wykorzystaniem interfejsu gniazd (nie wolno
  korzystać z libevent ani boost::asio);
– kompilować się za pomocą GCC (polecenie gcc lub g++) – wśród parametrów należy
  użyć -Wall i -O2, można korzystać ze standardów -std=c11 i -std=c++14.

Jako rozwiązanie należy dostarczyć pliki źródłowe oraz plik makefile, które
należy umieścić na studentsie w katalogu

/home/students/inf/PUBLIC/SIK/students/ab123456/zadanie2/

gdzie ab123456 to standardowy login osoby oddającej rozwiązanie, używany
na maszynach wydziału, wg schematu: inicjały, nr indeksu.
Nie wolno umieszczać tam plików binarnych ani pośrednich powstających podczas
kompilacji.

W wyniku wykonania polecenia make dla części A zadania ma powstać plik
wykonywalny siktacka-server, a dla części B zadania – plik wykonywalny
siktacka-client.
Ponadto makefile powinien obsługiwać cel 'clean', który po wywołaniu kasuje
wszystkie pliki powstałe podczas kompilacji.


## Ocena

Za rozwiązanie części A zadania można dostać maksymalnie 3 punkty.
Za rozwiązanie części B zadania można dostać maksymalnie 2 punkty.
Za rozwiązanie obu części zadania można dostać maksymalnie 6 punktów.

Jeśli student odda obie części zadania, to będą one ocenione osobno.
Jeśli obie części współdziałają ze sobą i każda z nich wykazuje działanie
zgodne ze specyfikacją, ocena końcowa będzie sumą ocen za poszczególne części
pomnożoną przez 1,2.

Ocena każdej z części zadania będzie się składała z trzech składników:
– ocena wzrokowa i manualna działania programu (30%);
– testy automatyczne (50%);
– jakość tekstu źródłowego (20%).

Termin: czwartek 8 czerwca 2017, godzina 19.00
        (liczy się czas na serwerze students)

Za spóźnienie do 24 godz. każda rozpoczęta godzina jest warta 0,04 p.
Za spóźnienie powyżej 24 godz., ale do 7 dni przed egzaminem –
0,96 p. + 0,02 p. za każdą rozpoczętą godzinę ponad 24 godz.
Rozwiązanie z późniejszą datą można oddać tylko w II terminie.
Punkty za spóźnienia dą odejmowane od końcowego wyniku, jednakże
W I terminie za spóźnienia nie odejmuje się więcej niż 2 p.

Rozwiązanie dostarczone w I terminie można poprawić jednokrotnie w II terminie.

W II terminie nie odejmuje się punktów za spóźnienia. Rozwiązania z datą
późniejszą niż 7 dni przed egzaminem poprawkowym nie podlegają ocenie.
