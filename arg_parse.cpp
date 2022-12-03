/**
 * IMS balistika ve vojenství
 * 
 * @file arg_parse.cpp
 * @brief Zpracování argumentů
 * @date 3.12.2022
 * 
 * @author David Konečný (xkonec83)
 * @author Martin Pech (xpechm00)
 */

#include "arg_parse.hpp"

#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include <string>

double cas_do = 1440.0;
double dezerce = 0.9;
double intenzita_pozadavku = 540.0;
double poruchovost = 1.0;

void arg_parse(int argc, char *argv[])
{
    int c = 0;

    while ((c = getopt(argc, argv, "t:d:f:p:")) != -1)
    {
        switch (c)
        {
            case 't':
            {
                cas_do = std::stof(optarg);
                if (cas_do < 0)
                {
                    std::cerr << "Je vyžadováno kladné čislo!" << std::endl;
                    exit(1);
                }

                break;
            }

            case 'd':
            {
                dezerce = std::stof(optarg);
                if (!(dezerce >= 0 && dezerce <= 100))
                {
                    std::cerr << "Je vyžadováno čislo v intervalu <0, 100>!" << std::endl;
                    exit(1);
                }
                
                break;
            }

            case 'f':
            {
                intenzita_pozadavku = std::stof(optarg);
                if (intenzita_pozadavku < 0)
                {
                    std::cerr << "Je vyžadováno kladné čislo!" << std::endl;
                    exit(1);
                }

                break;
            }

            case 'p':
            {
                poruchovost = std::stof(optarg);
                if (!(poruchovost >= 0 && poruchovost <= 100))
                {
                    std::cerr << "Je vyžadováno čislo v intervalu <0, 100>!" << std::endl;
                    exit(1);
                }
                break;
            }
            
            default:
            {
                exit(1);   
                break;
            }
        }
    }
}
