#ifndef __GAME_SAVES_H__
#define __GAME_SAVES_H__

#include "../SharedDefines.h"
#include "BaseGameApp.h"

#ifdef __EMSCRIPTEN__
#define IS_SAVE_SUPPORTED false
#else
#define IS_SAVE_SUPPORTED true
#endif

// This can be refactored into classes, but this is not a pressing concern for me right now
// as it works as intended

const uint32 LEVELS_COUNT = 14;

// TODO: Should be data driven
Point GetSpawnPosition(uint32 levelNumber, uint32 checkpointNumber);

struct CheckpointSave
{
    CheckpointSave()
    {
        checkpointIdx = 0;
        score = 0;
        health = 0;
        lives = 0;
        bulletCount = 0;
        magicCount = 0;
        dynamiteCount = 0;
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pCheckpointXml = new TiXmlElement("Checkpoint");
        XML_ADD_TEXT_ELEMENT("CheckpointNumber", ToStr(checkpointIdx).c_str(), pCheckpointXml);
        XML_ADD_TEXT_ELEMENT("Score", ToStr(score).c_str(), pCheckpointXml);
        XML_ADD_TEXT_ELEMENT("Health", ToStr(health).c_str(), pCheckpointXml);
        XML_ADD_TEXT_ELEMENT("Lives", ToStr(lives).c_str(), pCheckpointXml);
        XML_ADD_TEXT_ELEMENT("BulletCount", ToStr(bulletCount).c_str(), pCheckpointXml);
        XML_ADD_TEXT_ELEMENT("MagicCount", ToStr(magicCount).c_str(), pCheckpointXml);
        XML_ADD_TEXT_ELEMENT("DynamiteCount", ToStr(dynamiteCount).c_str(), pCheckpointXml);

        return pCheckpointXml;
    }

    void LoadFromXml(TiXmlElement* pCheckpointXml)
    {
        assert(pCheckpointXml);

        if (TiXmlElement* pElem = pCheckpointXml->FirstChildElement("CheckpointNumber"))
        {
            checkpointIdx = std::stoi(pElem->GetText());
        }
        if (TiXmlElement* pElem = pCheckpointXml->FirstChildElement("Score"))
        {
            score = std::stoi(pElem->GetText());
        }
        if (TiXmlElement* pElem = pCheckpointXml->FirstChildElement("Health"))
        {
            health = std::stoi(pElem->GetText());
        }
        if (TiXmlElement* pElem = pCheckpointXml->FirstChildElement("Lives"))
        {
            lives = std::stoi(pElem->GetText());
        }
        if (TiXmlElement* pElem = pCheckpointXml->FirstChildElement("BulletCount"))
        {
            bulletCount = std::stoi(pElem->GetText());
        }
        if (TiXmlElement* pElem = pCheckpointXml->FirstChildElement("MagicCount"))
        {
            magicCount = std::stoi(pElem->GetText());
        }
        if (TiXmlElement* pElem = pCheckpointXml->FirstChildElement("DynamiteCount"))
        {
            dynamiteCount = std::stoi(pElem->GetText());
        }

        assert(checkpointIdx <= 2);
    }

    // 0 - start / new game, 
    // 1 - first checkpoint
    // 2 - second checkpoint
    uint32 checkpointIdx; 

    uint32 score;
    uint32 health;
    uint32 lives;
    uint32 bulletCount;
    uint32 magicCount;
    uint32 dynamiteCount;
};

struct LevelSave
{
    LevelSave()
    {
        levelNumber = -1; // Undefined
    }

    LevelSave(uint32 levelNumber, std::string levelName)
    {
        this->levelNumber = levelNumber;
        this->levelName = levelName;
    }

    LevelSave(TiXmlElement* pLevelXml)
    {
        LoadFromXml(pLevelXml);
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pLevelXml = new TiXmlElement("Level");
        XML_ADD_TEXT_ELEMENT("LevelNumber", ToStr(levelNumber).c_str(), pLevelXml);
        XML_ADD_TEXT_ELEMENT("LevelName", levelName.c_str(), pLevelXml);
        for (auto saveIter : checkpointMap)
        {
            pLevelXml->LinkEndChild(saveIter.second.ToXml());
        }

        return pLevelXml;
    }

