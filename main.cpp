#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int distances[15][15] = { {0} };

enum OWNER
{
	ME, OPPONENT, NEUTRAL
};

struct Troop
{
	OWNER owner;
    int sourceFactory;
    int targetFactory;
    int cyborgs;
    int arrival;

    Troop(OWNER owner, int sourceFactory, int targetFactory, int cyborgs, int arrival) : owner(owner), sourceFactory(sourceFactory), targetFactory(targetFactory), cyborgs(cyborgs), arrival(arrival)
    {

    }

    Troop() : owner(NEUTRAL), sourceFactory(0), targetFactory(0), cyborgs(0), arrival(0)
    {

    }
};


struct Factory
{
	int id;
	OWNER owner;
	int cyborgs;
	int cyborgsAfterCalcul;
	int production;

	Factory(int id, OWNER owner, int cyborgs, int production) : id(id), owner(owner), cyborgs(cyborgs), cyborgsAfterCalcul(cyborgs), production(production)
	{

	}

	Factory() : id(0), owner(NEUTRAL), cyborgs(0), production(0)
	{

	}

	Factory(const Factory & other) : id(other.id), owner(other.owner), cyborgs(other.cyborgs), cyborgsAfterCalcul(other.cyborgs), production(other.production)
	{

	}

	void fight(const Troop & troop)
    {
        if (owner == troop.owner) {
            cyborgs += troop.cyborgs;
        } else {
            cyborgs -= troop.cyborgs;
            if (cyborgs < 0) {
                cyborgs = -cyborgs;
                owner = troop.owner;
            }
        }
    }



};


static OWNER argToOwner(int arg) {
	switch (arg) {
	case 1:
		return ME;
	case -1:
		return OPPONENT;
	case 0:
		return NEUTRAL;
	}
}

class IsOwnedBy
{
public:
	IsOwnedBy(OWNER owner) : owner(owner) {}
	bool operator() (const Factory & factory) const { return factory.owner == owner; }
private:
	OWNER owner;

};

class IsOther
{
public:
	IsOther() {}
	bool operator() (const Factory & factory) const { return factory.owner != ME; }

};

class IsMyTerritory
{
public:
	IsMyTerritory(bool pair): pair(pair) {}
	bool operator() (const Factory & factory) const { return factory.owner == NEUTRAL; }

private:
	bool pair;
};

class FightIfArrivedIn 
{
public:
	FightIfArrivedIn(Factory & factory, int nbTurns) : factory(factory), nbTurns(nbTurns) {}
	void operator() (const Troop & troop) { if (troop.arrival <= nbTurns) factory.fight(troop); }
private:
	Factory & factory;
	int nbTurns;
};

Factory calculateFactoryAt(const Factory & factory, int nbTurnsFromNow, std::vector<std::vector<Troop>> & troopsByFactory)
{
	Factory futureFactory(factory);
	std::vector<Troop> & troops = troopsByFactory[factory.id];

	std::sort(troops.begin(), troops.end(), [](const Troop & troop1, const Troop & troop2) { return troop1.arrival < troop2.arrival; });

	std::for_each(troops.begin(), troops.end(), FightIfArrivedIn(futureFactory, nbTurnsFromNow));

	return futureFactory;
}

int getDistance(const Factory & factory1, const Factory & factory2)
{
	return distances[factory1.id][factory2.id];
}


bool contains(int id, const std::vector<Factory> & v)
{
	return std::any_of(v.begin(), v.end(), [&id](const Factory & f) { return f.id == id; });
}

int getSumDistance(const Factory & factory, const std::vector<Factory> & factories)
{
	std::vector<Factory> sourceFactories;
	std::copy_if(factories.begin(), factories.end(), std::back_inserter(sourceFactories), IsOwnedBy(OWNER::ME));

	return std::accumulate(sourceFactories.begin(), sourceFactories.end(), 0, [&factory] (int sum, const Factory & sourceFactory) { return sum + distances[factory.id][sourceFactory.id]; });
}

vector<Factory> selectTargetFactory(const std::vector<Factory> & factories, bool iAmPair) {
	std::vector<Factory> neutralFactories;
	std::copy_if(factories.begin(), factories.end(), std::back_inserter(neutralFactories), IsOther());
	std::vector<Factory> myNeutralFactories;
	std::copy_if(neutralFactories.begin(), neutralFactories.end(), std::back_inserter(myNeutralFactories), IsMyTerritory(iAmPair));

	if (neutralFactories.size() != 0) {

		std::sort(neutralFactories.begin(), neutralFactories.end(), [&factories](const Factory & factory1, const Factory & factory2) {
			return getSumDistance(factory1, factories) <= getSumDistance(factory2, factories); });

		return neutralFactories;
	}
	else {
		std::vector<Factory> centerFactories;
		std::copy_if(factories.begin(), factories.end(), std::back_inserter(centerFactories), [](const Factory & f) {return f.id == 0; });
		return centerFactories;
	}

}

