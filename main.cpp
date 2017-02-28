#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum OWNER
{
    ME, OPPONENT, NEUTRAL
};

struct Factory
{
    int id;
    OWNER owner;
    int cyborgs;
    int production;

    Factory(int id, OWNER owner, int cyborgs, int production) : id(id), owner(owner), cyborgs(cyborgs), production(production)
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

Factory selectTargetFactory(const std::vector<Factory> & factories) {
    std::vector<Factory> otherFactories;
    std::copy_if(factories.begin(), factories.end(), std::back_inserter(otherFactories), IsOther());

    return *std::max_element(otherFactories.begin(), otherFactories.end(), [](const Factory & factory1, const Factory & factory2) { 
                                                                        return factory1.production < factory2.production; });
}

Factory selectSourceFactory(const std::vector<Factory> & factories) {
    std::vector<Factory> sourceFactories;
    std::copy_if(factories.begin(), factories.end(), std::back_inserter(sourceFactories), [](const Factory & factory) { return factory.owner == OWNER::ME; });

    return *std::max_element(sourceFactories.begin(), sourceFactories.end(), [](const Factory & factory1, const Factory & factory2) { 
                                                                        return factory1.cyborgs < factory2.cyborgs; });
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
    }


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
            } else if (entityType == "TROOP") {

            }
        }

        const Factory & targetFactory = selectTargetFactory(factories);

        const Factory & sourceFactory = selectSourceFactory(factories);

        // To debug: cerr << "Debug messages..." << endl;


        cout << "MOVE" << sourceFactory.id << targetFactory.id << sourceFactory.cyborgs / 2 << endl;
    }
}
