#pragma once

#include "debug.h"
#include <string>
#include <map>

/**
 * \class Fraction
 * \brief Fraktion, der Spieler und Monster angehoeren koennen
 */
class Fraction
{
	public:
		/**
		 * \enum Relation
		 * \brief Listet die verschiedenen Beziehungen zwischen Fraktionen auf
		 */
		enum Relation
		{
			NEUTRAL = 0,
			ALLIED = 1,
			HOSTILE =2
		};
		
		/**
		 * \enum SpecialFraction
		 * \brief einige besondere Werte fuer Fraktionids
		 */
		enum SpecialFraction
		{
			DEFAULT = -2,
			NOFRACTION = -1,
			PLAYER =0,
			NEUTRAL_TO_ALL= 9998,
			HOSTILE_TO_ALL = 9999,
			MONSTER = 10000,
		};
		
		typedef std::string Type;

		typedef short Id;
		
		/**
		 * \fn Fraction(Id id, Type type)
		 * \brief Konstruktor
		 */
		Fraction(Id id, Type type)
		{
			m_id = id;
			m_type = type;
			m_relations[DEFAULT] = NEUTRAL;
		}
		
		
		/**
		 * \fn int getId()
		 * \brief Gibt die ID aus
		 */
		int getId()
		{
			return m_id;
		}
		
		/**
		 * \fn Type getType()
		 * \brief Gibt den Typ aus
		 */
		Type getType()
		{
			return m_type;
		}
		
		/**
		 * \fn Relation getRelation(Id id)
		 * \brief Gibt Verhaeltnis zur Fraktion mit der ID id aus
		 * \param id ID einer anderen Fraktion
		 */
		Relation getRelation(Id id);
		
		/**
		 * \fn void setRelation(Id id, Relation relation)
		 * \brief Setzt des Verhaeltnis zu einer Fraktion
		 * \param id ID der Fraktion
		 * \param relation Verhaeltnis
		 */
		void setRelation(Id id, Relation relation)
		{
			m_relations[id] = relation;
		}
	
		
	private:
		/**
		 * \var Id m_id
		 * \brief Nummer der Fraktion
		 */
		Id m_id;
		
		/**
		 * \var Type m_type
		 * \brief Type der Fraktion
		 */
		Type m_type;
		
		/**
		 * \var std::string m_name
		 * \brief Name der Fraktion
		 */
		std::string m_name;
		
		/**
		 * \var std::map<Id, Relation> m_relations
		 * \brief Gibt Verhaeltnis zu anderen Fraktionen an, gespeichert wird nur Verhaeltnis zu Fraktionen mit hoeherer ID
		 */
		std::map<Id, Relation> m_relations;
};
