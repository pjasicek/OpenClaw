#ifndef CONFIG_H_
#define CONFIG_H_

class ConfigMgr
{
public:
    static ConfigMgr* Instance();

    void Initialize();
    void Terminate();

    

private:
    ConfigMgr();
    ~ConfigMgr();

    uint32_t _musicVolume;

};

#define sConfigMgr ConfigMgr::Instance()

#endif