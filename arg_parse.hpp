/**
 * IMS balistika ve vojenství
 * 
 * @file arg_parse.hpp
 * @brief Zpracování argumentů
 * @date 4.12.2022
 * 
 * @author David Konečný (xkonec83)
 * @author Martin Pech (xpechm00)
 */

#ifndef	_ARG_PARSE_H
#define	_ARG_PARSE_H

/**
 * Zpracuje argumenty a uloží je do příslušných globálních proměnných
 * 
 * @param argc Počet argumentů
 * @param argv Pole argumentů
 */
void arg_parse(int argc, char *argv[]);

/** Globální proměnná pro konec simulačního času */
extern double cas_do;

/** GLobální proměnná pro dezerci */
extern double dezerce;

/** Globální proměnná pro intenzitu požadavků palby */
extern double intenzita_pozadavku;

/** Globální proměnná poruchovost houfnice */
extern double poruchovost;

#endif
