#include "Raim.h"

#include <imgui.h>

#include <utils/TCPGecko.h>
#include <utils/Config.h>
#include "ui/RaimUI.h"
#include "ui/style/RaimUI_Theme.h"


bool WaitEvents = true;


Raim::Raim(GLFWwindow *window)
    : mWindow(window),
      mConfig(std::make_shared<Config>()),
      mTCPGecko(std::make_shared<TCPGecko>()),
      mLastSaveTime(std::chrono::steady_clock::now()),
      mConfigPath("zaqro_u/config.json")
{
    mRaimUI = new RaimUI(this);

    mConfig->load(mConfigPath);
}

Raim::~Raim()
{
    delete mRaimUI;
}

void Raim::Init()
{
    mRaimUI->Init();
}

void Raim::Update()
{
    mRaimUI->Update();

    // auto now = std::chrono::steady_clock::now();
    // if (std::chrono::duration_cast<std::chrono::seconds>(now - mLastSaveTime).count() >= 5 && !mConfig->empty())
    // {
    //     if (!mSaveFuture.valid() || mSaveFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    //     {
    //         mSaveFuture = std::async(std::launch::async, [config = mConfig, configPath = mConfigPath]() {
    //             config->save(configPath);
    //         });
    //         mLastSaveTime = now;
    //     }
    // }

    WaitEvents = getConfig()->get("wait_events", false);
}
