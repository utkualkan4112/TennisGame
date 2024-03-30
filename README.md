# Virtual Tennis Racket using Smartphone IMU

## Overview
This project ambitiously combines real-world physical interaction with a virtual gaming experience, leveraging smartphone technology and Unreal Engine (UE). It transforms a smartphone into a virtual tennis racket, aiming to recreate the experience of playing tennis in a virtual setup. This is achieved through sophisticated processing of the smartphone's Inertial Measurement Unit (IMU) data to control a racket object within the UE environment.

## Development Components
1. **Android Flask Application**:
   - A Flask-based server application runs on an Android device.
   - It captures IMU data, including accelerometer, gyroscope, and magnetometer readings, indicative of the device's motion and orientation.

2. **UDP Communication**:
   - Utilizes the User Datagram Protocol (UDP) to transmit data from the smartphone to the UE application.
   - UDP ensures real-time data transmission, crucial for maintaining game responsiveness.

3. **Unreal Engine Application**:
   - Receives IMU data sent from the smartphone.
   - Processes this data to update the game's state, particularly the virtual racket's position and orientation.

## Core Functionalities
1. **Madgwick Filter Implementation**:
   - The Madgwick filter is a widely used algorithm in IMU applications.
   - It fuses accelerometer, gyroscope, and magnetometer data to estimate orientation.
   - The filter provides a quaternion output representing the 3D orientation of the smartphone.

2. **Dynamic Calibration**:
   - A technique to reduce sensor noise and improve accuracy.
   - Records 100 samples when the device is stationary, calculating average biases for each sensor.
   - These biases are then used to correct real-time sensor data, mitigating the effects of sensor drift and noise.

3. **Zero-Velocity Updates (ZUPT)**:
   - A method to correct drift in IMU-based position tracking.
   - When the device is detected as stationary, velocity is reset to zero to prevent drift.
   - Involves detecting stationary periods during gameplay and responding accordingly.

4. **Handling Data Transmission Drift**:
   - Recognizes issues arising from potential data loss during UDP transmission.
   - Drift correction is considered through a manual game reset feature, returning the virtual environment to its initial state.

5. **Testing Environment**:
   - A simple tennis court environment in UE with physics properties.
   - The virtual ball's behavior mimics a real tennis ball, providing a realistic gaming experience.

## Implementation Details in Racket.cpp
- **Initialization**: Sets up the Madgwick filter, UDP communication, and initial values for various parameters like position and velocity.
- **Data Reception and Processing**:
  - Regularly receives IMU data from the smartphone.
  - Processes this data for real-time updates to the racket's position and orientation in UE.
- **Orientation Estimation**:
  - Utilizes the Madgwick filter for real-time orientation estimation.
  - Converts the quaternion output to UE's format and applies it to the racket object.
- **Position Estimation**:
  - More challenging due to the absence of external references (like cameras).
  - Uses accelerometer data, corrected for noise and bias, to estimate position.
  - Implements ZUPT for drift correction and maintaining positional accuracy.
- **Debugging and Visualization**:
  - Employs DrawDebugLine for visual debugging, aiding in the fine-tuning of motion tracking and data processing.

## Challenges and Solutions
- **Accurate Position Estimation**: A significant challenge due to reliance solely on IMU data. Addressed through dynamic calibration, ZUPT, and continuous monitoring of positional drift.
- **Real-Time Data Processing**: Critical for game responsiveness, achieved through UDP communication and efficient data processing algorithms.
- **Sensor Data Fusion**: The Madgwick filter plays a crucial role in combining data from different sensors to produce reliable orientation estimates.

## Future Work and Considerations
- **Optimizing Data Transmission**: Further improvements to handle lost data packets and reduce latency.
- **User Interface Enhancements**: Streamlining calibration processes and reset functionalities for a better user experience.
- **Expanded Testing**: Testing with various phone models and user scenarios to ensure robustness and wider applicability.
- **Feature Expansion**: Introducing additional gameplay features and enhancing the virtual environment for a more immersive experience.

## Conclusion
This project stands out as an innovative blend of physical interaction and virtual gaming, harnessing smartphone technology to create an accessible and engaging tennis game experience. While challenges like accurate position tracking and data transmission reliability remain, the project lays a solid foundation for a novel gaming experience. It highlights the potential of smartphones as versatile input devices for virtual gaming environments.
