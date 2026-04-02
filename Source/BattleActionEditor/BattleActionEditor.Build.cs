
using UnrealBuildTool;

public class BattleActionEditor : ModuleRules
{
    public BattleActionEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        OverridePackageType = PackageOverrideType.GameUncookedOnly;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]{
                "Core",
                "CoreUObject",
                "Engine",
                "EditorFramework",
                "PhysicsCore",
                "GameplayTagsEditor",
                "DeveloperToolSettings",
                "UnrealEd",
                "AnimGraph",
                "BattleActionGame",
                "BlueprintGraph"
                

            }
        );
        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                
            });
    }


}