// Copyright Epic Games, Inc. All Rights Reserved.

#include "BatchRenameToolCommands.h"

#define LOCTEXT_NAMESPACE "FBatchRenameToolModule"

void FBatchRenameToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenTool, "BatchRenameTool", "Bring up BatchRenameTool window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