    void LoadFromXml(TiXmlElement* pLevelXml)
    {
        if (TiXmlElement* pElem = pLevelXml->FirstChildElement("LevelNumber"))
        {
            levelNumber = std::stoi(pElem->GetText());
        }

        if (TiXmlElement* pElem = pLevelXml->FirstChildElement("LevelName"))
        {
            levelName = pElem->GetText();
        }

        for (TiXmlElement* pCheckpoint = pLevelXml->FirstChildElement("Checkpoint");
            pCheckpoint; pCheckpoint = pCheckpoint->NextSiblingElement("Checkpoint"))
        {
            CheckpointSave save;
            save.LoadFromXml(pCheckpoint);
            checkpointMap[save.checkpointIdx] = save;
        }

        //assert(!levelName.empty());
        if (levelName.empty())
        {
            LOG_WARNING("Leve name is empty ! Level number: " + ToStr(levelNumber));
        }
        LOG(ToStr(levelNumber));
    }

    void AddCheckpointSave(CheckpointSave& checkpointSave)
    {
        checkpointMap[checkpointSave.checkpointIdx] = checkpointSave;
    }

    CheckpointSave* GetLastCheckpointSave()
    {
        assert(!checkpointMap.empty());

        // Little bit unlucky when using map
        uint32 lastCheckpoint = 0;

        for (auto checkpointIter : checkpointMap)
        {
            if (checkpointIter.second.checkpointIdx > lastCheckpoint)
            {
                lastCheckpoint = checkpointIter.second.checkpointIdx;
            }
        }

        return &checkpointMap[lastCheckpoint];
    }

    // Can return NULL
    CheckpointSave* GetCheckpointSave(uint32 checkpointNumber)
    {
        auto findIt = checkpointMap.find(checkpointNumber);
        if (findIt == checkpointMap.end())
        {
            return NULL;
        }

        return &(findIt->second);
    }

    uint32 levelNumber;
    std::string levelName;
    std::map<uint32, CheckpointSave> checkpointMap;
};

typedef std::map<uint32, LevelSave> LevelSaveMap;
class GameSaveMgr
{
public:
    bool Initialize(TiXmlElement* pGameSaveData)
    {
        if (g_pApp->GetGlobalOptions()->loadAllLevelSaves || !IsSaveSupported())
        {
            InitializeWithAllLevels();
            return true;
        }

        if (!pGameSaveData)
        {
            return false;
        }

        for (TiXmlElement* pLevel = pGameSaveData->FirstChildElement("Level");
            pLevel; pLevel = pLevel->NextSiblingElement("Level"))
        {
            LevelSave levelSave(pLevel);
            assert(levelSave.levelNumber <= LEVELS_COUNT);

            m_LevelSaveMap[levelSave.levelNumber] = levelSave;
        }

        if (m_LevelSaveMap.empty())
        {
            LOG_ERROR("No saves were found. There should always be atleast first level.");
            return false;
        }

        return true;
    }

