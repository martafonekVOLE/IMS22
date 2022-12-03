/**
 * IMS balistika ve vojenství
 * 
 * @file ballistics.cpp
 * @brief Hlavní soubor, který obsahuje implementaci simulace
 * @date 3.12.2022
 * 
 * @author David Konečný (xkonec83)
 * @author Martin Pech (xpechm00)
 */

#include <simlib.h>
#include <cmath>

#include "arg_parse.hpp"

#define POCET_RAKETOMETU 2
#define POCET_VOJAKU 3

Facility LinkaRaketometu[POCET_RAKETOMETU];
Facility Vojaci[POCET_VOJAKU];

Stat dobaObsluhy("Doba obsluhy na lince");
Histogram dobaVSystemu("Celkova doba v systemu", 0, 40, 20);
Queue cekani;

int pozadavky = 0;
int bezCekani = 0;
int sCekanim = 0;
int obslouzeno = 0;
int pocet_dezertovanych = 0;
int pocet_poruch = 0;

class Porucha: public Process
{
    public:
        Porucha(int r, int j): Process()
        {
            raketomet = r;
            jednotka = j;
        };

    void Behavior()
    {
        // Přesun zpět do tábora
        double presun = Normal(48, 8);
        Wait(presun);
        Print("[%f]: Jednotka %d se vrací do tábora z důvodu poruchy raketometu %d\n", Time, jednotka + 1, raketomet + 1);
        pocet_poruch++;

        // Oprava
        double oprava = Uniform(30, 60);
        Wait(oprava);
        Print("[%f]: Probíhá oprava\n", Time);
    }

    int raketomet;
    int jednotka;
};

class Dezerce: public Process
{
    void Behavior()
    {
        int jednotka = -1;

        if (Random() >= (1 - pow(dezerce / 100, 5)))
        {
            for (int i = 0; i < POCET_VOJAKU; i++)
            {
                if (!Vojaci[i].Busy())
                {
                    jednotka = i;
                    break;
                }
            }

            // Pokud je v táboře nějaká jednotka, tak dezertuje
            if (jednotka != -1)
            {
                Seize(Vojaci[jednotka], 1);
                Print("[%f]: Jednotka %d dezertovala\n", Time, jednotka + 1);
                pocet_dezertovanych++;
            }
        }
    }
};

class Palba: public Process
{
    void Behavior()
    {
        double nabijeni;
        double palba;

        opak:

        int raketomet = -1;
        int jednotka = -1;

        // Kontrola, zda jsou raketomety volné
        for (int i = 0; i < POCET_RAKETOMETU; i++)
        {
            if (!LinkaRaketometu[i].Busy())
            {
                raketomet = i;
                break;
            }
        }

        // Kontrola, zda jsou vojáci k dispozici
        for (int i = 0; i < POCET_VOJAKU; i++)
        {
            if (!Vojaci[i].Busy())
            {
                jednotka = i;
                break;
            }
        }
        
        // Pokud nebyl žádný raketomet volný nebo nebyla volná jednotka vojáků, je požadavek vložen do fronty
        if (raketomet == -1 || jednotka == -1)
        {
            sCekanim++;
            cekani.Insert(this);
            Passivate();
            Print("[%f]: Požadavek byl zařazen do fronty\n", Time);
            goto opak;
        }
        else
        {
            bezCekani++;
            // Uvolnění jednotky a raketometu
            Seize(LinkaRaketometu[raketomet]);
            Print("[%f]: Raketomet %d byl zabrán\n", Time, raketomet + 1);
            Seize(Vojaci[jednotka]);
            Print("[%f]: Jednotka %d byla zaúkolována\n", Time, jednotka + 1);
        }

        // Přesun na bojiště
        Wait(Normal(48, 8));
        Print("[%f]: Raketomet %d a jednotka %d se přesunuli na bojiště\n", Time, raketomet + 1, jednotka + 1);
        
        // Porucha
        if (Random() <= (poruchovost / 100.0))
        {
            (new Porucha(raketomet, jednotka))->Activate();

            // Uvolnění jednotky a raketometu
            Release(LinkaRaketometu[raketomet]);
            Print("[%f]: Raketomet %d byl opraven a uvolněn\n", Time, raketomet + 1);
            Release(Vojaci[jednotka]);
            Print("[%f]: Jednotka %d byla uvolněna\n", Time,  jednotka + 1);
        }
        else
        {
            // Nabíjení
            nabijeni = Uniform((12.0 / 60.0), (15.0 / 60.0));
            Wait(nabijeni);
            dobaObsluhy(nabijeni);
            Print("[%f]: Raketomet %d byl nabit\n", Time, raketomet + 1);

            // Palba
            palba = 1.0 / 60.0;
            Wait(palba);
            Print("[%f]: Raketomet %d vypálil\n", Time, raketomet + 1);

            // Návrat zpět do tábora
            double presun_zpet = Normal(48, 8);
            Wait(presun_zpet);
            Print("[%f]: Raketomet %d a jednotka %d se přesunuli zpět do tábora\n", raketomet + 1, jednotka + 1, Time);

            // Uvolnění jednotky a raketometu
            Release(Vojaci[jednotka]);
            Print("[%f]: Jednotka %d byla uvolněna\n", Time, jednotka + 1);
            Release(LinkaRaketometu[raketomet]);
            Print("[%f]: Raketomet %d byl uvolněn\n", Time, raketomet + 1);
            obslouzeno++;   
        }

        if (cekani.Length() > 0)
        {
            (cekani.GetFirst())->Activate();
        }
    }
};

class Generator: public Event
{
    void Behavior()
    {
        (new Dezerce)->Activate();
        (new Palba)->Activate();

        Activate(Time + Exponential(intenzita_pozadavku));
        Print("[%f]: Požadavek palby byl vygenerován\n", Time);
        pozadavky++;
    }
};

int main(int argc, char *argv[])
{
    arg_parse(argc, argv);

    Init(0, cas_do);
    (new Generator)->Activate();
    
    Run();

    Print("====================================\n");
    Print("Požadavky celkem: %d\n", pozadavky);
    Print("Bez čekání: %d\n",pozadavky - sCekanim);
    Print("S čekáním: %d\n", sCekanim);
    Print("Počet jednotek, co dezertovaly: %d\n", pocet_dezertovanych);
    Print("Počet poruch: %d\n", pocet_poruch);
    Print("------------------------------------\n");
    Print("Úspěšně dokončeno: %d\n", obslouzeno);
    Print("Nevyřízené požadavky: %d\n", pozadavky - obslouzeno);

    return 0;
}
