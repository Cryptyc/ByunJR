#pragma once
#include "macro/BuildOrder.h"

typedef std::pair<sc2::UnitTypeID, size_t>  UnitPair;
typedef std::vector<UnitPair>               UnitPairVector;

class ByunJRBot;

struct Strategy
{
    std::string m_name;
    sc2::Race   m_race;
    int         m_wins;
    int         m_losses;
    BuildOrder  m_buildOrder;

    Strategy();
    Strategy(const std::string & name, const sc2::Race & race, const BuildOrder & buildOrder);
};

class StrategyManager
{
    ByunJRBot & m_bot;

    sc2::Race                       m_selfRace;
    sc2::Race                       m_enemyRace;
    std::map<std::string, Strategy> m_strategies;
    int                             m_totalGamesPlayed;
    const BuildOrder                m_emptyBuildOrder;

    bool  shouldExpandNow() const;
    UnitPairVector getProtossBuildOrderGoal() const;
    UnitPairVector getTerranBuildOrderGoal() const;
    UnitPairVector getZergBuildOrderGoal() const;

public:

    StrategyManager(ByunJRBot & bot);

    void onStart();
    void onFrame();
    void onEnd(const bool isWinner);
    void addStrategy(const std::string & name, const Strategy & strategy);
    UnitPairVector getBuildOrderGoal() const;
    const BuildOrder & getOpeningBookBuildOrder() const;
    void readStrategyFile(const std::string & str);


    void handleUnitAssignments();

    bool shouldSendInitialScout() const;
};
