// Fill out your copyright notice in the Description page of Project Settings.


#include "Madgwick.h"

UMadgwick::UMadgwick()
{
}

FMadgwickFilter* UMadgwick::mgos_imu_madgwick_create(void) {
    FMadgwickFilter* filter = new FMadgwickFilter();  // Use the FMadgwickFilter structure
    if (!filter) {
        return nullptr;
    }
    this->mgos_imu_madgwick_set_params(filter, 100.0f, 0.1f);
    this->mgos_imu_madgwick_reset(filter);
    return filter;
}

// Update the rest of the methods similarly...

bool UMadgwick::mgos_imu_madgwick_destroy(FMadgwickFilter** filter) {
    if (!*filter) {
        return false;
    }
    delete* filter;  // Use delete for C++ allocation
    *filter = nullptr;
    return true;
}

bool UMadgwick::mgos_imu_madgwick_set_params(FMadgwickFilter* filter, float freq, float beta) {
    if (!filter) {
        return false;
    }
    filter->Beta = beta;
    filter->Frequency = freq;
    filter->InvFrequency = 1.0f / freq;
    return true;
}

bool UMadgwick::mgos_imu_madgwick_reset(FMadgwickFilter* filter) {
    if (!filter) {
        return false;
    }
    filter->Q0 = 1.0f;
    filter->Q1 = 0.0f;
    filter->Q2 = 0.0f;
    filter->Q3 = 0.0f;
    filter->Counter = 0;
    return true;
}

static bool mgos_imu_madgwick_updateIMU(FMadgwickFilter* filter, float gx, float gy, float gz, float ax, float ay, float az) {
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    // No need to check filter pointer -- it's checked by _update()

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-filter->Q1 * gx - filter->Q2 * gy - filter->Q3 * gz);
    qDot2 = 0.5f * (filter->Q0 * gx + filter->Q2 * gz - filter->Q3 * gy);
    qDot3 = 0.5f * (filter->Q0 * gy - filter->Q1 * gz + filter->Q3 * gx);
    qDot4 = 0.5f * (filter->Q0 * gz + filter->Q1 * gy - filter->Q2 * gx);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * filter->Q0;
        _2q1 = 2.0f * filter->Q1;
        _2q2 = 2.0f * filter->Q2;
        _2q3 = 2.0f * filter->Q3;
        _4q0 = 4.0f * filter->Q0;
        _4q1 = 4.0f * filter->Q1;
        _4q2 = 4.0f * filter->Q2;
        _8q1 = 8.0f * filter->Q1;
        _8q2 = 8.0f * filter->Q2;
        q0q0 = filter->Q0 * filter->Q0;
        q1q1 = filter->Q1 * filter->Q1;
        q2q2 = filter->Q2 * filter->Q2;
        q3q3 = filter->Q3 * filter->Q3;

        // Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * filter->Q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * filter->Q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * filter->Q3 - _2q1 * ax + 4.0f * q2q2 * filter->Q3 - _2q2 * ay;
        recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);             // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback step
        qDot1 -= filter->Beta * s0;
        qDot2 -= filter->Beta * s1;
        qDot3 -= filter->Beta * s2;
        qDot4 -= filter->Beta * s3;
    }

    // Integrate rate of change of quaternion to yield quaternion
    filter->Q0 += qDot1 * filter->InvFrequency;
    filter->Q1 += qDot2 * filter->InvFrequency;
    filter->Q2 += qDot3 * filter->InvFrequency;
    filter->Q3 += qDot4 * filter->InvFrequency;

    // Normalise quaternion
    recipNorm = invSqrt(filter->Q0 * filter->Q0 + filter->Q1 * filter->Q1 + filter->Q2 * filter->Q2 + filter->Q3 * filter->Q3);
    filter->Q0 *= recipNorm;
    filter->Q1 *= recipNorm;
    filter->Q2 *= recipNorm;
    filter->Q3 *= recipNorm;

    filter->Counter++;
    return true;
}

