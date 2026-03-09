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
	PrimaryActorTick.bCanEverTick = false;
	TrackSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrackSpline"));
	SetRootComponent(TrackSpline);
}

TArray<ALandscapeProxy*> ATrackSplineBakerActor::ResolveLandscapes() const
{
	TArray<ALandscapeProxy*> ResolvedLandscapes;

	// If we have explicit landscape references, use those
	if (Landscapes.Num() > 0)
	{
		for (const TSoftObjectPtr<ALandscapeProxy>& softLandscape : Landscapes)
		{
			if (ALandscapeProxy* Landscape = softLandscape.Get())
			{
				ResolvedLandscapes.Add(Landscape);
			}
			else
			{
				UE_LOG(LogTrackSplineBaker, Warning, TEXT("Soft landscape reference is null or not loaded."));
			}
		}
		return ResolvedLandscapes;
	}

	// Otherwise, find all landscapes in the world
	UWorld* world = GetWorld();
	if (!world)
	{
		return ResolvedLandscapes;
	}

	for (TActorIterator<ALandscapeProxy> it(world); it; ++it)
	{
		ResolvedLandscapes.Add(*it);
	}

	return ResolvedLandscapes;
}


// Called when the game starts or when spawned
void ATrackSplineBakerActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATrackSplineBakerActor::RebuildFromLandscapeSplines()
{
#if WITH_EDITOR
	/** Initial message **/
	UE_LOG(LogTrackSplineBaker, Log, TEXT("=== RebuildFromLandscapeSplines START (%s) ==="), *GetName());

	/** Lets try to get the current world and our Track Spline Pointer **/
	UWorld* World = GetWorld();
	UE_LOG(LogTrackSplineBaker, Log, TEXT("World: %s"), World ? *World->GetName() : TEXT("NULL"));
	UE_LOG(LogTrackSplineBaker, Log, TEXT("TrackSpline ptr: %s"), TrackSpline ? TEXT("VALID") : TEXT("NULL"));

	if (!World || !TrackSpline)
	{
		UE_LOG(LogTrackSplineBaker, Error, TEXT("Abort: world or TrackSpline is null."));
		return;
	}

	/** Resolve all landscapes to use **/
	TArray<ALandscapeProxy*> LandscapeProxies = ResolveLandscapes();
	UE_LOG(LogTrackSplineBaker, Log, TEXT("Resolved %d landscape(s)"), LandscapeProxies.Num());

	if (LandscapeProxies.Num() == 0)
	{
		UE_LOG(LogTrackSplineBaker, Error, TEXT("Abort: No ALandscapeProxy found or assigned."));
		return;
	}

	/** Collect all baked points from all landscapes **/
	TArray<FVector> BakedPoints;
	BakedPoints.Reserve(4096);

	/** Iterate through each landscape and add the points to our bakedPoints Array **/
	for (int32 LandscapeIdx = 0; LandscapeIdx < LandscapeProxies.Num(); ++LandscapeIdx)
	{
		ALandscapeProxy* LandscapeProxy = LandscapeProxies[LandscapeIdx];

		/** Give an offset for the points in this landscape, so they dont pile up together, its editable on the editor **/
		const FVector LandscapeOffset = PerLandscapeOffset * static_cast<float>(LandscapeIdx);

		UE_LOG(LogTrackSplineBaker, Log, TEXT("=== Processing Landscape %d: %s ==="), LandscapeIdx,
		       *LandscapeProxy->GetName());
		UE_LOG(LogTrackSplineBaker, Log, TEXT("Applying offset: X=%.2f, Y=%.2f, Z=%.2f"),
		       LandscapeOffset.X, LandscapeOffset.Y, LandscapeOffset.Z);

		/** For this landscape, get me the splines **/
		ULandscapeSplinesComponent* SplinesComponent = LandscapeProxy->GetSplinesComponent();
		UE_LOG(LogTrackSplineBaker, Log, TEXT("SplinesComponent: %s"), SplinesComponent ? TEXT("VALID") : TEXT("NULL"));

		if (!SplinesComponent)
		{
			UE_LOG(LogTrackSplineBaker, Warning, TEXT("Landscape %s has no SplinesComponent, skipping."),
			       *LandscapeProxy->GetName());
			continue;
		}

		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Splines Component Name: %s"), *SplinesComponent->GetName());
		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Is Active: %s"),
		       SplinesComponent->IsActive() ? TEXT("YES") : TEXT("NO"));
		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Is Visible: %s"),
		       SplinesComponent->IsVisible() ? TEXT("YES") : TEXT("NO"));

		/** For this Specific Landscape Spline, get all of its segments and control points **/
		const TArray<TObjectPtr<ULandscapeSplineSegment>>& Segments = SplinesComponent->GetSegments();
		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Number of Segments: %d"), Segments.Num());

		const TArray<TObjectPtr<ULandscapeSplineControlPoint>>& ControlPoints = SplinesComponent->GetControlPoints();
		UE_LOG(LogTrackSplineBaker, Log, TEXT("  Number of Control Points: %d"), ControlPoints.Num());

		UE_LOG(LogTrackSplineBaker, Log, TEXT("=== PROCESSING SEGMENTS ==="));
		UE_LOG(LogTrackSplineBaker, Log, TEXT("Processing %d segments from landscape %s..."), Segments.Num(),
		       *LandscapeProxy->GetName());

		TArray<int32> SegmentIndices;
		TArray<bool> InvertSegments;

		/** Do we have a Segment Order component for this landscape based on its index? **/
		if (LandscapeSegmentReadOrder.IsValidIndex(LandscapeIdx) && !LandscapeSegmentReadOrder[LandscapeIdx].
		                                                             SegmentIndices.IsEmpty())
		{
			/** Split the string by comma to extract the numbers per segment and add to our segment indices **/
			const FString& OrderString = LandscapeSegmentReadOrder[LandscapeIdx].SegmentIndices;
			TArray<FString> Parts;
			OrderString.ParseIntoArray(Parts, TEXT(","), true);
			for (const FString& Part : Parts)
			{
				/** Atoi means String -> Integer **/
				SegmentIndices.Add(FCString::Atoi(*Part));
			}

			/** Split the string by comma to extract the numbers per segment and add to our inverse option indices **/
			if (!LandscapeSegmentReadOrder[LandscapeIdx].bInvertSegments.IsEmpty())
			{
				const FString& InvertString = LandscapeSegmentReadOrder[LandscapeIdx].bInvertSegments;
				TArray<FString> InvertParts;
				InvertString.ParseIntoArray(InvertParts, TEXT(","), true);
				for (const FString& Part : InvertParts)
				{
					InvertSegments.Add(FCString::Atoi(*Part) != 0);
				}
			}
		}

		/** If there are no segment order values, then read as is **/
		if (SegmentIndices.Num() == 0)
		{
			SegmentIndices.Reserve(Segments.Num());
			for (int32 Idx = 0; Idx < Segments.Num(); ++Idx)
			{
				SegmentIndices.Add(Idx);
			}
		}

		/** Iterate through the segments, but based on the indices we calculated before **/
		for (int32 OrderIdx = 0; OrderIdx < SegmentIndices.Num(); ++OrderIdx)
		{
			const int32 SegIdx = SegmentIndices[OrderIdx];
			if (!Segments.IsValidIndex(SegIdx))
			{
				UE_LOG(LogTrackSplineBaker, Warning, TEXT("  Segment index %d is invalid for landscape %s"), SegIdx,
				       *LandscapeProxy->GetName());
				continue;
			}

			/** Get the segment and check if it is valid **/
			const TObjectPtr<ULandscapeSplineSegment>& Segment = Segments[SegIdx];
			if (!Segment)
			{
				UE_LOG(LogTrackSplineBaker, Warning, TEXT("  Segment %d: NULL"), SegIdx);
				continue;
			}

			/** Lets take all the points inside the segment **/
			/** This are the baked points of the spline, not the ones you set in the editor **/
			const TArray<FLandscapeSplineInterpPoint>& PointsInSegment = Segment->GetPoints();
			UE_LOG(LogTrackSplineBaker, Log, TEXT("  Segment %d: %d interp points"), SegIdx, PointsInSegment.Num());

			if (PointsInSegment.Num() == 0)
			{
				continue;
			}

			/** This section determines whether to read the segment forward or backward **/
			/** to ensure continuity when merging multiple landscape segments together. **/

			/** Get the first and last points of this segment (in world space with landscape offset applied) **/
			const FVector StartPoint = PointsInSegment[0].Center + LandscapeOffset;
			const FVector EndPoint = PointsInSegment.Last().Center + LandscapeOffset;

			/** Look up if a invert segment node is found at the invert array based on the index **/
			bool bForceInvert = false;
			if (InvertSegments.IsValidIndex(OrderIdx))
			{
				bForceInvert = InvertSegments[OrderIdx];
			}

			/** 
			 * If not manually inverted, automatically determine which end of this segment
			 * is closer to the last point we added to the merged track.
			 * HOWEVER 👆: Skip this check for the very first segment of each landscape
			 * to ensure each landscape's first segment always reads forward. This prevents
			 * unnecessary reversals on the initial segment of each landscape while still
			 * ensuring smooth connections between segments within the same landscape. **/
			bool bReverse = bForceInvert;
			if (!bForceInvert && BakedPoints.Num() > 0 && OrderIdx > 0)
			{
				const FVector& LastBaked = BakedPoints.Last();
				/** Compare distances: if the END of this segment is closer to our last point,
				 * then we should traverse this segment in REVERSE (from end to start) **/
				bReverse = FVector::DistSquared(LastBaked, EndPoint) < FVector::DistSquared(LastBaked, StartPoint);
			}

			/** Configure loop to iterate either forward (0 -> N) or backward (N -> 0) **/
			const int32 StartIdx = bReverse ? PointsInSegment.Num() - 1 : 0; // Start from last if reversed
			const int32 EndIdx = bReverse ? -1 : PointsInSegment.Num(); // End condition
			const int32 Step = bReverse ? -1 : 1; // Step direction
			
			/** We need to maintain spacing between points, so track the last point we actually added **/
			FVector LastAccepted = FVector::ZeroVector;
			bool bHasLast = false;
			if (BakedPoints.Num() > 0)
			{
				LastAccepted = BakedPoints.Last();
				bHasLast = true;
			}

			/** Iterate through the points in the segment in the order we chose previously (Regular or Reverse) **/
			for (int32 PtIdx = StartIdx; PtIdx != EndIdx; PtIdx += Step)
			{
				/** Get the current candidate point (with landscape offset applied) **/
				const FVector Candidate = PointsInSegment[PtIdx].Center + LandscapeOffset;

				/** If this is the very first point in the entire merged track, add it unconditionally **/
				if (!bHasLast)
				{
					BakedPoints.Add(Candidate);
					LastAccepted = Candidate;
					bHasLast = true;
					continue;
				}

				/** Only add this point if it's far enough from the last accepted point.
				 * This prevents over-dense point clustering and maintains TargetPointSpacing **/
				if (FVector::Dist(LastAccepted, Candidate) >= TargetPointSpacing)
				{
					BakedPoints.Add(Candidate);
					LastAccepted = Candidate;
				}
			}
			
			/** After filtering by spacing, make sure the actual end of this segment
			 * is included (respecting the traversal direction). This ensures segments
			 * connect properly without missing the junction points.
			 * Ensure last point of the segment is included in the chosen direction **/
			if (const FVector FinalPoint = bReverse ? StartPoint : EndPoint; !BakedPoints.IsEmpty() && !FinalPoint.Equals(BakedPoints.Last(), 1.0f))
			{
				BakedPoints.Add(FinalPoint);
			}
		}
	}

	UE_LOG(LogTrackSplineBaker, Log, TEXT("Total baked points (after merge): %d"), BakedPoints.Num());
	
	if (BakedPoints.Num() < 2)
	{
		UE_LOG(LogTrackSplineBaker, Error, TEXT("Not enough baked points (%d < 2). Aborting."), BakedPoints.Num());
		return;
	}

	/** Cleanup the current track spline **/
	TrackSpline->ClearSplinePoints(false);

	/** Actually Create our track spline with the points we calculated **/
	for (const FVector& WorldPoint : BakedPoints)
	{
		TrackSpline->AddSplinePoint(WorldPoint, ESplineCoordinateSpace::World, false);
	}

	TrackSpline->SetClosedLoop(bClosedLoop, false);
	TrackSpline->UpdateSpline();

	UE_LOG(LogTrackSplineBaker, Log, TEXT("=== RebuildFromLandscapeSplines SUCCESS ==="));
	UE_LOG(LogTrackSplineBaker, Log, TEXT("Successfully baked %d points into TrackSpline (Closed Loop: %s)"),
	       BakedPoints.Num(), bClosedLoop ? TEXT("YES") : TEXT("NO"));
#endif
}