    void InitializeWithAllLevels()
    {
        int lastLevel = 13;

        CheckpointSave fullCheckpoint0;
        fullCheckpoint0.bulletCount = 15;
        fullCheckpoint0.checkpointIdx = 0;
        fullCheckpoint0.dynamiteCount = 10;
        fullCheckpoint0.health = 100;
        fullCheckpoint0.lives = 3;
        fullCheckpoint0.magicCount = 5;
        fullCheckpoint0.score = 0;

        CheckpointSave fullCheckpoint1;
        fullCheckpoint1.bulletCount = 15;
        fullCheckpoint1.checkpointIdx = 1;
        fullCheckpoint1.dynamiteCount = 10;
        fullCheckpoint1.health = 100;
        fullCheckpoint1.lives = 3;
        fullCheckpoint1.magicCount = 5;
        fullCheckpoint1.score = 0;

        CheckpointSave fullCheckpoint2;
        fullCheckpoint2.bulletCount = 15;
        fullCheckpoint2.checkpointIdx = 2;
        fullCheckpoint2.dynamiteCount = 10;
        fullCheckpoint2.health = 100;
        fullCheckpoint2.lives = 3;
        fullCheckpoint2.magicCount = 5;
        fullCheckpoint2.score = 0;

        for (int levelIdx = 1; levelIdx <= lastLevel; levelIdx++)
        {
            LevelSave levelSave;
            switch (levelIdx)
            {
                case 1: levelSave.levelName = "La Roca"; break;
                case 2: levelSave.levelName = "Battlements"; break;
                case 3: levelSave.levelName = "Thief's Forest"; break;
                case 4: levelSave.levelName = "Dark Woods"; break;
                case 5: levelSave.levelName = "Town"; break;
                case 6: levelSave.levelName = "Puerto De Lobo"; break;
                case 7: levelSave.levelName = "Docks"; break;
                case 8: levelSave.levelName = "Shipyards"; break;
                case 9: levelSave.levelName = "Pirate's Cove"; break;
                case 10: levelSave.levelName = "Cliffs"; break;
                case 11: levelSave.levelName = "Caverns"; break;
                case 12: levelSave.levelName = "Undersea Caves"; break;
                case 13: levelSave.levelName = "Tiger Island"; break;
                default: assert(false); break;
            }
            levelSave.levelNumber = levelIdx;

            levelSave.checkpointMap[0] = fullCheckpoint0;
            levelSave.checkpointMap[1] = fullCheckpoint1;
            levelSave.checkpointMap[2] = fullCheckpoint2;

            m_LevelSaveMap[levelIdx] = levelSave;
        }
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pGameSaves = new TiXmlElement("GameSaves");
        for (auto levelSave : m_LevelSaveMap)
        {
            pGameSaves->LinkEndChild(levelSave.second.ToXml());
        }

        return pGameSaves;
    }

    void AddLevelEntry(uint32 levelNumber, std::string levelName)
    {
        assert(levelNumber <= LEVELS_COUNT);

        m_LevelSaveMap[levelNumber] = LevelSave(levelNumber, levelName);
    }

    void AddCheckpointSave(uint32 levelNumber, CheckpointSave& save)
    {
        assert(levelNumber <= LEVELS_COUNT);

        auto findIt = m_LevelSaveMap.find(levelNumber);
        if (findIt == m_LevelSaveMap.end())
        {
            LOG_ERROR("Creating checkpoint for level save which is unitialized !");
            m_LevelSaveMap[levelNumber] = LevelSave(levelNumber, "Unknown");
        }

        m_LevelSaveMap[levelNumber].checkpointMap[save.checkpointIdx] = save;
    }

    // Can return NULL
    LevelSave* GetLevelSave(uint32 levelNumber)
    {
        assert(levelNumber <= LEVELS_COUNT);

        auto findIt = m_LevelSaveMap.find(levelNumber);
        if (findIt == m_LevelSaveMap.end())
        {
            return NULL;
        }

        return &(findIt->second);
    }

    bool HasLevelSave(uint32 levelNumber)
    {
        return (GetLevelSave(levelNumber) != NULL);
    }

    // Can return NULL
    const CheckpointSave* GetCheckpointSave(uint32 levelNumber, uint32 checkpointNumber)
    {
        LevelSave* pLevelSave = GetLevelSave(levelNumber);
        if (pLevelSave == NULL)
        {
            return NULL;
        }
        
        return pLevelSave->GetCheckpointSave(checkpointNumber);
    }

    bool HasCheckpointSave(uint32 levelNumber, uint32 checkpointNumber)
    {
        return (GetCheckpointSave(levelNumber, checkpointNumber) != NULL);
    }

    bool IsSaveSupported()
    {
        return IS_SAVE_SUPPORTED;
    }

private:
    LevelSaveMap m_LevelSaveMap;
};


#endif