vector<Factory> selectSourceFactory(const std::vector<Factory> & factories) {
	std::vector<Factory> sourceFactories;
	std::copy_if(factories.begin(), factories.end(), std::back_inserter(sourceFactories), IsOwnedBy(OWNER::ME));

	std::sort(sourceFactories.begin(), sourceFactories.end(), [](const Factory & factory1, const Factory & factory2) {
		return factory1.cyborgs < factory2.cyborgs; });

	std::reverse(sourceFactories.begin(), sourceFactories.end());
	return sourceFactories;
}

bool isPair(int factoryId) {
	return false;
}

/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/
int main()
{
	int factoryCount; // the number of factories
	cin >> factoryCount;
	cin.ignore();

	int linkCount; // the number of links between factories
	cin >> linkCount;
	cin.ignore();


	for (int i = 0; i < linkCount; i++) {
		int factory1;
		int factory2;
		int distance;
		cin >> factory1 >> factory2 >> distance; cin.ignore();

		distances[factory1][factory2] = distance;
        distances[factory2][factory1] = distance;

	}

	for (int i = 0; i < 15; ++i) {
		for (int j = 0; j < 15; ++j) {
			cerr << distances[i][j] << " ";
		}
		cerr << endl;
	}


	bool isMyTerritoryFound = false;
	bool isMyTerritoryPair = false;


	// game loop
	while (1) {
		std::vector<Factory> factories;
		std::vector<std::vector<Troop>> troopsByFactory(15);

		int entityCount; // the number of entities (e.g. factories and troops)
		cin >> entityCount; cin.ignore();
		for (int i = 0; i < entityCount; i++) {
			int entityId;
			string entityType;
			int arg1;
			int arg2;
			int arg3;
			int arg4;
			int arg5;
			cin >> entityId >> entityType >> arg1 >> arg2 >> arg3 >> arg4 >> arg5; cin.ignore();

			if (entityType == "FACTORY") {
				factories.push_back(Factory(entityId, argToOwner(arg1), arg2, arg3));
			}
			else if (entityType == "TROOP") {
				int targetFactoryId = arg3;
                troopsByFactory[targetFactoryId].push_back(Troop(argToOwner(arg1), arg2, targetFactoryId, arg4, arg5));
			}
		}

		if (!isMyTerritoryFound) {
			for (Factory factory : factories) {
				if (factory.owner == ME) {

					isMyTerritoryPair = isPair(factory.id);
				}
			}
		}


		string output = "";

		const Factory & futureFacto = calculateFactoryAt(factories[0], 3, troopsByFactory);
		cerr << futureFacto.id << " in 3: " << futureFacto.cyborgs << " " << futureFacto.owner<< endl;

		vector<Factory> sourceFactories = selectSourceFactory(factories);
		vector<Factory> targetFactories = selectTargetFactory(factories, isMyTerritoryPair);

		// To debug: cerr << "Debug messages..." << endl;

		for (Factory target : targetFactories) {

			for (Factory mine : sourceFactories) {

				if (mine.cyborgs > 10 && mine.production < 3)
				{
					mine.cyborgsAfterCalcul -= 10;
					output += "INC " + to_string(mine.id) + ";";
				}

				if (target.id != mine.id) {

					if (target.cyborgsAfterCalcul >= 0 && mine.cyborgsAfterCalcul > 0) {


						if (mine.cyborgs > target.cyborgsAfterCalcul) {
							output += "MOVE " + std::to_string(mine.id) + " " + std::to_string(target.id) + " " + std::to_string(target.cyborgsAfterCalcul + 1) + ";";
							target.cyborgsAfterCalcul = -1;
						}
						else {
							output += "MOVE " + std::to_string(mine.id) + " " + std::to_string(target.id) + " " + std::to_string(mine.cyborgs) + ";";
							target.cyborgsAfterCalcul -= mine.cyborgsAfterCalcul;
							mine.cyborgsAfterCalcul = 0;
						}

					}

				}
			}
		}
		if (output == "") {
			output = "WAIT;";
		}
		output += "MSG test";
		cout << output << endl;

	}
}
