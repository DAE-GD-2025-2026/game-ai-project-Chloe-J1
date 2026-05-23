// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Level_Base.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Level_BehaviorTree.generated.h"

UCLASS()
class GAMEAIPROG_API ALevel_BehaviorTree : public ALevel_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevel_BehaviorTree();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ASteeringAgent* Guard{nullptr}; // ref
	ASteeringAgent* Thief{nullptr}; // ref
	
	
};
