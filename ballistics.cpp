#include <iostream>
#include <simlib.h>

Facility Linka("Obsluzna linka");
Facility LinkaVojaku("Obslužná linka vojáků");
Stat dobaObsluhy("Doba obsluhy na lince");
Histogram dobaVSystemu("Celkova doba v systemu", 0, 40, 20);
Queue cekani;

extern int bezCekani = 0;
extern int sCekanim = 0;

class Palba: public Process
{
    void Behavior()
    {
        double tvstup = Time;
        double obsluha;
        int raketomet = 0;

        opak:

        if (!Linka.Busy())
        {
            bezCekani++;
            int raketomet = 1;

            Seize(Linka);
            obsluha = Exponential(10);
            std::cout << "Raketomet byl zabrán" << std::endl;
            //obsluha = 0;
            Wait(obsluha);
            dobaObsluhy(obsluha);
            // Tady bude chování v rámci zabrání linky
            Release(Linka);
            dobaVSystemu(Time - tvstup);
        }
        else
        {
            sCekanim++;
            cekani.Insert(this);
            Passivate();
            goto opak;
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
        (new Palba)->Activate();
        Activate(Time + Exponential(11));
        std::cout << "Požadavek palby byl vygenerován!" << std::endl;
    }
};

int main()
{
    std::cout << "Ahoj" << std::endl;
    std::cout << "test" << std::endl;

    Init(0, 100);
    (new Generator)->Activate();
    Run();

    std::cout << "Bez čekání: " << bezCekani << std::endl;
    //std::cout << "S čekáním: " << sCekanim << std::endl;
    Print("S čekáním: %d\n", sCekanim);

    //dobaObsluhy.Output();
    //Linka.Output();
    //dobaVSystemu.Output();

    return 0;
}
