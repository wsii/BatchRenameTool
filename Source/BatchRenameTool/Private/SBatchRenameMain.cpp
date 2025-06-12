// Fill out your copyright notice in the Description page of Project Settings.


#include "SBatchRenameMain.h"

#include "AssetToolsModule.h"
#include "SlateOptMacros.h"
#include "Styling/StyleColors.h"

#include "BatchRenameToolModel.h"
#include "EditorUtilityLibrary.h"
#include "IAssetTools.h"
#include "SBatchRenamingOperationList.h"
#include "SBatchRenamingOperationDetails.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "BatchRenameTool"

SBatchRenameMain::SBatchRenameMain():
	Model(MakeShared<FBatchRenameToolModel>())
{
}

void SBatchRenameMain::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		// Populate the widget

		SNew(SBox)
		.MaxDesiredWidth(1400.0f)
		.MaxDesiredHeight(500.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SSplitter)
			//left
			.PhysicalSplitterHandleSize(3.0f)
			.Style(FAppStyle::Get(), "DetailsView.Splitter")
			+ SSplitter::Slot().Value(0.7f)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.AutoWidth()
					[
						ConstructOperationButton()
					]
				]
				+SVerticalBox::Slot()

				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.AutoHeight()
				[
					ConstructAssetTable()
				]
			]

			//Right
			+ SSplitter::Slot().Value(0.3f)
			[
					SNew(SSplitter)
					.Orientation(EOrientation::Orient_Vertical)
					.PhysicalSplitterHandleSize(3.0f)
					.Style(FAppStyle::Get(), "DetailsView.Splitter")
					+ SSplitter::Slot().Value(0.6f)
					[
						SAssignNew(OperationList, SBatchRenamingOperationList, Model)
					]
					+ SSplitter::Slot().Value(0.4f)
					[
						SNew(SBatchRenamingOperationDetails, Model)
					]
					
				]
			]
	];

}

FString SBatchRenameMain::ApplyOperations(const FString& Str) const
{
	FString StrCopy = Str;
	for(const TObjectPtr<UBatchRenamingOperation>& Rule: Model->GetOperations())
	{
		StrCopy = Rule->Apply(StrCopy);
	}
	return StrCopy;
}

void SBatchRenameMain::OnOperationsChanged()
{
	for (const TSharedPtr<FAssetTableRowData>& Asset : AssetTableRows)
	{
		Asset->NewFilename = ApplyOperations(Asset->Filename);
		UE_LOG(LogTemp, Warning, TEXT("Selected Asset %s"), *Asset->NewFilename);
	}
	AssetTable->SetRows(AssetTableRows);
	
	AssetTable->Refresh();
}

