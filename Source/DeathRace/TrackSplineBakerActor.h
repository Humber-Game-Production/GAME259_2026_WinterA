// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackSplineBakerActor.generated.h"

class USplineComponent;
class ALandscapeProxy;

USTRUCT(BlueprintType)
struct FLandscapeSegmentOrder
{
	GENERATED_BODY()
	/** Current Values Used:
	 * Track 1
	 * 7,6,5,4,20,25,24,3,2,1,14,0,8,23,9,10,11,22,12,13,15,19,16,17,18,21
	 * 1,0
	 * Track 3
	 * 0,1,2,14,15,3,12,4,11,5,6,7,8,9,10
	 * 1,0 **/


	/** Comma-separated segment indices. Example: "0,1,13,5,6" **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track")
	FString SegmentIndices;

	/** Comma-separated inversion flags (0=false, 1=true). Example: "0,1,0,1,0"
	 * Must match the number of indices in SegmentIndices, or leave empty to disable per-segment inversion. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track")
	FString bInvertSegments;
};

UCLASS()
class DEATHRACE_API ATrackSplineBakerActor : public AActor
{
	GENERATED_BODY()

public:
	ATrackSplineBakerActor();

	/** The baked spline you will use for progress queries in BP. **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Track")
	USplineComponent* TrackSpline;


	/** 
	 * Array of landscape references to extract splines from.
	 * Soft references for level streaming support.
	 * If empty, all landscapes in the level will be used.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track")
	TArray<TSoftObjectPtr<ALandscapeProxy>> Landscapes;

	/**
	 * Offset applied to each landscape's points based on its index.
	 * Points from landscape N will be offset by (PerLandscapeOffset * N).
	 * This helps separate points from different landscapes for easier tuning.
	 * Set to FVector::ZeroVector to disable.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track", Meta=(EditCondition="Landscapes.Num() > 1"))
	FVector PerLandscapeOffset = FVector(0.0f, 0.0f, 0.0f);


	/** Whether the Spline should loop around, This in our case should always be false, but it doesn't hurt to have it **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track")
	bool bClosedLoop = true;

	/** Segment read order per landscape. Index 0 is landscape 0,w index 1 is landscape 1, etc.
	// Use comma-separated indices for each landscape. Example for 2 landscapes:
	// [0] SegmentIndices="0,1,2" bInvertSegments="0,0,0"
	// [1] SegmentIndices="5,3,4" bInvertSegments="1,0,0" **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track")
	TArray<FLandscapeSegmentOrder> LandscapeSegmentReadOrder;

	/** Editor button to rebuild from the Landscape spline. **/
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Track",
		DisplayName="Rebuild TrackSpline from Landscape Splines")
	void RebuildFromLandscapeSplines();

	/**
	 * Resolves the landscapes to use for spline baking.
	 * If the Landscapes array is empty, this function will find all
	 * landscapes in the level.
	 * 
	 * @return An array of pointers to the resolved ALandscapeProxy objects.
	 */
	TArray<ALandscapeProxy*> ResolveLandscapes() const;

	/**
	 * Spacing between points baked along the track spline.
	 * Determines the minimum distance required between consecutive points
	 * to avoid over-dense clustering of spline points.
	 */
	UPROPERTY(EditAnywhere, Category="Track")
	float TargetPointSpacing = 200.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
