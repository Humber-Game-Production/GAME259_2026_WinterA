// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandscapeSplineActor.h"
#include "GameFramework/Actor.h"
#include "TrackSplineBakerActor.generated.h"

class USplineComponent;
class ALandscapeProxy;

UCLASS()
class DEATHRACE_API ATrackSplineBakerActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrackSplineBakerActor();

	// The baked spline you will use for progress queries in BP.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Track")
	USplineComponent* TrackSpline;


	/** 
	 * Optional explicit reference to the landscape. 
	 * If null, the first landscape in the level will be used.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track")
	ALandscapeSplineActor* Landscape;

	// Editor button to rebuild from the Landscape spline.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track")
	float PointMergeDistance = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track")
	bool bClosedLoop = true;

	// Call this in editor to rebuild TrackSpline from Landscape splines.
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Track", DisplayName="Rebuild TrackSpline from Landscape Splines")
	void RebuildFromLandscapeSplines();

	/**
	 * Resolves the landscape to use for spline baking.
	 * If the Landscape property is null, this function will find the first
	 * landscape in the level.
	 * 
	 * @return A pointer to the resolved ALandscapeProxy.
	 */
	ALandscapeSplineActor* ResolveLandscape() const;
	
	UPROPERTY(EditAnywhere, Category="Track")
	float TargetPointSpacing = 200.0f; // centimeters

	void AppendUniquePoint(TArray<FVector>& points, const FVector& worldPoint) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
