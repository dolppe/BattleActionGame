#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FCommonGameModule : public IModuleInterface
{
public:
    FCommonGameModule();
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:

};

FCommonGameModule::FCommonGameModule()
{

}


void FCommonGameModule::StartupModule()
{
	
}

void FCommonGameModule::ShutdownModule()
{
	
}

IMPLEMENT_MODULE(FCommonGameModule, CommonGame);

