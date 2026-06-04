// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Level_Base.h"
#include <memory>
#include "Level_FSM.generated.h"

UCLASS()
class GAMEAIPROG_API ALevel_FSM : public ALevel_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevel_FSM();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	bool IsTargetVisible();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ASteeringAgent* Agent{nullptr}; // ref
	ASteeringAgent* Thief{nullptr}; // ref
	std::unique_ptr<Seek> m_pSeek{};
	void SetTarget_Seek();
	
	float m_passedTime{};
};
