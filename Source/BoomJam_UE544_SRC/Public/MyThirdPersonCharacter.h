// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyThirdPersonCharacter.generated.h"

UCLASS()
class BOOMJAM_UE544_SRC_API AMyThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	//DECLARE_EVENT(AMyThirdPersonCharacter,  )

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int nLineLen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double fClimbMoveDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double fOnWallOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bClimbing;

	// ��������ͼ�е���
	UFUNCTION(BlueprintCallable, Category = "MyClimbFunction", meta = (ToolTip = "Use On Climb Movement"))
	void ClimbMovement(double fINInputActValue, FVector fv3INVector);

	// ��������ͼ�е���
	UFUNCTION(BlueprintCallable, Category = "MyClimbFunction", meta = (ToolTip = "Use On Climb Start"))
	void ClimbStart();

	// ��������ͼ�е���
	UFUNCTION(BlueprintCallable, Category = "MyClimbFunction", meta = (ToolTip = "Use On Climb Stop"))
	void StopClimb();

	// ��������ͼ�е���
	UFUNCTION(Category = "MyClimbFunction", meta = (ToolTip = "Use On Climb Start"))
	void ClimbStartAssit();

public:
	// Sets default values for this character's properties
	AMyThirdPersonCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void ClimbToTop(FVector fv3INTopLoc);

	// �����������߼� ע�⣬fv3INMoveDist�������λ�������������ƶ����λ������
	bool ClimbTest(FVector fv3INMoveDist, FHitResult& fhrHipsHit, FHitResult& fhrHeadHit, bool& bHeadHit, FVector& fv3HeadLoc);
};
