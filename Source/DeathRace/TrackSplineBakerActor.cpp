// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackSplineBakerActor.h"

#include "EngineUtils.h"
#include "LandscapeProxy.h"
#include "LandscapeSplineSegment.h"
#include "Components/SplineComponent.h"

/** Define the Log Category **/
DEFINE_LOG_CATEGORY_STATIC(LogTrackSplineBaker, Log, All);

// Sets default values
ATrackSplineBakerActor::ATrackSplineBakerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	TrackSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrackSpline"));
	SetRootComponent(TrackSpline);
}

ALandscapeSplineActor* ATrackSplineBakerActor::ResolveLandscape() const
{
	if (Landscape)
	{
		return Landscape;
	}

	UWorld* world = GetWorld();
	if (!world)
	{
		return nullptr;
	}

	for (TActorIterator<ALandscapeSplineActor> it(world); it; ++it)
	{
		return *it;
	}

	return nullptr;
}

void ATrackSplineBakerActor::AppendUniquePoint(TArray<FVector>& points, const FVector& worldPoint) const
{
	if (points.Num() == 0)
	{
		points.Add(worldPoint);
		return;
	}

	const float mergeDistanceSquared = FMath::Square(FMath::Max(1.0f, PointMergeDistance));
	if (FVector::DistSquared(points.Last(), worldPoint) > mergeDistanceSquared)
	{
		points.Add(worldPoint);
	}
}

