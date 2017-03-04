#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>


int distances[15][15] = { { 0 } };

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

	int cyborgsRealValueDuringThisTurn;

	int production;

	Factory(int id, OWNER owner, int cyborgs, int production) : id(id), owner(owner), cyborgs(cyborgs), cyborgsRealValueDuringThisTurn(cyborgs), production(production)
	{

	}

	Factory() : id(0), owner(NEUTRAL), cyborgs(0), production(0)
	{

	}


};



struct Troop
{
	OWNER owner;
	int from;
	int to;
	int cyborgs;
	int turnsLeft;
	Troop(OWNER owner, int from, int to, int cyborgs, int turnsLeft) : owner(owner), from(from), to(to), cyborgs(cyborgs), turnsLeft(turnsLeft)
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


int calculateFactoryCyborgsAt(const Factory & factory, int nbTurnsFromNow, const std::vector<Troop> & troops)
{
	int cyborgAfterCalcul = factory.cyborgsRealValueDuringThisTurn;

	if (factory.owner != OWNER::NEUTRAL) {
		cyborgAfterCalcul += factory.production*nbTurnsFromNow;

	}


	std::vector<Troop> troopsIncoming;

	std::copy_if(troops.begin(), troops.end(), std::back_inserter(troopsIncoming), [&factory](const Troop & troop) { return factory.id == troop.to; });

	for (Troop troop : troopsIncoming) {

		if (troop.turnsLeft <= nbTurnsFromNow) {

			if (troop.owner == factory.owner)
			{
				cyborgAfterCalcul += troop.cyborgs;
			}
			else
			{
				cyborgAfterCalcul -= troop.cyborgs;
			}
		}
	}



	return cyborgAfterCalcul;
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

	return std::accumulate(sourceFactories.begin(), sourceFactories.end(), 0, [&factory](int sum, const Factory & sourceFactory) { return sum + distances[factory.id][sourceFactory.id]; });
}

std::vector<Factory> selectTargetFactory(const std::vector<Factory> & factories) {
	std::vector<Factory> neutralFactories;
	std::copy_if(factories.begin(), factories.end(), std::back_inserter(neutralFactories), IsOther());

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

std::vector<Factory> selectSourceFactory(const std::vector<Factory> & factories) {
	std::vector<Factory> sourceFactories;
	std::copy_if(factories.begin(), factories.end(), std::back_inserter(sourceFactories), IsOwnedBy(OWNER::ME));

	std::sort(sourceFactories.begin(), sourceFactories.end(), [](const Factory & factory1, const Factory & factory2) {
		return factory1.cyborgs < factory2.cyborgs; });

	std::reverse(sourceFactories.begin(), sourceFactories.end());
	return sourceFactories;
}

std::string generateActions(std::vector<Factory> factories, std::vector<Troop> troops)
{


	std::string output = "";

	std::vector<Factory> sourceFactories = selectSourceFactory(factories);

	//defense

	for (Factory &myFactory : sourceFactories)
	{
		int cyborgsAfterTroopsCalcul = calculateFactoryCyborgsAt(myFactory, 999, troops);

		if (myFactory.production > 0 && cyborgsAfterTroopsCalcul < 0)
		{
			for (Factory &factoryHelper : sourceFactories) {
				if (myFactory.id != factoryHelper.id) {
					if (factoryHelper.cyborgsRealValueDuringThisTurn > 0) {


						int totalSourcesCyborgs = std::accumulate(sourceFactories.begin(), sourceFactories.end(), 0, [](int sum, const Factory & sourceFactory) { return sum + sourceFactory.cyborgsRealValueDuringThisTurn; });

						if (totalSourcesCyborgs > cyborgsAfterTroopsCalcul * (-1)) {

							if (factoryHelper.cyborgs > myFactory.cyborgsRealValueDuringThisTurn) {
								output += "MOVE " + std::to_string(factoryHelper.id) + " " + std::to_string(myFactory.id) + " " + std::to_string(cyborgsAfterTroopsCalcul*(-1) + 1) + ";";
								factoryHelper.cyborgsRealValueDuringThisTurn -= cyborgsAfterTroopsCalcul*(-1);
								myFactory.cyborgsRealValueDuringThisTurn += cyborgsAfterTroopsCalcul*(-1);
							}
							else {
								output += "MOVE " + std::to_string(factoryHelper.id) + " " + std::to_string(myFactory.id) + " " + std::to_string(factoryHelper.cyborgs) + ";";
								myFactory.cyborgsRealValueDuringThisTurn -= factoryHelper.cyborgsRealValueDuringThisTurn;
								factoryHelper.cyborgsRealValueDuringThisTurn = 0;
							}

						}

					}
				}
			}
		}
	}


	std::vector<Factory> targetFactories = selectTargetFactory(factories);

	// attack

	for (Factory &target : targetFactories) {

		for (Factory &mine : sourceFactories) {

			if (mine.cyborgs > 10 && mine.production < 3)
			{
				mine.cyborgsRealValueDuringThisTurn -= 10;
				output += "INC " + std::to_string(mine.id) + ";";
			}

			if (target.id != mine.id) {

				int targetCyborgs = calculateFactoryCyborgsAt(target, getDistance(target, mine) + 1, troops);

				std::cerr << "target : " << target.id << std::endl;
				std::cerr << "troops : " << target.cyborgs << std::endl;
				std::cerr << "total : " << targetCyborgs << std::endl;
				std::cerr << "------------------------------------" << std::endl << std::endl;



				if (targetCyborgs >= 0 && mine.cyborgsRealValueDuringThisTurn > 0) {


					int totalSourcesCyborgs = std::accumulate(sourceFactories.begin(), sourceFactories.end(), 0, [](int sum, const Factory & sourceFactory) { return sum + sourceFactory.cyborgsRealValueDuringThisTurn; });

					if (totalSourcesCyborgs > targetCyborgs) {

						if (mine.cyborgs > targetCyborgs) {
							output += "MOVE " + std::to_string(mine.id) + " " + std::to_string(target.id) + " " + std::to_string(targetCyborgs + 1) + ";";
							target.cyborgsRealValueDuringThisTurn -= targetCyborgs + 1;

							std::cerr << "my troops before : " << mine.cyborgsRealValueDuringThisTurn << std::endl;
							mine.cyborgsRealValueDuringThisTurn -= targetCyborgs + 1;

							std::cerr << "my troops after : " << mine.cyborgsRealValueDuringThisTurn << std::endl;
						}
						else {
							output += "MOVE " + std::to_string(mine.id) + " " + std::to_string(target.id) + " " + std::to_string(mine.cyborgs) + ";";
							target.cyborgsRealValueDuringThisTurn -= mine.cyborgsRealValueDuringThisTurn;
							mine.cyborgsRealValueDuringThisTurn = 0;
						}

					}

				}

			}
		}
	}
	if (output == "") {
		output = "WAIT;";
	}
	output += "MSG test";

	return output;
}


/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/
int main()
{
	int factoryCount; // the number of factories
	std::cin >> factoryCount;
	std::cin.ignore();

	int linkCount; // the number of links between factories
	std::cin >> linkCount;
	std::cin.ignore();


	for (int i = 0; i < linkCount; i++) {
		int factory1;
		int factory2;
		int distance;
		std::cin >> factory1 >> factory2 >> distance; std::cin.ignore();

		distances[factory1][factory2] = distance;
		distances[factory2][factory1] = distance;

	}




	// game loop
	while (1) {
		std::vector<Factory> factories;
		std::vector<Troop> troops;

		int entityCount; // the number of entities (e.g. factories and troops)
		std::cin >> entityCount; std::cin.ignore();
		for (int i = 0; i < entityCount; i++) {
			int entityId;
			std::string entityType;
			int arg1;
			int arg2;
			int arg3;
			int arg4;
			int arg5;
			std::cin >> entityId >> entityType >> arg1 >> arg2 >> arg3 >> arg4 >> arg5; std::cin.ignore();

			if (entityType == "FACTORY") {
				factories.push_back(Factory(entityId, argToOwner(arg1), arg2, arg3));
			}
			else if (entityType == "TROOP") {
				troops.push_back(Troop(arg1 ? OWNER::ME : OWNER::OPPONENT, arg2, arg3, arg4, arg5));
			}
		}

		std::cout << generateActions(factories, troops) << std::endl;

	}
}
