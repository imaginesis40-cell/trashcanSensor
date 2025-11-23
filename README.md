# 프로젝트 소개 <br>

### 쓰레기통과 아두이노를 결합해 창의적 공학 설계를 실습하는 프로젝트 
<br><br>

프로젝트 목적  
  1. 사용자의 편의 증진  
  2. 공공의 안전 증진  <br><br>

설계 구성 품목

  1. 센서(입력장치)  
      1-a. 복수의 화재 감지 센서  
      1-b. 복수의 초음파 센서<br><br>
     
  3. 출력장치  
      2-a. LCD 디스플레이 1개  
      2-b. 피에조 부저 1개  <br><br>
     
  4. 컴퓨터 및 전력  
      1-c. 아두이노 UNO3 보드 1개  
      2-c. 9V 전지 1개  
      3-c. 배럴잭 1개  
      4-c. 복수의 100 Ω ~ 120 Ω 저항 (화재 감지 센서용)  
      5-c. 점퍼 케이블  <br><br>

  6. 결합-개선을 진행할 쓰레기통  <br><br><br>



# R 분석

<table>
  <tr>
    <td width="50%" align="center">
      <img src="analysis/센서_산점도/10cm_dot_graph.png" width="100%" alt="10cm 산점도"/>
      <br>
      <sub>그림 1. 10cm 거리에서 측정한 센서 데이터의 산점도<br>
      아날로그 값 range 74</td>
    <td width="50%" align="center">
      <img src="analysis/센서_산점도/5cm_dot_graph.png" width="100%" alt="5cm 산점도"/>
      <br>
      <sub>그림 2. 5cm 거리에서 측정한 센서 데이터의 산점도<br>
      아날로그 값 range 240</td>
  </tr>
</table>
<br><br><br>
  
<table>
  <tr>
    <td width="90%" align="center">
      <img src="analysis/센서_산점도/flame_sensor_error_by_distance.png" width="100%" alt="flame_sensor_error_by_distance"/>
      <br>
      <sub>그림 3. 거리에 따른 센서 값 변동 비교</td>
  </tr>
 </table>
<br><br>
  
<table>
  <tr>
    <td width="90%" align="center">
      <img src="analysis/센서_산점도/Sensor_value_by_distance.png" width="100%" alt="Sensor_value_by_distance"/>
      <br>
      <sub>그림 3-1. 그림3 그래프 확대<br>
      광원이 가까울수록 아날로그 값의 분산이 커짐을 알 수 있다</td>
  </tr>
 </table>
