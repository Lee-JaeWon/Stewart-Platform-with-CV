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
<p align="center"><img src="https://user-images.githubusercontent.com/72693388/117032049-237d7680-ad3c-11eb-81bc-1a64d069b721.png" width="500px"></p>  
<p align="center"> ~5/5 fisrt ball detecting(기존 계획) </p>  

- 기존 계획과 다르게 HSV색공간의 색상을 통해서 검출하는 방식으로 수정  
- 기존의 Erode, Dilate 등의 처리를 사용하며 연산과정에 대한 소스 코드에 대한 실행 부담감이 증가하는 것을 지연 시간을 통해 확인할 수 있었는데,  
단순히 1개의 공과 아닌 것만을 구별하면 되기 때문에 라벨링 과정에서 넓이에 대한 조건을 추가하고 마우스 이벤트를 통해 물체를 지정해 그 물체의 색과 아닌 것만을 이진화시켜 물체를 검출할 수 있도록 함.  
- 또한 다른 이벤트 콜백을 통해 목표좌표를 설정 및 공의 중심좌표와 목표좌표간의 벡터(x, y)를 추출할 수 있도록 함.

<p align="center"><img src="https://user-images.githubusercontent.com/72693388/123237389-9b844380-d518-11eb-8eeb-095a71b51d44.png" width="500px"></p>  
<p align="center"><img src="https://user-images.githubusercontent.com/72693388/123237734-e30acf80-d518-11eb-85ae-fb0fd271cb8d.png" width="500px"></p>  
<p align="center"><img src="https://user-images.githubusercontent.com/72693388/123237940-0f265080-d519-11eb-8f75-b5925c4e894a.png" width="500px"></p>  
<p align="center"> Edit ball detecting </p>   

