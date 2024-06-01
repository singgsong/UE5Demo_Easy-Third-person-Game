// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Demo_1 : ModuleRules
{
	public Demo_1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "AIModule", "MotionWarping" });
	}
}
