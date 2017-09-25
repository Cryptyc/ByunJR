#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"
#include "rapidjson/document.h"
#include "JSONTools.h"
#include "Util.h"

#include <iostream>
#include <string>
#include <random>
#include <cmath>

#include "ByunJRBot.h"

//#include <conio.h>


int main(int argc, char* argv[]) 
{

    rapidjson::Document doc;
    std::string config = JSONTools::ReadFile("BotConfig.txt");
    if (config.length() == 0)
    {
        std::cerr << "Config file could not be found, and is required for starting the bot\n";
        std::cerr << "Please read the instructions and try again\n";
        exit(-1);
    }

    bool parsingFailed = doc.Parse(config.c_str()).HasParseError();
    if (parsingFailed)
    {
        std::cerr << "Config file could not be parsed, and is required for starting the bot\n";
        std::cerr << "Please read the instructions and try again\n";
        exit(-1);
    }

    std::string botRaceString;
    std::string enemyRaceString;
    std::string mapString;

    if (doc.HasMember("Game Info") && doc["Game Info"].IsObject())
    {
        const rapidjson::Value & info = doc["Game Info"];
        JSONTools::ReadString("BotRace", info, botRaceString);
        JSONTools::ReadString("EnemyRace", info, enemyRaceString);
        JSONTools::ReadString("MapFile", info, mapString);
    }
    else
    {
        std::cerr << "Config file has no 'Game Info' object, required for starting the bot\n";
        std::cerr << "Please read the instructions and try again\n";
        exit(-1);
    }
    std::cout << mapString << std::endl;


    std::cout << "GLHF" << std::endl;
    // Step forward the game simulation.

    while (true) {
        sc2::Coordinator coordinator;
        if (!coordinator.LoadSettings(argc, argv))
        {
            std::cout << "Unable to find or parse settings." << std::endl;
            return 1;
        }

        coordinator.SetRealtime(false);

        // WARNING: Bot logic has not been thorougly tested on step sizes > 1
        //          Setting this = N means the bot's onFrame gets called once every N frames
        //          The bot may crash or do unexpected things if its logic is not called every frame
        coordinator.SetStepSize(2);

        // Add the custom bot, it will control the players.
        ByunJRBot bot;

        coordinator.SetParticipants({
                                            CreateParticipant(Util::GetRaceFromString(botRaceString), &bot),
                                            CreateComputer(Util::GetRaceFromString(enemyRaceString))
                                    });

        // Start the game.
        coordinator.LaunchStarcraft();
        coordinator.StartGame("AbyssalReefLE.SC2Map");
        while (coordinator.AllGamesEnded() != true && bot.IsWillingToFight())
        {
            coordinator.Update();
        }
        if (bot.Control()->SaveReplay("replay/asdf.Sc2Replay"))
        {
            std::cout << "REPLAYSUCESS" << "replay/asdf.Sc2Replay";
        }
        else
        {
            std::cout << "REPLAY FAIL" << "replay/asdf.Sc2Replay";
        }
        coordinator.LeaveGame();


    }


    std::cout << "Press any key to continue.";
    //getchar();

    return 0;
}
