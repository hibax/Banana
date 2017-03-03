#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum OWNER
{
	ME, OPPONENT, NEUTRAL
};

struct Distance
{
	int id1;
	int id2;
	int distance;
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

class IsOther
{
public:
	IsOther() {}
	bool operator() (const Factory & factory) const { return factory.owner != ME; }

};


class IsNeutral
{
public:
	IsNeutral() {}
	bool operator() (const Factory & factory) const { return factory.owner == NEUTRAL; }

};


int getDistance(Factory  factory1, Factory  factory2, std::vector<Distance> distances)
{
	for (Distance distance : distances) {
		if (distance.id1 == factory1.id && distance.id2 == factory2.id || distance.id1 == factory2.id && distance.id2 == factory1.id) {
			return distance.distance;
		}
	}
}


bool contains(int id, vector<Factory> v)
{
	std::vector<Factory>::iterator it = std::find_if(v.begin(), v.end(), [&id](const Factory & f) -> bool { return f.id == id; });

	return (it != v.end());
}

int getSumDistance(Factory factory, std::vector<Distance> distances, std::vector<Factory> factories)
{
	std::vector<Factory> sourceFactories;
	std::copy_if(factories.begin(), factories.end(), std::back_inserter(sourceFactories), [](const Factory & factory) { return factory.owner == OWNER::ME; });

	int sum = 0;
	for (Distance distance : distances) {
		if (distance.id1 == factory.id && contains(distance.id2, sourceFactories) || distance.id2 == factory.id  && contains(distance.id1, sourceFactories)) {

			sum = sum + distance.distance;
		}
	}
	return sum;
}

vector<Factory> selectTargetFactory(const std::vector<Factory> & factories, std::vector<Distance> distances, bool iAmPair) {
	std::vector<Factory> neutralFactories;
	std::copy_if(factories.begin(), factories.end(), std::back_inserter(neutralFactories), IsOther());
	std::vector<Factory> myNeutralFactories;
	std::copy_if(neutralFactories.begin(), neutralFactories.end(), std::back_inserter(myNeutralFactories), IsMyTerritory(iAmPair));

	//if (myNeutralFactories.size() != 0) {
	//    std::sort(myNeutralFactories.begin(), myNeutralFactories.end(), [&distances, &factories](const Factory & factory1, const Factory & factory2) { 
	//                                                                    return getSumDistance(factory1, distances, factories) > getSumDistance(factory2, distances, factories) ; });
	//    std::reverse(myNeutralFactories.begin(), myNeutralFactories.end());
	//    return myNeutralFactories;
	//}
	//else 
	if (neutralFactories.size() != 0) {

		std::sort(neutralFactories.begin(), neutralFactories.end(), [&distances, &factories](const Factory & factory1, const Factory & factory2) {
			return getSumDistance(factory1, distances, factories) > getSumDistance(factory2, distances, factories); });
		std::reverse(neutralFactories.begin(), neutralFactories.end());
		return neutralFactories;
	}
	else {
		std::vector<Factory> centerFactories;
		std::copy_if(factories.begin(), factories.end(), std::back_inserter(centerFactories), [](Factory f) {return f.id == 0; });
		return centerFactories;
	}

}

vector<Factory> selectSourceFactory(const std::vector<Factory> & factories) {
	std::vector<Factory> sourceFactories;
	std::copy_if(factories.begin(), factories.end(), std::back_inserter(sourceFactories), [](const Factory & factory) { return factory.owner == OWNER::ME; });

	std::sort(sourceFactories.begin(), sourceFactories.end(), [](const Factory & factory1, const Factory & factory2) {
		return factory1.cyborgs < factory2.cyborgs; });

	std::reverse(sourceFactories.begin(), sourceFactories.end());
	return sourceFactories;
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


	std::vector<Distance> distances;

	for (int i = 0; i < linkCount; i++) {
		int factory1;
		int factory2;
		int distance;
		cin >> factory1 >> factory2 >> distance; cin.ignore();

		distances.push_back({ factory1, factory2, distance });

	}


	bool isMyTerritoryFound = false;
	bool isMyTerritoryPair = false;


	// game loop
	while (1) {
		std::vector<Factory> factories;

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

		vector<Factory> sourceFactories = selectSourceFactory(factories);
		vector<Factory> targetFactories = selectTargetFactory(factories, distances, isMyTerritoryPair);

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