void SBatchRenameMain::RefreshAssetTableRows()
{
	//清空 AssetTableRows
	AssetTableRows.Empty();
	for (FAssetData& AssetData : HasSelectedAssets)
	{
		UObject* ActualAsset = AssetData.GetAsset();
		if (ActualAsset)
		{
			TSharedPtr<FAssetTableRowData> RowData = MakeShared<FAssetTableRowData>(ActualAsset->GetName(),ActualAsset->GetName(), ActualAsset->GetPathName());
			
			AssetTableRows.Add(RowData);
			
		}
	}
	
	AssetTable->SetRows(AssetTableRows);
	//重新 应用修改
	OnOperationsChanged();
	
	if(AssetTable.IsValid())
	{
		AssetTable->Refresh();
	}
}
//添加table上面的按钮
TSharedRef<SHorizontalBox> SBatchRenameMain::ConstructOperationButton()
{
	Model->OnOperationListModified().AddSP(this, &SBatchRenameMain::OnOperationsChanged);

	TSharedRef<SHorizontalBox> HorizontalButton =
		SNew(SHorizontalBox)

		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		.Padding(FVector2D(2.f, 0.f))
		[
			SNew(SButton)
			.ContentPadding(FMargin(FVector2D(20.f, 10.f)))
			.OnClicked_Raw(this,&SBatchRenameMain::OnAddAssets)
			[
				SNew(SHorizontalBox)
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					// .Image(FAppStyle::Get().GetBrush(TEXT("DataTableEditor.Add.Small")))
					.Image(FAppStyle::Get().GetBrush("Icons.Plus"))
					.ColorAndOpacity(FStyleColors::AccentGreen)
				]
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("添加文件")))
				]
			]
		]
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		.Padding(FVector2D(2.f, 0.f))
		[
			SNew(SButton)
			.ContentPadding(FMargin(FVector2D(20.f, 10.f)))
			.OnClicked_Raw(this, &SBatchRenameMain::OnDeleteSelect)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					// .Image(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Minus"))
					.Image(FAppStyle::Get().GetBrush(TEXT("Icons.Minus")))
					.ColorAndOpacity(FStyleColors::AccentBlue)
					
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[

					SNew(STextBlock).Text(FText::FromString(TEXT("移除选中项")))

				]
			]
		]

		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		.Padding(FVector2D(2.f, 0.f))
		[
			SNew(SButton)
			.ContentPadding(FMargin(FVector2D(20.f, 10.f)))
			.OnClicked_Raw(this, &SBatchRenameMain::OnClearTable)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)

					.Image(FAppStyle::Get().GetBrush(TEXT("Icons.Delete")))
					.ColorAndOpacity(FStyleColors::AccentYellow)
					
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(FMargin(3.0f,0.f,0.f,0.f))
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("清空列表")))
				]
			]
		]

		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		.Padding(FVector2D(2.f, 0.f))
		[
			SNew(SButton)
			.ContentPadding(FMargin(FVector2D(20.f, 10.f)))
			.OnClicked_Raw(this, &SBatchRenameMain::Run)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					// .Image(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Play"))
					.Image(FAppStyle::Get().GetBrush(TEXT("Icons.Play")))
					.ColorAndOpacity(FStyleColors::AccentRed)
					
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(FMargin(3.0f,0.f,0.f,0.f))
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("运行")))
				]
			]
		];
		return HorizontalButton;
}

TSharedRef<SBatchRenameAssetTable> SBatchRenameMain::ConstructAssetTable()
{
	SAssignNew(AssetTable, SBatchRenameAssetTable, AssetTableRows);
	
	return AssetTable.ToSharedRef();
}

FReply SBatchRenameMain::OnAddAssets()
{
	SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();

	for (FAssetData& AssetData : SelectedAssets)
	{
		UObject* ActualAsset = AssetData.GetAsset();
		if (ActualAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected Asset %s"), *ActualAsset->GetName());
			if (!HasSelectedAssets.Contains(AssetData))
			{
				HasSelectedAssets.Add(AssetData);
			}
		}
	}

	RefreshAssetTableRows();

	return FReply::Handled();
}

FReply SBatchRenameMain::OnDeleteSelect()
{
	SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();

	for (FAssetData& AssetData : SelectedAssets)
	{
		UObject* ActualAsset = AssetData.GetAsset();
		if (ActualAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("Remove Asset %s"), *ActualAsset->GetName());
			if (HasSelectedAssets.Contains(AssetData))
			{
				HasSelectedAssets.Remove(AssetData);
			}
		}
	}
	RefreshAssetTableRows();

	return FReply::Handled();
}

FReply SBatchRenameMain::OnClearTable()
{
	AssetTableRows.Empty();
	AssetTable->SetRows(AssetTableRows);

	HasSelectedAssets.Empty();
	
	if(AssetTable.IsValid())
	{
		AssetTable->Refresh();
	}
	return FReply::Handled();
}

FReply SBatchRenameMain::Run()
{
	if(!Model->GetOperations().IsEmpty())
	{
		const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		TArray<FAssetRenameData> AssetRenameData;

		for (FAssetData& AssetData : HasSelectedAssets)
		{
			UObject* Asset = AssetData.GetAsset();
			const FString PackagePath = FPackageName::GetLongPackagePath(Asset->GetPackage()->GetName());
			AssetRenameData.Add(FAssetRenameData(Asset, PackagePath, ApplyOperations(Asset->GetName())));
			
		}
		//更新资产命名
		AssetToolsModule.Get().RenameAssets(AssetRenameData);
	}
	// 清空列表
	OnClearTable();

	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE