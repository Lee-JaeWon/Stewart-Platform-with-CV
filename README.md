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
    => 투시변환, 내용 삭제(주석으로 달아놓음) 카메라와 Plate간의 수평을 정확히 보장할 수 없어 투시변환을 해서 길이를 맞춘다한들 유의미한 오차가 존재했기에 투시변환 내용은 디버깅 과정에서 삭제함  
    
<p align="center"><img src="https://user-images.githubusercontent.com/72693388/128622780-85578c69-aceb-441a-b839-a15f1c94302d.png" width = "400" ></p>  

### 2. Inverse Kinematics of 3DOF Stewart Platform  
- PKM(Parallel Kinematic Manipulator)에서는 일반적으로, 역기구학을 이용하는 것이 순기구학에 비해, 보다 사람에게 직관적이다. (비선형 폐쇄 형태에서 역기구학에 대한 솔루션이 일반적으로 존재하기 때문인 점도 있다.  
- yaw, roll, pitch 중 yaw를 제외한 성분을 다루어야 하는 수식의 특성상 Eigen Library를 활용해 행렬곱 연산
- 이를 통해, 각각의 다리 길이 도출

<p align="center"><img src="https://user-images.githubusercontent.com/72693388/128622592-a7a308e9-ea08-42b5-80f8-57f63f624d23.png" width = "400" ></p>

- 자세를 정해주기위해 카메라를 통해 X,Y 벡터를 얻은 후, 이를 Z벡터(하드웨어의 높이)와 함께 세타(theta)와 파이(phi)를 구한 후 이 성분들을 이용해 계속해서 위의 벡터를 계산해냈다.
- 이를 통해, 각각의 L_i (i=1,2,3)의 벡터를 구할 수 있으며, 계산하여 길이로 도출해낼 수 있다.


### 3. 하드웨어 설계 & 제작  
- Autodesk사의 Inventor pro를 이용해 설계  
- 3D 프린터로 출력

### 4. 균형 제어(PID제어)  
- X,Y 벡터를 이용해 PID제어를 하려고 했지만 계산하고 있는 수식에서 도출된 다리 길이가 오차가 심해서(카메라 캘리브레이션 문제)  
  다리 길이 간의 비율을 이용해 상황별 조건식을 통해 직접 제어함.
- 즉, 초기 제안 과정이나 코드 작성 과정에서는 PID를 구상하고 코드를 작성하였지만, 작품 과정에서 이 제어과정은 제외시킴.

## System Architecture
<p align="center"><img src="https://user-images.githubusercontent.com/72693388/126869080-d2140588-1c6b-4774-b4ee-0a119c15cff7.png" width="400px"></p>   
<p align="center"><img src="https://user-images.githubusercontent.com/72693388/128623209-87daf00f-486c-44c4-bbbd-924409296c2d.jpg" width="400px"></p>
<p align="center"> Cortex-M4(STM32F401RE)를 포함한 회로 구성 보드</p>

- 스텝 모터 모듈을 사용하여 펄스를 주거나 펄스를 세분화시킴. (모터 스펙은 200step에 1바퀴 -> 모듈을 통해 6400step에 1바퀴)  
- 전원부로는 3.3V와 12V 사용  
- PC로부터의 통신은 UART포트를 추가로 연결  

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

## 프로젝트 결과

<p align="center"><img src="https://user-images.githubusercontent.com/72693388/128623403-a9acb17f-749f-44f0-ae17-a6a590fb107d.png" width="400px"></p>  
<p align="center"> 전체적인 작품의 모습 </p>

<p align="center"><img src="https://user-images.githubusercontent.com/72693388/128623367-e5537875-c343-4496-a600-33bde3fbbd18.gif" width="300px"></p>  
<p align="center"> Balancing </p>

<p align="center"><img src="https://user-images.githubusercontent.com/72693388/128623552-cdd3d392-4ab3-4f19-8c24-08284d74ebc3.gif" width="300px"></p>  
<p align="center"> 다리 작동 영상 </p>

### 발생 문제점 및 해결
- 위에서 언급했듯이 카메라와 플레이트사이의 캘리브레이션이 부족하여 수식을 통해 계산한 값에 오차가 존재했다.
- 스텝 모터에 엔코더를 사용하지 않아서 플레이트가 시작시나 작동 중에 평행이어야 할 때 평행을 보장하지 못함. 그래서 오차가 누적되는 경우가 발생
  => 서보 모터를 사용했으면 더 간단했겠지만, 느낀바로는 스텝 모터가 작은각에도 굉장히 정확한 성능을 보여줬음.
- 이를, 계산된 다리간의 비율로 다양한 상황에서의 조건식을 통해 제어.
- 하드웨어 설계에 약간의 문제가 있어, 유격 발생 -> 모터에 무리가 가는 경우도 있었다.

### 추가 자료
[본 프로젝트 제안서](https://cafe.naver.com/roboticsbaram)