// Called when the game starts or when spawned
void ATrackSplineBakerActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATrackSplineBakerActor::RebuildFromLandscapeSplines()
{
#if WITH_EDITOR
	auto PrintScreen = [](const FString& message)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Emerald, message);
		}
	};
	
	auto PrintScreenError = [](const FString& message)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, message);
		}
	};
	
	/** Initial message **/
	UE_LOG(LogTrackSplineBaker, Log, TEXT("=== RebuildFromLandscapeSplines START (%s) ==="), *GetName());
	PrintScreen(FString::Printf(TEXT("Rebuild START: %s"), *GetName()));
	
	/** Step 1: Lets try to get the current world and our Track Spline Pointer **/
	UWorld* world = GetWorld();
	UE_LOG(LogTrackSplineBaker, Log, TEXT("World: %s"), world ? *world->GetName() : TEXT("NULL"));
	UE_LOG(LogTrackSplineBaker, Log, TEXT("TrackSpline ptr: %s"), TrackSpline ? TEXT("VALID") : TEXT("NULL"));

	if (!world || !TrackSpline)
	{
		UE_LOG(LogTrackSplineBaker, Error, TEXT("Abort: world or TrackSpline is null."));
		PrintScreenError(TEXT("Rebuild ABORT: world or TrackSpline null"));
		return;
	}
	
	/** Step 2: Lets get our landscape spline reference **/
	ALandscapeSplineActor* landscapeProxy = ResolveLandscape();
	UE_LOG(LogTrackSplineBaker, Log, TEXT("Landscape spline resolved: %s"), landscapeProxy ? *landscapeProxy->GetName() : TEXT("NULL"));

	if (!landscapeProxy)
	{
		UE_LOG(LogTrackSplineBaker, Error, TEXT("Abort: No ALandscapeProxy found or assigned."));
		PrintScreenError(TEXT("Rebuild ABORT: no LandscapeProxy"));
		return;
	}
	
	
	/** Step 3: On our Spline, Lets check the Splines Components itself is valid **/
	UE_LOG(LogTrackSplineBaker, Log, TEXT("=== SPLINES COMPONENT CHECK ==="));
	ULandscapeSplinesComponent* splinesComponent = landscapeProxy->GetSplinesComponent();
	UE_LOG(LogTrackSplineBaker, Log, TEXT("SplinesComponent (GetSplinesComponent): %s"), splinesComponent ? TEXT("VALID") : TEXT("NULL"));
	
	if (splinesComponent)
	{
		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Splines Component Name: %s"), *splinesComponent->GetName());
		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Is Active: %s"), splinesComponent->IsActive() ? TEXT("YES") : TEXT("NO"));
		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Is Visible: %s"), splinesComponent->IsVisible() ? TEXT("YES") : TEXT("NO"));
		
		const TArray<TObjectPtr<ULandscapeSplineSegment>>& segments = splinesComponent->GetSegments();
		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Number of Segments: %d"), segments.Num());
		
		const TArray<TObjectPtr<ULandscapeSplineControlPoint>>& controlPoints = splinesComponent->GetControlPoints();
		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Number of Control Points: %d"), controlPoints.Num());
	}

	/** No Spline component **/
	if (!splinesComponent)
	{
		UE_LOG(LogTrackSplineBaker, Error, TEXT("Abort: Landscape has no SplinesComponent. Is 'Enable Edit Layers' or splines present?"));
		PrintScreenError(TEXT("Rebuild ABORT: no SplinesComponent"));
		UE_LOG(LogTrackSplineBaker, Log, TEXT("=== END DEBUG INFO ==="));
		return;
	}
	
	

	/** Lets iterate through our segment inside our spline **/
	const TArray<TObjectPtr<ULandscapeSplineSegment>>& segments = splinesComponent->GetSegments();

	TArray<FVector> bakedPoints;
	bakedPoints.Reserve(4096);

	UE_LOG(LogTrackSplineBaker, Log, TEXT("=== PROCESSING SEGMENTS ==="));
	UE_LOG(LogTrackSplineBaker, Log, TEXT("Processing %d segments..."), segments.Num());

	for (int32 segIdx = 0; segIdx < segments.Num(); ++segIdx)
	{
		/** Get the segment and check if it is valid **/
		const TObjectPtr<ULandscapeSplineSegment>& segment = segments[segIdx];
		if (!segment)
		{
			UE_LOG(LogTrackSplineBaker, Warning, TEXT("  Segment %d: NULL"), segIdx);
			continue;
		}

		/** Lets take all the points inside the segment **/
		/** This are the baked points of the spline, not the ones you set in the editor **/
		const TArray<FLandscapeSplineInterpPoint>& interpPoints = segment->GetPoints();
		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Segment %d: %d interp points"), segIdx, interpPoints.Num());
		
		FVector lastAccepted = FVector::ZeroVector;
		bool hasLast = false;
		for (int32 ptIdx = 0; ptIdx < interpPoints.Num(); ++ptIdx)
		{
			const FVector candidate = interpPoints[ptIdx].Center;

			if (!hasLast)
			{
				bakedPoints.Add(candidate);
				lastAccepted = candidate;
				hasLast = true;
				continue;
			}
			
			if (FVector::Dist(lastAccepted, candidate) >= TargetPointSpacing)
			{
				AppendUniquePoint(bakedPoints, candidate);
				lastAccepted = candidate;
			}
		}
		
		// Ensure last point of the segment is included
		if (interpPoints.Num() > 0)
		{
			const FVector lastPoint = interpPoints.Last().Center;
			if (!bakedPoints.IsEmpty() && !lastPoint.Equals(bakedPoints.Last(), 1.0f))
			{
				bakedPoints.Add(lastPoint);
			}
		}
	}

	UE_LOG(LogTrackSplineBaker, Log, TEXT("Total baked points (after merge): %d"), bakedPoints.Num());

	if (bakedPoints.Num() < 2)
	{
		UE_LOG(LogTrackSplineBaker, Error, TEXT("Not enough baked points (%d < 2). Aborting."), bakedPoints.Num());
		PrintScreenError(TEXT("Rebuild FAILED: Not enough baked points"));
		return;
	}

	TrackSpline->ClearSplinePoints(false);

	for (const FVector& worldPoint : bakedPoints)
	{
		TrackSpline->AddSplinePoint(worldPoint, ESplineCoordinateSpace::World, false);
	}

	TrackSpline->SetClosedLoop(bClosedLoop, false);
	TrackSpline->UpdateSpline();
	
	UE_LOG(LogTrackSplineBaker, Log, TEXT("=== RebuildFromLandscapeSplines SUCCESS ==="));
	UE_LOG(LogTrackSplineBaker, Log, TEXT("Successfully baked %d points into TrackSpline (Closed Loop: %s)"), 
		bakedPoints.Num(), bClosedLoop ? TEXT("YES") : TEXT("NO"));
	PrintScreen(FString::Printf(TEXT("Rebuild SUCCESS: %d points baked"), bakedPoints.Num()));
#endif
}

// Called every frame
void ATrackSplineBakerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