bool UMadgwick::mgos_imu_madgwick_update(FMadgwickFilter* filter, float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
    if (!filter) {
        return false;
    }
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float hx, hy;
    float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

    // Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
    if ((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
        mgos_imu_madgwick_updateIMU(filter, gx, gy, gz, ax, ay, az);
        return false;
    }

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-filter->Q1 * gx - filter->Q2 * gy - filter->Q3 * gz);
    qDot2 = 0.5f * (filter->Q0 * gx + filter->Q2 * gz - filter->Q3 * gy);
    qDot3 = 0.5f * (filter->Q0 * gy - filter->Q1 * gz + filter->Q3 * gx);
    qDot4 = 0.5f * (filter->Q0 * gz + filter->Q1 * gy - filter->Q2 * gx);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Normalise magnetometer measurement
        recipNorm = invSqrt(mx * mx + my * my + mz * mz);
        mx *= recipNorm;
        my *= recipNorm;
        mz *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0mx = 2.0f * filter->Q0 * mx;
        _2q0my = 2.0f * filter->Q0 * my;
        _2q0mz = 2.0f * filter->Q0 * mz;
        _2q1mx = 2.0f * filter->Q1 * mx;
        _2q0 = 2.0f * filter->Q0;
        _2q1 = 2.0f * filter->Q1;
        _2q2 = 2.0f * filter->Q2;
        _2q3 = 2.0f * filter->Q3;
        _2q0q2 = 2.0f * filter->Q0 * filter->Q2;
        _2q2q3 = 2.0f * filter->Q2 * filter->Q3;
        q0q0 = filter->Q0 * filter->Q0;
        q0q1 = filter->Q0 * filter->Q1;
        q0q2 = filter->Q0 * filter->Q2;
        q0q3 = filter->Q0 * filter->Q3;
        q1q1 = filter->Q1 * filter->Q1;
        q1q2 = filter->Q1 * filter->Q2;
        q1q3 = filter->Q1 * filter->Q3;
        q2q2 = filter->Q2 * filter->Q2;
        q2q3 = filter->Q2 * filter->Q3;
        q3q3 = filter->Q3 * filter->Q3;

        // Reference direction of Earth's magnetic field
        hx = mx * q0q0 - _2q0my * filter->Q3 + _2q0mz * filter->Q2 + mx * q1q1 + _2q1 * my * filter->Q2 + _2q1 * mz * filter->Q3 - mx * q2q2 - mx * q3q3;
        hy = _2q0mx * filter->Q3 + my * q0q0 - _2q0mz * filter->Q1 + _2q1mx * filter->Q2 - my * q1q1 + my * q2q2 + _2q2 * mz * filter->Q3 - my * q3q3;
        _2bx = sqrtf(hx * hx + hy * hy);
        _2bz = -_2q0mx * filter->Q2 + _2q0my * filter->Q1 + mz * q0q0 + _2q1mx * filter->Q3 - mz * q1q1 + _2q2 * my * filter->Q3 - mz * q2q2 + mz * q3q3;
        _4bx = 2.0f * _2bx;
        _4bz = 2.0f * _2bz;

        // Gradient decent algorithm corrective step
        s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * filter->Q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * filter->Q3 + _2bz * filter->Q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * filter->Q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * filter->Q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * filter->Q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * filter->Q2 + _2bz * filter->Q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * filter->Q3 - _4bz * filter->Q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * filter->Q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * filter->Q2 - _2bz * filter->Q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * filter->Q1 + _2bz * filter->Q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * filter->Q0 - _4bz * filter->Q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * filter->Q3 + _2bz * filter->Q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * filter->Q0 + _2bz * filter->Q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * filter->Q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);             // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback step
        qDot1 -= filter->Beta * s0;
        qDot2 -= filter->Beta * s1;
        qDot3 -= filter->Beta * s2;
        qDot4 -= filter->Beta * s3;
    }

    // Integrate rate of change of quaternion to yield quaternion
    filter->Q0 += qDot1 * filter->InvFrequency;
    filter->Q1 += qDot2 * filter->InvFrequency;
    filter->Q2 += qDot3 * filter->InvFrequency;
    filter->Q3 += qDot4 * filter->InvFrequency;

    // Normalise quaternion
    recipNorm = invSqrt(filter->Q0 * filter->Q0 + filter->Q1 * filter->Q1 + filter->Q2 * filter->Q2 + filter->Q3 * filter->Q3);
    filter->Q0 *= recipNorm;
    filter->Q1 *= recipNorm;
    filter->Q2 *= recipNorm;
    filter->Q3 *= recipNorm;

    filter->Counter++;
    return true;
}

bool UMadgwick::mgos_imu_madgwick_get_quaternion(FMadgwickFilter* filter, float* q0, float* q1, float* q2, float* q3) {
    if (!filter) {
        return false;
    }
    if (q0) {
        *q0 = filter->Q0;
    }
    if (q1) {
        *q1 = filter->Q1;
    }
    if (q2) {
        *q2 = filter->Q2;
    }
    if (q3) {
        *q3 = filter->Q3;
    }
    return true;
}

bool UMadgwick::mgos_imu_madgwick_get_angles(FMadgwickFilter* filter, float* roll, float* pitch, float* yaw) {
    if (!filter) {
        return false;
    }
    if (roll) {
        *roll = asinf(-2.0f * (filter->Q1 * filter->Q3 - filter->Q0 * filter->Q2));
    }
    if (pitch) {
        *pitch = atan2f(filter->Q0 * filter->Q1 + filter->Q2 * filter->Q3, 0.5f - filter->Q1 * filter->Q1 - filter->Q2 * filter->Q2);
    }
    if (yaw) {
        *yaw = atan2f(filter->Q1 * filter->Q2 + filter->Q0 * filter->Q3, 0.5f - filter->Q2 * filter->Q2 - filter->Q3 * filter->Q3);
    }
    return true;
}

bool UMadgwick::mgos_imu_madgwick_get_counter(FMadgwickFilter* filter, uint32_t* counter) {
    if (!filter || !counter) {
        return false;
    }
    *counter = filter->Counter;
    return true;
}