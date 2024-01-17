// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <math.h>
#include "Madgwick.generated.h"

/* Madgwick filter structure. */
USTRUCT(BlueprintType)  // This makes the struct available to Blueprints
struct FMadgwickFilter
{
    GENERATED_USTRUCT_BODY()  // Macro to setup the struct for Unreal

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadgwickFilter")
    float Beta;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadgwickFilter")
    float Q0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadgwickFilter")
    float Q1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadgwickFilter")
    float Q2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadgwickFilter")
    float Q3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadgwickFilter")
    float Frequency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadgwickFilter")
    float InvFrequency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadgwickFilter")
    int32 Counter;

    // Constructor for initialization
    FMadgwickFilter()
        : Beta(0.1f), Q0(1.0f), Q1(0.0f), Q2(0.0f), Q3(0.0f), Frequency(100.0f), InvFrequency(0.01f), Counter(0)
    {}
};

static float invSqrt(float x) {
    union {
        float    f;
        uint32_t i;
    } conv;

    float       x2;
    const float threehalfs = 1.5F;

    x2 = x * 0.5F;
    conv.f = x;
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
    return conv.f;
}

/**
 * 
 */
UCLASS()
class VS_API UMadgwick : public UObject
{
	GENERATED_BODY()

public:
    UMadgwick();

    /* Clean up and return memory for the filter
 */
    bool mgos_imu_madgwick_destroy(FMadgwickFilter** filter);

    /* Sets the filter update rate and gain (defaults to freq=100Hz and gain=0.1)
     * The `mgos_imu_madgwick_update()` function then expects to be called at `freq`
     * per second.
     */
    bool mgos_imu_madgwick_set_params(FMadgwickFilter* filter, float freq, float beta);

    /* Resets the filter Quaternion to an initial state (of {1,0,0,0}).
     */
    bool mgos_imu_madgwick_reset(FMadgwickFilter* filter);

    /* Run an update cycle on the filter. Inputs gx/gy/gz are in any calibrated input (for example,
     * m/s/s or G), inputs of ax/ay/az are in Rads/sec, inputs of mx/my/mz are in any calibrated
     * input (for example, uTesla or Gauss). The inputs of mx/my/mz can be passed as 0.0, in which
     * case the magnetometer fusion will not occur.
     * Returns true on success, false on failure.
     */
    bool mgos_imu_madgwick_update(FMadgwickFilter* filter, float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);

    /*
     * Returns AHRS Quaternion, as values between -1.0 and +1.0.
     * Each of q0, q1, q2, q3 pointers may be NULL, in which case they will not be
     * filled in.
     * Returns true on success, false in case of error, in which case the values of
     * q0, q1, q2 and q3 are undetermined.
     */
    bool mgos_imu_madgwick_get_quaternion(FMadgwickFilter* filter, float* q0, float* q1, float* q2, float* q3);

    /*
     * Returns AHRS angles of roll, pitch and yaw, in Radians between -Pi and +Pi.
     * Each of the roll, pitch and yaw pointers may be NULL, in which case they will
     * not be filled in.
     * Returns true on success, false in case of error, in which case the values of
     * roll, pitch and yaw are undetermined.
     */
    bool mgos_imu_madgwick_get_angles(FMadgwickFilter* filter, float* roll, float* pitch, float* yaw);

    /*
     * Returns filter counter. Each call to `mgos_imu_madgwick_update()` increments the
     * counter by one.
     * Returns true on success, false in case of error, in which case the value of
     * counter is undetermined.
     */
    bool mgos_imu_madgwick_get_counter(FMadgwickFilter* filter, uint32_t* counter);

    FMadgwickFilter* mgos_imu_madgwick_create(void);
	
};
