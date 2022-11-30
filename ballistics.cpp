#include <iostream>
#include <simlib.h>

#define NUM_OF_ROCKET_LAUNCHERS 2

Facility Linka[NUM_OF_ROCKET_LAUNCHERS];
Facility LinkaVojaku("Obslužná linka vojáků");
Stat dobaObsluhy("Doba obsluhy na lince");
Histogram dobaVSystemu("Celkova doba v systemu", 0, 40, 20);
Queue cekani;

// Vojáci by po nějaké době mohli odcházet a jejich počet by se postupně snižoval 

int bezCekani = 0;
int sCekanim = 0;

class Palba: public Process
{
    void Behavior()
    {
        double tvstup = Time;
        double nabijeni;
        double palba;

        opak:

        int raketomet = -1;

        // Kontrola, zda jsou raketomety volné
        for (int i = 0; i < NUM_OF_ROCKET_LAUNCHERS; i++)
        {
            if (!Linka[i].Busy())
            {
                raketomet = i;
                bezCekani++;
                break;
            }
        }
        
        // Pokud nebyl žádný raketomet volný, požadavek se vloží do fronty
        if (raketomet == -1)
        {
            sCekanim++;
            cekani.Insert(this);
            Passivate();
            Print("Požadavek byl zařazen do fronty\n");
            goto opak;
        }
        else
        {
            Seize(Linka[raketomet]);
            Print("Raketomet %d byl zabrán\n", raketomet + 1);
        }
        
        // Nabíjení
        nabijeni = Exponential(10);
        Wait(nabijeni);
        dobaObsluhy(nabijeni);
        Print("Raketomet %d byl nabit\n", raketomet + 1);

        // Palba
        palba = Exponential(10);
        Wait(palba);
        Print("Raketomet %d vypálil\n", raketomet + 1);

        // Tady bude chování v rámci zabrání linky
        Release(Linka[raketomet]);
        Print("Raketomet %d byl uvolněn\n", raketomet + 1);
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
        (new Palba)->Activate();
        Activate(Time + Exponential(11));
        Print("Požadavek palby byl vygenerován\n");
    }
};

int main()
{
    std::cout << "Ahoj" << std::endl;
    std::cout << "test" << std::endl;

    Init(0, 100);
    (new Generator)->Activate();
    Run();

    Print("Bez čekání: %d\n",bezCekani);
    Print("S čekáním: %d\n", sCekanim);

    //dobaObsluhy.Output();
    //Linka.Output();
    //dobaVSystemu.Output();

    return 0;
}
