/**
 * IMS ballistics in the military
 * 
 * @file ballistics.cpp
 * @brief TODO
 * @date 1.12.2022
 * 
 * @author Martin Pech
 * @author David Konečný
 */

#include <iostream>
#include <simlib.h>

#define NUM_OF_ROCKET_LAUNCHERS 2
#define NUM_OF_SOLDIERS 6

Facility LinkaRaketometu[NUM_OF_ROCKET_LAUNCHERS];
Facility Vojaci[NUM_OF_SOLDIERS];

Stat dobaObsluhy("Doba obsluhy na lince");
Histogram dobaVSystemu("Celkova doba v systemu", 0, 40, 20);
Queue cekani;

int bezCekani = 0;
int sCekanim = 0;

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
        double presun = Normal(44 * 60, 8 * 60);
        Wait(presun);
        Print("Jednotka se vrací do tábora z důvodu poruchy raketometu\n");

        // Zabrat dalšího vojáka
        double oprava = Uniform(30 * 60, 60 * 60);
        Wait(oprava);
        Print("Probíhá oprava\n");

        Release(LinkaRaketometu[raketomet]);
        Print("Raketomet %d byl uvolněn\n", raketomet + 1);
        Release(Vojaci[jednotka]);
        Print("Jednotka %d byla uvolněna\n", jednotka + 1);
    }

    int raketomet;
    int jednotka;
};

class Dezerce: public Process
{
    void Behavior()
    {
        int jednotka = -1;

        if (Random() >= 0.95)
        {
            for (int i = 0; i < NUM_OF_SOLDIERS; i++)
            {
                if (!Vojaci[i].Busy())
                {
                    jednotka = i;
                    break;
                }
            }

            if (jednotka != -1)
            {
                Seize(Vojaci[jednotka]);
                Print("Jednotka %d dezertovala\n", jednotka + 1);
            }
        }
    }
};

class Main: public Process
{
    void Behavior()
    {
        double tvstup = Time;
        double nabijeni;
        double palba;

        opak:

        int raketomet = -1;
        int jednotka = -1;

        // Kontrola, zda jsou raketomety volné
        for (int i = 0; i < NUM_OF_ROCKET_LAUNCHERS; i++)
        {
            if (!LinkaRaketometu[i].Busy())
            {
                raketomet = i;
                bezCekani++;
                break;
            }
        }

        // Kontrola, zda jsou vojáci k dispozici
        for (int i = 0; i < NUM_OF_SOLDIERS; i++)
        {
            if (!Vojaci[i].Busy())
            {
                jednotka = i;
                break;
            }
        }
        
        // Pokud nebyl žádný raketomet volný, požadavek se vloží do fronty
        if (raketomet == -1 || jednotka == -1)
        {
            sCekanim++;
            cekani.Insert(this);
            Passivate();
            Print("Požadavek byl zařazen do fronty\n");
            goto opak;
        }
        else
        {
            Seize(LinkaRaketometu[raketomet]);
            Print("Raketomet %d byl zabrán\n", raketomet + 1);
            Seize(Vojaci[jednotka]);
            Print("Jednotka %d byla zaúkolována\n", jednotka + 1);
        }

        // Přesun na bojiště
        Wait(Normal(44 * 60, 8 * 60));
        Print("Raketomet %d a jednotka %d vojáků se přesunuli na bojiště\n", raketomet + 1, jednotka + 1);
        
        // Porucha
        if (Random() <= 0.1)
        {
            (new Porucha(raketomet, jednotka))->Activate();
        }
        else
        {
            // Nabíjení
            nabijeni = Uniform(12, 15);
            Wait(nabijeni);
            dobaObsluhy(nabijeni);
            Print("Raketomet %d byl nabit\n", raketomet + 1);

            // Palba
            palba = 1.0;
            Wait(palba);
            Print("Raketomet %d vypálil\n", raketomet + 1);

            // Návrat zpět do tábora
            double presun_zpet = Normal(44 * 60, 8 * 60);
            Wait(presun_zpet);
            Print("Raketomet a vojáci se přesunuli zpět do tábora\n");
            Release(Vojaci[jednotka]);
            Print("Jednotka %d byla uvolněna\n", jednotka + 1);
            Release(LinkaRaketometu[raketomet]);
            Print("Raketomet %d byl uvolněn\n", raketomet + 1);
        }

        dobaVSystemu(Time - tvstup);

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
        (new Main)->Activate();
        (new Dezerce)->Activate();
        Activate(Time + Exponential(500)); // 120
        Print("Požadavek palby byl vygenerován\n");
    }
};

int main()
{
    Init(0, 12000);
    (new Generator)->Activate();
    
    Run();

    Print("Bez čekání: %d\n",bezCekani);
    Print("S čekáním: %d\n", sCekanim);

    //dobaObsluhy.Output();
    //Linka.Output();
    //dobaVSystemu.Output();

    return 0;
}
