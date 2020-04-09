// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CP_MainMenu.generated.h"


class UImage;
class URichTextBlock;
class UTextBlock;
class UHorizontalBox;
class USaveGame;

/**
 * 
 */
UCLASS()
class BALLOONGAME_API UCP_MainMenu : public UUserWidget
{
	GENERATED_BODY()

/*Widgets*/
protected:
	UPROPERTY(meta = (BindWidget))
		UImage* BackgroundColor;
	UPROPERTY(meta = (BindWidget))
		UImage* ScreenBorder;
	UPROPERTY(meta = (BindWidget))
		UImage* CRTScreen;

	UPROPERTY(meta = (BindWidget))
		URichTextBlock* Title;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* Prompt;
	UPROPERTY(meta = (BindWidget))
		UHorizontalBox* Leaderboard;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TopRound;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TopScore;

public:
	// Constructor--Sets default values
	UCP_MainMenu(const FObjectInitializer& ObjectInitializer);

protected:
	// Called after widget is constructed
	virtual void NativeConstruct() override;

	// Called when widget is destroyed
	virtual void NativeDestruct() override;

private:
	/*Title color scroll*/
	UFUNCTION()
		// Switches highlighted letter in title
		void SwitchHighlightLetter();
	UFUNCTION()
		// Switches style of highlighting in title
		void SwitchHighlightStyle();

	/*Widget Animations->http://benhumphreys.ca/unreal/ui-animations-from-cpp/ */
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

	/*Leaderboard*/
	UFUNCTION()
		// Plays leaderboard transition animation
		void DisplayLeaderboard();
	UFUNCTION()
		// Reverses leaderboard transition animation
		void HideLeaderboard();
	UFUNCTION()
		// Retrieves leaderboard data
		void OnAsyncLoadRecord(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

protected:
	/*Title color scroll*/
	UPROPERTY(EditDefaultsOnly, Category = "Title Color Scroll")
		float LetterSwitchInterval;
	UPROPERTY(EditDefaultsOnly, Category = "Title Color Scroll")
		float StyleSwitchInterval;
	UPROPERTY(EditDefaultsOnly, Category = "Title Color Scroll")
		int32 NumberOfExtraStyles;

	/*Widget Animations*/
	// Returns animation corresponding to the name
	TMap<FName, UWidgetAnimation*> AnimationMap;

	/*Leaderboard*/
	UPROPERTY(EditDefaultsOnly, Category = "Leaderboard")
		float TransitionDelay;
	UPROPERTY(EditDefaultsOnly, Category = "Leaderboard")
		float DisplayTime;

private:
	/*Title color scroll*/
	FString TitleText;
	int32 HighlightPosition;
	int32 StyleIndex;
	FTimerHandle LetterSwitch_TimerHandle;
	FTimerHandle StyleSwitch_TimerHandle;

	/*Leaderboard*/
	FTimerHandle LeaderboardDisplay_TimerHandle;
	FTimerHandle LeaderboardHide_TimerHandle;
};
