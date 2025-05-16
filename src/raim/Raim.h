#pragma once

#include <memory>
#include <chrono>
#include <future>

#include <GLFW/glfw3.h>


class Config;
class RaimUI;
class TCPGecko;

class Raim
{
private:
    GLFWwindow *mWindow;
    RaimUI *mRaimUI;
    std::shared_ptr<Config> mConfig;
    std::shared_ptr<TCPGecko> mTCPGecko;
    const char *mConfigPath;
    std::future<void> mSaveFuture;

    std::chrono::steady_clock::time_point mLastSaveTime;
    
public:
    Raim(GLFWwindow *window);
    ~Raim();

    void Init();
    void Update();

    void LoadFonts();

    RaimUI* getRaimUI() const { return mRaimUI; }
    std::shared_ptr<Config> getConfig() const { return mConfig; }
    std::shared_ptr<TCPGecko> getTCPGecko() const { return mTCPGecko; }
    const char *getConfigPath() const { return mConfigPath; }
    GLFWwindow *getWindow() const { return mWindow; }
};
