// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "BatchRenameToolStyle.h"

class FBatchRenameToolCommands : public TCommands<FBatchRenameToolCommands>
{
public:

	FBatchRenameToolCommands()
		: TCommands<FBatchRenameToolCommands>(TEXT("BatchRenameTool"), NSLOCTEXT("Contexts", "BatchRenameTool", "BatchRenameTool Plugin"), NAME_None, FBatchRenameToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenTool;
};