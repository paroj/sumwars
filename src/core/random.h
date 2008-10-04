#ifndef RANDOM_H
#define RANDOM_H

#include <math.h>
#include <stdlib.h>
#include "debug.h"
#include <vector>


/**
 * \class Random
 * \brief Klasse fuer die Erzeugung von Zufallszahlen
 */
class Random
{
#define rez_rand_max  1.0 / RAND_MAX
	
	public:
		
	/**
	 * \fn static float random()
	 * \return Zufallszahl zwischen 0 und 1
	 */
	static float random()
	{
		return rand() *rez_rand_max;
	}
	
	/**
	 * \fn static float randf(float f)
	 * \return Zufallszahl zwischen 0 und f
	 */
	static float randf(float f)
	{
		return random()*f;
	}
	
	/**
	 * \fn static int randi(int i)
	 * \return Zufallszahl zwischen 0 und i-1
	 */
	static int randi(int i)
	{
		return (int) random()*i;
	}
	
	/**
	 * \fn static float randrangef(float f1, float f2)
	 * \return Zufallszahl zwischen f1 und f2
	 */
	static float randrangef(float f1, float f2)
	{
		return f1 + randf(f2-f1);
	}
	
	/**
	 * \fn static int randrangei(int i1, int i2)
	 * \return Zufallszahl zwischen i2 und i2
	 */
	static int randrangei(int i1, int i2)
	{
		return i1 + randi(i2-i1+1);
	}
	
	/**
	 * \fn 	int randDiscrete(float* distribution,int nr, float sum)
	 * \brief Sucht unter eine Menge von Ereignissen mit einer gewissen Wahrscheinlichkeit eines aus
	 * \param distribution Liste der Wahrscheinlichkeiten der Ereignisse
	 * \param nr Anzahl der Ereignisse
	 * \param sum Summe der Einzelwahrscheinlichkeiten (erspart normieren)
	 * \return Nummer des eingetreteten Ereignisses
	 */
	static int randDiscrete(float* distribution,int nr, float sum=1);
	
	/**
	 * \fn static int randDiscrete(std::vector<float> distribution)
	 * \brief Sucht unter eine Menge von Ereignissen mit einer gewissen Wahrscheinlichkeit eines aus
	 * \param distribution Liste der Wahrscheinlichkeiten der Ereignisse
	 * \return Nummer des eingetreteten Ereignisses
	 */
	static int randDiscrete(std::vector<float> distribution);
};

//float Random::rez_rand_max = 1.0 / RAND_MAX;

#endif

