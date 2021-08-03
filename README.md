# Stewart-Platform-with-CV
Ball Balancing through Real-Time Image Processing and Stewart Platform Control

## 개발 환경
|OS|사용 언어|사용 IDE|
|:---:|:---:|:---:|
|Windows|C, C++|Visual Studio, IAR Embedded Workbench|

## 프로젝트 개발 동기
Stewart Platform은 모션 시뮬레이션에도 다양하게 사용되지만 높은 분해능과 강성으로 인해 다양한 산업 공정은 물론 각종 정밀 제어에 사용되고 있다.  
보편적으로 사용하는 6DOF 스튜어트 플랫폼이 아닌 목표에 적합하게 3DOF의 기기의 간단한 형태를 이용해 제어기와 피드백을 이용한 ball balancing을 하는 것이 목표이다.  
개발 동기로는 영상 처리, MCU에서의 모터 제어, 제어기와 피드백을 통한 실시간 구동을 동시에 처리하는 것에 있고, 가장 큰 동기와 목표로는 다른 플랫폼들을 하나의 하드웨어에서 실현하기 위한 기술적 학습에 있다.  
추후에 다양한 프로세스들을 합치기 위해, ROS나 소프트웨어에 관련한 진전을 목표로 하고 있다.

## 프로젝트 개요
### 1. Computer vision (OpenCV)  
- 화면 상에서 원하는 색을 선택 시(MouseEvent), 그 색상만 Detecting 하도록 구현  
- 이를, 상단에 Threshold Bar를 삽입해 외부 조명이나 noise에 적합한 임계값을 찾도록 구현
- 다른 마우스버튼 이벤트를 통해 원하는 곳을 선택 시, 그곳으로 공을 유지시키도록 하는 동작 구현
- 카메라와 하드웨어의 Plate가 맞지 않는 상황 극복을 위해, 투시 변환 적용, 색 Detecting 전에 구역 설정

### 2. Inverse Kinematics of 3DOF Stewart Platform  
- yaw, roll, pitch 중 yaw를 제외한 성분을 다루어야 하는 수식의 특성상 Eigen Library를 활용해 행렬곱 연산
- 이를 통해, 각각의 다리 길이 도출

### 3. 하드웨어 설계 & 제작  
- Autodesk사의 Inventor pro를 이용해 설계  
- 3D 프린터로 출력

### 4. 균형 제어(PID제어)  

## System Architecture
<p align="center"><img src="https://user-images.githubusercontent.com/72693388/126869080-d2140588-1c6b-4774-b4ee-0a119c15cff7.png" width="400px"></p>   

### Code Overview  
- `main.cpp` : 영상처리(openCV)를 위한 system 중 main, Ball Detecting, 투시 변환 및 영상 처리를 위한 필터 적용 등의 구현 코드
- `Matrix_calc.cpp`, `Matrix_calc.h` : 3DOF Stewart Platform의 역기구학 수식을 위한 행렬 연산 구현 코드
- `serialcomm.cpp`, `serialcomm.h`, `serialport.cpp`, `serialport.h` : PC에서 MCU로의 시리얼 통신을 위한 코드(본인 작성 x)
- `PID_Control.cpp`, `PID_Control.h` : PID제어를 위한 코드
- `CortexM4_F401_Stewart_Project\Src\main.c` : Cortex-M4(STM32-F401)를 이용한 스텝모터 동작을 위한 코드

### Project scenario

1. visual studio의 openCV 환경에서 투시변환 후 Threshold를 조절해 공을 인식하고 공의 목표 위치를 정한다.  
2. 후에 수식을 계산하여 다리길이를 도출한 후 이를 MCU로 전송한다.  
3. 이를 알맞는 스텝모터의 값으로 계산한 후 반복하여 Pulse를 출력해준다.  
4. 위의 과정을 반복하여 공이 중심으로 가게 하거나 원하는 위치로 갈 수 있도록 한다.  

