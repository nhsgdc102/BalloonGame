// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CP_MainMenu.generated.h"

class UWidget;
class UTextBlock;

/**
 * 
 */
UCLASS()
class BALLOONGAME_API UCP_MainMenu : public UUserWidget
{
	GENERATED_BODY()

/*Sub-Widgets*/
protected:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TopRound;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TopScore;

protected:

	// Called after widget is constructed
	virtual void NativeConstruct() override;

	/*Title color scroll*/
	UFUNCTION(BlueprintPure)
		// Helper function to handle modifying title 
		FText GetScrolledTitleText(int32 InScrollPosition, int32 InTitleStyle) const;

	/*Miscellaneous*/
	UFUNCTION(BlueprintCallable)
		// Conviently hides or displays a list of widgets based on their current visibility
		void ToggleVisibility(TArray<UWidget*> InWidgetArray);
	UFUNCTION(BlueprintCallable)
		// Conviently clears a list of timer handles
		void ClearTimerHandles(TArray<FTimerHandle> InTimerHandleArray);
};

/*
private:
	/*Widget Animations->http://benhumphreys.ca/unreal/ui-animations-from-cpp/
	// Helper function to fill up AnimationMap with animations
	void FillAnimationMap();
	// Getter function for retrieving corresponding animation
	UWidgetAnimation* GetAnimationByName(FName AnimName) const;
	// Wrapper function for playing widget; handles validation
	bool PlayAnimationByName(FName AnimName,
		float StartAtTime,
		int32 NumLoopsToPlay,
		EUMGSequencePlayMode::Type PlayMode,
		float PlaybackSpeed);
	*/
	/*
	UFUNCTION()
		// Retrieves leaderboard data
		void OnAsyncLoadRecord(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
	*/

	/*
protected:
	/*Widget Animations
	// Returns animation corresponding to the name
	TMap<FName, UWidgetAnimation*> AnimationMap;
*/
