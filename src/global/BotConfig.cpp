#include <iostream>

#include "common/BotAssert.h"
#include "global/BotConfig.h"
#include "rapidjson/document.h"
#include "util/JSONTools.h"
#include "util/Util.h"
#include "StrategyManager.h"


BotConfig::BotConfig()
{
    RawConfigString                     = "";
    ConfigFileFound                     = true;
    ConfigFileParsed                    = true;
    ConfigFileLocation                  = "BotConfig.txt";
    BotName                             = "ByunJR";
    Authors                             = "Ian Gallacher & David Churchill";
    PrintInfoOnStart                    = false;
    StrategyName                        = "Terran_ReaperRush";
    MapName                             = "InterloperLE.SC2Map";
    ReadDir                             = "read/";
    WriteDir                            = "write/";
    TrainingMode                        = false;
    UseEnemySpecificStrategy            = false;
    FoundEnemySpecificStrategy          = false;
    UsingAutoObserver                   = false;

    // Micro
    KiteWithRangedUnits = true;
    ScoutHarassEnemy = true;
    CombatUnitsForAttack = 12;

    // Debug 
    DrawGameInfo                        = true;
    DrawProductionInfo                  = true;
    DrawTileInfo                        = false;
    DrawWalkableSectors                 = false;
    DrawScoutInfo                       = false;
    DrawResourceInfo                    = false;
    DrawWorkerInfo                      = false;
    DrawModuleTimers                    = false;
    DrawReservedBuildingTiles           = false;
    DrawBuildingInfo                    = false;
    DrawEnemyUnitInfo                   = false;
    DrawLastSeenTileInfo                = false;
    DrawUnitTargetInfo                  = false;
    DrawSquadInfo                       = false;

    ColorLineTarget                     = sc2::Colors::White;
    ColorLineMineral                    = sc2::Colors::Teal;
    ColorUnitNearEnemy                  = sc2::Colors::Red;
    ColorUnitNotNearEnemy               = sc2::Colors::Green;
    
    BuildingSpacing                     = 1;

    ProxyLocationX                      = 0;
    ProxyLocationY                      = 0;
}

void BotConfig::readConfigFile()
{
    rapidjson::Document doc;

    RawConfigString = JSONTools::ReadFile(ConfigFileLocation);

    if (RawConfigString.length() == 0)
    {
        std::cerr << "Error: Config File Not Found or is Empty\n";
        std::cerr << "Config Filename: " << ConfigFileLocation << std::endl;
        std::cerr << "The bot will not run without its configuration file\n";
        std::cerr << "Please check that the file exists and is not empty. Incomplete paths are relative to the bot .exe file\n";
        std::cerr << "You can change the config file location in Config::ConfigFile::ConfigFileLocation\n";
        ConfigFileFound = false;
        return;
    }

    const bool parsingFailed = doc.Parse(RawConfigString.c_str()).HasParseError();
    if (parsingFailed)
    {
        std::cerr << "Error: Config File Found, but could not be parsed\n";
        std::cerr << "Config Filename: " << ConfigFileLocation << std::endl;
        std::cerr << "The bot will not run without its configuration file\n";
        std::cerr << "Please check that the file exists, is not empty, and is valid JSON. Incomplete paths are relative to the bot .exe file\n";
        std::cerr << "You can change the config file location in Config::ConfigFile::ConfigFileLocation\n";
        ConfigFileParsed = false;
        return;
    }

    // Parse the Bot Info
    if (doc.HasMember("Bot Info") && doc["Bot Info"].IsObject())
    {
        const rapidjson::Value & info = doc["Bot Info"];
        JSONTools::ReadString("BotName", info, BotName);
        JSONTools::ReadString("Authors", info, Authors);
        JSONTools::ReadBool("PrintInfoOnStart", info, PrintInfoOnStart);
    }

    // Parse some of the Game Info
    if (doc.HasMember("Game Info") && doc["Game Info"].IsObject())
    {
        const rapidjson::Value & gameInfo = doc["Game Info"];
        JSONTools::ReadString("MapName", gameInfo, MapName);
    }

    // Parse the Micro Options
    if (doc.HasMember("Micro") && doc["Micro"].IsObject())
    {
        const rapidjson::Value & micro = doc["Micro"];
        JSONTools::ReadBool("KiteWithRangedUnits", micro, KiteWithRangedUnits);
        JSONTools::ReadBool("ScoutHarassEnemy", micro, ScoutHarassEnemy);
        JSONTools::ReadInt("CombatUnitsForAttack", micro, CombatUnitsForAttack);
    }

    // Parse the Debug Options
    if (doc.HasMember("Debug") && doc["Debug"].IsObject())
    {
        const rapidjson::Value & debug = doc["Debug"];
        JSONTools::ReadBool("DrawGameInfo",             debug, DrawGameInfo);
        JSONTools::ReadBool("DrawTileInfo",             debug, DrawTileInfo);
        JSONTools::ReadBool("DrawBaseLocationInfo",     debug, DrawBaseLocationInfo);
        JSONTools::ReadBool("DrawWalkableSectors",      debug, DrawWalkableSectors);
        JSONTools::ReadBool("DrawResourceInfo",         debug, DrawResourceInfo);
        JSONTools::ReadBool("DrawWorkerInfo",           debug, DrawWorkerInfo);
        JSONTools::ReadBool("DrawProductionInfo",       debug, DrawProductionInfo);
        JSONTools::ReadBool("DrawScoutInfo",            debug, DrawScoutInfo);
        JSONTools::ReadBool("DrawSquadInfo",            debug, DrawSquadInfo);
        JSONTools::ReadBool("DrawBuildingInfo",         debug, DrawBuildingInfo);
        JSONTools::ReadBool("DrawModuleTimers",         debug, DrawModuleTimers);
        JSONTools::ReadBool("DrawEnemyUnitInfo",        debug, DrawEnemyUnitInfo);
        JSONTools::ReadBool("DrawLastSeenTileInfo",     debug, DrawLastSeenTileInfo);
        JSONTools::ReadBool("DrawUnitTargetInfo",       debug, DrawUnitTargetInfo);
        JSONTools::ReadBool("DrawReservedBuildingTiles",debug, DrawReservedBuildingTiles);
    }

    // Parse the Module Options
    if (doc.HasMember("Modules") && doc["Modules"].IsObject())
    {
        const rapidjson::Value & module = doc["Modules"];

        JSONTools::ReadBool("UseAutoObserver", module, UsingAutoObserver);
    }


    // Yes, the playerRace was already parsed in main.cpp.
    // This data does not need to be stored in the config object.
    // We still need to parse it again so that we can get the strategy from the config text file. 
    std::string playerRace;
    if (doc.HasMember("Game Info") && doc["Game Info"].IsObject())
    {
        const rapidjson::Value & info = doc["Game Info"];
        JSONTools::ReadString("BotRace", info, playerRace);
    }

    // Parse the Strategy Options
    if (doc.HasMember("Strategy") && doc["Strategy"].IsObject())
    {
        const rapidjson::Value & strategy = doc["Strategy"];

        // read in the various strategic elements
        JSONTools::ReadBool("TrainingMode", strategy, TrainingMode);
        JSONTools::ReadBool("ScoutHarassEnemy", strategy, ScoutHarassEnemy);
        JSONTools::ReadString("ReadDirectory", strategy, ReadDir);
        JSONTools::ReadString("WriteDirectory", strategy, WriteDir);

        // if we have set a strategy for the current race, use it
        if (strategy.HasMember(playerRace.c_str()) && strategy[playerRace.c_str()].IsString())
        {
            StrategyName = strategy[playerRace.c_str()].GetString();
        }

        // check if we are using an enemy specific strategy
        JSONTools::ReadBool("UseEnemySpecificStrategy", strategy, UseEnemySpecificStrategy);

        if (UseEnemySpecificStrategy && strategy.HasMember("EnemySpecificStrategy") && strategy["EnemySpecificStrategy"].IsObject())
        {
            // TODO: Figure out enemy name
            const std::string enemyName = "ENEMY NAME";
            const rapidjson::Value & specific = strategy["EnemySpecificStrategy"];

            // check to see if our current enemy name is listed anywhere in the specific strategies
            if (specific.HasMember(enemyName.c_str()) && specific[enemyName.c_str()].IsObject())
            {
                const rapidjson::Value & enemyStrategies = specific[enemyName.c_str()];

                // if that enemy has a strategy listed for our current race, use it
                if (enemyStrategies.HasMember(playerRace.c_str()) && enemyStrategies[playerRace.c_str()].IsString())
                {
                    StrategyName = enemyStrategies[playerRace.c_str()].GetString();
                    FoundEnemySpecificStrategy = true;
                }
            }
        }
    }
}

void BotConfig::setProxyLocation(const int x, const int y)
{
    ProxyLocationX = x;
    ProxyLocationY = y;
}